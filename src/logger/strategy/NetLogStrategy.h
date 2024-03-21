#pragma once
#include "../../Utils.h"
#include "../content-writer/NetworkContentWriter.h"
#include "LogStrategy.h"

enum LogCode { INFO, ERROR, QUERY, LATENCY };

class NetLogStrategy : public LogStrategy {
  public:
	NetLogStrategy() { wt = new NetworkContentWriter(); }

	LogCode getLogCode(std::string key) {
		if (key == "QUERY") {
			return LogCode::QUERY;
		} else if (key == "LATENCY") {
			return LogCode::LATENCY;
		} else if (key == "INFO") {
			return LogCode::INFO;
		} else {
			return LogCode::ERROR;
		}
	}

	bool Log(std::string app, std::string key, std::string cause) override {
		switch (getLogCode(key)) {
		case LogCode::QUERY: {
			return DoLog("{\"type\": \"query\",\"query\":\"" + cause + "\",\"correlation_id\":\"" + app + "\"}\n");
		}
		case LogCode::LATENCY: {
			std::vector<std::string> params = Utils::split(cause, ",");
			std::cout << "latency: " << params[0] << " " << "client_ip: " << params[1] << " " << "correlation_id: " << app
					  << std::endl;
			return DoLog("{\"type\": \"latency\",\"latency\": \"" + params[0] + "\",\"correlation_id\":\"" + app +
						 "\",\"host\": \"" + params[1] + "\"}\n");
		}
		default: {
			std::cout << app + " " + key + " " + cause << std::endl;
			return DoLog("{\"type\": \"application_log\",\"application_log\": \"" + app + " " + key + " " + cause + "\"}\n");
		}
		}
	}

	bool DoLog(std::string logitem) override {
		// Log into the Network Socket
		wt->Write(logitem);
		return true;
	}
};
