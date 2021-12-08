#include "online_bot_starter.h"

OnlineBotStarter::OnlineBotStarter(const std::string& bot_address,
                                   const std::string logger_filepath)
    : filename_(logger_filepath),
      offset_(GetInitOffset(filename_)),
      logger_(std::make_unique<Logger>(filename_)),
      bot_(std::make_unique<TelegramBot>(bot_address, ServerType::HTTPS)) {
}

void OnlineBotStarter::EnableTelegramBot() {
    logger_->Log(std::to_string(offset_));

    while (true) {
        std::vector<Message> messages;

        messages = bot_->GetUpdate(5, offset_);

        for (const Message& message : messages) {
            offset_ = message.offset.value() + 1;
            logger_->Log(std::to_string(offset_));

            auto command = bot_->ProcessMassage(message);

            if (command == Command::ABORT) {
                bot_ = nullptr;
                logger_ = nullptr;
                abort();
            } else if (command == Command::STOP) {
                return;
            }
        }
    }
}

int64_t GetInitOffset(const std::string& filename) {
    std::ifstream reader;
    reader.open(filename, std::ios_base::out);

    int64_t init_offset = 0;

    while (!reader.eof()) {
        reader >> init_offset;
    }

    return init_offset;
}