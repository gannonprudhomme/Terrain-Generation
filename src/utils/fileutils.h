#pragma once

#include <string>

namespace framework {
	class FileUtils {
		public:
		static std::string read_file(const char* filepath) {
			//C input/output library
			FILE *file = fopen(filepath, "rt"); //Read-only as a Text file
			fseek(file, 0, SEEK_END); //very end of the file
			unsigned long length = ftell(file);
			char *data = new char[length + 1]; //like a normal string
			memset(data, 0, length + 1);
			fseek(file, 0, SEEK_SET); //file position is back at the begining
			fread(data, 1, length, file);
			fclose(file);

			std::string result(data);
			delete[] data; //why does the [] have to be here
			//data wont be garbage collected bc its a pointer
			return result;
		}
	};
}