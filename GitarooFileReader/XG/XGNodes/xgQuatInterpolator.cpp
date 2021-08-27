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
#include "xgQuatInterpolator.h"
#include <glm/gtc/type_ptr.hpp>
unsigned long xgQuatInterpolator::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	xgInterpolator::read(inFile, nodeList);

	PString::pull(inFile);
	unsigned long size;
	fread(&size, 4, 1, inFile);
	m_keys.reserve(size);
	m_keys.resize(size);
	fread(glm::value_ptr(m_keys.front()), sizeof(glm::quat), size, inFile);

	unsigned long sizechange = 0;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		// Removes duplicate xgTime node pointers
		if (m_inputTime)
			sizechange += 22 + m_inputTime->getName().m_size;
		m_inputTime.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return sizechange;
}

void xgQuatInterpolator::create(FILE* outFile, bool full) const
{
	PString::push("xgQuatInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);

		PString::push("keys", outFile);
		unsigned long size = (unsigned long)m_keys.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(glm::value_ptr(m_keys.front()), sizeof(glm::quat), size, outFile);

		PString::push("inputTime", outFile);
		m_inputTime->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgQuatInterpolator::write_to_txt(FILE* txtFile, const char* tabs)
{
	xgInterpolator::write_to_txt(txtFile, tabs);

	fprintf_s(txtFile, "\t%s       # of Keys (XYZW): %zu\n", tabs, m_keys.size());
	for (unsigned long index = 0; index < m_keys.size(); ++index)
	{
		fprintf_s(txtFile, "\t\t\t%s   Key %lu: %g, %g, %g, %g\n", tabs, index + 1,
			m_keys[index][0], m_keys[index][1], m_keys[index][2], m_keys[index][3]);
	}

	
}
