#pragma once
#include <string>

class Shader
{
public:
	// the program ID
	unsigned int ID = 0;
	static unsigned activeID;

	// constructor reads and builds the shader
	void createProgram(const char* vertexPath, const char* fragmentPath);

	// constructor reads and builds the shader
	void createProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

	operator bool() { return ID > 0; }

	void use();
	void closeProgram();

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, float* vect, const size_t size = 1) const;
	void setVec4(const std::string& name, float* vect, const size_t size = 1) const;
	void setMat4(const std::string& name, float* matrix, const size_t size = 1) const;
};
