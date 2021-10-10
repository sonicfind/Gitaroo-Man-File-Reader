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

	fread(m_unk, 1, 12, inFile);

	fread(m_junk, 1, 16, inFile);

	fread(&m_numFixedSprites, 4, 1, inFile);
	fread(&m_numUnkSprites_1, 4, 1, inFile);
	fread(&m_numUnkSprites_2, 4, 1, inFile);
	fread(&m_unused, 4, 1, inFile);

	if (m_numFixedSprites)
		m_fixedSpriteSetup = std::make_unique<FixedSpriteSetup>(inFile);

	if (m_numUnkSprites_1)
		m_unk1SpriteSetup = std::make_unique<Unk1SpriteSetup>(inFile);

	if (m_numUnkSprites_2)
		m_unk2SpriteSetup = std::make_unique<Unk2SpriteSetup>(inFile);
}

void SpritesSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMSP");

	fwrite(&m_headerVersion, 4, 1, outFile);

	fwrite(m_unk, 1, 12, outFile);

	fwrite(m_junk, 1, 16, outFile);

	fwrite(&m_numFixedSprites, 4, 1, outFile);
	fwrite(&m_numUnkSprites_1, 4, 1, outFile);
	fwrite(&m_numUnkSprites_2, 4, 1, outFile);
	fwrite(&m_unused, 4, 1, outFile);

	if (m_fixedSpriteSetup)
		m_fixedSpriteSetup->create(outFile);

	if (m_unk1SpriteSetup)
		m_unk1SpriteSetup->create(outFile);

	if (m_unk2SpriteSetup)
		m_unk2SpriteSetup->create(outFile);
}

bool SpritesSetup::hasSprites()
{
	return m_fixedSpriteSetup || m_unk1SpriteSetup || m_unk2SpriteSetup;
}

void SpritesSetup::generateSpriteBuffers()
{
	if (m_fixedSpriteSetup)
		m_fixedSpriteSetup->generateSpriteBuffer();

	/*if (m_unk1SpriteSetup)
		m_unk1SpriteSetup->generateSpriteBuffer();

	if (m_unk2SpriteSetup)
		m_unk2SpriteSetup->generateSpriteBuffer();*/
}

void SpritesSetup::deleteSpriteBuffers()
{
	if (m_fixedSpriteSetup)
		m_fixedSpriteSetup->deleteSpriteBuffer();

	/*if (m_unk1SpriteSetup)
		m_unk1SpriteSetup->deleteSpriteBuffer();

	if (m_unk2SpriteSetup)
		m_unk2SpriteSetup->deleteSpriteBuffer();*/
}

void SpritesSetup::update(const float frame)
{
	if (m_fixedSpriteSetup)
		m_fixedSpriteSetup->update(frame);

	/*if (m_unk1SpriteSetup)
		m_unk1SpriteSetup->update(frame);

	if (m_unk2SpriteSetup)
		m_unk2SpriteSetup->update(frame);*/
}

void SpritesSetup::draw(const bool doTransparents)
{
	if (m_fixedSpriteSetup)
		m_fixedSpriteSetup->draw(doTransparents);

	/*if (m_unk1SpriteSetup)
		m_unk1SpriteSetup->draw(doTransparents);

	if (m_unk2SpriteSetup)
		m_unk2SpriteSetup->draw(doTransparents);*/
}
