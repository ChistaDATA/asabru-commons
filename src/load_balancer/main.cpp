/**
 * Load balancing using strategy pattern
 * Build : g++ -o main.exe main.cpp -std=c++17
 * Run: ./main.exe
*/
#include "LoadBalancer.h"
#include "load_balancer/RoundRobinStrategy.h"
#include "load_balancer/RandomStrategy.h"
#include "load_balancer/WeightedRoundRobinStrategy.h"

class WeightedServer : public WeightedItem {
public:
    int id;
    WeightedServer(int id, int weight) : id(id), WeightedItem(weight) {}
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    // Create load balancer with round-robin strategy
    LoadBalancer<int> roundRobinBalancer(new RoundRobinStrategy<int>());

    // Add servers to the load balancer
    roundRobinBalancer.addServer(1);
    roundRobinBalancer.addServer(2);
    roundRobinBalancer.addServer(3);

    // Simulate incoming requests
    for (int i = 0; i < 10; ++i) {
        int server = roundRobinBalancer.requestServer();
        std::cout << "Request sent to server " << server << std::endl;
    }

    std::cout << std::endl;


    // Create load balancer with round-robin strategy
    LoadBalancer<WeightedServer> weightedRoundRobinBalancer(new WeightedRoundRobinStrategy<WeightedServer>());

    // Add servers to the load balancer
    weightedRoundRobinBalancer.addServer(WeightedServer(1, 3));
    weightedRoundRobinBalancer.addServer(WeightedServer(2, 2));
    weightedRoundRobinBalancer.addServer(WeightedServer(3, 1));

    // Simulate incoming requests
    for (int i = 0; i < 10; ++i) {
        WeightedServer server = weightedRoundRobinBalancer.requestServer();
        std::cout << "Request sent to server " << server.id << std::endl;
    }

    std::cout << std::endl;

    // Create load balancer with random strategy
    LoadBalancer<int> randomBalancer(new RandomStrategy<int>());

    // Add servers to the load balancer
    randomBalancer.addServer(1);
    randomBalancer.addServer(2);
    randomBalancer.addServer(3);

    // Simulate incoming requests
    for (int i = 0; i < 10; ++i) {
        int server = randomBalancer.requestServer();
        std::cout << "Request sent to server " << server << std::endl;
    }

    return 0;
}