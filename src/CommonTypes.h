#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <any>

const std::string CRLF = "\r\n";

typedef struct std::unordered_map<std::string, std::any> EXECUTION_CONTEXT;

struct HttpMetadata
{
    std::string url;
    std::string method;
    std::string protocol;
    std::string body;
    std::map<std::string, std::string> headers;
};
