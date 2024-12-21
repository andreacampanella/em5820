cmake_minimum_required(VERSION 3.20)

project(EM5820_Printer VERSION 1.0 LANGUAGES CXX)

add_library(em5820_printer INTERFACE)
target_compile_definitions(em5820_printer INTERFACE EM5820_PRINTER)
target_include_directories(em5820_printer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB libusb-1.0 IMPORTED_TARGET)
if(NOT LIBUSB_FOUND)
    message(FATAL_ERROR "LibUSB 1.0 required to compile EM5820 printer module")
endif()

target_link_libraries(em5820_printer INTERFACE ${LIBUSB_LIBRARIES})
set_target_properties(em5820_printer PROPERTIES
    DESCRIPTION "Header-only library for the EM5820 printer"
    VERSION "1.0"
)

message(STATUS "EM5820 Printer library is ready to be used")
