#ifndef FILE_H_
#define FILE_H_
#include <vector>
#include<chrono>
#include<string>
namespace base
{
	namespace File
	{
		class FileUtil
		{
		public:
			static std::string getCurrentAppPath();
			static size_t getFileSize(FILE* file);
			static size_t getFileSize(const char* filePath);
			static std::vector<unsigned char> readFromFile(const char* filePath);
		};
	}


} // namespace

#endif // FILE_H_