#pragma once
#include <any>
#include "AuthenticationStrategy.h"
#include "CachedFileList.h"
#include "jwt-cpp/jwt.h"

// Concrete imlementation of a basic authentication strategy using json web tokens
class BasicAuthenticationStrategy : public AuthenticationStrategy
{
private:
    std::string secret;
    std::string username;
    std::string password;
    CachedFileList blacklistedTokens;
    const std::string jwt_issuer = "auth0";
    const std::string jwt_type = "JWS";
    int expiration;
    std::thread compactThread;
    bool stopThread = false;

    void compactThreadFunction() {
        while (!stopThread) {
            std::this_thread::sleep_for(std::chrono::hours(1));
            LOG_INFO("Compacting blacklisted tokens file");
            auto allTokens = blacklistedTokens.getAll();
            auto verifiedTokens = jwt::verify()
                                      .allow_algorithm(jwt::algorithm::hs256{secret})
                                      .with_issuer(jwt_issuer);
            
            for (const auto &token : allTokens) {
                try {
                    jwt::decoded_jwt decoded = jwt::decode(token);
                    verifiedTokens.verify(decoded);
                } catch (const std::exception &e) {
                    blacklistedTokens.remove(token);
                }
            }

            blacklistedTokens.compactFile();
        }
    }


public:
    BasicAuthenticationStrategy() : secret(std::getenv("AUTH_BASIC_SECRET") ? std::getenv("AUTH_BASIC_SECRET") : throw std::runtime_error("AUTH_BASIC_SECRET environment variable not set")),
                                    username(std::getenv("AUTH_BASIC_USERNAME") ? std::getenv("AUTH_BASIC_USERNAME") : throw std::runtime_error("AUTH_BASIC_USERNAME environment variable not set")),
                                    password(std::getenv("AUTH_BASIC_PASSWORD") ? std::getenv("AUTH_BASIC_PASSWORD") : throw std::runtime_error("AUTH_BASIC_PASSWORD environment variable not set")),
                                    expiration(std::getenv("AUTH_BASIC_EXPIRATION") ? std::stoi(std::getenv("AUTH_BASIC_EXPIRATION")) : 3600),
                                    blacklistedTokens(std::getenv("AUTH_BASIC_BLACKLIST_FILE_PATH") ? std::getenv("AUTH_BASIC_BLACKLIST_FILE_PATH") : throw std::runtime_error("AUTH_BASIC_BLACKLIST_FILE_PATH environment variable not set"))
    {
        compactThread = std::thread(&BasicAuthenticationStrategy::compactThreadFunction, this);
    }

    ~BasicAuthenticationStrategy() {
        stopThread = true;
        if (compactThread.joinable()) {
            compactThread.join();
        }
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
                             .set_issued_at(std::chrono::system_clock::now())
                             .set_expires_in(std::chrono::seconds{expiration})
                             .set_not_before(std::chrono::system_clock::now())
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
        if (blacklistedTokens.find(token))
        {
            context->Put("authenticated", false);
            return;
        }
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
        blacklistedTokens.add(token);
    }
};