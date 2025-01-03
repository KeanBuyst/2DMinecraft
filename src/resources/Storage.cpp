#include "Storage.h"

#include <SDL_filesystem.h>

using namespace res;

std::string res::basePath = std::string(SDL_GetBasePath()) + "/data/";

DataFile::DataFile(std::string path, const std::_Ios_Openmode &type)
{
	path = basePath + path;
	stream.open(path,std::ios::binary | type);
}

DataFile::~DataFile()
{
	stream.flush();
	stream.close();
}

bool res::DataFile::available() const
{
	return stream.is_open();
}

// writing

void DataFile::write(const uint64_t* data,unsigned int size)
{
	stream.write(reinterpret_cast<const char*>(data),sizeof(uint64_t) * size);
}

void res::DataFile::write(const uint8_t* data, unsigned int size)
{
	stream.write(reinterpret_cast<const char*>(data), sizeof(uint8_t) * size);
}

void DataFile::write(const uint32_t *data, unsigned int size)
{
	stream.write(reinterpret_cast<const char*>(data),sizeof(uint32_t) * size);
}

// reading

void DataFile::read(uint64_t* data,unsigned int size)
{
	stream.read(reinterpret_cast<char*>(data), sizeof(uint64_t) * size);
}

void DataFile::read(uint8_t* data, unsigned int size)
{
	stream.read(reinterpret_cast<char*>(data), sizeof(uint8_t) * size);
}

void DataFile::read(uint32_t *data, unsigned int size)
{
	stream.read(reinterpret_cast<char*>(data), sizeof(uint32_t) * size);
}
