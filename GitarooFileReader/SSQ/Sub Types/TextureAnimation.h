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
#include "SSQ_BaseStructs.h"
class TexAnim
{
public:
	struct CutOut
	{
		glm::vec2 m_topLeft;
		glm::vec2 m_bottomRight;
	};

	struct TexFrame : public Frame
	{
		unsigned long m_cutOutIndex;
		unsigned long m_unknown;
	};
private:
	// 
	unsigned long m_headerVersion;

	char m_unk1[12] = { 0 };

	Val m_junk[4] = { 0 };

	glm::u32vec2 m_offset;

	char m_texture[24] = { 0 };

public:
	std::vector<CutOut> m_cutOuts;
	std::vector<TexFrame> m_textureFrames;

	TexAnim(FILE* inFile);
	void create(FILE* outFile);
};
