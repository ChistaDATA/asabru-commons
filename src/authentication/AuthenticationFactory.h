#pragma once

#include "AuthenticationStrategy.h"
#include "BasicAuthenticationStrategy.h"

// Class that manages the creation of the authentication strategy
class AuthenticationFactory {
public:
    static AuthenticationStrategy *createAuthenticationStrategy(std::string type) {
        if (type == "basic") {
            return new BasicAuthenticationStrategy();
        }
        return nullptr;
    }
};