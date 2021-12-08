#include "message_maker_class.h"

std::string WeatherMessage::GetMessage() const {
    return "Winter is coming";
}

std::string RandomMessage::GetMessage() const {
    return std::to_string(rand());
}

std::string StyleguideMessage::GetMessage() const {
    return "Of course I can write a program without tests, pff";
}