#pragma once

#include <iostream>
#include <math.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "oceancv/util/file_system.h"

namespace ocv {

/**
 * Returns the current time as a string.
 **/
inline std::string getTime();


/**
 * Create a random string of given length. E.g. to be stored
 * as a hash to identity the images in the Biigle database
 **/
inline std::string createHash(int len = 16);


/**
 * Append 0's to idx to make the combined thing a str_len
 * long string. Useful if files are renamed to have a running index
 * with identical length. Otherwise OS's sometimes get confused.
 **/
inline std::string createIndex(size_t idx, int str_len);

/**
 * Checks if the given string is a valid floating point number.
 */
bool isFloat(std::string str);

template<typename T>
bool inArray(const std::vector<T>& ar, T v);

std::string exec(const std::string);

/**
* @author: Timm Schoening
* @date: 2020-07-14
* If condition is true, couts the message and returns the condition.
* Just to make plenty of sanity checks with debug outputs more compact.
**/
bool checkConditionWithMessage(bool condition,std::string message);

std::string md5(std::string path);

std::string trim(std::string);

double deg2rad (double degrees);




}
