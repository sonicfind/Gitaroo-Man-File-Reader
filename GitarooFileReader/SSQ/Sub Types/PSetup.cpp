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
#include "PSetup.h"
void PSetup::read(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "PSTP"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'PSTP' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);

	fread(&m_numMystery, 4, 1, inFile);
}

void PSetup::create(FILE* outFile)
{
	fprintf(outFile, "PSTP");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);

	fwrite(&m_numMystery, 4, 1, outFile);
}
