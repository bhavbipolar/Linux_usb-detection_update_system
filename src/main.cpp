#include <QApplication>
#include <QThread>
#include <QMessageBox>

#include "UI_Window.h"
#include "dynamic_usb.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    UI_Window window;
    window.show();

    USBMonitor *monitor = new USBMonitor();
    QThread *usbThread = new QThread();

    monitor->moveToThread(usbThread);

    // Start listening when thread starts
    QObject::connect(usbThread, &QThread::started,
                     monitor, &USBMonitor::listen);

    // Device detected
    QObject::connect(monitor, &USBMonitor::deviceDetected,
                     &window, [&window, monitor](QString mountPoint) {
        
        window.updateStatus("USB detected at: " + mountPoint);
        
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(&window,
                                      "USB Detected",
                                      "USB mounted at:\n" + mountPoint +
                                      "\n\nStart copying files now?",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes)
        {
            // Use QMetaObject to call across threads safely
           // QMetaObject::invokeMethod(monitor, "startFileCopy", Qt::QueuedConnection);
        }
    });

    // Start copy button
    QObject::connect(&window, &UI_Window::startCopyRequested,
                     monitor, &USBMonitor::startFileCopy);

    // Cancel button
    QObject::connect(&window, &UI_Window::cancelCopyRequested,
                     monitor, &USBMonitor::cancelCopy);

    // Progress updates
    QObject::connect(monitor, &USBMonitor::fileScanProgress,
                     &window, &UI_Window::updateProgressBar);

    // Status updates
    QObject::connect(monitor, &USBMonitor::updateStatus,
                     &window, &UI_Window::updateStatus);

    // Device removed
    QObject::connect(monitor, &USBMonitor::deviceRemoved,
                     &window, [&window]() {
        window.updateStatus("USB device removed");
        QMessageBox::information(&window,
                                 "USB Removed",
                                 "USB device has been unmounted");
    });

    // Cleanup
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        usbThread->quit();
        usbThread->wait(3000);  // Wait max 3 seconds
        delete monitor;
        delete usbThread;
    });

    usbThread->start();

    return app.exec();
}