#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <any>
#include "ComputationContext.h"

// Abstract base class for all authentication strategies
class AuthenticationStrategy {
public:
    virtual void authenticate(ComputationContext *context) const = 0;
    virtual void isAuthenticated(ComputationContext *context) = 0; 
    virtual void removeAuthentication(ComputationContext *context) = 0;
    virtual ~AuthenticationStrategy() = default;
};
