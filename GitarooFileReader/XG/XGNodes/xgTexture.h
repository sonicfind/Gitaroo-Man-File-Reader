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
#include "XG/XG_Nodes.h"
#include "IMX/IMX.h"
class xgTexture : public XGNode
{
	PString m_imxName;
	unsigned long m_mipmap_depth = 0;
	IMX* m_imxPtr = nullptr;

public:
	using XGNode::XGNode;
	unsigned long read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	static bool compareType(const PString& str) { return strcmp("xgTexture", str.m_pstring) == 0; }

	void connectTexture(std::vector<IMX>& textures);
	void generateTextureBuffer();
	void bindTextureBuffer() const;
	void deleteTextureBuffer();
};
