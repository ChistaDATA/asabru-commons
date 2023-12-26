#pragma once
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <cstring>

class Utils
{
public:
    static std::vector<std::string> split(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }
    static std::pair<std::string, std::string> ChopLine(std::string str)
    {
        char buffer[4096];
        ::strcpy(buffer, str.c_str());
        char *ptr = buffer;
        std::string key;
        std::string value;
        while (*ptr != 0 && *ptr != ':')
        {
            ptr++;
        }
        if (*ptr == 0)
        {
            return std::pair<std::string, std::string>{"", ""};
        }
        *ptr++ = 0;
        key = std::string(buffer);
        value = std::string(ptr);
        return std::pair<std::string, std::string>{key, value};
    };
    static void printStringWithEscapes(const std::string &input)
    {
        for (char c : input)
        {
            switch (c)
            {
            case '\n':
                std::cout << "\\n";
                break;
            case '\r':
                std::cout << "\\r";
                break;
            case '\t':
                std::cout << "\\t";
                break;
            case '\\':
                std::cout << "\\\\";
                break;
            // Add more cases for other escape sequences as needed
            default:
                std::cout << c;
            }
        }
        std::cout << std::endl;
    }
};
