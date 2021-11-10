#pragma once

#include <string>
#include <vector>

namespace ocv {

/**
 * Explode a string (aka the haystack) at some string (please let it be only
 * one character) and return a vector of all parts of the string that were
 * seperated by the needle. The needle itself is not part of the returned
 * values.
 **/
std::vector<std::string> explode(std::string needle, std::string haystack);

std::string implode(std::string filler, const std::vector<std::string>& elements);


/**
 * Replace all occurrences of the needle with new_needle in the string haystack.
 **/
std::string replace(std::string needle, std::string new_needle, std::string haystack);


/**
 * Split a string representation of a list (something like [a,b,d]) to
 * a vector of the same: std::vector<string>({a,b,c}).
 **/
std::vector<std::string> listToVector(std::string list);

}