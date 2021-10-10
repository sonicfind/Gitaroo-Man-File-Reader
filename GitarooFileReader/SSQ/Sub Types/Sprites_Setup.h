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
#include "FixedSprite_Setup.h"
#include "UnkSprite_Setups.h"
class SpritesSetup
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	Val m_junk[4] = { 0 };

	unsigned long m_numFixedSprites;
	unsigned long m_numUnkSprites_1;
	unsigned long m_numUnkSprites_2;
	unsigned long m_unused;

	std::unique_ptr<FixedSpriteSetup> m_fixedSpriteSetup;
	std::unique_ptr<Unk1SpriteSetup> m_unk1SpriteSetup;
	std::unique_ptr<Unk2SpriteSetup> m_unk2SpriteSetup;

public:
	void read(FILE* inFile);
	void create(FILE* outFile);

	void generateSpriteBuffers();
	void deleteSpriteBuffers();
	bool hasSprites();
	void update(const float frame);
	void draw(const bool doTransparents);
};
