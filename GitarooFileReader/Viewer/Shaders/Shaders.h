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
	void createProgram(const char* vertex, const char* fragment);

	// constructor reads and builds the shader
	void createProgram(const char* vertex, const char* geometry, const char* fragment);

	operator bool() { return ID > 0; }

	void use();
	void closeProgram();

	void bindUniformBlock(unsigned int bufferIndex, const char* const blockName);
	void bindStorageBlock(unsigned int bufferIndex, const char* const blockName);

	// utility uniform functions
	static void setBool(const std::string& name, bool value);
	static void setInt(const std::string& name, int value);
	static void setFloat(const std::string& name, float value);
	static void setIVec2(const std::string& name, int* vect, const int size = 1);
	static void setVec3(const std::string& name, float* vect, const int size = 1);
	static void setVec4(const std::string& name, float* vect, const int size = 1);
	static void setMat3(const std::string& name, float* matrix, const int size = 1);
	static void setMat4(const std::string& name, float* matrix, const int size = 1);
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

class ShaderList
{
	// For debug, load in the files at runtime to make testing
	// shader changes simpler to do without a rebuild
	//
	// Release will have the shaders embedded in the exe
#ifdef _DEBUG
	const char* base_vert = "Viewer/Shaders/base.vert";
	const char* bones_vert = "Viewer/Shaders/bones.vert";
	const char* material_frag = "Viewer/Shaders/material.frag";
	const char* normals_vert = "Viewer/Shaders/normals.vert";
	const char* normals_bones_vert = "Viewer/Shaders/normals - bones.vert";
	const char* normals_geo = "Viewer/Shaders/normals.geo";
	const char* normals_frag = "Viewer/Shaders/normals.frag";
	const char* sprite_vert = "Viewer/Shaders/sprite.vert";
	const char* sprite_geo = "Viewer/Shaders/sprite.geo";
	const char* sprite_frag = "Viewer/Shaders/sprite.frag";
	const char* sprite_vectors_vert = "Viewer/Shaders/sprite - vectors.vert";
	const char* sprite_vectors_geo = "Viewer/Shaders/sprite - vectors.geo";
	const char* sprite_vectors_frag = "Viewer/Shaders/sprite - vectors.frag";
	const char* shadow_vert = "Viewer/Shaders/shadow.vert";
	const char* shadow_geo = "Viewer/Shaders/shadow.geo";
	const char* shadow_frag = "Viewer/Shaders/shadow.frag";
	const char* shadow_vectors_vert = "Viewer/Shaders/shadow - vectors.vert";
	const char* shadow_vectors_geo = "Viewer/Shaders/shadow - vectors.geo";
	const char* shadow_vectors_frag = "Viewer/Shaders/shadow - vectors.frag";
	const char* light_vert = "Viewer/Shaders/light.vert";
	const char* light_geo = "Viewer/Shaders/light.geo";
	const char* light_frag = "Viewer/Shaders/light.frag";
	const char* sky_vert = "Viewer/Shaders/sky.vert";
	const char* sky_geo =  "Viewer/Shaders/sky.geo";
	const char* sky_frag = "Viewer/Shaders/sky.frag";
#else
	const char* base_vert =
	#include "base.vert"
	;
	const char* bones_vert =
	#include "bones.vert"
	;
	const char* material_frag =
	#include "material.frag"
	;
	const char* normals_vert =
	#include "normals.vert"
	;
	const char* normals_bones_vert =
	#include "normals - bones.vert"
	;
	const char* normals_geo =
	#include "normals.geo"
	;
	const char* normals_frag =
	#include "normals.frag"
	;
	const char* sprite_vert =
	#include "sprite.vert"
	;
	const char* sprite_geo =
	#include "sprite.geo"
	;
	const char* sprite_frag =
	#include "sprite.frag"
	;
	const char* sprite_vectors_vert =
	#include "sprite - vectors.vert"
	;
	const char* sprite_vectors_geo =
	#include "sprite - vectors.geo"
	;
	const char* sprite_vectors_frag =
	#include "sprite - vectors.frag"
	;
	const char* shadow_vert =
	#include "shadow.vert"
	;
	const char* shadow_geo =
	#include "shadow.geo"
	;
	const char* shadow_frag =
	#include "shadow.frag"
	;
	const char* shadow_vectors_vert =
	#include "shadow - vectors.vert"
	;
	const char* shadow_vectors_geo =
	#include "shadow - vectors.geo"
	;
	const char* shadow_vectors_frag =
	#include "shadow - vectors.frag"
	;
	const char* light_vert =
	#include "light.vert"
	;
	const char* light_geo =
	#include "light.geo"
	;
	const char* light_frag =
	#include "light.frag"
	;
	const char* sky_vert =
	#include "sky.vert"
	;
	const char* sky_geo =
	#include "sky.geo"
	;
	const char* sky_frag =
	#include "sky.frag"
	;
#endif
public:
	ShaderCombo m_baseShaders;
	ShaderCombo m_boneShaders;
	ShaderCombo m_spriteShaders;
	ShaderCombo m_shadowShaders;
	Shader m_lightShader;
	Shader m_skyShader;

	void createPrograms();
	void closePrograms();
	void bindUniformBlock(unsigned int bufferIndex, const char* const blockName);
	void bindStorageBlock(unsigned int bufferIndex, const char* const blockName);
};
extern ShaderList g_shaderList;
