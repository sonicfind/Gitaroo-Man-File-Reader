#pragma once
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
#include <glad/glad.h>
class Triangle_Data
{
protected:
	std::vector<unsigned long> m_counts;

public:
	Triangle_Data(FILE* inFile);
	virtual void create(FILE* outFile) const;
	virtual void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	virtual std::vector<std::vector<unsigned long>> extract() const;
	virtual void draw(GLenum mode) const {}
};
