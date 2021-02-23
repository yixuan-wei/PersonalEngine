#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>

void* FileReadToNewBuffer( std::string const& filename, size_t* out_size )
{
	FILE* fp = nullptr;
	fopen_s( &fp, filename.c_str(), "r" );
	if( fp == nullptr )
	{
		g_theConsole->PrintError(Stringf("Failed to open file %s", filename.c_str()));
		return nullptr;
	}

	fseek( fp, 0, SEEK_END );
	long fileSize = ftell( fp );

	char* buffer = new char[fileSize + 1];
	if( nullptr != buffer )
	{
		fseek( fp, 0, SEEK_SET );
		size_t bytesRead = fread( buffer, sizeof(char), fileSize, fp );
		buffer[bytesRead] = NULL;
		if( out_size != nullptr )
		{
			*out_size = bytesRead;
		}
	}

	fclose( fp );
	return buffer;
}

//////////////////////////////////////////////////////////////////////////
std::vector<std::string> FileReadLines(std::string const& filename)
{
	size_t size = 0;
	void* fileBuffer = FileReadToNewBuffer(filename, &size);
	if (fileBuffer == nullptr) {
		std::vector<std::string> result;
		return result;
	}
	std::string rawString(static_cast<char const*>(fileBuffer), size+1);
	delete[] fileBuffer;
	return SplitStringOnDelimiter(rawString, '\n');
}

//////////////////////////////////////////////////////////////////////////
std::string FileReadString(std::string const& filename)
{	
	size_t size=0;
    void* fileBuffer = FileReadToNewBuffer(filename, &size);
    if (fileBuffer == nullptr) {
		std::string result;
        return result;
    }
    std::string rawString(static_cast<char const*>(fileBuffer), size + 1);
	delete[] fileBuffer;
	return rawString;
}

//////////////////////////////////////////////////////////////////////////
bool FileWriteToDisk(std::string const& filename, void const* bufferPtr, size_t const& bufferSize)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename.c_str(), "w");
	if (fp == nullptr) {
		return false;
	}
	
	fwrite(bufferPtr, sizeof(char), bufferSize, fp);
	fclose(fp);
	return true;
}

//////////////////////////////////////////////////////////////////////////
std::vector<std::string> FilesFindInDirectory(char const* directoryPath, char const* fileFormat)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	std::string fullPath = directoryPath;
	fullPath += fileFormat;
	std::vector<std::string> fileList;

	if ((hFile = _findfirst(fullPath.c_str(), &c_file)) == -1L) {
		g_theConsole->PrintError(Stringf("No %s files in directory %s", fileFormat, directoryPath));
	}
	else {
		do 
		{
			std::string newName = directoryPath;
			newName += c_file.name;
			fileList.push_back(newName);
		} while (_findnext(hFile, &c_file)==0);
	}
	_findclose(hFile);

	return fileList;
}
