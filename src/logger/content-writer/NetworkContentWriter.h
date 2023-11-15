#pragma once
#include "BaseContentWriter.h"
#include "../util/TcpClient.h"
#include <string>

class NetworkContentWriter: public BaseContentWriter
{
public:
    const std::string domain = "127.0.0.1";
    static const int port = 4500;
    bool WriteToMedia(std::string content) override
    {
        TcpClient *_client = new TcpClient(domain.c_str(), port);
        _client->sendTcpMessage(content.c_str());
        return true;
    }
};

