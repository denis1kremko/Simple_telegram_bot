#pragma once

#include <string>
#include <memory>
#include <vector>

#include "logger.h"
#include "telegram_bot.h"

int64_t GetInitOffset(const std::string& filename);

class OnlineBotStarter {
public:
    OnlineBotStarter(const std::string& bot_address,
                     const std::string logger_filepath = "../bot/telegram/logger.txt");
    void EnableTelegramBot();

private:
    std::string filename_;
    int64_t offset_;
    std::unique_ptr<Logger> logger_;

    std::unique_ptr<TelegramBot> bot_;
};