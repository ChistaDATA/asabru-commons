/**
 * The wrapper / bag class that is used to pass
 * parameters to the commands
 */
#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include "ComputationContext.h"

void ComputationContext::Put(const std::string &k, std::any value)
{
    symbols[k] = value;
};

std::any ComputationContext::Get(const std::string &k)
{
    auto it = symbols.find(k);
    if (it != symbols.end())
    {
        return it->second;
    }
    else
    {
        // Handle the case when the key is not found, e.g., return a default value or throw an exception.
        // Here, we assume Object is a placeholder type; replace it with the actual type you are using.
        return 0; // Return a default-constructed Object.
    }
};
