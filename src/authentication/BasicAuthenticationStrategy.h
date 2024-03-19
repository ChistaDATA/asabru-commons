#pragma once
#include <any>
#include <unordered_set>
#include <shared_mutex>
#include "AuthenticationStrategy.h"
#include "jwt-cpp/jwt.h"

// Concrete implementation of a basic authentication strategy using JSON Web Tokens
class BasicAuthenticationStrategy : public AuthenticationStrategy
{
private:
    std::string secret;
    std::string username;
    std::string password;
    mutable std::unordered_set<std::string> blacklistedTokens;
    mutable std::shared_mutex mutex; // Shared mutex for protecting the blacklist
    const std::string jwt_issuer = "auth0";
    const std::string jwt_type = "JWS";

public:
    BasicAuthenticationStrategy() : secret(std::getenv("AUTH_BASIC_SECRET") ? std::getenv("AUTH_BASIC_SECRET") : throw std::runtime_error("AUTH_BASIC_SECRET environment variable not set")),
                                    username(std::getenv("AUTH_BASIC_USERNAME") ? std::getenv("AUTH_BASIC_USERNAME") : throw std::runtime_error("AUTH_BASIC_USERNAME environment variable not set")),
                                    password(std::getenv("AUTH_BASIC_PASSWORD") ? std::getenv("AUTH_BASIC_PASSWORD") : throw std::runtime_error("AUTH_BASIC_PASSWORD environment variable not set"))
    {
    }
    
    void authenticate(ComputationContext *context) const override
    {
        const std::string username = std::any_cast<std::string>(context->Get("username"));
        const std::string password = std::any_cast<std::string>(context->Get("password"));

        if (username == this->username && password == this->password)
        {
            auto token = jwt::create()
                             .set_issuer(jwt_issuer)
                             .set_type(jwt_type)
                             .set_payload_claim("username", jwt::claim(std::string(username)))
                             .sign(jwt::algorithm::hs256{secret});
            context->Put("token", token);
            context->Put("authenticated", true);
        }
        else
        {
            context->Put("authenticated", false);
        }
    }

    void isAuthenticated(ComputationContext *context) override
    {
        const std::string token = std::any_cast<std::string>(context->Get("token"));
        
        // Use shared_lock for read operations
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (blacklistedTokens.find(token) != blacklistedTokens.end())
        {
            context->Put("authenticated", false);
            return;
        }
        lock.unlock(); // Unlock
        
        try
        {
            jwt::decoded_jwt decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{secret})
                                .with_issuer(jwt_issuer);
            verifier.verify(decoded);
            context->Put("authenticated", true);
        }
        catch (const std::exception &e)
        {
            context->Put("authenticated", false);
        }
    }

    void removeAuthentication(ComputationContext *context) override
    {
        const std::string &token = std::any_cast<std::string>(context->Get("token"));
        
        // Use unique_lock for write operations
        std::unique_lock<std::shared_mutex> lock(mutex);
        blacklistedTokens.insert(token);
    } // lock released here
};
