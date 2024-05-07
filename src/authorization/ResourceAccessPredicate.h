#ifndef RESOURCE_ACCESS_PREDICATE_H
#define RESOURCE_ACCESS_PREDICATE_H

#include <string>

class ResourceAccessPredicate {
public:
    // Constructor using member initializer list
    ResourceAccessPredicate(const std::string& principalName,
                             const std::string& operationId,
                             const std::string& resourceId,
                             const std::string& predicateType)
        : principalName(principalName), operationId(operationId),
          resourceId(resourceId), predicateType(predicateType) {}

    // Getters
    std::string getPrincipalName() const { return principalName; }
    std::string getOperationId() const { return operationId; }
    std::string getResourceId() const { return resourceId; }
    std::string getPredicateType() const { return predicateType; }

private:
    std::string principalName;
    std::string operationId;
    std::string resourceId;
    std::string predicateType;
};

#endif // RESOURCE_ACCESS_PREDICATE_H
