#include "logger.h"
#include <iostream>

Logger::Logger(const std::string& filename) : filename_(filename) {
    f_writer_.open(filename_, std::ios_base::out);
}

void Logger::Log(const std::string message) {
    f_writer_ << message << '\n';
}

Logger::~Logger() {
    f_writer_.close();
}