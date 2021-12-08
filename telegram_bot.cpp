#include "telegram_bot.h"

void CheckResponse(const Poco::Net::HTTPResponse &response) {
    if (response.getStatus() / 100 == 4) {
        std::string response_status = std::to_string(response.getStatus());
        throw std::runtime_error("Client error, status: " + response_status);
    } else if (response.getStatus() / 100 == 5) {
        std::string response_status = std::to_string(response.getStatus());
        throw std::runtime_error("Internal server error, response status: " + response_status);
    }
}

TelegramBot::TelegramBot(const std::string &bot_address, const ServerType server_type)
    : basic_uri_(bot_address), session_(nullptr), parser_() {

    session_ = (server_type == ServerType::HTTP)
                   ? std::make_unique<Poco::Net::HTTPClientSession>()
                   : std::make_unique<Poco::Net::HTTPSClientSession>();

    session_->setHost(basic_uri_.getHost());
    session_->setPort(basic_uri_.getPort());

    message_map_["/random"] = std::make_unique<RandomMessage>();
    message_map_["/weather"] = std::make_unique<WeatherMessage>();
    message_map_["/styleguide"] = std::make_unique<StyleguideMessage>();
}

void TelegramBot::GetMe() {
    Poco::URI get_me_uri(basic_uri_.toString() + "/getMe");

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, get_me_uri.getPathAndQuery());

    session_->sendRequest(request);

    Poco::Net::HTTPResponse response;
    auto &body = session_->receiveResponse(response);
    CheckResponse(response);
}

std::vector<Message> TelegramBot::GetUpdate(OptionalParameter timeout, OptionalParameter offset) {
    Poco::URI update_uri(basic_uri_.toString() + "/getUpdates");

    if (offset.has_value()) {
        update_uri.addQueryParameter("offset", std::to_string(offset.value()));
    }
    if (timeout.has_value()) {
        update_uri.addQueryParameter("timeout", std::to_string(timeout.value()));
    }

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, update_uri.getPathAndQuery());

    session_->sendRequest(request);

    Poco::Net::HTTPResponse response;
    auto &body = session_->receiveResponse(response);
    CheckResponse(response);

    auto reply = parser_.parse(body);
    Poco::JSON::Array::Ptr json_message_array =
        reply.extract<Poco::JSON::Object::Ptr>()->getArray("result");

    std::vector<Message> message_array;
    for (auto &message : *json_message_array) {
        int64_t message_offset =
            message.extract<Poco::JSON::Object::Ptr>()->getValue<int64_t>("update_id");

        Poco::JSON::Object::Ptr message_info =
            message.extract<Poco::JSON::Object::Ptr>()->getObject("message");

        int64_t chat_id = message_info->getObject("chat")->getValue<int64_t>("id");
        int64_t message_id = message_info->getValue<int64_t>("message_id");

        std::string text;
        try {
            text = message_info->getValue<std::string>("text");
        } catch (...) {
        }

        message_array.push_back(Message{text, chat_id, message_id, message_offset});
    }

    return message_array;
}

void TelegramBot::SendMassage(const Message &message) {
    Poco::URI message_uri(basic_uri_.toString() + "/sendMessage");

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST,
                                   message_uri.getPathAndQuery());

    Poco::JSON::Object message_info;
    message_info.set("chat_id", message.chat_id);
    message_info.set("text", message.text);
    // reply_case
    if (message.id.has_value()) {
        message_info.set("reply_to_message_id", message.id.value());
    }

    request.setContentType("application/json");
    std::stringstream ss;
    message_info.stringify(ss);
    request.setContentLength(ss.str().size());

    std::ostream &req_stream = session_->sendRequest(request);
    message_info.stringify(req_stream);

    Poco::Net::HTTPResponse response;
    auto &body = session_->receiveResponse(response);
    CheckResponse(response);
}

Command TelegramBot::ProcessMassage(const Message &message) {
    if (message.text == "/stop") {
        return Command::STOP;
    } else if (message.text == "/crash") {
        return Command::ABORT;
    }

    if (message_map_.count(message.text) > 0) {
        SendMassage({message_map_[message.text]->GetMessage(), message.chat_id});
    }

    return Command::OK;
}