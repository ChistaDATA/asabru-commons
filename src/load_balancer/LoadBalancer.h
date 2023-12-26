#pragma once
#include "LoadBalancerStrategy.h"

// Context class that uses the load balancing strategy
template <class T>
class LoadBalancer {
private:
    LoadBalancingStrategy<T>* strategy;
    std::vector<T> servers;

public:
    explicit LoadBalancer<T>(LoadBalancingStrategy<T>* strategy) : strategy(strategy) {}

    void addServer(T server) {
        servers.push_back(server);
    }

    T requestServer() {
        return strategy->selectServer(servers);
    }
};
