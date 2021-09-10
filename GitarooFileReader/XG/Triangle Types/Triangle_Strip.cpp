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
#include "Triangle_Strip.h"
#include "PString/PString.h"
void Triangle_Strip::create(FILE* outFile) const
{
	PString::push("triStripCount", outFile);
	m_data->create(outFile, false);
	PString::push("triStripData", outFile);
	m_data->create(outFile, true);
}

void Triangle_Strip::write_to_txt(FILE* txtFile, const char* tabs) const
{
	fprintf_s(txtFile, "\t\t\t%s  Triangle Strips\n", tabs);
	m_data->write_to_txt(txtFile, tabs);
}

void Triangle_Strip::write_to_obj(FILE* objFile, const size_t offset, const bool writeTexture) const
{
	for (const auto& vect : m_data->extract())
		if (writeTexture)
			for (size_t index = 0; index < vect.size() - 2; ++index)
				if (!(index & 1))
					fprintf(objFile, "f %zu/%zu/%zu %zu/%zu/%zu %zu/%zu/%zu\n", vect[index] + offset, vect[index] + offset, vect[index] + offset,
																				vect[index + 1] + offset, vect[index + 1] + offset, vect[index + 1] + offset,
																				vect[index + 2] + offset, vect[index + 2] + offset, vect[index + 2] + offset);
				else
					fprintf(objFile, "f %zu/%zu/%zu %zu/%zu/%zu %zu/%zu/%zu\n", vect[index] + offset, vect[index] + offset, vect[index] + offset,
																				vect[index + 2] + offset, vect[index + 2] + offset, vect[index + 2] + offset,
																				vect[index + 1] + offset, vect[index + 1] + offset, vect[index + 1] + offset);
		else
			for (size_t index = 0; index < vect.size() - 2; ++index)
				if (!(index & 1))
					fprintf(objFile, "f %zu//%zu %zu//%zu %zu//%zu\n", vect[index] + offset, vect[index] + offset,
																		vect[index + 1] + offset, vect[index + 1] + offset,
																		vect[index + 2] + offset, vect[index + 2] + offset);
				else
					fprintf(objFile, "f %zu//%zu %zu//%zu %zu//%zu\n", vect[index] + offset, vect[index] + offset,
																		vect[index + 2] + offset, vect[index + 2] + offset,
																		vect[index + 1] + offset, vect[index + 1] + offset);
}

const size_t Triangle_Strip::getSize() const
{
	return PSTRING_LEN("triStripCount", unsigned long)
		+ m_data->getSize()
		+ PSTRING_LEN("triStripData", unsigned long);
}
