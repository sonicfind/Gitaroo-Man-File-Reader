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
#include "xgTexture.h"
#include "XGM/Viewer/Shaders.h"
class MaterialNode : public XGNode
{
public:
	using XGNode::XGNode;
	virtual bool hasTransparency() const = 0;
	virtual void intializeBuffers() = 0;
	virtual void deleteBuffers() = 0;
	virtual void setShaderValues(Shader* shader, const std::string index) const = 0;
};

class xgMaterial : public MaterialNode
{
	unsigned long m_blendType = 0;
	unsigned long m_shadingType = 0;
	struct Diffuse
	{
		float red = 0;
		float green = 0;
		float blue = 0;
		float alpha = 1;
	} m_diffuse;
	struct Specular
	{
		float red = 0;
		float green = 0;
		float blue = 0;
		float exponent = 0;
	} m_specular;
	unsigned long m_flags = 0;
	unsigned long m_textureEnv = 0;
	unsigned long m_uTile = 0;
	unsigned long m_vTile = 0;
	SharedNode<xgTexture> m_inputTexture;

public:
	using MaterialNode::MaterialNode;
	unsigned long read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	static bool compareType(const PString& str) { return strcmp("xgMaterial", str.m_pstring) == 0; }
	bool hasTransparency() const;

	void connectTexture(std::vector<IMX>& textures);
	void intializeBuffers();
	void deleteBuffers();
	void setShaderValues(Shader* shader, const std::string index) const;
};
