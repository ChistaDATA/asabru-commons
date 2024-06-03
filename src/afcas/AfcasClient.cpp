#include "AfcasClient.h"
#include "CommandDispatcher.h"
#include "http/http_message.h"
#include "nlohmann/json.hpp"

struct HttpResponse {
	std::string response;
	std::string response_content;
};
HttpResponse sendRequest(const std::string& host,
						const std::string& port,
						const std::string& requestUri,
						simple_http_server::HttpMethod method,
						const std::string& content,
						const std::string& contentType) {
	// Resolve the Handler class
	CommandDispatcher::RegisterCommand<BaseComputationCommand>(
		"HTTPRequestCommand");
	ComputationContext context;

	// Set up request parameters
	context.Put("host", host);
	context.Put("port", std::stoi(port));
	context.Put("method", method);
	context.Put("requestUri", requestUri);

	// Construct JSON payload
	context.Put("content", content);
	context.Put("content-type", contentType);

	// Dispatch HTTP request command
	if (!CommandDispatcher::Dispatch("HTTPRequestCommand", &context)) {
		LOG_ERROR("Failed to dispatch HTTP request command.");
		return {""};
	}

	// Retrieve and validate response
	std::string response = context.GetString("response");
	std::string response_content = context.GetString("response_content");
	return { response, response_content };
}
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

	std::string response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/principals",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
	).response;

    if (response.empty()) {
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

	std::string response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/resources",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
		).response;
    if (response.empty()) {
		LOG_ERROR("Failed to add resource");
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

	std::string response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/operations",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
	).response;
	if (response.empty()) {
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

	std::string response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/permissions",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
	).response;
	if (response.empty()) {
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
	HttpResponse res = sendRequest(
		this->host,
		this->port,
		"/authorization-provider/is-authorized?principalId=" + principalName + "&operationId=" + operationId + "&resourceId=" + resourceId,
		simple_http_server::HttpMethod::GET,
		"",
		"application/json"
	);
	if (res.response.empty()) {
        throw std::runtime_error("Failed to check authorization");
    }

    nlohmann::json responseBody = nlohmann::json::parse(res.response_content);
    return responseBody["data"].get<bool>();
}

void AfcasClient::runMigration() {
    for (const auto& principal : this->principals) {
        this->addPrincipal(principal);
    }

    for (const auto& resource : this->resources) {
        this->addResource(resource);
    }

    for (const auto& operation : this->operations) {
        this->addOperation(operation);
    }

    for (const auto& resourceAccessPredicate : this->resourceAccessPredicates) {
        this->addResourceAccessPredicate(resourceAccessPredicate);
    }
}