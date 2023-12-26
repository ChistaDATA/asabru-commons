#pragma once
#include "LoadBalancerStrategy.h"

// Concrete implementation of a random load balancing strategy
template <class T>
class RandomStrategy : public LoadBalancingStrategy<T> {
public:
    int selectServer(const std::vector<int>& servers) const override {
        return servers[rand() % servers.size()];
    }
};
