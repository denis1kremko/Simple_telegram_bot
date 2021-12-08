#pragma once

#include <fstream>
#include <string>
#include <stdexcept>

class Logger {
public:
    Logger(const std::string& filename);
    void Log(const std::string message);
    ~Logger();
private:
    std::string filename_;
    std::ofstream f_writer_;
};