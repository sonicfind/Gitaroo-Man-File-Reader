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
