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
void ModelSetup::XGEntry::read(FILE* inFile, unsigned long& instance)
{
	fread(m_name, 1, 16, inFile);
	fread(&m_isClone, 4, 1, inFile);
	fread(&m_cloneID, 4, 1, inFile);
	fread(&m_instanceIndex, 4, 1, inFile);
	if (m_isClone)
		m_instanceIndex = ++instance;
	else
		m_instanceIndex = instance = 0;
	fread(&m_type, 4, 1, inFile);
	fread(&m_length, 4, 1, inFile);
	fread(&m_speed, 4, 1, inFile);
	fread(&m_framerate, 4, 1, inFile);
	fread(m_junk, 1, 4, inFile);
}

void ModelSetup::XGEntry::create(FILE* outFile) const
{
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_isClone, 4, 1, outFile);
	fwrite(&m_cloneID, 4, 1, outFile);
	const unsigned long zero = 0;
	fwrite(&zero, 4, 1, outFile);
	fwrite(&m_type, 4, 1, outFile);
	fwrite(&m_length, 4, 1, outFile);
	fwrite(&m_speed, 4, 1, outFile);
	fwrite(&m_framerate, 4, 1, outFile);
	fwrite(m_junk, 1, 4, outFile);
}
