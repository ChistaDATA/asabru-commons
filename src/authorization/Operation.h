#ifndef OPERATION_H
#define OPERATION_H

#include <string>

class Operation {
public:
    // Constructor using member initializer list
    Operation(const std::string& id,
              const std::string& name,
              const std::string& description)
        : id(id), name(name), description(description) {}

    // Getters
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }

private:
    std::string id;
    std::string name;
    std::string description;
};

#endif // OPERATION_H
