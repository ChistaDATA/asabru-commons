#pragma once

#include "AuthenticationStrategy.h"
#include "BasicAuthenticationStrategy.h"

// Class that manages the creation of the authentication strategy
class AuthenticationFactory {
public:
    static AuthenticationStrategy *createAuthenticationStrategy(std::string type) {
        if (type == "basic") {
            return new BasicAuthenticationStrategy();
        } else {
            auto error = "Unknown authentication strategy type: " + type;
            LOG_ERROR(error);
            throw std::runtime_error(error);
        }
        return nullptr;
    }
};