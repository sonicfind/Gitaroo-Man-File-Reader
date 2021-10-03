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
#include "Triangle_Data.h"
class Triangle_Group : public Triangle_Data
{
	/*struct DrawArraysIndirectCommand {
		unsigned long count;
		unsigned long instanceCount;
		unsigned long first;
		unsigned long baseInstance;
	};
	std::vector<DrawArraysIndirectCommand> m_commands;*/
	unsigned long m_initialIndex;
	std::vector<unsigned long> m_counts;

public:
	Triangle_Group() = default;
	Triangle_Group(FILE* inFile);
	void create(FILE* outFile) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	std::vector<std::vector<unsigned long>> extract() const;
	const size_t getSize() const;

	//void intializeBuffers();
	//void deleteBuffers();
	void draw(GLenum mode, unsigned int numInstances);
};
