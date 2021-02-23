#pragma once

#include <string>
#include <vector>

void* FileReadToNewBuffer( std::string const& filename, size_t* out_size );
std::vector<std::string> FileReadLines(std::string const& filename);
std::string FileReadString(std::string const& filename);

bool FileWriteToDisk(std::string const& filename, void const* bufferPtr, size_t const& bufferSize);

std::vector<std::string> FilesFindInDirectory(char const* directoryPath, char const* fileFormat = nullptr);
