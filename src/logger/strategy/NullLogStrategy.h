#pragma once
#include "LogStrategy.h"
#include <iostream>

class NullLogStrategy: public LogStrategy {
    bool DoLog(std::string logItem) override {
           // Log into the Console
           std::cout << logItem << std::endl;
           return true;
    } 
};
