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
#include "FixedSprite.h"
class FixedSpriteSetup
{
	bool m_used = false;
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	Val m_junk[4] = { 0 };

	struct Struct80_7f
	{
		unsigned long m_IMXindex;
		float m_initial_BottmLeft_X;
		float m_initial_BottmLeft_Y;
		float m_boxSize_X;
		float m_boxSize_Y;
		float m_worldPosition_X;
		float m_worldPosition_Y;
		float m_worldPosition_Z;
		float m_worldScale_X;
		float m_worldScale_Y;
		float m_worldScale_Z_maybe;
		float m_worldScale_W_maybe;
		float float_l;
		unsigned long ulong_b;
		float float_m;
		unsigned long ulong_c;
		unsigned long m_depthTest;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
	};

	std::vector<Struct80_7f> m_80bytes;

	std::vector<FixedSprite> m_fixedSprites;

public:
	bool used() { return m_used; }
	void read(FILE* inFile);
	void create(FILE* outFile);
};
