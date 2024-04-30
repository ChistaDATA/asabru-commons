#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <any>
#include "ComputationContext.h"

const std::string AUTHORIZATION_AUTHORIZED_KEY = "authorized";
const std::string AUTHORIZATION_STRATEGY_KEY = "authorization";
const std::string AUTHORIZATION_DATA_KEY = "authorization_data";

// Abstract base class for all authorization strategies
class AuthorizationStrategy {
public:
    virtual void isAuthorized(ComputationContext *context) = 0;
    virtual ~AuthorizationStrategy() = default;
};
