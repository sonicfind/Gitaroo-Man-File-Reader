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
#include "xgInterpolator.h"
class xgShapeInterpolator
	: public xgTimedInterpolator<VertexList, Interpolation::shapeStrings>
{
public:
	using xgTimedInterpolator::xgTimedInterpolator;
	void read_keys(FILE* inFile)
	{
		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_keys.reserve(size);
		m_keys.resize(size);
		for (auto& key : m_keys)
			key.read(inFile);
	}

	void create_keys(FILE* outFile) const
	{
		PString::push("keys", outFile);
		unsigned long size = (unsigned long)m_keys.size();
		fwrite(&size, 4, 1, outFile);
		for (auto& key : m_keys)
			key.create(outFile);
	}

	const size_t getSize_keys() const
	{
		size_t size = 0;
		for (const auto& list : this->m_keys)
			size += list.getFileSize();
		return size;
	}
	static bool compareType(const PString& str) { return strcmp("xgShapeInterpolator", str.m_pstring) == 0; }
};
