#include "oceancv/util/file_system.h"

namespace ocv {

bool rmkdir(std::string path, int permission) {
	std::vector<std::string> p = explode("/",path);
	std::string t_path = "";
	for(unsigned int i = 0; i < p.size(); i++) {
		t_path += "/" + p[i];
		mkdir(t_path.c_str(),permission);
	}
	return true;
}

bool fileExists(std::string p) {
	struct stat Status;
	if(stat(p.c_str(), &Status) != 0) {
		return false;
	} else {
		return true;
	}
}

void forceFileExists(std::string p) {
	if(!fileExists(p)) {
		std::cout << "File not found: " << p << std::endl;
		exit(-1);
	}
}

long fileSize(std::string p) {
    struct stat Status;
    int rc = stat(p.c_str(), &Status);
    return rc == 0 ? Status.st_size : -1;
}

std::vector<std::string> filesInFolder(std::string folder,std::vector<std::string> file_types) {
	std::vector<std::string> ret;

	struct dirent **e;

	std::string tmp_name;
	std::vector<std::string> parts;

    int n,i;

    n = scandir(folder.c_str(), &e, NULL, alphasort);//versionsort);
    if(n < 0)
        perror("scandir");
    else {
        for(i = 0; i < n; i++) {
            tmp_name = e[i]->d_name;

			// Omit hidden files
            if(tmp_name[0] == '.')
            	continue;

			if(file_types.size() == 0) {
				ret.push_back(tmp_name);
			} else {
				parts = explode(".",tmp_name);
				for(std::string ext : file_types) {
					if(parts[parts.size()-1] == ext) {
						ret.push_back(tmp_name);
						break;
					}
				}
			}


            free(e[i]);
        }
        free(e);
    }

	return ret;
}

std::vector<std::string> directoriesInFolder(std::string folder) {
	std::vector<std::string> ret;

	DIR * d;
	struct dirent * e;
	struct stat st;
	d = opendir(folder.c_str());
	std::string tmp_name;
	if(d != NULL) {
		while((e = readdir(d)) != NULL) {
			tmp_name = e->d_name;
	    	if(tmp_name != "." && tmp_name != "..") {
	    		lstat((folder + e->d_name).c_str(), &st);
	    		if(S_ISDIR(st.st_mode))
	    			ret.push_back(tmp_name);
	    	}
		}
	  	closedir(d);
	}
	sort(ret.begin(),ret.end());
	return ret;
}

bool isDir(std::string path) {
	struct stat s;
	if(stat(path.c_str(),&s) == 0) {
		if(s.st_mode & S_IFDIR !=0)
			return true;
	}
	return false;
}

std::string fileName(std::string path) {
	std::vector<std::string> p = ocv::explode("/",path);
	return p[p.size()-1];
}

std::string fileType(std::string path) {
	std::vector<std::string> p = ocv::explode(".",path);
	return p[p.size()-1];
}

std::string fileDirectory(std::string path) {
	return ocv::replace(ocv::fileName(path),"",path);
}

}
