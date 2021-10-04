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
#include "Sprites_Setup.h"
void SpritesSetup::read(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMSP"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMSP' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);

	fread(&m_spritesToDraw, 4, 1, inFile);
	fread(&m_spritesToDraw_NoDepth, 4, 1, inFile);
	fread(&m_unk, 4, 1, inFile);

	fread(m_junk, 1, 16, inFile);

	fread(&m_numFixedSprites, 4, 1, inFile);
	fread(&m_numUnkSprites_1, 4, 1, inFile);
	fread(&m_numUnkSprites_2, 4, 1, inFile);
	fread(&m_unused, 4, 1, inFile);

	if (m_numFixedSprites)
		m_fixedSpriteSetup.read(inFile);

	if (m_numUnkSprites_1)
		m_unk1SpriteSetup.read(inFile);

	if (m_numUnkSprites_2)
		m_unk2SpriteSetup.read(inFile);
}

void SpritesSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMSP");

	fwrite(&m_headerVersion, 4, 1, outFile);

	fwrite(&m_spritesToDraw, 4, 1, outFile);
	fwrite(&m_spritesToDraw_NoDepth, 4, 1, outFile);
	fwrite(&m_unk, 4, 1, outFile);

	fwrite(m_junk, 1, 16, outFile);

	fwrite(&m_numFixedSprites, 4, 1, outFile);
	fwrite(&m_numUnkSprites_1, 4, 1, outFile);
	fwrite(&m_numUnkSprites_2, 4, 1, outFile);
	fwrite(&m_unused, 4, 1, outFile);

	if (m_numFixedSprites)
		m_fixedSpriteSetup.create(outFile);

	if (m_numUnkSprites_1)
		m_unk1SpriteSetup.create(outFile);

	if (m_numUnkSprites_2)
		m_unk2SpriteSetup.create(outFile);
}

#include <glad/glad.h>
void SpritesSetup::generateSpriteBuffer()
{
	if (m_numFixedSprites || m_numUnkSprites_1 || m_numUnkSprites_2)
	{
		glGenBuffers(1, &m_spriteVBO);
		glGenVertexArrays(1, &m_spriteVAO);
		glBindVertexArray(m_spriteVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(SpriteValues), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(1 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(4 * sizeof(float)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(6 * sizeof(float)));
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(8 * sizeof(float)));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteValues), (void*)(10 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		glBufferData(GL_ARRAY_BUFFER, ((size_t)m_numFixedSprites + m_numUnkSprites_1 + m_numUnkSprites_2) * sizeof(SpriteValues), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void SpritesSetup::deleteSpriteBuffer()
{
	if (m_spriteVAO)
	{
		glDeleteVertexArrays(1, &m_spriteVAO);
		glDeleteBuffers(1, &m_spriteVBO);
		m_spriteVAO = 0;
		m_spriteVBO = 0;

		m_spritesToDraw = 0;
		m_spritesToDraw_NoDepth = 0;
		m_unk = 0;
	}
}

void SpritesSetup::updateSprites(const float frame)
{
	if (m_spriteVAO)
	{
		std::vector<SpriteValues> sprites, noDepth;
		if (m_numFixedSprites)
			m_fixedSpriteSetup.update(frame, sprites, noDepth);

		/*if (m_numUnkSprites_1)
			m_unk1SpriteSetup.update(frame, values);

		if (m_numUnkSprites_2)
			m_unk2SpriteSetup.update(frame, values);*/

		glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sprites.size() * sizeof(SpriteValues), sprites.data());
		glBufferSubData(GL_ARRAY_BUFFER, sprites.size() * sizeof(SpriteValues), noDepth.size() * sizeof(SpriteValues), noDepth.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_spritesToDraw = (unsigned long)sprites.size();
		m_spritesToDraw_NoDepth = (unsigned long)noDepth.size();
	}
}

bool SpritesSetup::hasBuffers()
{
	return m_spriteVAO > 0;
}

void SpritesSetup::draw()
{
	if (m_spriteVAO)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
		glBindVertexArray(m_spriteVAO);
		if (m_spritesToDraw)
		{
			glEnable(GL_DEPTH_TEST);
			glDrawArrays(GL_POINTS, 0, m_spritesToDraw);
		}

		if (m_spritesToDraw_NoDepth)
		{
			glDisable(GL_DEPTH_TEST);
			glDrawArrays(GL_POINTS, m_spritesToDraw, m_spritesToDraw_NoDepth);
			glEnable(GL_DEPTH_TEST);
		}

	}
}
