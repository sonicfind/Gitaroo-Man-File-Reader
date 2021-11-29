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
#include "IMX/IMX.h"
class TexAnim
{
public:
	struct CutOut
	{
		glm::vec2 topLeft;
		glm::vec2 bottomRight;
		unsigned char* subImage;
	};

	struct TexFrame : public Frame
	{
		unsigned long cutOutIndex;
		unsigned long unknown;
	};
private:
	// 
	unsigned long m_headerVersion;

	char m_unk1[12] = { 0 };

	Val m_junk[4] = { 0 };

	glm::u32vec2 m_offset;

	char m_textureName[24] = { 0 };
	
	IMX* m_imxPtr = nullptr;
	unsigned long m_bytesPerPixel = 0;

	std::vector<CutOut> m_cutOuts;
	size_t m_cutOutIndex = 0;
	IteratedVector<TexFrame> m_textureFrames;
public:
	
	TexAnim(FILE* inFile);
	void create(FILE* outFile);
	const char* getTextureName() { return m_textureName; }
	void connectTexture(IMX* imx);
	// It's imperative that the subImages are re-loaded on each viewer session
	// since the actual image data held in the IMX object can be swapped out
	void loadCuts();
	void unloadCuts();
	void substitute(const float frame);
	void apply();
};
