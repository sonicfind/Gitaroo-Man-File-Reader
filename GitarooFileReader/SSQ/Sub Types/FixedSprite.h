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
struct SpriteValues
{
	unsigned int index;
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec2 texOffsets;
	glm::vec2 worldSize;
	glm::vec4 colorMultipliers;
	unsigned int blendType;
};

class FixedSprite
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	Val m_junk[4] = { 0 };

	struct Struct64_7f
	{
		unsigned long IMXEntryIndex;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long transparent;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long blendingType;
		unsigned long mipmapDepth;
		unsigned long ulong_k;
		unsigned long ulong_l;
		unsigned long ulong_m;
		unsigned long ulong_n;
		unsigned long ulong_o;
		unsigned long ulong_p;
	} m_64bytes;

	struct SpriteWorldValues : public Frame
	{
		glm::vec3 position;
		glm::vec2 worldSize;
		unsigned long noDrawing;
		unsigned long ulong_b;
		unsigned long doInterpolation;
		unsigned long ulong_d;
		// Essentially m_frame * 160
		unsigned long otherPos;
	};

	std::vector<SpriteWorldValues> m_48bytes;

	struct ColorMultipliers : public Frame
	{
		glm::vec4 colors;
		unsigned long ulong_a;
		unsigned long doInterpolation;
	};

	std::vector<ColorMultipliers> m_colors;

	struct SpriteFrame : public Frame
	{
		glm::vec2 initial_BottomLeft;
		glm::vec2 boxSize;
		unsigned long doInterpolation;
		// Essentially m_frame * 160
		unsigned long otherPos;
	};

	std::vector<SpriteFrame> m_spriteFrames;

public:
	FixedSprite(FILE* inFile);
	void create(FILE* outFile);

	bool update(const float frame, SpriteValues& values);
};
