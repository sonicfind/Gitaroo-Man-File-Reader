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
#include "Camera_Setup.h"
void CameraSetup::read(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMPX"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMPX' Tag (Camera) at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_size, 4, 1, inFile);
	fread(m_unk, 1, 8, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(&m_baseGlobalValues, sizeof(BaseGlobalValues), 1, inFile);

	unsigned long numPositions, numRotations;
	fread(&numPositions, 4, 1, inFile);
	fread(&numRotations, 4, 1, inFile);

	m_positions.resize(numPositions);
	fread(&m_positions.front(), sizeof(Position), numPositions, inFile);

	m_rotations.resize(numRotations);
	fread(&m_rotations.front(), sizeof(Rotation), numRotations, inFile);

	unsigned long numSettings;
	fread(&numSettings, 4, 1, inFile);
	if (numSettings > 1)
	{
		m_projections.resize(numSettings);
		fread(&m_projections.front(), sizeof(Projection), numSettings, inFile);
	}

	unsigned long numAmbientColors;
	fread(&numAmbientColors, 4, 1, inFile);
	if (numAmbientColors > 1)
	{
		m_ambientColors.resize(numAmbientColors);
		fread(&m_ambientColors.front(), sizeof(AmbientColor), numAmbientColors, inFile);
	}

	unsigned long numlights;
	fread(&numlights, 4, 1, inFile);

	for (unsigned long i = 0; i < numlights; ++i)
		m_lights.emplace_back(inFile);

	if (m_headerVersion >= 0x1200)
	{
		unsigned long unk3;
		fread(&unk3, 4, 1, inFile);
		if (unk3 > 1)
		{
			m_64bytes_v.resize(unk3);
			fread(&m_64bytes_v.front(), sizeof(Struct64_7f), unk3, inFile);
		}
	}
}

void CameraSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMPX");
	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_size, 4, 1, outFile);
	fwrite(m_unk, 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_baseGlobalValues, sizeof(BaseGlobalValues), 1, outFile);

	unsigned long numPositions = (unsigned long)m_positions.size();
	unsigned long numRotations = (unsigned long)m_rotations.size();

	fwrite(&numPositions, 4, 1, outFile);
	fwrite(&numRotations, 4, 1, outFile);
	fwrite(&m_positions.front(), sizeof(Position), numPositions, outFile);
	fwrite(&m_rotations.front(), sizeof(Rotation), numRotations, outFile);

	unsigned long size = (unsigned long)m_projections.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);
	if (size > 1)
		fwrite(&m_projections.front(), sizeof(Projection), size, outFile);

	size = (unsigned long)m_ambientColors.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);
	if (size > 1)
		fwrite(&m_ambientColors.front(), sizeof(AmbientColor), size, outFile);

	size = (unsigned long)m_lights.size();
	fwrite(&size, 4, 1, outFile);

	for (auto& light : m_lights)
		light.create(outFile);

	if (m_headerVersion >= 0x1200)
	{
		size = (unsigned long)m_64bytes_v.size();
		if (!size)
			size = 1;
		fwrite(&size, 4, 1, outFile);
		if (size > 1)
			fwrite(&m_64bytes_v.front(), sizeof(Struct64_7f), size, outFile);
	}
}
