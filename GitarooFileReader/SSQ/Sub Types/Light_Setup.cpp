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
#include "Light_Setup.h"
LightSetup::LightSetup(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(&m_baseValues, sizeof(BaseValues), 1, inFile);
	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMLT"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMLT' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_size, 4, 1, inFile);
	fread(m_unk, 1, 8, inFile);
	fread(m_junk, 1, 16, inFile);

	unsigned long numRotations, numColors;
	fread(&numRotations, 4, 1, inFile);
	fread(&numColors, 4, 1, inFile);

	if (numRotations > 1)
	{
		m_rotations.resize(numRotations);
		fread(&m_rotations.front(), sizeof(Rotation), numRotations, inFile);
	}

	if (numColors > 1)
	{
		m_colors.resize(numColors);
		fread(&m_colors.front(), sizeof(LightColors), numColors, inFile);
	}
}

void LightSetup::create(FILE* outFile)
{
	fwrite(&m_baseValues, sizeof(BaseValues), 1, outFile);
	fprintf(outFile, "GMLT");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_size, 4, 1, outFile);
	fwrite(m_unk, 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);

	unsigned long numRotations = (unsigned long)m_rotations.size(), numColors = (unsigned long)m_colors.size();
	if (!numRotations)
		numRotations = 1;

	if (!numColors)
		numColors = 1;

	fwrite(&numRotations, 4, 1, outFile);
	fwrite(&numColors, 4, 1, outFile);

	if (numRotations > 1)
		fwrite(&m_rotations.front(), sizeof(Rotation), numRotations, outFile);

	if (numColors > 1)
		fwrite(&m_colors.front(), sizeof(LightColors), numColors, outFile);
}

glm::vec3 LightSetup::getDirection(const float time) const
{
	// The actual direction of the light can not yet be determined at this point,
	// so we're going with these basic returns for now
	if (m_rotations.empty())
		return glm::rotate(m_baseValues.m_rotation, glm::vec3(0, 0, -1));
	else
	{
		glm::quat rotation;
		auto iter = getIter(m_rotations, time);
		if (!iter->m_doInterpolation || iter + 1 == m_rotations.end())
			return glm::rotate(iter->m_rotation, glm::vec3(0, 0, -1));
		else
			return glm::rotate(glm::slerp(iter->m_rotation, (iter + 1)->m_rotation, (time - iter->m_frame) * iter->m_coefficient), glm::vec3(0, 0, -1));
	}
}

void LightSetup::getColors(const float time, glm::vec3& diffuse, glm::vec3& specular) const
{
	if (m_colors.empty())
	{
		diffuse = m_baseValues.m_diffuse;
		specular = m_baseValues.m_specular;
	}
	else
	{
		auto iter = getIter(m_colors, time);
		if (!iter->m_doInterpolation || iter + 1 == m_colors.end())
		{
			diffuse = iter->m_diffuse;
			specular = iter->m_specular;
		}
		else
		{
			diffuse = glm::mix(iter->m_diffuse, (iter + 1)->m_diffuse, (time - iter->m_frame) * iter->m_coefficient);
			specular = glm::mix(iter->m_specular, (iter + 1)->m_specular, (time - iter->m_frame) * iter->m_coefficient);
		}
	}
}
