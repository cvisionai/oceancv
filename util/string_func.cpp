#include "oceancv/util/string_func.h"

namespace ocv {

/**
 * Explode a string (aka the haystack) at some string (please let it be only
 * one character) and return a vector of all parts of the string that were
 * seperated by the needle. The needle itself is not part of the returned
 * values.
 **/
std::vector<std::string> explode(std::string needle, std::string haystack) {
	std::vector<std::string> ret;
	std::string tmp = "";
	std::string t;
	for(size_t i = 0; i < haystack.length(); i++) {
		t = haystack[i];
		if(needle.compare(t) == 0) {
			ret.push_back(tmp);
			tmp = "";
		} else {
			tmp += t;
		}
	}
	if(tmp.compare("") != 0)
		ret.push_back(tmp);
	return ret;
}

std::string implode(std::string filler, const std::vector<std::string>& elements) {
	if(elements.size() == 0)
		return "";
	std::string ret = elements[0];
	for(size_t i = 1; i < elements.size(); i++)
		ret += filler + elements[i];
	return ret;
}


/**
 * Replace all occurrences of the needle with new_needle in the string haystack.
 **/
std::string replace(std::string needle, std::string new_needle, std::string haystack) {
	std::string ret = "";
	
	for(size_t i = 0; i < haystack.length(); i++) {
		if(haystack.substr(i,needle.length()) == needle) {
			ret += new_needle;
			i += needle.length() - 1;
		} else {
			ret += haystack[i];
		}
	}
	
	return ret;
}


/**
 * Split a string representation of a list (something like [a,b,d]) to
 * a vector of the same: std::vector<string>({a,b,c}).
 **/
std::vector<std::string> listToVector(std::string list) {
	std::string tmp_p = replace("[","",replace("]","",list));
	return explode(",",tmp_p);
}

}
