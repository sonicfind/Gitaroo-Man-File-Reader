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
unsigned long xgBgGeometry::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_density, 4, 1, inFile);

	PString::pull(inFile);
	m_vertexList.read(inFile);

	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		SharedNode<> node(inFile, nodeList);
		if (xgEnvelope* env = node.get<xgEnvelope>())
			m_inputEnvelopes.push_back(env);
		else if (xgShapeInterpolator* shape = node.get<xgShapeInterpolator>())
			m_inputShapeInterpolator = shape;
		else
		{
			// Operator= will handle checking whether the node is valid for the type
			m_inputVertexInterpolator = node;
			m_inputNormalInterpolator = node;
			m_inputTexCoordInterpolator = node;
		}
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}

void xgBgGeometry::create(FILE* outFile, bool full) const
{
	PString::push("xgBgGeometry", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("density", outFile);
		fwrite(&m_density, 4, 1, outFile);
		
		m_vertexList.create(outFile);

		for (auto& node : m_inputEnvelopes)
		{
			PString::push("inputGeometry", outFile);
			node->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputVertexInterpolator)
		{
			PString::push("inputGeometry", outFile);
			m_inputVertexInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputNormalInterpolator)
		{
			PString::push("inputGeometry", outFile);
			m_inputNormalInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputTexCoordInterpolator)
		{
			PString::push("inputGeometry", outFile);
			m_inputTexCoordInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputShapeInterpolator)
		{
			PString::push("inputGeometry", outFile);
			m_inputShapeInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgBgGeometry::write_to_txt(FILE* txtFile, const char* tabs)
{
	fprintf_s(txtFile, "\t\t\t%s     Density: %g\n", tabs, m_density);
	Interpolation::write_to_txt(txtFile, m_vertexList, tabs + 1);

	if (m_inputEnvelopes.size())
	{
		fprintf_s(txtFile, "\t\t%s# of Input Envelopes: %zu\n", tabs, m_inputEnvelopes.size());
		for (size_t index = 0; index < m_inputEnvelopes.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s   %zu. %s\n", tabs, index + 1, m_inputEnvelopes[index]->getName().m_pstring);
	}
	else
	{
		if (m_inputVertexInterpolator)
			fprintf_s(txtFile, "%s   Input Vertex Interpolator: %s\n", tabs, m_inputVertexInterpolator->getName().m_pstring);

		if (m_inputNormalInterpolator)
			fprintf_s(txtFile, "%s   Input Normal Interpolator: %s\n", tabs, m_inputNormalInterpolator->getName().m_pstring);

		if (m_inputTexCoordInterpolator)
			fprintf_s(txtFile, "%s Input TexCoord Interpolator: %s\n", tabs, m_inputTexCoordInterpolator->getName().m_pstring);

		if (m_inputShapeInterpolator)
			fprintf_s(txtFile, "%s    Input Shape Interpolator: %s\n", tabs, m_inputShapeInterpolator->getName().m_pstring);
	}
}

void xgBgGeometry::positions_to_obj(FILE* objFile) const
{
	for (const Vertex& vertex : m_vertexList.m_vertices)
		fprintf(objFile, "v %f %f %f\n", vertex.m_position.x, vertex.m_position.y, vertex.m_position.z);
}

void xgBgGeometry::texCoords_to_obj(FILE* objFile) const
{
	if (m_vertexList.m_vertexFlags & 8)
		for (const Vertex& vertex : m_vertexList.m_vertices)
			fprintf(objFile, "vt %f %f\n", vertex.m_texCoord.s, vertex.m_texCoord.t);
}

void xgBgGeometry::normals_to_obj(FILE* objFile) const
{
	for (const Vertex& vertex : m_vertexList.m_vertices)
		fprintf(objFile, "vn %f %f %f\n", vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z);
}
