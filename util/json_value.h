#pragma once

#include <iostream>
#include <fstream>

#include "jsoncpp/json/json.h"

#include "oceancv/util/file_system.h"

namespace ocv {
	
/**
 * Reads a JSON file from a given path
 **/
Json::Value getJson(std::string path);

/**
 * Checks if a key exists as part of a Json::Value.
 **/
bool has(Json::Value j, std::string key);

/**
 * Writes a Json::Value as a file to the given path.
 **/
bool writeJson(std::string path, Json::Value root);

}
 
