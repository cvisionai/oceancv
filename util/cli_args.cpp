#include "oceancv/util/cli_args.h"

namespace ocv {

cli_args::cli_args(int argc, char** argv, std::vector<cli_arg> values) {

	// Intialize parameter map
	args_.clear();
	for(cli_arg a : values) {
		args_[a.key_] = a;
	}

	// Flag to see, whether this is a real run or information shall be displayed
	bool help = false;

	std::string targ,key,val,kkey;

	// Iterate over all cmdline arguments
	for(int i = 1; i < argc; i++) {

		// The current argument
		targ = argv[i];

		// Is this a key (bool or key-val)?
		if(targ[0] == '-') {

			kkey = "-";

			// Is this a flag (--flag)?
			if(targ[1] == '-') {
				kkey = "--";
				key = targ.substr(2);
				val = "1";
			} else {
				key = targ.substr(1);
				if(i+1 < argc)
					val = argv[i+1];
				else
					val = "1";
			}

			// Is help requested?
			if(key == "?" || key == "help") {
				help = true;
				continue;
			}

			// Store current values in parameter map
			args_[key].key_ = kkey + key;
			args_[key].val_ = val;

			// Mark (eventually) required parameter as available
			args_[key].req_ = false;

		}

	}

	// Shall help be shown?
	if(help) {

		for(std::pair<std::string,cli_arg> a : args_) {

			// Show name, required and (most important) the description
			std::cout << std::setw(40) << a.first << " (" << a.second.val_;
			if(a.second.val_ == a.second.def_)
			 	std::cout << " - default";
			std::cout << "): ";

			if(a.second.req_) {
				std::cout << " (required)";
			}
			if(a.second.desc_ == "")
				std::cout << " UNKNOWN CLI ARG" << std::endl;
			else
				std::cout << " " << a.second.desc_ << std::endl;

		}

		// Terminate the whole program!
		exit(0);

	}

	// Finally check whether all required parameters are set
	for(std::pair<std::string,cli_arg> a : args_) {
		if(a.second.req_) {

			std::cout << "Missing argument: " << a.second.key_ << " " << a.second.desc_ << std::endl;

			// If not terminate the program
			exit(-1);

		}
	}



}

std::string cli_args::operator[](std::string key) {
	return args_[key].val_;
}

bool cli_args::b(std::string key) {
	return (args_[key].val_ == "true" || args_[key].val_ == "1");
}

int cli_args::i(std::string key) {
	return stoi(args_[key].val_);
}

float cli_args::f(std::string key) {
	return stof(args_[key].val_);
}

std::vector<std::string> cli_args::vs(std::string key, std::string delim) {
	return ocv::explode(delim,args_[key].val_);
}

std::vector<int> cli_args::vi(std::string key, std::string delim) {
	std::vector<std::string> vec_s = ocv::explode(delim,args_[key].val_);
	std::vector<int> vec_i(vec_s.size());
	for(size_t i = 0; i < vec_s.size(); i++)
		vec_i[i] = stoi(vec_s[i]);
	return vec_i;
}

std::vector<float> cli_args::vf(std::string key, std::string delim) {
	std::vector<std::string> vec_s = ocv::explode(delim,args_[key].val_);
	std::vector<float> vec_f(vec_s.size());
	for(size_t i = 0; i < vec_s.size(); i++)
		vec_f[i] = stof(vec_s[i]);
	return vec_f;
}

std::string cli_args::s(std::string key) {
	return args_[key].val_;
}

std::map<std::string,std::string> cli_args::args() {
	std::map<std::string,std::string> ret = {};
	for(std::pair<std::string,cli_arg> arg : args_) {
		ret.insert(std::pair<std::string,std::string>(arg.first,arg.second.val_));
	}
	return ret;
}

}
