#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <libgen.h>
#include <ctime>
#include <unistd.h>

//#include "jsoncpp/json/json.h"
#include "opencv2/core.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/misc_func.h"
#include "oceancv/util/json_value.h"
#include "oceancv/util/file_system.h"
#include "oceancv/util/data_file_structure.h"

namespace ocv {

class cli_arg_provenance {

public:
	cli_arg_provenance(cli_args& args, std::string path);//, bool append_time_to_path = false);
	std::string time() const;
	std::string name() const;

private:
	std::string _path;
	std::string _time;
	std::string _name;
};

}
