#include "oceancv/util/misc_func.h"

namespace ocv {


/**
 * Returns the current time as a string.
 **/
inline std::string getTime() {
	time_t now;
	time(&now);
	return ctime(&now);
}


/**
 * Create a random string of given length. E.g. to be stored
 * as a hash to identity the images in the Biigle database
 **/
inline std::string createHash(int len) {
	std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string ret = "";
	int r;

	for(int i = 0; i < len; i++) {
		r = rand() % 62;
		ret += allowed.substr(r,1);
	}
	return ret;
}


/**
 * Append 0's to idx to make the combined thing a str_len
 * long string. Useful if files are renamed to have a running index
 * with identical length. Otherwise OS's sometimes get confused.
 **/
inline std::string createIndex(size_t idx, int str_len) {

	std::string ret = "";
	for(int i = 0; i < str_len - log10(idx); i++) {
		ret += "0";
	}
	ret += std::to_string(idx);
	return ret;

}


bool isFloat(std::string str) {
    std::istringstream iss(str);
	float f;
    iss >> f;
    return (iss.eof() && !iss.fail());
}

template<typename T>
bool inArray(const std::vector<T>& ar, T v) {
	for(size_t i = 0; i < ar.size(); i++) {
		if(ar[i] == v)
			return true;
	}
	return false;
}
/*
template<typename T>
bool inArray(T v,const std::vector<T>& ar) {
	return inArray(ar,v);
}*/

std::string trim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string exec(const std::string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string md5(std::string path) {

	std::string cmd = "md5sum ";
	if(ocv::fileExists(path)) {
		std::string ret = ocv::exec(cmd + path);
		std::vector<std::string> p = ocv::explode(" ",ret);
		return p[0];
	}
	return "";


}

bool checkConditionWithMessage(bool condition,std::string message) {
	if(condition)
		std::cout << message << std::flush;
	return condition;
}

template bool inArray<std::string>(const std::vector<std::string>&,std::string);
template bool inArray<float>(const std::vector<float>&,float);
template bool inArray<char>(const std::vector<char>&, char);

double deg2rad (double degrees) {
    return degrees * 4.0 * atan (1.0) / 180.0;
}



}
