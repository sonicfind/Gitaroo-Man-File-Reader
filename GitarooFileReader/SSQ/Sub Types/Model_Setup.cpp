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
#include "Model_Setup.h"
ModelSetup::ModelSetup(FILE* inFile, char(&name)[16])
	: m_name(name)
{
	// Block Tag
	char tmp[5] = { 0 };
	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMPX"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMPX' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_controllableIndex, 4, 1, inFile);
	fread(&m_bpmStartFrame, 4, 1, inFile);
	fread(&m_controllableStartFrame, 4, 1, inFile);

	fread(m_junk, 1, 16, inFile);
	unsigned long numPositions, numRotations;
	fread(&numPositions, 4, 1, inFile);
	fread(&numRotations, 4, 1, inFile);

	m_positions.resize(numPositions);
	fread(&m_positions.front(), sizeof(Position), numPositions, inFile);

	m_rotations.resize(numRotations);
	fread(&m_rotations.front(), sizeof(Rotation), numRotations, inFile);

	unsigned long numAnimations;
	fread(&numAnimations, 4, 1, inFile);
	if (numAnimations > 1)
	{
		m_animations.resize(numAnimations);
		fread(&m_animations.front(), sizeof(ModelAnim), numAnimations, inFile);
	}

	if (m_headerVersion >= 0x1100)
	{
		unsigned long numScalars;
		fread(&numScalars, 4, 1, inFile);
		if (numScalars > 1)
		{
			m_scalars.resize(numScalars);
			fread(&m_scalars.front(), sizeof(ModelScalar), numScalars, inFile);
		}
		fread(&m_baseValues, sizeof(BaseValues), 1, inFile);
	}
}

void ModelSetup::loadXG(XG* xg)
{
	m_xg = xg;
}

void ModelSetup::create(FILE* outFile) const
{
	// Block Tag
	fprintf(outFile, "GMPX");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_controllableIndex, 4, 1, outFile);
	fwrite(&m_bpmStartFrame, 4, 1, outFile);
	fwrite(&m_controllableStartFrame, 4, 1, outFile);

	fwrite(m_junk, 1, 16, outFile);

	unsigned long numPositions = (unsigned long)m_positions.size(), numRotations = (unsigned long)m_rotations.size();

	fwrite(&numPositions, 4, 1, outFile);
	fwrite(&numRotations, 4, 1, outFile);
	fwrite(&m_positions.front(), sizeof(Position), numPositions, outFile);
	fwrite(&m_rotations.front(), sizeof(Rotation), numRotations, outFile);

	unsigned long size = (unsigned long)m_animations.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);

	if (size > 1)
		fwrite(&m_animations.front(), sizeof(ModelAnim), size, outFile);

	if (m_headerVersion >= 0x1100)
	{
		size = (unsigned long)m_scalars.size();
		if (!size)
			size = 1;
		fwrite(&size, 4, 1, outFile);

		if (size > 1)
			fwrite(&m_scalars.front(), sizeof(ModelScalar), size, outFile);
		fwrite(&m_baseValues, sizeof(BaseValues), 1, outFile);
	}
}
