#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#include "oceancv/util/string_func.h"

namespace ocv {

/**
 * @brief One command line argument
 *
 * I can't really remember why this isn't a struct. Bracket-notation
 * instantiation maybe.
 * Anyway, it contains:
 * a key (like the p in -p)
 * a value (the thing you specified after -p)
 * an optional description (to be shown when ?,--help is typed)
 * a required flag (states whether this thing has to be set
 **/
class cli_arg {

public:
	cli_arg(std::string key = "", std::string desc = "", std::string def = "") : key_ (key), desc_(desc), val_(def), def_(def), req_(def == "") {};
	std::string key_;
	std::string desc_;
	std::string val_;
	std::string def_;
	bool req_;
};


/**
 * @brief Define, read and validate command line arguments.
 *
 * You define this in the head of your binary, pass it the argc and argv
 * values given to the binary itself and then everything will be settled
 * for you.
 * This does not(!) require pre-instantiated variable instances (like
 * boost). This means, that to access the values, you have to type a
 * function (like cmd.b(<parameter key>) to get the value.
 * I prefer that, others may prefer the boost-style.
 * Oh and by the way: strings are stored, that means, each parameter
 * request implies a cast to the desired type!
 **/
class cli_args {

public:

	/**
	 * Create the parameter storage object. Requires the argv, argc
	 * values given to the binary.
	 * Constructs a map of parameters, checks if all required values
	 * are set, handles flags (--name) as well as key-value pairs
	 * (-key value).
	 **/
	cli_args(int argc, char** argv, std::vector<cli_arg> values);

	/**
	 * Get param with identifier key as string
	 * @param key the identifier to look for in the parameter list
	 * @returns the parameter value
	 **/
	std::string operator[](std::string key);

	/**
	 * Get param with identifier key as bool
	 * @param key the identifier to look for in the parameter list
	 * @returns the parameter value
	 **/
	bool b(std::string key);

	/**
	 * Get param with identifier key as int
	 * @param key the identifier to look for in the parameter list
	 * @returns the parameter value
	 **/
	int i(std::string key);

	/**
	 * Get param with identifier key as float
	 * @param key the identifier to look for in the parameter list
	 * @returns the parameter value
	 **/
	float f(std::string key);

	/**
	 * Get param with identifier key as string
	 * @param key the identifier to look for in the parameter list
	 * @returns the parameter value
	 **/
	std::string s(std::string key);

	/**
	 * Get param with identifier key as vector of strings
	 * @param key the identifier to look for in the parameter list
	 * @param delim the delimeter to cut the given value at
	 * @returns the parameter values as a vector of strings
	 **/
	std::vector<std::string> vs(std::string key, std::string delim = ",");

	/**
	 * Get param with identifier key as vector of integers
	 * @param key the identifier to look for in the parameter list
	 * @param delim the delimeter to cut the given value at
	 * @returns the parameter values as a vector of integers
	 **/
	std::vector<int> vi(std::string key, std::string delim = ",");

	/**
	 * Get param with identifier key as vector of floats
	 * @param key the identifier to look for in the parameter list
	 * @param delim the delimeter to cut the given value at
	 * @returns the parameter values as a vector of floats
	 **/
	std::vector<float> vf(std::string key, std::string delim = ",");

	/**
	 * Return all the arguments in their final form (set values or defaults).
	 * @return all arguments as a map
	 **/
	std::map<std::string,std::string> args();

private:

	// The internat map of parameters (or arguments, as you like)
	std::map<std::string,cli_arg> args_;


};

}
