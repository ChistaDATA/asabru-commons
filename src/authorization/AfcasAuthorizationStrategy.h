#pragma once
#include "AuthorizationStrategy.h"
#include "afcas/AfcasClient.h"
#include "Operation.h"
#include "Principal.h"
#include "Resource.h"
#include "ResourceAccessPredicate.h"
#include "logger/Logger.h"
#include "tinyxml2.h"
#include <any>
#include <vector>
#include "BaseAuthorizationStrategy.h"

// Concrete implementation of afcas Authorization strategy
class AfcasAuthorizationStrategy : public BaseAuthorizationStrategy {
  private:
	AfcasClient afcasClient;

  public:
	AfcasAuthorizationStrategy(ComputationContext *context)
		: afcasClient(
			  std::getenv("AFCAS_HOST") ? std::getenv("AFCAS_HOST")
										: throw std::runtime_error("AFCAS_HOST environment variable not set"),
			  std::getenv("AFCAS_PORT") ? std::getenv("AFCAS_PORT")
										: throw std::runtime_error("AFCAS_PORT environment variable not set"),
			 [context, this]() {
              parseInitData(context);
              return principals;
          }(),
			  this->resources,
        this->operations,
        this->accessPredicates) {
        }

	~AfcasAuthorizationStrategy() {}

	void isAuthorized(ComputationContext *context) override {
    auto predicates = parseAfcasAuthorizationData(context);
    for (auto &predicate : predicates) {
      if (!afcasClient.isAuthorized(predicate.principalName, predicate.operationId, predicate.resourceId)) {
        context->Put(AUTHORIZATION_AUTHORIZED_KEY, false);
        return;
      }
    }
    context->Put(AUTHORIZATION_AUTHORIZED_KEY, true);
	}
};