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
#include "FixedSprite.h"
FixedSprite::FixedSprite(FILE* inFile)
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
	fread(&m_64bytes, sizeof(Struct64_7f), 1, inFile);

	unsigned long num48, pair1, numFrames;
	fread(&num48, 4, 1, inFile);
	fread(&pair1, 4, 1, inFile);
	fread(&numFrames, 4, 1, inFile);

	if (num48 > 1)
	{
		m_48bytes.resize(num48);
		fread(&m_48bytes.front(), sizeof(SpriteWorldValues), num48, inFile);
	}

	if (pair1 > 1)
	{
		m_colors.resize(pair1);
		fread(&m_colors.front(), sizeof(ColorMultipliers), pair1, inFile);
	}

	if (numFrames > 1)
	{
		m_spriteFrames.resize(numFrames);
		fread(&m_spriteFrames.front(), sizeof(SpriteFrame), numFrames, inFile);
	}
}

void FixedSprite::create(FILE* outFile)
{
	fprintf(outFile, "GMSP");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_64bytes, sizeof(Struct64_7f), 1, outFile);

	unsigned long num48 = (unsigned long)m_48bytes.size(), colors = (unsigned long)m_colors.size(), numFrames = (unsigned long)m_spriteFrames.size();
	if (!num48)
		num48 = 1;

	if (!colors)
		colors = 1;

	if (!numFrames)
		numFrames = 1;

	fwrite(&num48, 4, 1, outFile);
	fwrite(&colors, 4, 1, outFile);
	fwrite(&numFrames, 4, 1, outFile);

	if (num48 > 1)
		fwrite(&m_48bytes.front(), sizeof(SpriteWorldValues), num48, outFile);

	if (colors > 1)
		fwrite(&m_colors.front(), sizeof(ColorMultipliers), colors, outFile);

	if (numFrames > 1)
		fwrite(&m_spriteFrames.front(), sizeof(SpriteFrame), numFrames, outFile);
}

#include <math.h>
bool FixedSprite::update(const float frame, SpriteValues& values)
{
	if (!m_48bytes.empty())
	{
		auto iter = getIter(m_48bytes, frame);
		if (iter->noDrawing)
			return false;

		if (!iter->doInterpolation || iter + 1 == m_48bytes.end())
		{
			values.position = iter->position;
			values.worldSize = iter->worldSize;
		}
		else
		{
			const float coefficient = (frame - iter->frame) * iter->coefficient;
			values.position = glm::mix(iter->position, (iter + 1)->position, coefficient);
			values.worldSize = glm::mix(iter->worldSize, (iter + 1)->worldSize, coefficient);
		}
	}

	if (!m_colors.empty())
	{
		auto iter = getIter(m_colors, frame);
		if (!iter->doInterpolation || iter + 1 == m_colors.end())
			values.colorMultipliers = iter->colors;
		else
			values.colorMultipliers = glm::mix(iter->colors, (iter + 1)->colors, (frame - iter->frame) * iter->coefficient);
	}

	if (!m_spriteFrames.empty())
	{
		const float spriteFrame = fmod(frame, float(m_spriteFrames.size()));
		auto iter = m_spriteFrames.begin() + (size_t)spriteFrame;
		if (!iter->doInterpolation)
		{
			values.texCoord = iter->initial_BottomLeft;
			values.texOffsets = iter->boxSize;
		}
		else
		{
			const float coefficient = (spriteFrame - (size_t)spriteFrame) * iter->coefficient;
			if (iter + 1 != m_spriteFrames.end())
			{
				values.texCoord = glm::mix(iter->initial_BottomLeft, (iter + 1)->initial_BottomLeft, coefficient);
				values.texOffsets = glm::mix(iter->boxSize, (iter + 1)->boxSize, coefficient);
			}
			else
			{
				values.texCoord = glm::mix(iter->initial_BottomLeft, m_spriteFrames.front().initial_BottomLeft, coefficient);
				values.texOffsets = glm::mix(iter->boxSize, m_spriteFrames.front().boxSize, coefficient);
			}
		}
	}

	if (m_64bytes.transparent)
		values.blendType = m_64bytes.blendingType;
	else
		values.blendType = 0;
	return true;
}
