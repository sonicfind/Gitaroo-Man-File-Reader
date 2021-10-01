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
	fwrite(m_unk, 1, 12, outFile);
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
