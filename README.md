# EM5820 Thermal Printer USB Driver

> A modern C++ header-only driver for the EM5820 thermal receipt printer with USB support on Linux. Print images with dithering or pipe text directly from any command-line program!

![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| 📦 **Header-only library** | Easy integration into your projects |
| 🖼️ **Image printing** | JPEG, PNG, BMP, TGA, GIF support with Floyd-Steinberg dithering |
| 📝 **Text sink** | Pipe stdout from any program directly to the printer |
| ⚡ **Fast printing** | Optimized batch transfers to avoid timeouts |
| 🎨 **Text formatting** | Bold, underline, alignment, and size controls |
| 📏 **Auto-scaling** | Images automatically fit printer width (384px) |

---

## 🖨️ Hardware

| Specification | Value |
|---------------|-------|
| **Model** | EM5820 58mm embedded thermal printer |
| **Interface** | USB (also has RS232/TTL but not used here) |
| **Protocol** | ESC/POS commands |
| **Resolution** | 8 dots/mm, 384 dots/line |
| **Print width** | 48mm (384 pixels) |

---

## 📋 Requirements

- Linux system
- CMake 3.20+
- libusb-1.0
- C++11 compiler

---

## 🚀 Installation


# Install dependencies
sudo apt-get install libusb-1.0-0-dev cmake build-essential

# Clone and build
git clone <your-repo-url>
cd em5820
cmake -B build
cmake --build build


---

## 📖 Usage

### 🖼️ Print Images


# Print any image file
sudo ./build/print_image photo.jpg

# Supports multiple formats
sudo ./build/print_image screenshot.png
sudo ./build/print_image diagram.bmp


> **Note:** Images wider than 384 pixels are automatically scaled down while maintaining aspect ratio. Floyd-Steinberg dithering is applied for high-quality black and white conversion.

### 📝 Print Text


# Basic usage
echo "Hello, World!" | sudo ./build/print_text

# Print files
cat document.txt | sudo ./build/print_text

# With formatting
echo "RECEIPT" | sudo ./build/print_text --bold --center --large

# System output
ls -la | sudo ./build/print_text
date | sudo ./build/print_text --center --bold
fortune | sudo ./build/print_text --center


### 🎛️ Text Formatting Options

| Option | Long Form | Description |
|--------|-----------|-------------|
| \`-b\` | \`--bold\` | Bold text |
| \`-u\` | \`--underline\` | Underlined text |
| \`-l\` | \`--left\` | Left align (default) |
| \`-c\` | \`--center\` | Center align |
| \`-r\` | \`--right\` | Right align |
| \`-w\` | \`--wide\` | Double width |
| \`-t\` | \`--tall\` | Double height |
| \`-L\` | \`--large\` | Double width AND height |
| \`-f N\` | \`--feed N\` | Feed N lines after printing (default: 5) |
| \`-h\` | \`--help\` | Show help message |

---

## 💻 Library Usage

Include \`printer.hpp\` in your project:

cpp
#include "printer.hpp"
using namespace em5820;

Printer pos;
pos.open_usb();
pos.reset();

// Print text
pos.set_alignment(Printer::Alignment::CENTER);
pos.write_string("Hello, World!");

// Print image (with line-by-line sending to avoid timeouts)
std::vector<uint8_t> bitmap = /* your bitmap data */;
pos.print_bitmap_lines(Printer::BitmapMode::NORMAL, 384, 100, bitmap);

pos.feed_lines(5);
pos.reset();


### 📚 Available Methods

<details>
<summary>Click to expand API reference</summary>

#### Connection
- \`void open_usb()\` - Connect to printer via USB

#### Basic Commands
- \`uint16_t reset()\` - Reset printer to default settings
- \`uint16_t feed_lines(uint8_t lines)\` - Feed paper by N lines
- \`uint16_t feed_dots(uint8_t dots)\` - Feed paper by N dots

#### Text Formatting
- \`uint16_t set_alignment(Alignment align)\` - Set text alignment
- \`uint16_t set_print_text_type(uint8_t print_type)\` - Set text style
- \`uint16_t set_text_scale(uint8_t horizontal, uint8_t vertical)\` - Set text scale
- \`uint16_t set_underline(uint8_t thickness)\` - Set underline thickness
- \`uint16_t write_string(const std::string &str)\` - Write text string

#### Image Printing
- \`uint16_t print_bitmap_lines(BitmapMode mode, uint16_t width, uint16_t height, const std::vector<uint8_t> &bitmap, uint16_t lines_per_batch = 50)\` - Print bitmap image

#### Text Formatting Helpers
- \`static uint8_t enable_bold(uint8_t optbit)\` - Enable bold
- \`static uint8_t enable_underline(uint8_t optbit)\` - Enable underline
- \`static uint8_t enable_double_height(uint8_t optbit)\` - Enable double height
- \`static uint8_t enable_double_wide(uint8_t optbit)\` - Enable double width

</details>

---

## 📁 Project Structure


em5820/
├── CMakeLists.txt       # Build configuration
├── printer.hpp          # Header-only printer library
├── main.cpp             # Image printing with dithering
├── print_text.cpp       # Text sink for piping
├── stb_image.h          # Image loading library (download separately)
└── README.md            # This file


---

## 🎯 Examples

### 🧾 Receipt Printer Script


#!/bin/bash
# receipt.sh

echo "================================" | print --center
echo "MY COFFEE SHOP" | print --center --bold --large
echo "================================" | print --center
echo "" | print
echo "Date: \$(date '+%Y-%m-%d %H:%M')" | print --left
echo "Barista: \$USER" | print --left
echo "" | print
echo "1x Cappuccino............ \$4.50" | print --left
echo "1x Croissant............. \$3.00" | print --left
echo "--------------------------------" | print --center
echo "Total................... \$7.50" | print --right --bold
echo "" | print
echo "Thank you!" | print --center


### 📊 System Monitor


#!/bin/bash
# sysmon.sh - Print system stats

echo "SYSTEM STATUS" | sudo ./build/print_text --center --bold --large
echo "" | sudo ./build/print_text
uptime | sudo ./build/print_text --left
df -h / | sudo ./build/print_text --left
free -h | sudo ./build/print_text --left


### 📸 Photo Booth


#!/bin/bash
# Take a photo and print it

fswebcam -r 640x480 photo.jpg
sudo ./build/print_image photo.jpg
echo "\$(date)" | sudo ./build/print_text --center


---

## 🔧 Troubleshooting

### ❌ Permission Denied

Add udev rule to allow non-root access:


echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="28e9", ATTR{idProduct}=="0289", MODE="0666"' | \\
  sudo tee /etc/udev/rules.d/99-em5820.rules
sudo udevadm control --reload-rules


Or add your user to the dialout group:


sudo usermod -a -G dialout \$USER
# Log out and back in


### ⏱️ Timeout Errors

If you get \`LIBUSB_ERROR_TIMEOUT\`, try:

1. Reduce \`lines_per_batch\` in \`print_bitmap_lines()\` (default is 50)
2. Increase \`TIMEOUT\` constant in \`printer.hpp\`
3. Check USB cable quality

### 🔀 Garbled Output

- Verify printer is EM5820 model
- Check USB Vendor ID (\`0x28e9\`) and Product ID (\`0x0289\`)
- Ensure correct paper is loaded

---

## 🔬 Technical Details

The driver implements the ESC/POS protocol used by most thermal receipt printers. Key technical points:

- Uses libusb-1.0 for USB bulk transfers
- Bitmap data is sent in batches to avoid USB timeouts
- Images are converted to 1-bit monochrome using Floyd-Steinberg dithering
- Width must be multiple of 8 pixels (hardware requirement)
- Each byte represents 8 horizontal pixels in bitmap format

---

## 🙏 Credits

This driver was created by reverse engineering the printer library code in \`PrinterLib.dll\` from the manufacturer. Special thanks to the [Adafruit thermal printer community](https://deathandthepenguinblog.wordpress.com/2019/12/08/adafruit-mini-thermal-printer-part-1-getting-better-pictures/) for dithering inspiration.

---

## 📄 License

[Your License Here]

---

<p align="center">Made with ❤️ for thermal printing enthusiasts</p>
