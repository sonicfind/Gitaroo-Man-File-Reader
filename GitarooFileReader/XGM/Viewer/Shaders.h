#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020 Gitaroo Pals
 *
 *  Gitaroo Man File Reader is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  Gitaroo Man File Reader is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Gitaroo Man File Reader.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
class Shader
{
	// the program ID
	unsigned int ID = 0;
	static unsigned int s_activeID;

public:
	// constructor reads and builds the shader
	void createProgram(const char* vertexPath, const char* fragmentPath);

	// constructor reads and builds the shader
	void createProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

	operator bool() { return ID > 0; }

	void use();
	void closeProgram();

	void bindUniformBlock(unsigned int bufferIndex, const char* const blockName);
	void bindStorageBlock(unsigned int bufferIndex, const char* const blockName);

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setIVec2(const std::string& name, int* vect, const int size = 1) const;
	void setVec3(const std::string& name, float* vect, const int size = 1) const;
	void setVec4(const std::string& name, float* vect, const int size = 1) const;
	void setMat3(const std::string& name, float* matrix, const int size = 1) const;
	void setMat4(const std::string& name, float* matrix, const int size = 1) const;
};

struct ShaderCombo
{
	Shader m_base;
	Shader m_normals;
	void createPrograms(const char* vertexPath, const char* fragmentPath
						, const char* normalsVertexPath, const char* normalsGeometryPath, const char* normalsFragmentPath);
	void createPrograms(const char* vertexPath, const char* geometryPath, const char* fragmentPath
						, const char* normalsVertexPath, const char* normalsGeometryPath, const char* normalsFragmentPath);
	void closePrograms();
	void bindUniformBlock(unsigned int bufferIndex, const char* const blockName);
	void bindStorageBlock(unsigned int bufferIndex, const char* const blockName);
};

extern ShaderCombo g_shaders;
extern ShaderCombo g_boneShaders;
extern ShaderCombo g_spriteShaders;
