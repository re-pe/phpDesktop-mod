// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include "executable.h"
#include "file_utils.h"
#include "json.h"
#include "log.h"

std::string g_applicationSettingsError = "";



json_value* GetJson(std::string jsonFileName){
	json_value* ret = new json_value();

	std::string jsonFile = GetExecutableDirectory() + "\\" + jsonFileName;
	LOG_DEBUG << "Fetching json from " << jsonFileName.c_str() << " file";
	std::string contents = GetFileContents(jsonFile);
	if (contents.empty()) {
		LOG_WARNING << "Error while opening the " << jsonFileName.c_str() << " file";
		g_applicationSettingsError = "Error while opening the " + jsonFileName + " file.";
		return ret;
	}

	json_settings settings;
	memset(&settings, 0, sizeof(json_settings));
	char error[256];
	json_value* json_parsed = json_parse_ex(&settings, contents.c_str(),
		&error[0]);
	if (json_parsed == 0) {
		LOG_WARNING << "Error while parsing " << jsonFileName.c_str() << " file: " << error;
		g_applicationSettingsError = "Error while parsing the " + jsonFileName + " file. ";
		return ret;
	}
	ret = json_parsed;
	return ret;
}

json_value* GetApplicationSettings() {
    static json_value* ret = new json_value();
    static bool settings_fetched = false;
	if (settings_fetched){
		return ret;
	}
	std::string settingsFile = "start.conf";
	json_value* jsonValue;
	json_value branch;
	do {
		LOG_DEBUG << "Fetching settings from " << settingsFile.c_str() << " file";
		jsonValue = GetJson(settingsFile);
		if (jsonValue == 0) {
			return ret;
		}
		std::string newSettingsFile;
		std::string path;
		std::string conf;
		branch = (*jsonValue)["Desktop"];
		if (branch.type == json_object && branch.u.object.length > 0) {
			path = (*jsonValue)["Desktop"]["Path"];
			conf = (*jsonValue)["Desktop"]["Conf"];
			if (conf.empty()) {
				LOG_WARNING << "Error while getting settings: [Desktop.Conf] is empty.";
				g_applicationSettingsError = "Error while getting settings: [Desktop.Conf] is empty.";
				return ret;
			}
			newSettingsFile = conf;
			if (!path.empty()){
				newSettingsFile = path + "\\" + newSettingsFile;
			}
			settings_fetched = true;
		} else {
			branch = (*jsonValue)["Settings"];
			if (branch.type != json_object || branch.u.object.length < 1) {
				return ret;
			}
			path = (*jsonValue)["Settings"]["Path"];
			conf = (*jsonValue)["Settings"]["Conf"];
			if (conf.empty()) {
				LOG_WARNING << "Error while getting settings: [Settings.Conf] is empty.";
				g_applicationSettingsError = "Error while getting settings: [Settings.Conf] is empty.";
				return ret;
			}
			newSettingsFile = conf;
			if (!path.empty()){
				newSettingsFile = path + "\\" + newSettingsFile;
			}

		}

		if (newSettingsFile.empty()){
			LOG_WARNING << "Error while getting settings: file has not contain necessary data.";
			g_applicationSettingsError = "Error while getting settings: file has not contain necessary data.";
			return ret;
		}
		if (!settings_fetched && settingsFile == newSettingsFile){
			LOG_WARNING << "Error while getting settings: endless cycle.";
			g_applicationSettingsError = "Error while getting settings: endless cycle.";
			return ret;
		}
		settingsFile = newSettingsFile;
	} while (!settings_fetched);

	json_value* settings = GetJson(settingsFile);
	if (settings == 0) {
		return ret;
	}
	ret = settings;
    return ret;
}

std::string GetApplicationSettingsError() {
	return g_applicationSettingsError;
}
