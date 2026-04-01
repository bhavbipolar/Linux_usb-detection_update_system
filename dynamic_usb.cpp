#include "dynamic_usb.h"
#include <iostream>
#include <filesystem>
#include <poll.h>
#include <QThread>
#include <chrono>
#include <cstring>
#include <fstream>
#include <sstream>
#include <QCoreApplication>

namespace fs = std::filesystem;

USBMonitor::USBMonitor() : pollTimer(nullptr)
{
    udev_ptr = udev_new();
    monitor_ptr = udev_monitor_new_from_netlink(udev_ptr, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(monitor_ptr, "block", "partition");
    udev_monitor_enable_receiving(monitor_ptr);
    fd = udev_monitor_get_fd(monitor_ptr);
}

USBMonitor::~USBMonitor()
{
    if(pollTimer) {
        pollTimer->stop();
        delete pollTimer;
    }
    if(monitor_ptr) udev_monitor_unref(monitor_ptr);
    if(udev_ptr) udev_unref(udev_ptr);
}

void USBMonitor::listen()
{
    std::cout << "Starting USB monitoring..." << std::endl;
    
    // Create timer in the thread
    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &USBMonitor::checkForEvents);
    pollTimer->start(500);  // Check every 500ms
}

void USBMonitor::checkForEvents()
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    
    int ret = poll(&pfd, 1, 0);  // Non-blocking poll
    
    if(ret > 0 && (pfd.revents & POLLIN))
    {
        struct udev_device *dev = udev_monitor_receive_device(monitor_ptr);
        if(dev)
        {
            handleDevice(dev);
            udev_device_unref(dev);
        }
    }
}

void USBMonitor::handleDevice(struct udev_device *dev)
{
    const char *action = udev_device_get_action(dev);
    const char *devnode = udev_device_get_devnode(dev);
    const char *bus = udev_device_get_property_value(dev, "ID_BUS");

    if(!action || !devnode) return;
    
    std::cout << "Event: " << action << " on " << devnode << std::endl;

    if(bus && strcmp(bus, "usb") == 0)
    {
        if(strcmp(action, "add") == 0)
        {
            std::cout << "USB device added, waiting for mount..." << std::endl;
            
            // Wait a bit for mount
            QThread::sleep(2);
            
            // Find mount point
            std::ifstream mounts("/proc/mounts");
            std::string line;
            
            while(std::getline(mounts, line))
            {
                std::istringstream iss(line);
                std::string device, mountpoint;
                
                if(iss >> device >> mountpoint && device == devnode)
                {
                    current_mount_point = mountpoint;
                    std::cout << "USB mounted at: " << mountpoint << std::endl;
                    emit deviceDetected(QString::fromStdString(mountpoint));
                    return;
                }
            }
            
            // Fallback: check common locations
            std::string username = getenv("USER") ? getenv("USER") : "";
            std::vector<std::string> locations = {
                "/media/" + username,
                "/run/media/" + username,
                "/mnt"
            };
            
            for(const auto& base : locations)
            {
                try {
                    if(fs::exists(base))
                    {
                        for(const auto& entry : fs::directory_iterator(base))
                        {
                            if(fs::is_directory(entry))
                            {
                                current_mount_point = entry.path().string();
                                std::cout << "Found USB at: " << current_mount_point << std::endl;
                                emit deviceDetected(QString::fromStdString(current_mount_point));
                                return;
                            }
                        }
                    }
                } catch(...) { continue; }
            }
        }
        else if(strcmp(action, "remove") == 0)
        {
            std::cout << "USB device removed" << std::endl;
            current_mount_point.clear();
            emit deviceRemoved();
        }
    }
}

void USBMonitor::startFileCopy()
{
    if(current_mount_point.empty())
    {
        emit updateStatus("No USB mounted!");
        std::cout << "Error: No mount point" << std::endl;
        return;
    }

    std::cout << "Starting file copy from: " << current_mount_point << std::endl;
    cancelRequested = false;
    File_Scanner(current_mount_point);
}

void USBMonitor::cancelCopy()
{
    std::cout << "Copy cancelled" << std::endl;
    cancelRequested = true;
    emit updateStatus("Cancelled");
}

