# Linux USB Detection & File Update System

A C++ system that automatically detects USB insertion and updates files with a GUI progress interface.

## Features

- Detect USB insertion using **libudev**
- Show GUI notification when USB is mounted
- Ask user confirmation before copying files
- Categorize files into folders
- Real-time progress bar
- Cancel operation support
- Auto stop when USB is removed

## Tech Stack

- C++
- Qt (GUI)
- libudev (USB detection)
- CMake
- Linux filesystem APIs

## Architecture

USB Insert
↓
libudev detection
↓
Qt popup confirmation
↓
File scan
↓
Copy files
↓
Progress bar update

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
./usb_system
