#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <string>

class Principal {
public:
    // Constructor using member initializer list
    Principal(const std::string& name,
              const std::string& email,
              const std::string& displayName,
              const std::string& description,
              const std::string& dataSource,
              const std::string& principalType)
        : name(name), email(email), displayName(displayName),
          description(description), dataSource(dataSource),
          principalType(principalType) {}

    Principal(const std::string& objectId,
              const std::string& name,
              const std::string& email,
              const std::string& displayName,
              const std::string& description,
              const std::string& dataSource,
              const std::string& principalType)
        : objectId(objectId), name(name), email(email),
          displayName(displayName), description(description),
          dataSource(dataSource), principalType(principalType) {}

    // Getters
    
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getDisplayName() const { return displayName; }
    std::string getDescription() const { return description; }
    std::string getDataSource() const { return dataSource; }
    std::string getPrincipalType() const { return principalType; }

private:
    std::string objectId;
    std::string name;
    std::string email;
    std::string displayName;
    std::string description;
    std::string dataSource;
    std::string principalType;
};

#endif // PRINCIPAL_H
