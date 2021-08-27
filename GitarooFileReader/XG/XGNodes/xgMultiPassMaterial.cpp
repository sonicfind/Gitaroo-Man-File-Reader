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
#include "xgMultiPassMaterial.h"
unsigned long xgMultiPassMaterial::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	PString test;
	int sizechange = 0;
	do
	{
		// Removes duplicate Materials
		SharedNode<xgMaterial> compare(inFile, nodeList);
		for (size_t i = 0; i < m_inputMaterials.size(); ++i)
			if (m_inputMaterials[i] == compare)
			{
				sizechange += 30 + compare->getName().m_size;
				goto Grab_Characters;
			}
		m_inputMaterials.push_back(compare);

	Grab_Characters:
		PString::pull(inFile);
		test.fill(inFile);
	} while (!strchr(test.m_pstring, '}'));
	return sizechange;
}

void xgMultiPassMaterial::create(FILE* outFile, bool full) const
{
	PString::push("xgMultiPassMaterial", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		for (auto& node : m_inputMaterials)
		{
			PString::push("inputMaterial", outFile);
			node->push(outFile);
			PString::push("outputMaterial", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgMultiPassMaterial::write_to_txt(FILE* txtFile, const char* tabs)
{
	fprintf_s(txtFile, "\t%s     # of Materials: %zu\n", tabs, m_inputMaterials.size());
	for (size_t index = 0; index < m_inputMaterials.size(); ++index)
		fprintf_s(txtFile, "\t\t%s %zu. %s\n", tabs, index + 1, m_inputMaterials[index]->getName().m_pstring);
}
