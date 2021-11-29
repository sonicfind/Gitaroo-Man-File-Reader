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
#include "pch.h"
#include "FixedSprite_Setup.h"
FixedSpriteSetup::FixedSpriteSetup(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMF0"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMF0' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);

	fread(&m_numSprites, 4, 1, inFile);

	m_80bytes.resize(m_numSprites);
	fread(&m_80bytes.front(), sizeof(Struct80_7f), m_numSprites, inFile);

	for (size_t i = 0; i < m_numSprites; ++i)
		m_fixedSprites.emplace_back(inFile);
}

void FixedSpriteSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMF0");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);

	fwrite(&m_numSprites, 4, 1, outFile);
	fwrite(&m_80bytes.front(), sizeof(Struct80_7f), m_numSprites, outFile);

	for (auto& fixed : m_fixedSprites)
		fixed.create(outFile);
}

#include <glad/glad.h>
void FixedSpriteSetup::generateSpriteBuffer()
{
	glGenBuffers(1, &m_fixedSpriteVBO);
	glGenVertexArrays(1, &m_fixedSpriteVAO);
	glBindVertexArray(m_fixedSpriteVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_fixedSpriteVBO);
	// Texture index
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(SpriteValues), (void*)0);
	// Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(1 * sizeof(float)));
	// Base tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(4 * sizeof(float)));
	// Texture offsets
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(6 * sizeof(float)));
	// Size in world space
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(8 * sizeof(float)));
	// Color multipliers
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(10 * sizeof(float)));
	// Color Algorithm
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(SpriteValues), (void*)(14 * sizeof(float)));
	// Blend type
	glVertexAttribIPointer(7, 1, GL_UNSIGNED_INT, sizeof(SpriteValues), (void*)(15 * sizeof(uint32_t)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);

	glBufferData(GL_ARRAY_BUFFER, m_numSprites * sizeof(SpriteValues), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FixedSpriteSetup::deleteSpriteBuffer()
{
	glDeleteVertexArrays(1, &m_fixedSpriteVAO);
	glDeleteBuffers(1, &m_fixedSpriteVBO);
}

void FixedSpriteSetup::jumpToFrame(const float frame)
{
	for (auto& fixed : m_fixedSprites)
		fixed.jumpToFrame(frame);
}

void FixedSpriteSetup::update(const float frame)
{
	m_spritesToDraw.clear();
	m_depthlessDraws.clear();
	for (size_t index = 0; index < m_80bytes.size(); ++index)
	{
		if (m_80bytes[index].modelTypeMapping < ModelType::Player1AttDef ||
			g_gameState.isModelTypeActive(static_cast<int>(m_80bytes[index].modelTypeMapping)))
		{
			SpriteValues vals{
				m_80bytes[index].IMXindex,
				m_80bytes[index].worldPosition,
				m_80bytes[index].initial_BottomLeft,
				m_80bytes[index].boxSize,
				m_80bytes[index].worldSize,
				glm::vec4(1),
				2,
				// Additive blending default
				1,
			};

			if (m_fixedSprites[index].update(frame, vals))
			{
				if (m_80bytes[index].depthTest)
					m_spritesToDraw.push_back(vals);
				else
					m_depthlessDraws.push_back(vals);
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_fixedSpriteVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_spritesToDraw.size() * sizeof(SpriteValues), m_spritesToDraw.data());
	glBufferSubData(GL_ARRAY_BUFFER, m_spritesToDraw.size() * sizeof(SpriteValues), m_depthlessDraws.size() * sizeof(SpriteValues), m_depthlessDraws.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FixedSpriteSetup::draw(const bool doTransparents)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_fixedSpriteVBO);
	glBindVertexArray(m_fixedSpriteVAO);

	unsigned long first = 0;
	while (first < m_spritesToDraw.size())
	{
		const int blend = m_spritesToDraw[first].blendType;
		const int alg = m_spritesToDraw[first].colorAlg;
		bool tmp = blend > 0;
		if (tmp == doTransparents)
		{
			size_t count = 1;
			while (first + count < m_spritesToDraw.size() &&
				blend == m_spritesToDraw[first + count].blendType &&
				alg == m_spritesToDraw[first + count].colorAlg)
				++count;
			drawSprite(first, count, blend, alg);
			first += (unsigned long)count;
		}
		else
			++first;
	}

	glDisable(GL_DEPTH_TEST);
	for (unsigned long i = 0; i < m_depthlessDraws.size();)
	{
		const int blend = m_depthlessDraws[i].blendType;
		const int alg = m_depthlessDraws[i].colorAlg;
		bool tmp = blend > 0;
		if (tmp == doTransparents)
		{
			size_t count = 1;
			while (i + count < m_depthlessDraws.size() &&
				blend == m_depthlessDraws[i + count].blendType &&
				alg == m_depthlessDraws[i + count].colorAlg)
				++count;
			drawSprite(first + i, count, blend, alg);
			i += (unsigned long)count;
		}
		else
			++i;
	}
	glEnable(GL_DEPTH_TEST);
}

void FixedSpriteSetup::drawSprite(const unsigned long first, const size_t count, const unsigned long blend, const unsigned long alg)
{
	switch (blend)
	{
	case 1:
		if (alg == 1)
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
		else
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
		break;
	case 2:
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
		break;
	case 3:
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		break;
	default:
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	}
	glDrawArrays(GL_POINTS, first, unsigned(count));
}
