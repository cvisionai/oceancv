#include "oceancv/util/file_parser.h"

namespace ocv {

	std::pair<std::string,std::string> guessASCIFileSeparators(std::string path) {

		// Read 10000 chars from file
		int length = 10000;
		char* buffer = new char[length];

		std::ifstream file(path);
		file.read(buffer,length);
		file.close();
		std::string f = buffer;

		// Guess row separators
		std::vector<std::string> row_splits = {"\r\n","\n"};
		int rn,max_rn = 0;
		std::string max_rs = "";

		for(std::string s : row_splits) {
			rn = ocv::explode(s,f).size();
			if(rn > max_rn) {
				max_rn = rn;
				max_rs = s;
			}
		}

		// GUess col separators
		std::vector<std::string> col_splits = {",",";","\t"};
		std::vector<std::string> fs = ocv::explode(max_rs,f);
		int cn,max_cn = 0;
		std::string max_cs = "";
		for(std::string s : col_splits) {
			cn = ocv::explode(s,fs[0]).size();
			if(cn > max_cn) {
				max_cn = cn;
				max_cs = s;
			}
		}

		return std::pair<std::string,std::string>(max_rs,max_cs);

	}

	bool guessASCIFileHasHeader(std::string path) {

		// Read 10000 chars from file
		int length = 10000;
		char* buffer = new char[length];

		std::ifstream file(path);
		file.read(buffer,length);
		file.close();
		std::string f = buffer;

		// Guess row / col delimeter
		std::pair<std::string,std::string> seps = ocv::guessASCIFileSeparators(path);

		// Fetch first two rows
		std::vector<std::string> rows = ocv::explode(std::get<0>(seps),f);
		std::vector<std::string> row_0 = ocv::explode(std::get<1>(seps),rows[0]);
		std::vector<std::string> row_1 = ocv::explode(std::get<1>(seps),rows[1]);

		// Check how many numerical values are in row one vs row two
		int num_numeric = 0;
		for(size_t i = 0; i < row_0.size(); i++) {

			if(ocv::isFloat(ocv::trim(row_0[i])))
				num_numeric++;
			if(ocv::isFloat(ocv::trim(row_1[i])))
				num_numeric--;

		}

		return (num_numeric != 0);

	}

	std::vector<std::string> getASCIFileHeader(std::string path) {
		std::pair<std::string,std::string> seps = ocv::guessASCIFileSeparators(path);
		return getASCIFileHeader(path, seps.first, seps.second);
	}

	std::vector<std::string> getASCIFileHeader(std::string path, std::string row_sep, std::string col_sep) {

		// Read 10000 chars from file
		int length = 10000;
		char* buffer = new char[length];

		std::ifstream file(path);
		file.read(buffer,length);
		file.close();
		std::string f = buffer;

		std::vector<std::string> rows = ocv::explode(row_sep,f);
		return ocv::explode(col_sep,rows[0]);

	}

	std::vector<std::vector<std::string>> readASCIFile(std::string path) {
		std::pair<std::string,std::string> seps = ocv::guessASCIFileSeparators(path);
		return readASCIFile(path, seps.first, seps.second);
	}

	std::vector<std::vector<std::string>> readASCIFile(std::string path, std::string row_split, std::string col_split) {

		ocv::forceFileExists(path);

		std::vector<std::vector<std::string>> ret = {};
		std::string line;
		std::ifstream file(path);

		// Read always until the last char of the row_separator is reached
		int row_split_length = row_split.length() - 1;
		char tmp_row_split = row_split[row_split_length];
		bool match;

		std::string tmp_line;

		if(file.is_open()) {
			while(getline(file, tmp_line, tmp_row_split)) {

				// Check if all separators have been matched
				match = true;
				for(int i = 0; i < row_split_length; i++) {
					if(tmp_line[tmp_line.length()-row_split_length] != row_split[i]) {
						match = false;
						break;
					}
				}

				line += tmp_line + tmp_row_split;

				if(match) {
					ret.push_back(explode(col_split,ocv::replace(row_split,"",line)));
					line = "";
				}

			}
		}
		file.close();

		return ret;

	}


