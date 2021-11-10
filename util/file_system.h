#pragma once

#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>

#include "oceancv/util/string_func.h"

namespace ocv {

/**
 * Recursively make directory. That means, if the path to
 * the directory contains several nested folders, all of them
 * are created.
 * E.g. you want to have a folder /foo/bar/baz/inga and /foo/bar
 * already exists, then first the folder /foo/bar/baz will be created
 * and then the final /foo/bar/baz/inga
 **/
bool rmkdir(std::string path, int permission);

/**
 * Returns true if a file (or folder) exists.
 **/
bool fileExists(std::string p);

/**
 * Kills the running program if the given file (or folder)
 * does not exist.
 **/
void forceFileExists(std::string p);

/**
 * Returns the file size in bytes
 **/
long fileSize(std::string p);

/**
 * Reads all files (and folders!) in a directory. Does not read recursively (i.e.
 * does not descend into folders). The names of all files (and folders) are returned
 * as a vector of strings.
 * Additionally, the desired file types can be specified as a vector of strings.
 * The file names are sorted before they are returned.
 **/
std::vector<std::string> filesInFolder(std::string folder,std::vector<std::string> file_types = {});

/**
 * Returns all names of folders within a folder. Files are not considered!
 * Does not descend into the subfolders.
 **/
std::vector<std::string> directoriesInFolder(std::string folder);

/**
 * Check if a given path is a directory
 **/
bool isDir(std::string path);

/**
 * Returns the file name part of a full path
 */
std::string fileName(std::string path);

/**
 * Returns the part of a file name behind the last dot (.)
 * So it cannot handle .tar.gz files!
 */
std::string fileType(std::string path);

/**
 * Returns the folder part of a full path
 */
std::string fileDirectory(std::string path);

}