// New helper for interruptible copying
bool USBMonitor:: copyFileChunked(const fs::path& source, const fs::path& destination)
{
std::ifstream src(source, std::ios::binary);
    std::ofstream dst(destination, std::ios::binary);
    
    if (!src.is_open() || !dst.is_open()) return false;

    // 1MB buffer size
    std::vector<char> buffer(1024 * 1024); 
    
    while (src.read(buffer.data(), buffer.size()) || src.gcount() > 0) {
        // Allow the UI/Cancel signals to be processed
        QCoreApplication::processEvents();

        // Check if user clicked cancel DURING the large file copy
        if (cancelRequested) {
            src.close();
            dst.close();
            fs::remove(destination); // Delete the partial/corrupted file
            return false;
        }

        dst.write(buffer.data(), src.gcount());
    }

    return true;
}

void USBMonitor::File_Scanner(const std::string &mount_point)
{
    fs::path usb_path(mount_point);
    fs::path dest = fs::path(getenv("HOME")) / "OrganizedFiles";

    if(!fs::exists(usb_path))
    {
        emit updateStatus("Path not found!");
        return;
    }

    emit updateStatus("Scanning...");
    std::cout << "Scanning: " << usb_path << std::endl;

    uintmax_t total = 0;
    int file_count = 0;
    
    try
    {
        for(const auto &e : fs::recursive_directory_iterator(usb_path))
        {
            if(e.is_regular_file())
            {
                total += e.file_size();
                file_count++;
            }
        }
    }
    catch(const std::exception& e)
    {
        emit updateStatus(QString("Scan error: ") + e.what());
        return;
    }

    if(file_count == 0)
    {
        emit updateStatus("No files found");
        return;
    }

    std::cout << "Found " << file_count << " files" << std::endl;

    uintmax_t processed = 0;
    int copied = 0;
    auto start = std::chrono::steady_clock::now();

    try
    {
        for(const auto &e : fs::recursive_directory_iterator(usb_path))
        {
            if(cancelRequested)
            {
                emit updateStatus("Cancelled");
                return;
            }

            if(!e.is_regular_file()) continue;

            fs::path file = e.path();
            std::string ext = file.extension().string();
            
            for(auto& c : ext) c = std::tolower(c);

            fs::path folder;

            if(ext == ".jpg" || ext == ".jpeg" || ext == ".png")
                folder = dest / "Images";
            else if(ext == ".mp4" || ext == ".avi" || ext == ".mkv")
                folder = dest / "Videos";
            else if(ext == ".mp3" || ext == ".wav")
                folder = dest / "Audio";
            else if(ext == ".pdf")
                folder = dest / "PDFs";
            else if(ext == ".txt")
                folder = dest / "TextFiles";
            else
                continue;
            fs::create_directories(folder);
            
            // 1. Define the target path (Fixes the "targetPath" error)
            fs::path targetPath = folder / file.filename();
            // 2. ONLY use the chunked copy (Removes the double copy bug)
            if (!copyFileChunked(file, targetPath)) {
                if (cancelRequested) {
                    std::cout << "File copy interrupted: " << file.filename() << std::endl;
                    return; // Exit the scanner entirely
                } else {
                    throw std::runtime_error("Copy failed for: " + file.string());
                }
            }
            copied++;
            processed += e.file_size();

            int progress = total > 0 ? (processed * 100) / total : 0;
            emit fileScanProgress(progress);

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
            double speed = (elapsed > 0) ? (processed / 1024.0 / 1024.0) / elapsed : 0;

            QString status = QString("Copied %1/%2 | %3 MB/s")
                .arg(copied).arg(file_count).arg(speed, 0, 'f', 2);

            emit updateStatus(status);

            std::cout << "Copied: " << file.filename() << std::endl;

            QThread::msleep(20);
        }
    }
    catch(const std::exception& e)
    {
        emit updateStatus(QString("Error: ") + e.what());
        return;
    }

    emit fileScanProgress(100);
    emit updateStatus(QString("Done! Copied %1 files").arg(copied));
    std::cout << "Copy complete!" << std::endl;
}