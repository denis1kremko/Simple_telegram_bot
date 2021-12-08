#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

#include <Poco/URI.h>
#include <Poco/JSON/Object.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/JSON/Parser.h>

#include "message_maker_class.h"

typedef std::optional<int64_t> OptionalParameter;

struct Message {
    std::string text;
    int64_t chat_id = 0;
    OptionalParameter id = {};
    OptionalParameter offset = {};
};

enum Command { OK, STOP, ABORT };

enum ServerType { HTTP, HTTPS };

class TelegramBot {
public:
    TelegramBot(const std::string& bot_address, const ServerType server_type = ServerType::HTTP);

    void GetMe();
    std::vector<Message> GetUpdate(OptionalParameter timeout = {}, OptionalParameter offset = {});
    void SendMassage(const Message& message);

    Command ProcessMassage(const Message& message);

    friend class BotWrapper;

private:
    const Poco::URI basic_uri_;
    std::unique_ptr<Poco::Net::HTTPClientSession> session_;
    Poco::JSON::Parser parser_;

    std::map<std::string, std::unique_ptr<MessageMakerBase>> message_map_;
};