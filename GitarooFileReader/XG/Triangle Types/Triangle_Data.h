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
	unsigned long m_numPrimitives;
	//unsigned int m_IBO = 0;

public:
	Triangle_Data() = default;
	Triangle_Data(FILE* inFile);
	void createCount(FILE* outFile) const;
	virtual void create(FILE* outFile) const {}
	virtual void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	virtual const size_t getSize() const;
	virtual std::vector<std::vector<unsigned long>> extract() const;

	//virtual void intializeBuffers() {}
	//virtual void deleteBuffers() {}
	virtual void draw(GLenum mode, unsigned int numInstances) {}

protected:
	//void generateIBO(const size_t structSize);
};
