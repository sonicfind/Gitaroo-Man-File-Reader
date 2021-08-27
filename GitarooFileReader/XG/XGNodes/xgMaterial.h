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
	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList) = 0;
	void create(FILE* outFile, bool full) const = 0;
	void write_to_txt(FILE* txtFile, const char* tabs = "") = 0;
	const char* getType() = 0;
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
	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "");
	const char* getType() { return "xgMaterial"; }
	static bool compare(const PString& str) { return strcmp("xgMaterial", str.m_pstring) == 0; }
