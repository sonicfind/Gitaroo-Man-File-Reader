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
#include "Triangle_Primitive.h"
#include "PString/PString.h"
Triangle_Prim::Triangle_Prim(FILE* inFile, unsigned long type)
{
	switch (type)
	{
		case 4:
			m_data = std::make_unique<Triangle_Separate>(inFile);
			break;
		case 5:
			m_data = std::make_unique<Triangle_Group>(inFile);
			break;
		default:
			m_data = std::make_unique<Triangle_Data>(inFile);
			PString::pull(inFile);
			// Skips the arraySize variable of 0
			fseek(inFile, 4, SEEK_CUR);
	}
}

void Triangle_Prim::create(FILE* outFile) const
{
	PString::push("primCount", outFile);
	m_data->create(outFile, false);
	PString::push("primData", outFile);
	m_data->create(outFile, true);
}

void Triangle_Prim::draw(GLenum mode) const
{
	m_data->draw(mode);
}
