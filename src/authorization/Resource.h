#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

class Resource {
public:
    // Constructor using member initializer list
    Resource(const std::string& id,
             const std::string& name)
        : id(id), name(name) {}

    // Getters
    std::string getId() const { return id; }
    std::string getName() const { return name; }

private:
    std::string id;
    std::string name;
};

#endif // RESOURCE_H
