#include "Storage.h"

#include <cstring>
#include <SDL_filesystem.h>

using namespace res;

std::string res::basePath = std::string(SDL_GetBasePath()) + "/data/";

DataFile::DataFile(std::string path, const std::_Ios_Openmode& type) : cursor(0), pending_char(0), pending_count(0)
{
	path = basePath + path;
	stream.open(path,std::ios::binary | type);

	for (size_t i = 0; i < BUFFER_SIZE; ++i)
	{
		internal[i] = 0;
	}
}

DataFile::~DataFile()
{
	flush();
	stream.close();
}

bool DataFile::available() const
{
	return stream.is_open();
}


void DataFile::flush()
{
	if (cursor == 0) return;

	// compress data
	char currentByte = internal[0];
	uint8_t currentLength = 1;

	for (size_t i = 1; i < cursor; ++i)
	{
		if (internal[i] != currentByte || currentLength == 255)
		{
			stream.write(reinterpret_cast<const char*>(&currentLength),1);
			stream.write(&currentByte,1);

			currentLength = 0;
			currentByte = internal[i];
		}
		++currentLength;
	}

	stream.write(reinterpret_cast<const char*>(&currentLength), 1);
	stream.write(&currentByte, 1);

	cursor = 0;
}

void DataFile::write(const char* data, size_t size)
{
	size_t index = 0;
	// overflow handling
	while (size > 0)
	{
		size_t space_left = BUFFER_SIZE - cursor;
		size_t chunk_size = (size < space_left) ? size : space_left;

		std::memcpy(internal + cursor, data + index, chunk_size);

		cursor += chunk_size;
		index += chunk_size;
		size -= chunk_size;

		if (cursor == BUFFER_SIZE)
		{
			flush();
		}
	}
}

void DataFile::read(char* data, size_t size)
{
	size_t count = 0;

	while (pending_count > 0 && count < size)
	{
		data[count++] = pending_char;
		pending_count--;
	}

	while (count < size)
	{
		uint8_t segment[2];
		stream.read(reinterpret_cast<char*>(segment),2);

		if (stream.eof())
		{
			throw "DataFile::read -> Unexpected End Of File";
		}

		size_t space_remaining = size - count;

		if (space_remaining >= segment[0])
		{
			for (size_t i = 0; i < segment[0]; ++i)
			{
				data[count++] = segment[1];
			}
		} else
		{
			for (size_t i = 0; i < space_remaining; ++i)
			{
				data[count++] = segment[1];
			}

			pending_char = segment[1];
			pending_count = segment[0] - space_remaining;
		}
	}
}

bool DataFile::hasNext()
{
	return stream.peek() != EOF || pending_count > 0;
}

int DataFile::getPosition()
{
	return stream.tellg();
}
