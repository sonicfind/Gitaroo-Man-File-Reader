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
class FixedSprite
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	Val m_junk[4] = { 0 };

	struct Struct64_7f
	{
		unsigned long m_IMXEntryIndex;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long m_transparent;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long m_transparent_2;
		unsigned long m_mipmapDepth;
		unsigned long ulong_k;
		unsigned long ulong_l;
		unsigned long ulong_m;
		unsigned long ulong_n;
		unsigned long ulong_o;
		unsigned long ulong_p;
	} m_64bytes;

	struct Struct48_8f : public Frame
	{
		float m_position[3];
		float m_worldScale_X;
		float m_worldScale_Y;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
	};

	std::vector<Struct48_8f> m_48bytes;

	struct Struct32_6f : public Frame
	{
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};

	std::vector<Struct32_6f> m_32Pair_1;

	struct SpriteFrame : public Frame
	{
		float m_initial_BottmLeft_X;
		float m_initial_BottmLeft_Y;
		float m_boxSize_X;
		float m_boxSize_Y;
		unsigned long ulong_a;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
	};

	std::vector<SpriteFrame> m_spriteFrames;

public:
	FixedSprite(FILE* inFile);
	void create(FILE* outFile);
};
