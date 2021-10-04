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
void FixedSpriteSetup::read(FILE* inFile)
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

	unsigned long fixed;
	fread(&fixed, 4, 1, inFile);

	m_80bytes.resize(fixed);
	fread(&m_80bytes.front(), sizeof(Struct80_7f), fixed, inFile);

	for (size_t i = 0; i < fixed; ++i)
		m_fixedSprites.emplace_back(inFile);
}

void FixedSpriteSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMF0");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);

	unsigned long fixed = (unsigned long)m_80bytes.size();
	fwrite(&fixed, 4, 1, outFile);
	fwrite(&m_80bytes.front(), sizeof(Struct80_7f), fixed, outFile);

	for (auto& fixed : m_fixedSprites)
		fixed.create(outFile);
}

void FixedSpriteSetup::update(const float frame, std::vector<SpriteValues>& sprites, std::vector<SpriteValues>& sprites_noDepth)
{
	for (size_t index = 0; index < m_80bytes.size(); ++index)
	{
		SpriteValues vals{
			m_80bytes[index].m_IMXindex,
			m_80bytes[index].m_worldPosition,
			m_80bytes[index].m_initial_BottomLeft,
			m_80bytes[index].m_boxSize,
			m_80bytes[index].m_worldSize,
			glm::vec4(1)
		};

		if (m_fixedSprites[index].update(frame, vals))
		{
			if (m_80bytes[index].m_depthTest)
				sprites.push_back(vals);
			else
				sprites_noDepth.push_back(vals);
		}
	}
}
