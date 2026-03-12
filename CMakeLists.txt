cmake_minimum_required(VERSION 3.10)
project(usb_system VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 1. Find the udev library safely
find_library(UDEV_LIB udev REQUIRED)

# 2. Define your source files 
# Including the .h file here helps IDEs like VS Code show them in the project tree
set(SOURCES 
    UI_Window.cpp
    UI_Window.h
    dynamic_usb.cpp
    dynamic_usb.h
    main.cpp
)

#3 .Enable Qt MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#4 . Add Qt Widgets package
find_package(Qt5 REQUIRED COMPONENTS Widgets)
#target_link_libraries(usb_system PRIVATE Qt5::Widgets)

# 5. Add the executable using the SOURCE list
add_executable(usb_system ${SOURCES})

# 6. Link the library found in step 1
target_link_libraries(usb_system PRIVATE udev Qt5::Widgets)



