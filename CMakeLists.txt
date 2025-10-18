cmake_minimum_required(VERSION 3.20)
project(EM5820_Printer VERSION 1.0 LANGUAGES CXX)

# Create the header-only library
add_library(em5820_printer INTERFACE)
target_compile_definitions(em5820_printer INTERFACE EM5820_PRINTER)
target_include_directories(em5820_printer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Find LibUSB
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)
target_link_libraries(em5820_printer INTERFACE PkgConfig::LIBUSB)

# Build the image printing executable
add_executable(print_image main.cpp)
target_link_libraries(print_image PRIVATE em5820_printer)
set_target_properties(print_image PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# Build the text printing executable
add_executable(print_text print_text.cpp)
target_link_libraries(print_text PRIVATE em5820_printer)
set_target_properties(print_text PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

message(STATUS "EM5820 Printer programs configured:")
message(STATUS "  - print_image: Print JPEG/PNG images with dithering")
message(STATUS "  - print_text: Print text from stdin (pipe-friendly)")