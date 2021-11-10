#pragma once

#include <map>

#include "oceancv/util/misc_func.h"
#include "oceancv/util/string_func.h"

namespace ocv {

	/**
	 * Tries to guess the separators (row and col) that delimit the data in an
	 * ASCI file.
	 * @param path the path to the ASCI file to analyze
	 * @return a pair containing first the row separator and second the col separator
	 */
	std::pair<std::string,std::string> guessASCIFileSeparators(std::string path);

	/**
	 * Tries to guess whether a given ASCI file has a header row. Analyzes only the first two rows.
	 * @param path the path to the ASCI file to analyze
	 * @return true if the file is deemed to have a header row
	 */
	bool guessASCIFileHasHeader(std::string path);

	/**
	 * Returns the header names of an ASCI file (the exploded first row)
	 * @param path the path to the ASCI file to analyze
	 * @return vector of string column names
	 */
	std::vector<std::string> getASCIFileHeader(std::string path);
	std::vector<std::string> getASCIFileHeader(std::string path, std::string row_split, std::string col_split);

	/**
	 * Reads a file and separates each line to a vector of strings according to
	 * the split character.
	 **/
	std::vector<std::vector<std::string>> readASCIFile(std::string path);
	std::vector<std::vector<std::string>> readASCIFile(std::string path, std::string row_split, std::string col_split);
	std::vector<std::string> readASCIFileLines(std::string path);
	std::vector<std::string> readASCIFile(std::string path, std::string row_split);

	/**
	 * Fetch data from an ASCI file in column format. Expects a key column name and a vector of further data columns.
	 * Returns the ASCI data as C++ strings. There is a version of this function that cast data values to doubles.
	 * @param file path the path to the ASCI file to analyze
	 * @param key_col the name of the column to use a the key to the data
	 * @param val_cols a vector of data column names to fetch from the ASCI file
	 * @return a map in the format {val_cols[0]:{data[key_col][0]:data[key_col][val_col][0],...},val_cols[1]:{...},...}
	 */
	std::map<std::string,std::map<std::string,std::string>> getASCIFileColumnsAsString(std::string file,std::string key_col,std::vector<std::string> val_cols);

	/**
	 * A version of the function getASCIFileColumnsAsString that casts each data value to double.
	 * Please see that function's documentation for parameter details.
	 */
	std::map<std::string,std::map<std::string,double>> getASCIFileColumnsAsDouble(std::string file,std::string key_col,std::vector<std::string> val_cols);

}
