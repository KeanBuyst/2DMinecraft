#pragma once

#include <fstream>
#include <glm.hpp>

namespace res
{
	extern std::string basePath;

	class DataFile
	{
	public:
		explicit DataFile(std::string path,const std::_Ios_Openmode &type);
		~DataFile();

		bool available() const;

		void write(const uint64_t* data,unsigned int size);
		void write(const uint8_t* data, unsigned int size);
		void write(const uint32_t* data, unsigned int size);

		void read(uint64_t* data,unsigned int size);
		void read(uint8_t* data, unsigned int size);
		void read(uint32_t* data, unsigned int size);
	private:
		std::fstream stream;
	};

	
}