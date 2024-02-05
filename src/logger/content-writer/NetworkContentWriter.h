#pragma once
#include "BaseContentWriter.h"
#include "../util/TcpClient.h"
#include <string>

class NetworkContentWriter: public BaseContentWriter
{
public:
    std::string domain;
    int port;
    NetworkContentWriter() {
        if (std::getenv("NETWORK_LOGGER_HOST")) {
            domain = std::string(std::getenv("NETWORK_LOGGER_HOST"));
        } else {
            domain = "127.0.0.1";
        }

        if (std::getenv("NETWORK_LOGGER_PORT")) {
            port = std::stoi(std::getenv("NETWORK_LOGGER_PORT"));
        } else {
            port = 5170;
        }
    }


    bool WriteToMedia(std::string content) override
    {
        TcpClient *_client = new TcpClient(domain.c_str(), port);
        _client->sendTcpMessage(std::string(content + "\n").c_str());
        return true;
    }
};

