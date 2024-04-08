/**
 * The wrapper / bag class that is used to pass
 * parameters to the commands
 */
#include <unordered_map>
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
        // Return a default-constructed std::any object if the key is not found.
        return std::any();
    }
};

std::string ComputationContext::GetString(const std::string& k) {
  std::any value_any = Get(k);
  if (!value_any.has_value()) {
    std::cout << "Value for key \"" << k << "\" not found." << std::endl;
    return {};
  }

  try {
    return std::any_cast<std::string>(value_any);
  } catch (const std::bad_any_cast&) {
    std::cout << "Value for key \"" << k << "\" not a string." << std::endl;
    return {};
  }
}

