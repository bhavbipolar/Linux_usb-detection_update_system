cmake_minimum_required(VERSION 3.10)
project(usb_system VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find libraries
find_library(UDEV_LIB udev REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(OpenSSL REQUIRED)

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES 
    include/UI_Window.h
    include/dynamic_usb.h
    include/Logger.h
    src/main.cpp
    src/UI_Window.cpp
    src/dynamic_usb.cpp
    src/Logger.cpp
)

# Create executable
add_executable(usb_system ${SOURCES})

# Link libraries (clean version)
target_include_directories(usb_system PRIVATE include)
target_link_libraries(usb_system PRIVATE ${UDEV_LIB} Qt5::Widgets OpenSSL::Crypto)