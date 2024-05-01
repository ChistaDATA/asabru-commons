#pragma once
#include "AuthorizationStrategy.h"
#include "Operation.h"
#include "Principal.h"
#include "Resource.h"
#include "ResourceAccessPredicate.h"
#include "afcas/AfcasClient.h"
#include "logger/Logger.h"
#include "tinyxml2.h"
#include <any>
#include <vector>
#include "authentication/AuthenticationStrategy.h"

const std::string AUTHORIZATION_PRINCIPALS_KEY = "authorizationPrincipals";
const std::string AUTHORIZATION_RESOURCES_KEY = "authorizationResources";
const std::string AUTHORIZATION_OPERATIONS_KEY = "authorizationOperations";
const std::string AUTHORIZATION_ACCESS_PREDICATES_KEY = "authorizationAccessPredicates";

typedef struct {
	std::string resourceId;
	std::string operationId;
	std::string principalName;
} Predicate;

class BaseAuthorizationStrategy: public AuthorizationStrategy {
  public:
	std::vector<Principal> principals;
	std::vector<Resource> resources;
	std::vector<Operation> operations;
	std::vector<ResourceAccessPredicate> accessPredicates;
	void parseInitData(ComputationContext *context) {
		auto data = std::any_cast<std::string>(context->Get(AUTHORIZATION_DATA_KEY));
		tinyxml2::XMLDocument doc;
		auto xmlError = doc.Parse(data.c_str());
		if (xmlError != tinyxml2::XML_SUCCESS) {
			auto error = "Error parsing afcas data: " + std::to_string(xmlError);
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		tinyxml2::XMLElement *rootElement = doc.FirstChildElement("data");
		if (!rootElement) {
			auto error = "Error parsing afcas data: root element not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}
		tinyxml2::XMLElement *principalsElement = rootElement->FirstChildElement("principals");
		if (principalsElement) {
			tinyxml2::XMLElement *principalElement = principalsElement->FirstChildElement("principal");
			while (principalElement) {
				Principal p(principalElement->FirstChildElement("Name")->GetText(),
							principalElement->FirstChildElement("Email")->GetText(),
							principalElement->FirstChildElement("DisplayName")->GetText(),
							principalElement->FirstChildElement("Description")->GetText(),
							principalElement->FirstChildElement("DataSource")->GetText(),
							principalElement->FirstChildElement("PrincipalType")->GetText());
				principals.push_back(p);
				principalElement = principalElement->NextSiblingElement("principal");
			}
		} else {
			auto error = "Error parsing afcas data: principals element not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		tinyxml2::XMLElement *operationsElement = rootElement->FirstChildElement("operations");
		if (operationsElement) {
			tinyxml2::XMLElement *operationElement = operationsElement->FirstChildElement("operation");
			while (operationElement) {
				Operation o(operationElement->FirstChildElement("Id")->GetText(),
							operationElement->FirstChildElement("Name")->GetText(),
							operationElement->FirstChildElement("Description")->GetText());

				operations.push_back(o);
				operationElement = operationElement->NextSiblingElement("operation");
			}
		} else {
			auto error = "Error parsing afcas data: operations element not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		tinyxml2::XMLElement *resourcesElement = rootElement->FirstChildElement("resources");
		if (resourcesElement) {
			tinyxml2::XMLElement *resourceElement = resourcesElement->FirstChildElement("resource");
			while (resourceElement) {
				Resource r(resourceElement->FirstChildElement("Id")->GetText(),
						   resourceElement->FirstChildElement("Name")->GetText());

				resources.push_back(r);
				resourceElement = resourceElement->NextSiblingElement("resource");
			}
		} else {
			auto error = "Error parsing afcas data: resources element not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		tinyxml2::XMLElement *predicatesElement = rootElement->FirstChildElement("predicates");
		if (predicatesElement) {
			tinyxml2::XMLElement *predicateElement = predicatesElement->FirstChildElement("predicate");
			while (predicateElement) {
				ResourceAccessPredicate p(predicateElement->FirstChildElement("principalName")->GetText(),
										  predicateElement->FirstChildElement("operationId")->GetText(),
										  predicateElement->FirstChildElement("resourceId")->GetText(),
										  predicateElement->FirstChildElement("predicateType")->GetText());

				accessPredicates.push_back(p);
				predicateElement = predicateElement->NextSiblingElement("predicate");
			}
		} else {
			auto error = "Error parsing afcas data: predicates element not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		LOG_INFO("data parsed successfully");
	}

	std::vector<Predicate> parseAfcasAuthorizationData(ComputationContext *context) {
		auto data = std::any_cast<std::string>(context->Get(AUTHORIZATION_DATA_KEY));
		auto username = std::any_cast<std::string>(context->Get(AUTH_AUTHENTICATED_INFO));

		LOG_INFO("username is " + username);

		tinyxml2::XMLDocument doc;
		auto xmlError = doc.Parse(data.c_str());
		if (xmlError != tinyxml2::XML_SUCCESS) {
			auto error = "Error parsing afcas data: " + std::to_string(xmlError);
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		tinyxml2::XMLElement *rootElement = doc.FirstChildElement("authorization");
		if (!rootElement) {
			auto error = "Error parsing afcas data: root element authorization not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		tinyxml2::XMLElement *predicatesElement = rootElement->FirstChildElement("predicates");
		if (!predicatesElement) {
			auto error = "Error parsing afcas data: predicates element not found";
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}

		std::vector<Predicate> predicates;
		tinyxml2::XMLElement *predicateElement = predicatesElement->FirstChildElement("predicate");
		while (predicateElement) {
			Predicate p;
			p.resourceId = predicateElement->FirstChildElement("resourceId")->GetText();
			p.operationId = predicateElement->FirstChildElement("operationId")->GetText();
			p.principalName = username;
			predicates.push_back(p);
			predicateElement = predicateElement->NextSiblingElement("predicate");
		}

		return predicates;
	}
};