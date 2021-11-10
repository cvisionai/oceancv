#pragma once

#include <string>
#include <cassert>

#include "oceancv/util/cli_args.h"
#include "oceancv/util/misc_func.h"
#include "oceancv/util/json_value.h"
#include "oceancv/util/file_system.h"

namespace ocv {

class data_file_structure {

public:

	enum TYPES {
		RAW,
		PROCESSED,
		EXTERNAL,
		PRODUCTS,
		PROTOCOL,
		INTERMEDIATE,
		NONE
	};

	data_file_structure(std::string path,bool create = true);
	//std::string getFolder(std::string type, bool create) const;
	std::string getFolder(data_file_structure::TYPES type, bool create = true) const;

	std::string protocol(bool create = true) const;
	std::string raw(bool create = true) const;
	std::string external(bool create = true) const;
	std::string processed(bool create = true) const;
	std::string products(bool create = true) const;
	std::string intermediate(bool create = true) const;

	bool valid() const;


	std::map<std::string,TYPES> type_map = {{"raw",RAW},{"processed",PROCESSED},{"external",EXTERNAL},{"products",PRODUCTS},{"protocol",PROTOCOL},{"intermediate",INTERMEDIATE}};


private:
	std::string _path;
	std::string _folder;
	std::string _file;
	TYPES _type;
	std::string _base_folder;
	std::string _location;
	int _location_idx;
	std::vector<std::string> _parts;
	bool _is_valid_folder;
};

}
