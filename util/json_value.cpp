#include "oceancv/util/json_value.h"

namespace ocv {
	
/**
 * Reads a JSON file from a given path
 **/
Json::Value getJson(std::string path) {
	
	ocv::forceFileExists(path);
	
	Json::Value j;
	Json::Reader j_reader;
	std::ifstream jf;
	jf.open(path);
	if(!j_reader.parse(jf, j)) {
		return -1;
	}
	jf.close();
	return j;
}

/**
 * Checks if a key exists as part of a Json::Value.
 **/
bool has(Json::Value j, std::string key) {
	return j.isMember(key);
}

/**
 * Writes a Json::Value as a file to the given path.
 **/
bool writeJson(std::string path, Json::Value root) {

	Json::StyledStreamWriter ssw;
	
	std::ofstream stream;
	stream.open(path);
	
	ssw.write(stream, root);
	
	return true;

}

}
 
