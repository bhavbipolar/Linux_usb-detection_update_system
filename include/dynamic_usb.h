#ifndef DYNAMIC_USB_H
#define DYNAMIC_USB_H

#include <QObject>
#include <QTimer>  // ADD THIS
#include <filesystem>
#include <atomic>
#include <libudev.h>

class USBMonitor : public QObject
{
    Q_OBJECT

public:
    USBMonitor();
    ~USBMonitor();

public slots:
    void listen();
    void checkForEvents();  
    void startFileCopy();
    void cancelCopy();

signals:
    void deviceDetected(QString mountPoint);
    void deviceRemoved();
    void fileScanProgress(int progress);
    void updateStatus(QString text);

private:
    void handleDevice(struct udev_device *dev);
    void File_Scanner(const std::string& mount_point);
    bool copyFileChunked(const std::filesystem::path& source, const std::filesystem::path& destination);
    std::string computeSHA256(const std::filesystem::path& filePath);
    bool verifyFileIntegrity(const std::filesystem::path& source, const std::filesystem::path& destination);
    struct udev *udev_ptr;
    struct udev_monitor *monitor_ptr;
    int fd;
    
    QTimer *pollTimer;  // NEW: timer to check for events

    std::string current_mount_point;
    std::atomic<bool> cancelRequested{false};
};

#endif