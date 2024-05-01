#ifndef AFCAS_CLIENT_H
#define AFCAS_CLIENT_H

#include <string>
#include "authorization/Principal.h"
#include "authorization/Resource.h"
#include "authorization/Operation.h"
#include "authorization/ResourceAccessPredicate.h"
#include <vector>
#include <map>

class AfcasClient {
public:
    // Constructor using member initializer list
    AfcasClient(const std::string& host,
                 const std::string& port,
                std::vector<Principal> principals,
                std::vector<Resource> resources,
                std::vector<Operation> operations,
                std::vector<ResourceAccessPredicate> resourceAccessPredicates)
        : host(host), port(port), principals(principals),
          resources(resources), operations(operations),
          resourceAccessPredicates(resourceAccessPredicates) {
            runMigration();
          }
    
    void addPrincipal(const Principal& principal);
    void addResource(const Resource& resource);
    void addOperation(const Operation& operation);
    void addResourceAccessPredicate(const ResourceAccessPredicate& resourceAccessPredicate);
    bool isAuthorized(const std::string& principalName,
                                      const std::string& operationId,
                                      const std::string& resourceId);
private:
    std::string host;
    std::string port;
    std::vector<Principal> principals;
    std::vector<Resource> resources;
    std::vector<Operation> operations;
    std::vector<ResourceAccessPredicate> resourceAccessPredicates;
    void runMigration();
};

#endif // AFCAS_CLIENT_H