	std::vector<std::string> readASCIFileLines(std::string path) {
		std::pair<std::string,std::string> seps = ocv::guessASCIFileSeparators(path);
		return readASCIFile(path, seps.first);
	}

	std::vector<std::string> readASCIFile(std::string path, std::string row_split) {

		ocv::forceFileExists(path);

		std::vector<std::string> ret = {};
		std::string line;
		std::ifstream file(path);

		// Read always until the last char of the row_separator is reached
		int row_split_length = row_split.length() - 1;
		char tmp_row_split = row_split[row_split_length];
		bool match;

		std::string tmp_line;

		if(file.is_open()) {
			while(getline(file, tmp_line, tmp_row_split)) {

				// Check if all separators have been matched
				match = true;
				for(int i = 0; i < row_split_length; i++) {
					if(tmp_line[tmp_line.length()-row_split_length] != row_split[i]) {
						match = false;
						break;
					}
				}

				line += tmp_line;

				if(match) {
					ret.push_back(line);
					line = "";
				}

			}
    }
		file.close();

		return ret;
	}

	std::map<std::string,std::map<std::string,std::string>> getASCIFileColumnsAsString(std::string file,std::string key_col,std::vector<std::string> val_cols) {

		if(!ocv::fileExists(file)) {
			std::cout << "File " << file << " not found" << std::endl;
			return {};
		}

		std::pair<std::string,std::string> seps = ocv::guessASCIFileSeparators(file);
		std::vector<std::string> h = ocv::getASCIFileHeader(file);

		int key_idx = -1;
		std::map<std::string,int> val_idcs;
		for(std::string val : val_cols) {
			val_idcs[val] = -1;
		}

		std::string col,key;
		for(size_t idx = 0; idx < h.size(); idx++) {

			col = ocv::trim(h[idx]);

			if(col == key_col)
				key_idx = idx;
			else {
				for(std::string val : val_cols) {
					if(val == col)
						val_idcs[val] = idx;
				}
			}

		}

		if(key_col != "___ROW_INDEX___" && key_idx < 0) {
			std::cout << "Column error - key column not found: " << key_col << std::endl;
			return {};
		}
		for(std::string val : val_cols) {
			if(val_idcs[val] < 0) {
				std::cout << "Column error - val column not found: " << val << std::endl;
				return {};
			}
		}

		std::map<std::string,std::map<std::string,std::string>> data = {};
		for(std::string val : val_cols) {
			data.insert(std::pair<std::string,std::map<std::string,std::string>>(val,{}));
		}

		//std::string rs = "\n";
		std::vector<std::vector<std::string>> f = ocv::readASCIFile(file,std::get<0>(seps),std::get<1>(seps));

		std::vector<std::string> r;
 		for(size_t i = 1; i < f.size(); i++) {

 			r = f[i];
			key = "";

 			if(key_col != "___ROW_INDEX___" && r[key_idx] != "") {
				key = ocv::trim(r[key_idx]);
			} else if(key_col == "___ROW_INDEX___") {
				key = std::to_string(i);
			}

			if(key.compare("") != 0) {
 				for(std::string val : val_cols) {
 					if((size_t) val_idcs[val] < r.size() && r[val_idcs[val]].compare("") != 0) {
 						data[val][key] = ocv::trim(r[val_idcs[val]]);
 					}
 				}
 			}

 		}

		return data;

	}

	std::map<std::string,std::map<std::string,double>> getASCIFileColumnsAsDouble(std::string file,std::string key_col,std::vector<std::string> val_cols) {

		std::map<std::string,std::map<std::string,std::string>> src = ocv::getASCIFileColumnsAsString(file, key_col, val_cols);
		std::map<std::string,std::map<std::string,double>> ret = {};

		for(std::pair<std::string,std::map<std::string,std::string>> p : src) {
			ret.insert(std::pair<std::string,std::map<std::string,double>>(p.first,{}));
			for(std::pair<std::string,std::string> q : p.second) {
				ret[p.first].insert(std::pair<std::string,double>(q.first,std::stod(q.second)));
			}
		}

		return ret;

	}

}
