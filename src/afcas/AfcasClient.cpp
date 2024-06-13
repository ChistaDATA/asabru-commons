#include "AfcasClient.h"
#include "CommandDispatcher.h"
#include "http/http_message.h"
#include "nlohmann/json.hpp"

struct HttpResponse {
    int status_code;
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
	CommandDispatcher::Dispatch("HTTPRequestCommand", &context);

	// Retrieve and validate response
    int status_code = std::any_cast<int>(context.Get("status_code"));
	std::string response = context.GetString("response");
	std::string response_content = context.GetString("response_content");
	return { status_code, response, response_content };
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

	HttpResponse response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/principals",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
	);

    if (response.status_code != 200) {
        throw std::runtime_error("Failed to add principal");
    }
}

// call the Afacas API to add a group // POST /authorization-management/principals/group-members
// {
//   "groupName": "string",
//   "memberName": "string"
// }

void AfcasClient::addGroup(const Group& group) {
    for (auto member : group.getMembers()) {
        nlohmann::json json = {
            {"groupName", group.getGroupName()},
            {"memberName", member}
        };

        HttpResponse response = sendRequest(
            this->host,
            this->port,
            "/authorization-management/principals/group-members",
            simple_http_server::HttpMethod::POST,
            json.dump(),
            "application/json"
        );

        if (response.status_code != 200) {
            throw std::runtime_error("Failed to add group");
        }
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

	HttpResponse response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/resources",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
		);

    if (response.status_code != 200) {
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

	HttpResponse response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/operations",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
	);

    if (response.status_code != 200) {
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

	HttpResponse response = sendRequest(
		this->host,
		this->port,
		"/authorization-management/permissions",
		simple_http_server::HttpMethod::POST,
		json.dump(),
		"application/json"
	);

    if (response.status_code == 500 && response.response_content.find("already exists") != std::string::npos) {
        return;
    }

    if (response.status_code != 200) {
        throw std::runtime_error("Failed to add resource access predicate");
    }
}

nlohmann::json processResponse(const std::string& response) {
  // Find the start and end indices of the JSON data
  size_t start_index = response.find("{");
  size_t end_index = response.rfind("}");

  // Check if valid JSON content is found
  if (start_index == std::string::npos || end_index == std::string::npos) {
    throw std::invalid_argument("Invalid JSON format");
  }

  // Extract the JSON data
  std::string json_string = response.substr(start_index, end_index - start_index + 1);

  // Parse the JSON data
  try {
    return nlohmann::json::parse(json_string);
  } catch (const nlohmann::detail::exception& e) {
    throw std::invalid_argument(std::string("Error parsing JSON: ") + e.what());
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

    if (res.status_code != 200) {
        throw std::runtime_error("Failed to check authorization");
    }

    nlohmann::json responseBody = processResponse(res.response_content);
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