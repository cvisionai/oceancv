#include "oceancv/util/data_file_structure.h"

namespace ocv {
	data_file_structure::data_file_structure(std::string path, bool create) {

		std::string location;

		assert(ocv::fileExists(path) || create);

		if(!ocv::fileExists(path) && create) {
			mkdir(path.c_str(),0755);
		}

		if(ocv::isDir(path)) {
			_file = "";
			if(path[path.length()-1] != '/')
				path += "/";
			_folder = path;
		} else {
			_file = ocv::fileName(path);
			_folder = ocv::replace(_file,"",_path);
		}
		_path = path;

		_parts = ocv::explode("/",_path);

		_location_idx = -1;
		_location = "";
		_type = data_file_structure::TYPES::NONE;
		_is_valid_folder = false;

		for(int i = _parts.size()-1; i >= 0; i--) {
			if(type_map.find(_parts[i]) != type_map.end()) {
				_location = _parts[i];
				_location_idx = i;
				_type = type_map[_location];
				_is_valid_folder = true;
				break;
			}
		}

	}


	std::string data_file_structure::getFolder(data_file_structure::TYPES type, bool create) const {

		std::string ret_name, ret_folder;
		for(auto p : type_map) {
			if(p.second == type) {
				ret_name = p.first;
				break;
			}
		}

		if(_type == data_file_structure::TYPES::NONE) {
			// This object's path does not follow the file structuring scheme, lets append the requested type
			ret_folder = _folder + ret_name + "/";
		} else {
			ret_folder = "";
			for(int i = 0; i < _parts.size(); i++)
				if(i == _location_idx)
					ret_folder += ret_name + "/";
				else
					ret_folder += _parts[i] + "/";
		}
		if(create && !ocv::fileExists(ret_folder)) {
			mkdir(ret_folder.c_str(),0755);
		}
		return ret_folder;
	}

	std::string data_file_structure::protocol(bool create) const {
		return getFolder(data_file_structure::TYPES::PROTOCOL,create);
	}
	std::string data_file_structure::raw(bool create) const {
		return getFolder(data_file_structure::TYPES::RAW,create);
	}
	std::string data_file_structure::external(bool create) const {
		return getFolder(data_file_structure::TYPES::EXTERNAL,create);
	}
	std::string data_file_structure::processed(bool create) const {
		return getFolder(data_file_structure::TYPES::PROCESSED,create);
	}
	std::string data_file_structure::products(bool create) const {
		return getFolder(data_file_structure::TYPES::PRODUCTS,create);
	}
	std::string data_file_structure::intermediate(bool create) const {
		return getFolder(data_file_structure::TYPES::INTERMEDIATE,create);
	}

	bool data_file_structure::valid() const {
		return _is_valid_folder;
	}
}
