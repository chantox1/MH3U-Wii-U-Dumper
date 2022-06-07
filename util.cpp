#include <iostream>
#include "util.h"

bool existsAndIsDir(fs::path path) {
	if (fs::exists(path)) {
		if (fs::is_directory(path)) {
			return true;
		}
		std::cout << path << " is not a directory.\n";
		return false;
	}
	std::cout << path << " does not exist.\n";
	return false;
}

bool existsAndIsFile(fs::path path) {
	if (fs::exists(path)) {
		if (fs::is_regular_file(path)) {
			return true;
		}
		std::cout << path << " is not a file.\n";
		return false;
	}
	std::cout << path << " does not exist.\n";
	return false;
}

bool recursiveSearch(const fs::path searchDir,
					 const fs::path fname,
						   fs::path &res) {
	fs::recursive_directory_iterator it(searchDir), end;

	while (it != end) {
		if (it->path().filename() == fname) {
			res = it->path();
			return true;
		}
		it++;
	}
	return false;
}

int32_t int32BigEndian(unsigned char *buffer) {
	return buffer[3] | (buffer[2]<<8) | (buffer[1]<<16) | (buffer[0]<<24);
}

int16_t int16BigEndian(unsigned char *buffer) {
	return buffer[1] | (buffer[0]<<8);
}
