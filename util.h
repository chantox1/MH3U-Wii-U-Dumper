#pragma once

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

bool existsAndIsDir(fs::path path);
bool existsAndIsFile(fs::path path);
bool recursiveSearch(const fs::path searchDir,
					 const fs::path fname,
						   fs::path &res);
int32_t int32BigEndian(unsigned char *buffer);
int16_t int16BigEndian(unsigned char *buffer);
