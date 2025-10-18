#include "printer.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <getopt.h>

using namespace em5820;

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Read text from stdin and print to thermal printer.\n\n"
              << "Options:\n"
              << "  -b, --bold           Print in bold\n"
              << "  -u, --underline      Print with underline\n"
              << "  -l, --left           Left align (default)\n"
              << "  -c, --center         Center align\n"
              << "  -r, --right          Right align\n"
              << "  -w, --wide           Double width text\n"
              << "  -t, --tall           Double height text\n"
              << "  -L, --large          Double width and height\n"
              << "  -f, --feed N         Feed N lines after printing (default: 2)\n"
              << "  -h, --help           Show this help message\n\n"
              << "Examples:\n"
              << "  echo 'Hello World' | " << program_name << "\n"
              << "  cat file.txt | " << program_name << " --center --bold\n"
              << "  ls -la | " << program_name << " --left\n"
              << "  fortune | " << program_name << " --center\n"
              << "  date | " << program_name << " --bold --center\n";
}

int main(int argc, char* argv[]) {
    // Default options
    bool bold = false;
    bool underline = false;
    bool double_width = false;
    bool double_height = false;
    Printer::Alignment alignment = Printer::Alignment::LEFT;
    int feed_lines = 2;
    
    // Parse command line options
    static struct option long_options[] = {
        {"bold",      no_argument,       0, 'b'},
        {"underline", no_argument,       0, 'u'},
        {"left",      no_argument,       0, 'l'},
        {"center",    no_argument,       0, 'c'},
        {"right",     no_argument,       0, 'r'},
        {"wide",      no_argument,       0, 'w'},
        {"tall",      no_argument,       0, 't'},
        {"large",     no_argument,       0, 'L'},
        {"feed",      required_argument, 0, 'f'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "bulcrwtLf:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'b':
                bold = true;
                break;
            case 'u':
                underline = true;
                break;
            case 'l':
                alignment = Printer::Alignment::LEFT;
                break;
            case 'c':
                alignment = Printer::Alignment::CENTER;
                break;
            case 'r':
                alignment = Printer::Alignment::RIGHT;
                break;
            case 'w':
                double_width = true;
                break;
            case 't':
                double_height = true;
                break;
            case 'L':
                double_width = true;
                double_height = true;
                break;
            case 'f':
                feed_lines = std::stoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    try {
        // Connect to printer
        Printer pos;
        pos.open_usb();
        pos.reset();
        
        // Set alignment
        pos.set_alignment(alignment);
        
        // Build text formatting byte
        uint8_t format = 0;
        if (bold) format = Printer::enable_bold(format);
        if (underline) format = Printer::enable_underline(format);
        if (double_width) format = Printer::enable_double_wide(format);
        if (double_height) format = Printer::enable_double_height(format);
        
        if (format != 0) {
            pos.set_print_text_type(format);
        }
        
        // Read from stdin and print
        std::string line;
        bool first_line = true;
        
        while (std::getline(std::cin, line)) {
            if (!first_line) {
                pos.write_string("\n");
            }
            pos.write_string(line);
            first_line = false;
        }
        
        // Add two empty lines at the end
        pos.write_string("\n\n");
        
        // Feed paper and reset
        pos.feed_lines(feed_lines);
        pos.reset();
        
        return 0;
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}