// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include "executable.h"
#include "file_utils.h"
#include "json.h"
#include "log.h"

std::string g_applicationSettingsError = "";

json_value* GetJson(std::string jsonFileName) {
	json_value* ret = new json_value();

	std::string jsonFile = GetExecutableDirectory() + "\\" + jsonFileName;
	LOG_DEBUG << "Fetching json from " << jsonFileName << " file";
	std::string contents = GetFileContents(jsonFile);
	if (contents.empty()) {
		g_applicationSettingsError = "Error while opening the " + jsonFileName + " file.";
		LOG_WARNING << g_applicationSettingsError;
		return ret;
	}

	json_settings settings;
	memset(&settings, 0, sizeof(json_settings));
	char error[256];
	json_value* json_parsed = json_parse_ex(&settings, contents.c_str(), &error[0]);
	if (json_parsed == 0) {
		g_applicationSettingsError = "Error while parsing " + jsonFileName + " file: " + error;
		LOG_WARNING << g_applicationSettingsError;
		return ret;
	}
	ret = json_parsed;
	return ret;
}

json_value* GetApplicationSettings() {
    static json_value* ret = new json_value();
    static bool settings_fetched = false;
    if (settings_fetched)
        return ret;
	const std::string startConf = "start.conf";
	std::string settingsFile = startConf;
	std::string confDir;
	json_value* jsonValue = new json_value();
	do {
		if ((*jsonValue).type > json_none) {
			std::string path = (*jsonValue)["Path"];
			std::string settings = (*jsonValue)["Settings"];
			std::string conf;
			if (settings.empty()) {
				conf = (*jsonValue)["Conf"];
				if (conf.empty()) {
					g_applicationSettingsError = "Error: conf file has neither Conf nor Settings.";
					LOG_WARNING << g_applicationSettingsError;
					return ret;
				}
			}
			else {
				settings_fetched = true;
				conf = settings;
			}
			if (settingsFile == startConf) { 
				confDir = path; 
			} else if (!confDir.empty()) conf = confDir + "\\" + conf;
			if (!path.empty()) conf = path + "\\" + conf;
			settingsFile = conf;
		}
		LOG_DEBUG << "Fetching settings from " << settingsFile << " file";
		jsonValue = GetJson(settingsFile);
		if ((*jsonValue).type == json_none || (*jsonValue).type == json_null) {
			g_applicationSettingsError = "Error while searching settings: file " + settingsFile + " does not contain json value.";
			LOG_WARNING << g_applicationSettingsError;
			return ret;
		}
	} while (!settings_fetched);

	ret = jsonValue;
	return ret;
}

std::string GetApplicationSettingsError() {
    return g_applicationSettingsError;
}
