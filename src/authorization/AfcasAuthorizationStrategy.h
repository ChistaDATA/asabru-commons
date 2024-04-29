#pragma once
#include "AuthorizationStrategy.h"
#include "afcas/AfcasClient.h"
#include "afcas/Operation.h"
#include "afcas/Principal.h"
#include "afcas/Resource.h"
#include "afcas/ResourceAccessPredicate.h"
#include "logger/Logger.h"
#include "tinyxml2.h"
#include <any>
#include <vector>

const std::string AFCAS_PRINCIPAL_NAME_KEY = "afcasPrincipalName";
const std::string AFCAS_OPERATION_ID_KEY = "afcasOperationId";
const std::string AFCAS_RESOURCE_ID_KEY = "afcasResourceId";
const std::string AUTHORIZATION_AFCAS_PRINCIPALS_KEY = "afcasPrincipals";
const  std::string AUTHORIZATION_AFCAS_RESOURCES_KEY = "afcasResources";
const  std::string AUTHORIZATION_AFCAS_OPERATIONS_KEY = "afcasOperations";
const  std::string AUTHORIZATION_AFCAS_ACCESS_PREDICATES_KEY = "afcasAccessPredicates";

// Concrete implementation of afcas Authorization strategy
class AfcasAuthorizationStrategy : public AuthorizationStrategy {
  private:
	AfcasClient afcasClient;
	static bool parseAfcasData(ComputationContext *context) {
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
				std::vector<Principal> principals;
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
				context->Put(AUTHORIZATION_AFCAS_PRINCIPALS_KEY, principals);
			} else {
				auto error = "Error parsing afcas data: principals element not found";
				LOG_ERROR(error);
				throw std::runtime_error(error);
			}

			tinyxml2::XMLElement *operationsElement = rootElement->FirstChildElement("operations");
			if (operationsElement) {
				std::vector<Operation> operations;
				tinyxml2::XMLElement *operationElement = operationsElement->FirstChildElement("operation");
				while (operationElement) {
					Operation o(operationElement->FirstChildElement("Id")->GetText(),
								operationElement->FirstChildElement("Name")->GetText(),
								operationElement->FirstChildElement("Description")->GetText());

					operations.push_back(o);
					operationElement = operationElement->NextSiblingElement("operation");
				}
				context->Put(AUTHORIZATION_AFCAS_OPERATIONS_KEY, operations);
			} else {
				auto error = "Error parsing afcas data: operations element not found";
				LOG_ERROR(error);
				throw std::runtime_error(error);
			}

			tinyxml2::XMLElement *resourcesElement = rootElement->FirstChildElement("resources");
			if (resourcesElement) {
				std::vector<Resource> resources;
				tinyxml2::XMLElement *resourceElement = resourcesElement->FirstChildElement("resource");
				while (resourceElement) {
					Resource r(resourceElement->FirstChildElement("Id")->GetText(),
							   resourceElement->FirstChildElement("Name")->GetText());

					resources.push_back(r);
					resourceElement = resourceElement->NextSiblingElement("resource");
				}
				context->Put(AUTHORIZATION_AFCAS_RESOURCES_KEY, resources);
			} else {
				auto error = "Error parsing afcas data: resources element not found";
				LOG_ERROR(error);
				throw std::runtime_error(error);
			}

			tinyxml2::XMLElement *predicatesElement = rootElement->FirstChildElement("predicates");
			if (predicatesElement) {
				std::vector<ResourceAccessPredicate> accessPredicates;
				tinyxml2::XMLElement *predicateElement = predicatesElement->FirstChildElement("predicate");
				while (predicateElement) {
					ResourceAccessPredicate p(predicateElement->FirstChildElement("principalName")->GetText(),
											  predicateElement->FirstChildElement("operationId")->GetText(),
                        predicateElement->FirstChildElement("resourceId")->GetText(),
											  predicateElement->FirstChildElement("predicateType")->GetText());

					accessPredicates.push_back(p);
					predicateElement = predicateElement->NextSiblingElement("predicate");
				}
				context->Put(AUTHORIZATION_AFCAS_ACCESS_PREDICATES_KEY, accessPredicates);
			} else {
				auto error = "Error parsing afcas data: predicates element not found";
				LOG_ERROR(error);
				throw std::runtime_error(error);
			}

    LOG_INFO("AFCAS data parsed successfully");
		return true;
	}

  public:
	AfcasAuthorizationStrategy(ComputationContext *context)
		: afcasClient(
			  std::getenv("AFCAS_HOST") ? std::getenv("AFCAS_HOST")
										: throw std::runtime_error("AFCAS_HOST environment variable not set"),
			  std::getenv("AFCAS_PORT") ? std::getenv("AFCAS_PORT")
										: throw std::runtime_error("AFCAS_PORT environment variable not set"),
			  parseAfcasData(context) ? std::any_cast<std::vector<Principal>>(context->Get(AUTHORIZATION_AFCAS_PRINCIPALS_KEY))
									  : throw std::runtime_error("Error parsing afcas data"),
			  std::any_cast<std::vector<Resource>>(context->Get(AUTHORIZATION_AFCAS_RESOURCES_KEY)),
			  std::any_cast<std::vector<Operation>>(context->Get(AUTHORIZATION_AFCAS_OPERATIONS_KEY)),
			  std::any_cast<std::vector<ResourceAccessPredicate>>(context->Get(AUTHORIZATION_AFCAS_ACCESS_PREDICATES_KEY))) {
        }

	~AfcasAuthorizationStrategy() {}

	void isAuthorized(ComputationContext *context) override {
		auto principalName = std::any_cast<std::string>(context->Get(AFCAS_PRINCIPAL_NAME_KEY));
		auto operationId = std::any_cast<std::string>(context->Get(AFCAS_OPERATION_ID_KEY));
		auto resourceId = std::any_cast<std::string>(context->Get(AFCAS_RESOURCE_ID_KEY));
		context->Put(AUTHORIZATION_AUTHORIZED_KEY, afcasClient.isAuthorized(principalName, operationId, resourceId));
	}
};