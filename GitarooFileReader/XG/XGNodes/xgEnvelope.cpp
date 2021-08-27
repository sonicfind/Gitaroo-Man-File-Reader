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
#include "xgBgGeometry.h"
unsigned long xgEnvelope::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_startVertex, 4, 1, inFile);

	PString::pull(inFile);
	unsigned long size;
	fread(&size, 4, 1, inFile);
	m_weights.reserve(size);
	m_weights.resize(size);
	fread(m_weights.data(), sizeof(Weight), size, inFile);

	PString::pull(inFile);
	fread(&size, 4, 1, inFile);
	for (unsigned long i = 0; i < size; ++i)
	{
		long val = -1;
		std::vector<unsigned long> vect;
		fread(&val, 4, 1, inFile);
		while (val != -1)
		{
			vect.push_back(val);
			fread(&val, 4, 1, inFile);
			++i;
		}

		if (vect.size())
		{
			vect.shrink_to_fit();
			m_vertexTargets.push_back(vect);
		}
	}
	m_vertexTargets.shrink_to_fit();

	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputMatrix"))
			m_inputMatrices.emplace_back(inFile, nodeList);
		else
			m_inputGeometry.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}

void xgEnvelope::create(FILE* outFile, bool full) const
{
	PString::push("xgEnvelope", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("startVertex", outFile);
		fwrite(&m_startVertex, 4, 1, outFile);

		PString::push("weights", outFile);
		unsigned long size = (unsigned long)m_weights.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_weights.data(), sizeof(Weight), size, outFile);

		PString::push("vertexTargets", outFile);
		size = (unsigned long)m_vertexTargets.size();
		for (const auto& vect : m_vertexTargets)
			size += (unsigned long)vect.size();
		fwrite(&size, 4, 1, outFile);

		const long end = -1;
		for (const auto& vect : m_vertexTargets)
		{
			for (const auto target : vect)
				fwrite(&target, 4, 1, outFile);
			fwrite(&end, 4, 1, outFile);
		}

		for (size_t index = 0; index < m_inputMatrices.size(); ++index)
		{
			PString::push("inputMatrix" + char(index + 49), outFile);
			m_inputMatrices[index]->push(outFile);
			PString::push("envelopeMatrix", outFile);
		}

		PString::push("inputGeometry", outFile);
		m_inputGeometry->push(outFile);
		PString::push("outputGeometry", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgEnvelope::write_to_txt(FILE* txtFile, const char* tabs)
{
	fprintf_s(txtFile, "\t\t%s   Starting Vertex: %lu\n", tabs, m_startVertex);
	fprintf_s(txtFile, "\t\t%s      # of Weights: %zu\n", tabs, m_weights.size());
	for (unsigned long index = 0; index < m_weights.size(); ++index)
		fprintf_s(txtFile, "\t\t\t%s     Weight %03lu: %g, %g, %g, %g\n", tabs, index + 1,
			m_weights[index].values[0], m_weights[index].values[1], m_weights[index].values[2], m_weights[index].values[3]);

	fprintf_s(txtFile, "\t%s       # of Vertex Targets: %zu\n", tabs, m_vertexTargets.size());
	for (size_t index = 0; index < m_vertexTargets.size(); ++index)
	{
		fprintf_s(txtFile, "\t\t\t%sVertex Envelope %03zu\n", tabs, index + 1);
		for (size_t i = 0; i < m_vertexTargets[index].size(); ++i)
			fprintf_s(txtFile, "\t\t\t%s   Target Mesh Vertex %zu: %li\n", tabs, i + 1, m_vertexTargets[index][i]);
	}

	if (m_inputMatrices.size())
	{
		fprintf_s(txtFile, "\t%s       # of Input Matrices: %zu\n", tabs, m_inputMatrices.size());
		for (size_t index = 0; index < m_inputMatrices.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s %zu. %s\n", tabs, index + 1, m_inputMatrices[index]->getName().m_pstring);
	}

	fprintf_s(txtFile, "\t\t%s    Input Geometry: %s\n", tabs, m_inputGeometry->getName().m_pstring);
}
