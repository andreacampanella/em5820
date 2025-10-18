#include "printer.hpp"
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace em5820;

// Convert RGB to grayscale with gamma correction
float rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    // Standard luminance formula
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;
    // Apply gamma correction (2.2 is standard)
    return std::pow(gray / 255.0f, 1.0f / 2.2f);
}

// Floyd-Steinberg dithering algorithm
std::vector<uint8_t> dither_image(const std::vector<float>& grayscale, 
                                   int width, int height) {
    // Create a copy for error diffusion
    std::vector<float> working_copy = grayscale;
    
    // Output bitmap (1 bit per pixel, packed into bytes)
    int bytes_per_row = (width + 7) / 8;
    std::vector<uint8_t> bitmap(bytes_per_row * height, 0);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            float old_pixel = working_copy[idx];
            
            // Quantize to black or white
            float new_pixel = old_pixel > 0.5f ? 1.0f : 0.0f;
            float error = old_pixel - new_pixel;
            
            // Set the output bit (1 = black, 0 = white for thermal printers)
            if (new_pixel < 0.5f) {
                int byte_idx = y * bytes_per_row + (x / 8);
                int bit_idx = 7 - (x % 8);
                bitmap[byte_idx] |= (1 << bit_idx);
            }
            
            // Distribute error to neighboring pixels (Floyd-Steinberg)
            if (x + 1 < width)
                working_copy[idx + 1] += error * 7.0f / 16.0f;
            
            if (y + 1 < height) {
                if (x > 0)
                    working_copy[idx + width - 1] += error * 3.0f / 16.0f;
                working_copy[idx + width] += error * 5.0f / 16.0f;
                if (x + 1 < width)
                    working_copy[idx + width + 1] += error * 1.0f / 16.0f;
            }
        }
    }
    
    return bitmap;
}

// Load and process image
bool load_and_process_image(const std::string& filename, 
                            std::vector<uint8_t>& bitmap,
                            int& out_width, int& out_height,
                            int max_width = 384) {
    int width, height, channels;
    uint8_t* img_data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    
    if (!img_data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    std::cout << "Loaded image: " << width << "x" << height 
              << " (" << channels << " channels)" << std::endl;
    
    // Scale image to fit printer width (384 pixels max)
    float scale = 1.0f;
    if (width > max_width) {
        scale = static_cast<float>(max_width) / width;
        std::cout << "Scaling image by " << scale << " to fit printer width" << std::endl;
    }
    
    int scaled_width = static_cast<int>(width * scale);
    int scaled_height = static_cast<int>(height * scale);
    
    // Make width a multiple of 8
    scaled_width = (scaled_width / 8) * 8;
    if (scaled_width == 0) scaled_width = 8;
    
    std::cout << "Scaled size: " << scaled_width << "x" << scaled_height << std::endl;
    
    // Convert to grayscale and scale
    std::vector<float> grayscale(scaled_width * scaled_height);
    
    for (int y = 0; y < scaled_height; y++) {
        for (int x = 0; x < scaled_width; x++) {
            // Calculate source pixel position
            int src_x = static_cast<int>(x / scale);
            int src_y = static_cast<int>(y / scale);
            
            // Clamp to image bounds
            src_x = std::min(src_x, width - 1);
            src_y = std::min(src_y, height - 1);
            
            int src_idx = (src_y * width + src_x) * channels;
            
            uint8_t r, g, b;
            if (channels == 1) {
                r = g = b = img_data[src_idx];
            } else if (channels == 2) {
                r = g = b = img_data[src_idx];
            } else if (channels == 3) {
                r = img_data[src_idx];
                g = img_data[src_idx + 1];
                b = img_data[src_idx + 2];
            } else { // channels == 4
                r = img_data[src_idx];
                g = img_data[src_idx + 1];
                b = img_data[src_idx + 2];
            }
            
            grayscale[y * scaled_width + x] = rgb_to_gray(r, g, b);
        }
    }
    
    stbi_image_free(img_data);
    
    std::cout << "Applying Floyd-Steinberg dithering..." << std::endl;
    bitmap = dither_image(grayscale, scaled_width, scaled_height);
    
    out_width = scaled_width;
    out_height = scaled_height;
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_file.jpg>" << std::endl;
        std::cerr << "Supported formats: JPG, PNG, BMP, TGA, GIF" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    
    try {
        std::cout << "Loading and processing image: " << filename << std::endl;
        
        std::vector<uint8_t> bitmap;
        int width, height;
        
        if (!load_and_process_image(filename, bitmap, width, height)) {
            return 1;
        }
        
        std::cout << "Final bitmap: " << width << "x" << height 
                  << " (" << bitmap.size() << " bytes)" << std::endl;
        
        std::cout << "Connecting to printer..." << std::endl;
        Printer pos;
        pos.open_usb();
        pos.reset();
        
        std::cout << "Printing image..." << std::endl;
        pos.set_alignment(Printer::Alignment::CENTER);
        pos.print_bitmap_lines(Printer::BitmapMode::NORMAL, width, height, bitmap);
        
        std::cout << "Feeding paper..." << std::endl;
        pos.feed_lines(5);
        pos.reset();
        
        std::cout << "Done!" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}