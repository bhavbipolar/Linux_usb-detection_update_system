# 🔌 USB Detection & File Transfer System (Linux)

A **robust, multithreaded USB file management system** built in C++ using Qt and Linux system APIs.
The system automatically detects USB devices, organizes files, verifies integrity using SHA-256, and ensures reliable transfer with retry mechanisms.

---

##  Features

*  **Automatic USB Detection** using libudev
*  **File Categorization** (Images, Videos, Audio, PDFs, Text)
*  **Chunked File Copy** for large files (interruptible & efficient)
*  **SHA-256 Integrity Verification** after copy
*  **Retry Mechanism** for corrupted transfers
*  **Multithreaded Architecture** using Qt (QThread)
*  **Cancel Support** during file transfer
*  **Real-time Progress Updates**
*  **Logging System** for debugging & monitoring

---

## Tech Stack

* **C++17**
* **Qt5 (QThread, UI)**
* **Linux (libudev)**
* **OpenSSL (SHA-256)**
* **CMake**

---

##  Project Structure

```
usb_system/
├── include/        # Header files
├── src/            # Source files
├── CMakeLists.txt  # Build configuration
├── README.md
```

---

##  Build Instructions

```bash
git clone https://github.com/bhavbipolar/Linux_usb-detection_update_system.git
cd usb_system
mkdir build && cd build
cmake ..
make
./usb_system
```

---

##  Testing & Validation

* Verified correct file transfer for large files
* Simulated corruption during copy to test retry mechanism
* Used SHA-256 hash comparison to validate integrity
* Observed logs using:

  ```bash
  tail -f ~/usb_copy.log
  ```

---

##  Key Design Highlights

* Separation of concerns (UI, USB monitoring, logging modules)
* Fault-tolerant design with retry logic
* Efficient memory usage via chunked file transfer
* Real-time responsiveness using event-driven Qt architecture

---

##  Future Improvements

* Thread-safe logging (mutex-based)
* Resume support for interrupted transfers
* Better UI/UX enhancements
* Replace deprecated OpenSSL APIs with EVP

---

##  Author

Bhavna Bisht

---

##  Notes

This project demonstrates **system-level programming, multithreading, and fault-tolerant design**, making it suitable for roles involving **C++, Linux systems, and performance-critical applications**.
