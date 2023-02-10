#include "oceancv/util/cli_arg_provenance.h"

namespace ocv {

cli_arg_provenance::cli_arg_provenance(cli_args& cmd_args, std::string path/*, bool append_time_to_path*/) : _path(path) {

    // Log cmd line args as json to result folder, add date and time

    // Stop execution if no-prov flag has been set (e.g. in debug / test mode)
    if(cmd_args.b("procv-off")) {
        std::cout << "[OceanCV] Provenance disabled" << std::endl;
        return;
    }

    _name = "";
    if(cmd_args.s("procv-name") != "") {
        _name = cmd_args.s("procv-name");
    }

    // Get time
    auto t = std::time(nullptr);
    _time = std::to_string(t);

    auto ltm = std::localtime(&t);
    char buffer[80];
    strftime(buffer,80,"%Y%m%d%H%M%S",ltm);

    std::string time = buffer;
    ocv::data_file_structure dfs(path);

    // Get executable path
    char result[PATH_MAX];
    std::string res_str;
    ssize_t rd_result = readlink("/proc/self/exe", result, PATH_MAX);
    if(rd_result < 0) {
        res_str = "EXECUTABLE COULD NOT BE DETERMINED";
    } else {
        result[rd_result] = '\0';
        res_str = std::string(result);
    }
    std::string executable_name = ocv::fileName(res_str);

    std::string write_folder = dfs.getFolder(data_file_structure::TYPES::PROTOCOL, true);
    std::string file_name = "procv_" + executable_name + "_" + time + ".json";
    if(_name != "")
        file_name = _name + "_" + file_name;

    // Prepare JSON object
    cv::FileStorage file(write_folder + file_name,cv::FileStorage::WRITE);

    //std::map<std::string,std::string> json_md5 = {};

    // Prepare cmd line arguments
    std::map<std::string,std::string> cargs = cmd_args.args();
    file << "parameter" << "[";
    for(std::pair<std::string,std::string> p : cargs) {
        file << "{" << "name" << p.first << "value" << p.second;
        if(ocv::fileExists(p.second) && !ocv::isDir(p.second))
            file << "hash" << ocv::md5(p.second);
        file << "}";
    }
    file << "]";




    // Store executable data, TODO (added 04/2021): implement versioning mechanism
    file << "executable" << "{" << "path" << result << "hash" << ocv::md5(result) << "time" << time << "version" << "0" << "}";

    /*json_md5.insert(std::pair<std::string,std::string>("execution_binary",));
    file << "MD5" << "{";
    for(auto p : json_md5) {
        file << p.first << p.second;
    }
    file << "}";*/

    std::cout << "[OceanCV] Writing provenance info to: " << write_folder << file_name << std::endl;
    file.release();

}

std::string cli_arg_provenance::time() const {
    return _time;
}

std::string cli_arg_provenance::name() const {
    return _name;
}

}
