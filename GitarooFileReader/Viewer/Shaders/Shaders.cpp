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
#include "pch.h"
#include "Shaders.h"
#include "Global_Functions.h"
#include <glad/glad.h>
#include <iostream>

unsigned int Shader::s_activeID = 0;

#ifdef _DEBUG
#include <fstream>
void Shader::createProgram(const char* vertexPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		vertexCode = vertexCode.substr(3, vertexCode.length() - 6);
		fragmentCode = fShaderStream.str();
		fragmentCode = fragmentCode.substr(3, fragmentCode.length() - 6);
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertexShader = 0, fragmentShader = 0;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512] = { 0 };
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		// Stuff
		std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream gShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		gShaderFile.open(geometryPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		gShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		vertexCode = vertexCode.substr(3, vertexCode.length() - 6);
		geometryCode = gShaderStream.str();
		geometryCode = geometryCode.substr(3, geometryCode.length() - 6);
		fragmentCode = fShaderStream.str();
		fragmentCode = fragmentCode.substr(3, fragmentCode.length() - 6);
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertexShader = 0, fragmentShader = 0, geometryShader = 0;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512] = { 0 };
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &gShaderCode, NULL);
	glCompileShader(geometryShader);

	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, geometryShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		// Stuff
		std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
	}
#else
void Shader::createProgram(const char* vertex, const char* fragment)
{
	unsigned int vertexShader = 0, fragmentShader = 0;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512] = { 0 };
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		// Stuff
		std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createProgram(const char* vertex, const char* geometry, const char* fragment)
{
	unsigned int vertexShader = 0, fragmentShader = 0, geometryShader = 0;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512] = { 0 };
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &geometry, NULL);
	glCompileShader(geometryShader);

	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, geometryShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		// Stuff
		std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}
#endif // DEBUG

void Shader::use()
{
	if (s_activeID != ID)
	{
		glUseProgram(ID);
		s_activeID = ID;
	}
}

void Shader::closeProgram()
{
	if (ID)
		glDeleteProgram(ID);
	s_activeID = 0;
}

void Shader::bindUniformBlock(unsigned int bufferIndex, const char* const blockName)
{
	unsigned int uniform_index = glGetUniformBlockIndex(ID, blockName);
	glUniformBlockBinding(ID, uniform_index, bufferIndex);
}

void Shader::bindStorageBlock(unsigned int bufferIndex, const char* const blockName)
{
	unsigned int uniform_index = glGetProgramResourceIndex(ID, GL_SHADER_STORAGE_BLOCK, blockName);
	glShaderStorageBlockBinding(ID, uniform_index, bufferIndex);
}

// utility uniform functions
void Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(s_activeID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(s_activeID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(s_activeID, name.c_str()), value);
}
void Shader::setIVec2(const std::string& name, int* vect, const int size)
{
	glUniform2iv(glGetUniformLocation(s_activeID, name.c_str()), size, vect);
}
void Shader::setVec3(const std::string& name, float* vect, const int size)
{
	glUniform3fv(glGetUniformLocation(s_activeID, name.c_str()), size, vect);
}
void Shader::setVec4(const std::string& name, float* vect, const int size)
{
	glUniform4fv(glGetUniformLocation(s_activeID, name.c_str()), size, vect);
}
void Shader::setMat3(const std::string& name, float* matrix, const int size)
{
	glUniformMatrix3fv(glGetUniformLocation(s_activeID, name.c_str()), size, GL_FALSE, matrix);
}
void Shader::setMat4(const std::string& name, float* matrix, const int size)
{
	glUniformMatrix4fv(glGetUniformLocation(s_activeID, name.c_str()), size, GL_FALSE, matrix);
}

void ShaderCombo::createPrograms(const char* vertexPath, const char* fragmentPath
								, const char* normalsVertexPath, const char* normalsGeometryPath, const char* normalsFragmentPath)
{
	m_base.createProgram(vertexPath, fragmentPath);
	m_normals.createProgram(normalsVertexPath, normalsGeometryPath, normalsFragmentPath);
}

void ShaderCombo::createPrograms(const char* vertexPath, const char* geometryPath, const char* fragmentPath
								, const char* normalsVertexPath, const char* normalsGeometryPath, const char* normalsFragmentPath)
{
	m_base.createProgram(vertexPath, geometryPath, fragmentPath);
	m_normals.createProgram(normalsVertexPath, normalsGeometryPath, normalsFragmentPath);
}

void ShaderCombo::closePrograms()
{
	m_base.closeProgram();
	m_normals.closeProgram();
}

void ShaderCombo::bindUniformBlock(unsigned int bufferIndex, const char* const blockName)
{
	m_base.bindUniformBlock(bufferIndex, blockName);
	m_normals.bindUniformBlock(bufferIndex, blockName);
}

void ShaderCombo::bindStorageBlock(unsigned int bufferIndex, const char* const blockName)
{
	m_base.bindStorageBlock(bufferIndex, blockName);
	m_normals.bindStorageBlock(bufferIndex, blockName);
}

ShaderList g_shaderList;
void ShaderList::createPrograms()
{
	m_baseShaders.createPrograms(base_vert, material_frag, normals_vert, normals_geo, normals_frag);
	m_boneShaders.createPrograms(bones_vert, material_frag, normals_bones_vert, normals_geo, normals_frag);
	m_spriteShaders.createPrograms(sprite_vert, sprite_geo, sprite_frag, sprite_vectors_vert, sprite_vectors_geo, sprite_vectors_frag);
	m_shadowShaders.createPrograms(shadow_vert, shadow_geo, shadow_frag, shadow_vectors_vert, shadow_vectors_geo, shadow_vectors_frag);
	m_lightShader.createProgram(light_vert, light_geo, light_frag);
	m_skyShader.createProgram(sky_vert, sky_frag);
}

void ShaderList::closePrograms()
{
	m_baseShaders.closePrograms();
	m_boneShaders.closePrograms();
	m_spriteShaders.closePrograms();
	m_shadowShaders.closePrograms();
	m_lightShader.closeProgram();
	m_skyShader.closeProgram();
}

void ShaderList::bindUniformBlock(unsigned int bufferIndex, const char* const blockName)
{
	m_baseShaders.bindUniformBlock(bufferIndex, blockName);
	m_boneShaders.bindUniformBlock(bufferIndex, blockName);
	m_spriteShaders.bindUniformBlock(bufferIndex, blockName);
	m_shadowShaders.bindUniformBlock(bufferIndex, blockName);
	m_lightShader.bindUniformBlock(bufferIndex, blockName);
}

void ShaderList::bindStorageBlock(unsigned int bufferIndex, const char* const blockName)
{
	m_baseShaders.bindStorageBlock(bufferIndex, blockName);
	m_boneShaders.bindStorageBlock(bufferIndex, blockName);
	m_spriteShaders.bindStorageBlock(bufferIndex, blockName);
	m_shadowShaders.bindStorageBlock(bufferIndex, blockName);
	m_lightShader.bindStorageBlock(bufferIndex, blockName);
}
