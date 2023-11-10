#pragma once

/**
 * The wrapper / bag class that is used to pass 
 * parameters to the commands
*/
#include <iostream>
#include <unordered_map>
#include <string>
#include <any>

class ComputationContext {

public:
    void Put(const std::string& k, std::any value);
    std::any Get(const std::string& k);

    std::unordered_map<std::string, std::any> symbols;
};
