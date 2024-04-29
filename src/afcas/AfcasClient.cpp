#include "AfcasClient.h"
#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "../logger/Logger.h"


// call the Afacas API to add a principal // POST /authorization-management/principals
// {
//   "name": "string",
//   "email": "string",
//   "displayName": "string",
//   "description": "string",
//   "dataSource": "string",
//   "principalType": "User"
// }
void AfcasClient::addPrincipal(const Principal& principal) {
    nlohmann::json json = {
        {"name", principal.getName()},
        {"email", principal.getEmail()},
        {"displayName", principal.getDisplayName()},
        {"description", principal.getDescription()},
        {"dataSource", principal.getDataSource()},
        {"principalType", principal.getPrincipalType()}
    };

    cpr::Response r = cpr::Post(cpr::Url{this->host + ":" + this->port + "/authorization-management/principals"},
                                cpr::Body{json.dump()},
                                cpr::Header{{"Content-Type", "application/json"}});
    if (r.status_code != 200) {
        LOG_ERROR("Failed to add principal");
        throw std::runtime_error("Failed to add principal");
    }
}

// call the Afacas API to add a resource // POST /authorization-management/resources
//   "id": "string",
//   "name": "string"
// }
void AfcasClient::addResource(const Resource& resource) {
    nlohmann::json json = {
        {"id", resource.getId()},
        {"name", resource.getName()}
    };

    cpr::Response r = cpr::Post(cpr::Url{this->host + ":" + this->port + "/authorization-management/resources"},
                                cpr::Body{json.dump()},
                                cpr::Header{{"Content-Type", "application/json"}});
    if (r.status_code != 200) {
        throw std::runtime_error("Failed to add resource");
    }
}

// call the Afacas API to add an operation // POST /authorization-management/operations
// {
//   "id": "string",
//   "name": "string",
//   "description": "string"
// }
void AfcasClient::addOperation(const Operation& operation) {
    nlohmann::json json = {
        {"id", operation.getId()},
        {"name", operation.getName()},
        {"description", operation.getDescription()}
    };

    cpr::Response r = cpr::Post(cpr::Url{this->host + ":" + this->port + "/authorization-management/operations"},
                                cpr::Body{json.dump()},
                                cpr::Header{{"Content-Type", "application/json"}});
    if (r.status_code != 200) {
        throw std::runtime_error("Failed to add operation");
    }
}

// call the Afacas API to add a resource access predicate // POST /authorization-management/permissions
// {
//   "principalId": "string",
//   "operationId": "string",
//   "resourceId": "string",
//   "predicateType": "Grant"
// }
void AfcasClient::addResourceAccessPredicate(const ResourceAccessPredicate& resourceAccessPredicate) {
    nlohmann::json json = {
        {"principalId", resourceAccessPredicate.getPrincipalName()},
        {"operationId", resourceAccessPredicate.getOperationId()},
        {"resourceId", resourceAccessPredicate.getResourceId()},
        {"predicateType", resourceAccessPredicate.getPredicateType()}
    };

    cpr::Response r = cpr::Post(cpr::Url{this->host + ":" + this->port + "/authorization-management/permissions"},
                                cpr::Body{json.dump()},
                                cpr::Header{{"Content-Type", "application/json"}});

    if (r.status_code != 200) {
        if (r.status_code == 500) {
            nlohmann::json responseBody = nlohmann::json::parse(r.text);
            if (responseBody["message"].get<std::string>().find("duplicate key value violates unique constraint") != std::string::npos) {
                return;
            }
        }

        throw std::runtime_error("Failed to add resource access predicate");
    }
}

// call the Afacas API to get all principals // GET /authorization-provider/is-authorized
// principalId *
// string
// (query)
// principalId
// operationId *
// string
// (query)
// operationId
// resourceId *
// string
// (query)
// resourceId
bool AfcasClient::isAuthorized(const std::string& principalName,
                                const std::string& operationId,
                                const std::string& resourceId) {
    cpr::Response r = cpr::Get(cpr::Url{this->host + ":" + this->port + "/authorization-provider/is-authorized"},
                                cpr::Parameters{{"principalId", principalName},
                                                {"operationId", operationId},
                                                {"resourceId", resourceId}});
    if (r.status_code != 200) {
        throw std::runtime_error("Failed to check authorization");
    }

    nlohmann::json responseBody = nlohmann::json::parse(r.text);
    return responseBody["data"].get<bool>();
}

void AfcasClient::runMigration() {
    for (auto principal : this->principals) {
        this->addPrincipal(principal);
    }

    for (auto resource : this->resources) {
        this->addResource(resource);
    }

    for (auto operation : this->operations) {
        this->addOperation(operation);
    }

    for (auto resourceAccessPredicate : this->resourceAccessPredicates) {
        this->addResourceAccessPredicate(resourceAccessPredicate);
    }
}