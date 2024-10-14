#pragma once

#include <fstream>
#include <glm.hpp>

#include "Chunk.h"

namespace res
{
	extern std::string basePath;

	class DataFile
	{
	public:
		explicit DataFile(std::string path,std::_Ios_Openmode type);
		~DataFile();

		bool available();

		void write(const uint64_t* data,unsigned int size);
		void write(const uint8_t* data, unsigned int size);

		void read(uint64_t* data,unsigned int size);
		void read(uint8_t* data, unsigned int size);
	private:
		std::fstream stream;
	};

	
}