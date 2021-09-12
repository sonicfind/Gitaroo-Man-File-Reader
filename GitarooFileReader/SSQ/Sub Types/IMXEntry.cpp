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
#include "IMXEntry.h"
IMXEntry::IMXEntry(FILE* inFile)
{
	fread(m_name, 1, 16, inFile);
	fread(&m_unused_1, 4, 1, inFile);
	fread(&m_unused_2, 4, 1, inFile);
	fread(m_unused_3, 8, 1, inFile);
}

void IMXEntry::create(FILE* outFile)
{
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_unused_1, 4, 1, outFile);
	fwrite(&m_unused_2, 4, 1, outFile);
	fwrite(m_unused_3, 8, 1, outFile);
}