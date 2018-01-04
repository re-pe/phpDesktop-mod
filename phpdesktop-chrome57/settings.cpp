// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include "executable.h"
#include "file_utils.h"
#include "json.h"
#include "log.h"

std::string g_applicationSettingsError = "";

inline bool FileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

json_value* GetJson(std::string jsonFileName) {
	json_value* ret = new json_value();

	std::string jsonFile = GetExecutableDirectory() + "/" + jsonFileName;
	if (!FileExists(jsonFile)) {
		g_applicationSettingsError = "Error: file " + jsonFileName + " does not exist.";
		LOG_WARNING << g_applicationSettingsError;
		return ret;
	}
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
	const std::string startConfFile = "start.conf";
	std::string settingsFileName = startConfFile;
	LOG_DEBUG << "Fetching settings from " << settingsFileName << " file";
	json_value* jsonValue = new json_value();
	jsonValue = GetJson(settingsFileName);
	if ((*jsonValue).type == json_none) {
		return ret;
	}
	std::string location = "";
	const std::string folder = (*jsonValue)["Folder"];
	do {
		std::string settings = (*jsonValue)["Settings"];
		std::string fileName;
		if (settings.empty()) {
			fileName = (*jsonValue)["FileName"];
			if (fileName.empty()) {
				g_applicationSettingsError = "Error: " + settingsFileName + " file has neither FileName nor Settings.";
				LOG_WARNING << g_applicationSettingsError;
				return ret;
			}
		}
		else {
			settings_fetched = true;
			fileName = settings;
		}
		std::string path = (*jsonValue)["Path"];
		if (!path.empty()) if (location.empty()) 
			location = path;
		else 
			location = location + "/" + path;
		if (!folder.empty()) fileName = folder + "/" + fileName;
		if (!location.empty()) fileName = location + "/" + fileName;
		settingsFileName = fileName;
		LOG_DEBUG << "Fetching settings from " << settingsFileName << " file";
		jsonValue = GetJson(settingsFileName);
		if ((*jsonValue).type == json_none) {
			return ret;
		}
	} while (!settings_fetched);

	ret = jsonValue;
	return ret;
}

std::string GetApplicationSettingsError() {
    return g_applicationSettingsError;
}
