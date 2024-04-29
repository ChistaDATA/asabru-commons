#pragma once

#include "AuthenticationStrategy.h"
#include "BasicAuthenticationStrategy.h"

const std::string AUTH_BASIC = "basic";

// Class that manages the creation of the authentication strategy
class AuthenticationFactory {
public:
    static AuthenticationStrategy *createAuthenticationStrategy(std::string type) {
        if (type == AUTH_BASIC) {
            return new BasicAuthenticationStrategy();
        } else {
            auto error = "Unknown authentication strategy type: " + type;
            LOG_ERROR(error);
            throw std::runtime_error(error);
        }
        return nullptr;
    }
};