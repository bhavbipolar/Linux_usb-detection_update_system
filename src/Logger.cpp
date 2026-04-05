#include "Logger.h"
#include <fstream>
#include <ctime>
#include <cstring>

void Logger::log(const std::string& level, const std::string& message)
{
    std::ofstream logFile(std::string(getenv("HOME")) + "/usb_copy.log", std::ios::app);

    if (!logFile) return;

    std::time_t now = std::time(nullptr);
    char* dt = std::ctime(&now);
    dt[strlen(dt) - 1] = '\0'; // remove newline

    logFile << "[" << dt << "] [" << level << "] " << message << std::endl;
}