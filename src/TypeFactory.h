#pragma once
#include "logger/Logger.h"
#include "util/globothy.h"
#include "util/libnameothy.h"
#include <algorithm>
#include <dlfcn.h>
#include <set>
#include <string>
#include <unordered_map>

class TypeFactory {
  private:
	std::unordered_map<std::string, void *> commands_map;
	std::set<std::string> libNames;

  public:
	TypeFactory() {
		updateLibs();
	};

	void updateLibs() {
		if (!std::getenv("PLUGINS_FOLDER_PATH")) {
			throw std::runtime_error("PLUGINS_FOLDER_PATH environment variable is missing");
		}
		LOG_INFO("Checking for new libraries :");
		std::string pluginsFolderPath = std::getenv("PLUGINS_FOLDER_PATH");

#if __APPLE__
		const std::string libGlob(pluginsFolderPath + "/*.dylib");
#else
		const std::string libGlob(pluginsFolderPath + "/*.so");
#endif

		std::vector<std::string> filenames = globothy(libGlob);

		size_t before = libNames.size();

		for (std::string plugin : filenames) {
			libNames.insert(plugin);
		}
		size_t after = libNames.size();

		if (after - before > 0) {
			LOG_INFO("Found " + std::to_string(after - before) + " new plugins");
		}
	}

	/**
	 * Loads the plugin into the program space
	 */
	template <class T> void loadPlugin(std::string pluginName) {
		auto it = std::find_if(libNames.begin(), libNames.end(),
							   [pluginName](const std::string &s) { return (s.find(pluginName) != std::string::npos); });
		if (it == libNames.end()) {
			throw std::runtime_error("Plugin is not available!");
		}
		std::string plugin = *it;
		try {
			std::pair<std::string, std::string> delibbed = libnameothy(plugin);
			if (commands_map[delibbed.second] != nullptr) {
				// Command already loaded
				return;
			}

			void *dlhandle = dlopen(plugin.c_str(), RTLD_LAZY);

			if (dlhandle == NULL) {
				printf("Error: %s\n", dlerror());
				exit(1);
			}

			T *(*create)();
			void (*destroy)(T *);

			std::string cn = "create" + delibbed.second;
			std::string dn = "destroy" + delibbed.second;

			create = (T * (*)()) dlsym(dlhandle, cn.c_str());
			destroy = (void (*)(T *))dlsym(dlhandle, dn.c_str());

			T *a = create();
			commands_map[delibbed.second] = a;
		} catch (std::exception &e) {
			LOG_ERROR(e.what());
			LOG_ERROR("Error when trying to create dynamic handler instance");
		}
	}
	/**
	 * Fetches the base plugin
	 * @param type
	 * @return
	 */
	template <class T> T *GetType(std::string type);
};
