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
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	Val m_junk[4] = { 0 };

	struct Struct80_7f
	{
		unsigned long IMXindex;
		glm::vec2 initial_BottomLeft;
		glm::vec2 boxSize;
		glm::vec3 worldPosition;
		glm::vec2 worldSize;
		glm::vec2 worldSize_ZW_maybe;
		float float_l;
		unsigned long ulong_b;
		float float_m;
		unsigned long ulong_c;
		unsigned long depthTest;
		ModelType modelTypeMapping = ModelType::Normal;
		unsigned long ulong_f;
		unsigned long ulong_g;
	};

	unsigned long m_numSprites;

	std::vector<Struct80_7f> m_80bytes;
	std::vector<FixedSprite> m_fixedSprites;

	unsigned m_fixedSpriteVAO;
	unsigned m_fixedSpriteVBO;
	std::vector<SpriteValues> m_spritesToDraw;
	std::vector<SpriteValues> m_depthlessDraws;

public:
	FixedSpriteSetup(FILE* inFile);
	void create(FILE* outFile);
	void generateSpriteBuffer();
	void deleteSpriteBuffer();
	void update(const float frame);
	void draw(const bool doTransparents);

private:
	void drawSprite(const unsigned long first, const size_t count, const unsigned long blend, const unsigned long alg);
};
