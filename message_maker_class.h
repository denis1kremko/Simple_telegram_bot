#pragma once

#include <string>

class MessageMakerBase {
public:
    virtual ~MessageMakerBase() = default;
    virtual std::string GetMessage() const = 0;
};

class RandomMessage : public MessageMakerBase {
public:
    std::string GetMessage() const override;
};

class WeatherMessage : public MessageMakerBase {
public:
    std::string GetMessage() const override;
};

class StyleguideMessage : public MessageMakerBase {
public:
    std::string GetMessage() const override;
};