#pragma once

#include "AfcasAuthorizationStrategy.h"
#include "AuthorizationStrategy.h"
#include "logger/Logger.h"
#include "afcas/Operation.h"
#include "afcas/Principal.h"
#include "afcas/Resource.h"
#include "afcas/ResourceAccessPredicate.h"
#include "tinyxml2.h"
#include <vector>

const std::string AUTHORIZATION_TYPE_KEY = "authorization_type";
const std::string AUTHORIZATION_TYPE_AFCAS = "afcas";
// Class that manages the creation of the authorization strategy
class AuthorizationFactory {
  public:
	static AuthorizationStrategy *createAuthorizationStrategy(ComputationContext *context) {
		auto type = std::any_cast<std::string>(context->Get(AUTHORIZATION_TYPE_KEY));
		if (type == AUTHORIZATION_TYPE_AFCAS) {
			return new AfcasAuthorizationStrategy(context);
		} else {
			auto error = "Unknown authorization strategy type: " + type;
			LOG_ERROR(error);
			throw std::runtime_error(error);
		}
		return nullptr;
	}
};