#pragma once

#include <fstream>
#include <glm/glm.hpp>

namespace res
{
	extern std::string basePath;

	class DataFile
	{
	private:
		static constexpr size_t BUFFER_SIZE = 1024; // 1 KB
		char internal[BUFFER_SIZE];
		size_t cursor;
		std::fstream stream;
		char pending_char;
		uint8_t pending_count;
	public:
		explicit DataFile(std::string path,const std::_Ios_Openmode &type);
		~DataFile();

		bool available() const;

		void flush();
		void write(const char* data,size_t size);

		void read(char* data,size_t size);

		template<typename T>
		void write(const T* data,size_t size);

		template<typename T>
		void read(T* data,size_t size);

		bool hasNext();
		int getPosition();
	};

	template <typename T>
	void DataFile::write(const T* data, size_t size)
	{
		write(reinterpret_cast<const char*>(data), size * sizeof(T));
	}

	template <typename T>
	void DataFile::read(T* data, size_t size)
	{
		read(reinterpret_cast<char*>(data), size * sizeof(T));
	}
}
