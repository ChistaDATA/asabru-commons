#pragma once
#include "LogStrategy.h"
#include "../content-writer/NetworkContentWriter.h"

class NetLogStrategy : public LogStrategy
{
public:
    NetLogStrategy()
    {
        wt = new NetworkContentWriter();
    }
    bool DoLog(std::string logitem) override
    {
        // Log into the Network Socket
        wt->Write(logitem);
        return true;
    }
};
