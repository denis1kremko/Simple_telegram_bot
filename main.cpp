#include <iostream>
#include "online_bot_starter.h"

#define BOT_ADDRESS \
    std::string("https://api.telegram.org/bot5082458993:AAFk8fZlXKavOQf3HPrnTCCRI-FsQtx-cbg")

int main(int argc, char *argv[]) {
    OnlineBotStarter bot_starter(BOT_ADDRESS);
    bot_starter.EnableTelegramBot();

    return 0;
}