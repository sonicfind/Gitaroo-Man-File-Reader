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
AttDefModelSetup::AttDefModelSetup(FILE* inFile, char(&name)[16])
	: ModelSetup(inFile, name)
{
	if (m_headerVersion >= 0x1200)
	{
		fread(&m_attdef_float32, 4, 1, inFile);
		fread(&m_attdef_64bytes, sizeof(Struct64_9f), 1, inFile);
	}
}

void AttDefModelSetup::create(FILE* outFile) const
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1200)
	{
		fwrite(&m_attdef_float32, 4, 1, outFile);
		fwrite(&m_attdef_64bytes, sizeof(Struct64_9f), 1, outFile);
	}
}
