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
#include "xgDagTransform.h"
unsigned long xgDagTransform::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString test(inFile);
	if (!strchr(test.m_pstring, '}'))
	{
		m_inputMatrix.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}

void xgDagTransform::create(FILE* outFile, bool full) const
{
	PString::push("xgDagTransform", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		if (m_inputMatrix)
		{
			PString::push("inputMatrix", outFile);
			m_inputMatrix->push(outFile);
			PString::push("outputMatrix", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgDagTransform::write_to_txt(FILE* txtFile, const char* tabs)
{
	if (m_inputMatrix)
		fprintf_s(txtFile, "\t\t%s      Input Matrix: %s\n", tabs, m_inputMatrix->getName().m_pstring);
	else
		fprintf_s(txtFile, "\t\t%s      No Input Matrix\n", tabs);
}
