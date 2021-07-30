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
#include "XG_Nodes.h"

void XGNode::push(FILE* outFile) const
{
	m_name.push(outFile);
}

xgBgGeometry::xgBgGeometry(const xgBgGeometry& geo)
	: m_density(geo.m_density), m_vertexFlags(geo.m_vertexFlags), m_numVerts(geo.m_numVerts), m_inputEnvelopes(geo.m_inputEnvelopes),
		m_inputVertexInterpolator(m_inputVertexInterpolator), m_inputNormalInterpolator(m_inputNormalInterpolator), m_inputTexCoordInterpolator(m_inputTexCoordInterpolator)
{
	m_name = geo.m_name;
	int size = 0;
	if (m_vertexFlags & 1)
		size += 4;
	if (m_vertexFlags & 2)
		size += 3;
	if (m_vertexFlags & 4)
		size += 4;
	if (m_vertexFlags & 8)
		size += 2;
	m_vertices = new float[m_numVerts * size];
	std::copy(geo.m_vertices, geo.m_vertices + m_numVerts * size, m_vertices);
}
xgBgGeometry::~xgBgGeometry()
{
	int size = 0;
	if (m_vertexFlags & 1)
		size += 4;
	if (m_vertexFlags & 2)
		size += 3;
	if (m_vertexFlags & 4)
		size += 4;
	if (m_vertexFlags & 8)
		size += 2;
	delete[m_numVerts * size] m_vertices;
}
int xgBgGeometry::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_density, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_vertexFlags, 4, 1, inFile);
	fread(&m_numVerts, 4, 1, inFile);
	int size = 0;
	if (m_vertexFlags & 1)
		size += 4;
	if (m_vertexFlags & 2)
		size += 3;
	if (m_vertexFlags & 4)
		size += 4;
	if (m_vertexFlags & 8)
		size += 2;
	m_vertices = new float[m_numVerts * size];
	fread(m_vertices, 4, m_numVerts * size, inFile);
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		SharedNode<XGNode> node(inFile, nodeList);
		if (dynamic_cast<xgEnvelope*>(node.m_node))
			m_inputEnvelopes.emplace_back(node.m_node);
		else if (dynamic_cast<xgVertexInterpolator*>(node.m_node))
			m_inputVertexInterpolator.m_node = (xgVertexInterpolator*)node.m_node;
		else if (dynamic_cast<xgNormalInterpolator*>(node.m_node))
			m_inputNormalInterpolator.m_node = (xgNormalInterpolator*)node.m_node;
		else if (dynamic_cast<xgTexCoordInterpolator*>(node.m_node))
			m_inputTexCoordInterpolator.m_node = (xgTexCoordInterpolator*)node.m_node;
		else
			m_inputShapeInterpolator.m_node = (xgShapeInterpolator*)node.m_node;
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgBgGeometry::create(FILE* outFile, bool full)
{
	PString::push("xgBgGeometry", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("density", outFile);
		fwrite(&m_density, 4, 1, outFile);
		PString::push("vertices", outFile);
		fwrite(&m_vertexFlags, 4, 1, outFile);
		fwrite(&m_numVerts, 4, 1, outFile);
		int size = 0;
		if (m_vertexFlags & 1)
			size += 4;
		if (m_vertexFlags & 2)
			size += 3;
		if (m_vertexFlags & 4)
			size += 4;
		if (m_vertexFlags & 8)
			size += 2;
		fwrite(m_vertices, 4, size * m_numVerts, outFile);
		for (auto& node : m_inputEnvelopes)
		{
			PString::push("inputGeometry", outFile);
			node->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputVertexInterpolator.isValid())
		{
			PString::push("inputGeometry", outFile);
			m_inputVertexInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputNormalInterpolator.isValid())
		{
			PString::push("inputGeometry", outFile);
			m_inputNormalInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputTexCoordInterpolator.isValid())
		{
			PString::push("inputGeometry", outFile);
			m_inputTexCoordInterpolator->push(outFile);
			PString::push("outputGeometry", outFile);
		}

		if (m_inputShapeInterpolator.isValid())
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
void xgBgGeometry::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t\t%s     Density: %g\n", tabs, m_density);
	fprintf_s(outTXT, "\t\t\t%sVertex Flags: %lu\n", tabs, m_vertexFlags);
	fprintf_s(outTXT, "\t\t%s       # of Vertices: %lu\n", tabs, m_numVerts);
	for (unsigned long index = 0, vertex = 0; index < m_numVerts; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s    Vertex %03lu\n", tabs, index + 1);
		if (m_vertexFlags & 1) // Position
		{
			fprintf_s(outTXT, "\t\t\t\t\t%sPosition (XYZW): %g, %g, %g, %g\n", tabs, m_vertices[vertex], m_vertices[vertex + 1], m_vertices[vertex + 2], m_vertices[vertex + 3]);
			vertex += 4;
		}
		if (m_vertexFlags & 2) // Normal
		{
			fprintf_s(outTXT, "\t\t\t\t\t%s   Normal (XYZ): %g, %g, %g\n", tabs, m_vertices[vertex], m_vertices[vertex + 1], m_vertices[vertex + 2]);
			vertex += 3;
		}
		if (m_vertexFlags & 4) // Color
		{
			fprintf_s(outTXT, "\t\t\t\t\t%s   Color (RGBA): %g, %g, %g, %g\n", tabs, m_vertices[vertex], m_vertices[vertex + 1], m_vertices[vertex + 2], m_vertices[vertex + 3]);
			vertex += 4;
		}
		if (m_vertexFlags & 8) // Texture Coordinate
		{
			fprintf_s(outTXT, "\t\t\t\t%sTexture Coordinate (ST): %g, %g\n", tabs, m_vertices[vertex], m_vertices[vertex + 1]);
			vertex += 2;
		}
	}
	if (m_inputEnvelopes.size())
	{
		fprintf_s(outTXT, "\t%s       # of Input Geometries: %zu\n", tabs, m_inputEnvelopes.size());
		for (size_t index = 0; index < m_inputEnvelopes.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s   %zu. %s\n", tabs, index + 1, m_inputEnvelopes[index].getPString()->m_pstring);
	}
	else
	{
		if (m_inputVertexInterpolator.isValid())
			fprintf_s(outTXT, "%s   Input Vertex Interpolator: %s\n", tabs, m_inputVertexInterpolator.getPString()->m_pstring);

		if (m_inputNormalInterpolator.isValid())
			fprintf_s(outTXT, "%s   Input Normal Interpolator: %s\n", tabs, m_inputNormalInterpolator.getPString()->m_pstring);

		if (m_inputTexCoordInterpolator.isValid())
			fprintf_s(outTXT, "%s Input TexCoord Interpolator: %s\n", tabs, m_inputTexCoordInterpolator.getPString()->m_pstring);

		if (m_inputShapeInterpolator.isValid())
			fprintf_s(outTXT, "%s    Input Shape Interpolator: %s\n", tabs, m_inputShapeInterpolator.getPString()->m_pstring);
	}
}

int xgBgMatrix::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(m_position, 4, 3, inFile);
	PString::pull(inFile);
	fread(m_rotation, 4, 4, inFile);
	PString::pull(inFile);
	fread(m_scale, 4, 3, inFile);
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputPosition"))
			m_inputPosition.fill(inFile, nodeList);
		else if (strstr(test.m_pstring, "inputRotation"))
			m_inputRotation.fill(inFile, nodeList);
		else if (strstr(test.m_pstring, "inputScale"))
			m_inputScale.fill(inFile, nodeList);
		else
			m_inputParentMatrix.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgBgMatrix::create(FILE* outFile, bool full)
{
	PString::push("xgBgMatrix", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("position", outFile);
		fwrite(m_position, 4, 3, outFile);
		PString::push("rotation", outFile);
		fwrite(m_rotation, 4, 4, outFile);
		PString::push("scale", outFile);
		fwrite(m_scale, 4, 3, outFile);
		if (m_inputPosition.isValid())
		{
			PString::push("inputPosition", outFile);
			m_inputPosition->push(outFile);
			PString::push("outputVec3", outFile);
		}
		if (m_inputRotation.isValid())
		{
			PString::push("inputRotation", outFile);
			m_inputRotation->push(outFile);
			PString::push("outputQuat", outFile);
		}
		if (m_inputScale.isValid())
		{
			PString::push("inputScale", outFile);
			m_inputScale->push(outFile);
			PString::push("outputVec3", outFile);
		}
		if (m_inputParentMatrix.isValid())
		{
			PString::push("inputParentMatrix", outFile);
			m_inputParentMatrix->push(outFile);
			PString::push("outputMatrix", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgBgMatrix::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t\t%s Grid Position (XYZ): %g, %g, %g\n", tabs, m_position[0], m_position[1], m_position[2]);
	fprintf_s(outTXT, "\t\t\t%sGrid Rotation (XYZW): %g, %g, %g, %g\n", tabs, m_rotation[0], m_rotation[1], m_rotation[2], m_rotation[3]);
	fprintf_s(outTXT, "\t\t\t%s    Grid Scale (XYZ): %g, %g, %g\n", tabs, m_scale[0], m_scale[1], m_scale[2]);
	if (m_inputPosition.isValid())
		fprintf_s(outTXT, "\t\t%s     Position overwritten by: %s\n", tabs, m_inputPosition.getPString()->m_pstring);
	if (m_inputRotation.isValid())
		fprintf_s(outTXT, "\t\t%s     Rotation overwritten by: %s\n", tabs, m_inputRotation.getPString()->m_pstring);
	if (m_inputScale.isValid())
		fprintf_s(outTXT, "\t\t\t%sScale overwritten by: %s\n", tabs, m_inputScale.getPString()->m_pstring);
	if (m_inputParentMatrix.isValid())
		fprintf_s(outTXT, "\t\t\t%s Parent Input Matrix: %s\n", tabs, m_inputParentMatrix.getPString()->m_pstring);
}

int xgBone::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(m_restMatrix, 4, 16, inFile);
	PString::pull(inFile);
	m_inputMatrix.fill(inFile, nodeList);
	PString::pull(inFile);
	PString::pull(inFile);
	return 0;
}
void xgBone::create(FILE* outFile, bool full)
{
	PString::push("xgBone", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("restMatrix", outFile);
		fwrite(m_restMatrix, 4, 16, outFile);
		PString::push("inputMatrix", outFile);
		m_inputMatrix->push(outFile);
		PString::push("outputMatrix", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgBone::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t\t%s Rest Matrix:\n", tabs);
	for (int row = 0, pos = 0; row < 4; ++row, pos += 4)
		fprintf_s(outTXT, "\t\t\t%s      Row %u: %g, %g, %g, %g\n", tabs, row + 1,
			m_restMatrix[pos], m_restMatrix[pos + 1], m_restMatrix[pos + 2], m_restMatrix[pos + 3]);
	fprintf_s(outTXT, "\t\t\t%sInput Matrix: %s\n", tabs, m_inputMatrix.getPString()->m_pstring);
}

xgDagMesh::Data::Data(const Data& data)
{
	m_arraySize = data.m_arraySize;
	m_arrayData = new unsigned long[m_arraySize];
	std::copy(data.m_arrayData, data.m_arrayData + m_arraySize, m_arrayData);
}
xgDagMesh::Data::~Data()
{
	if (m_arrayData != nullptr)
		delete[m_arraySize] m_arrayData;
}
int xgDagMesh::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_primType, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_primCount, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_primData.m_arraySize, 4, 1, inFile);
	if (m_primData.m_arraySize > 0)
	{
		m_primData.m_arrayData = new unsigned long[m_primData.m_arraySize];
		if (fread(m_primData.m_arrayData, 4, m_primData.m_arraySize, inFile) != m_primData.m_arraySize)
			throw;
	}
	PString::pull(inFile);
	fread(&m_triFanCount, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_triFanData.m_arraySize, 4, 1, inFile);
	if (m_triFanData.m_arraySize > 0)
	{
		m_triFanData.m_arrayData = new unsigned long[m_triFanData.m_arraySize];
		if (fread(m_triFanData.m_arrayData, 4, m_triFanData.m_arraySize, inFile) != m_triFanData.m_arraySize)
			throw;
	}
	PString::pull(inFile);
	fread(&m_triStripCount, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_triStripData.m_arraySize, 4, 1, inFile);
	if (m_triStripData.m_arraySize > 0)
	{
		m_triStripData.m_arrayData = new unsigned long[m_triStripData.m_arraySize];
		if (fread(m_triStripData.m_arrayData, 4, m_triStripData.m_arraySize, inFile) != m_triStripData.m_arraySize)
			throw;
	}
	PString::pull(inFile);
	fread(&m_triListCount, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_triListData.m_arraySize, 4, 1, inFile);
	if (m_triListData.m_arraySize > 0)
	{
		m_triListData.m_arrayData = new unsigned long[m_triListData.m_arraySize];
		if (fread(m_triListData.m_arrayData, 4, m_triListData.m_arraySize, inFile) != m_triListData.m_arraySize)
			throw;
	}
	PString::pull(inFile);
	fread(&m_cullFunc, 4, 1, inFile);
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputGeometry"))
			m_inputGeometries.emplace_back(inFile, nodeList);
		else
			m_inputMaterials.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgDagMesh::create(FILE* outFile, bool full)
{
	PString::push("xgDagMesh", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("primType", outFile);
		fwrite(&m_primType, 4, 1, outFile);
		PString::push("primCount", outFile);
		fwrite(&m_primCount, 4, 1, outFile);
		PString::push("primData", outFile);
		fwrite(&m_primData.m_arraySize, 4, 1, outFile);
		fwrite(m_primData.m_arrayData, 4, m_primData.m_arraySize, outFile);
		PString::push("triFanCount", outFile);
		fwrite(&m_triFanCount, 4, 1, outFile);
		PString::push("triFanData", outFile);
		fwrite(&m_triFanData.m_arraySize, 4, 1, outFile);
		fwrite(m_triFanData.m_arrayData, 4, m_triFanData.m_arraySize, outFile);
		PString::push("triStripCount", outFile);
		fwrite(&m_triStripCount, 4, 1, outFile);
		PString::push("triStripData", outFile);
		fwrite(&m_triStripData.m_arraySize, 4, 1, outFile);
		fwrite(m_triStripData.m_arrayData, 4, m_triStripData.m_arraySize, outFile);
		PString::push("triListCount", outFile);
		fwrite(&m_triListCount, 4, 1, outFile);
		PString::push("triListData", outFile);
		fwrite(&m_triListData.m_arraySize, 4, 1, outFile);
		fwrite(m_triListData.m_arrayData, 4, m_triListData.m_arraySize, outFile);
		PString::push("cullFunc", outFile);
		fwrite(&m_cullFunc, 4, 1, outFile);

		for (auto& node : m_inputGeometries)
		{
			PString::push("inputGeometry", outFile);
			node->push(outFile);
			PString::push("outputGeometry", outFile);
		}

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
void xgDagMesh::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t\t%s    PrimType: ", tabs);
	if (m_primType == 4)
		fprintf_s(outTXT, "Kick vertices separately\n");
	else if (m_primType == 5)
		fprintf_s(outTXT, "Kick vertices in groups\n");
	else
		fprintf_s(outTXT, "Who the heck knows\n");
	if (m_primType == 4)
	{
		fprintf_s(outTXT, "\t\t\t%s TriFanCount: %lu\n", tabs, m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; valueIndex < m_triFanCount; ++index)
		{
			const unsigned long numIndexes = m_triFanData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t\t\t%s   TriFan %03lu\n", tabs, index + 1);
			fprintf_s(outTXT, "\t\t\t%s       # of Vertex Indexes: %03lu\n", tabs, numIndexes);
			for (unsigned long vertex = 0; vertex < numIndexes; ++vertex)
				fprintf_s(outTXT, "\t\t\t\t%s       Index %03lu: %lu\n", tabs, vertex + 1, m_triFanData.m_arrayData[valueIndex++]);
		}
		fprintf_s(outTXT, "\t\t%s       TriStripCount: %lu\n", tabs, m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; valueIndex < m_triStripCount; ++index)
		{
			const unsigned long numIndexes = m_triStripData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t\t\t%s   TriStrip %03lu\n", tabs, index + 1);
			fprintf_s(outTXT, "\t\t\t%s       # of Vertex Indexes: %03lu\n", tabs, numIndexes);
			for (unsigned long vertex = 0; vertex < numIndexes; ++vertex)
				fprintf_s(outTXT, "\t\t\t\t%s       Index %03lu: %lu\n", tabs, vertex + 1, m_triStripData.m_arrayData[valueIndex++]);
		}
		fprintf_s(outTXT, "\t\t\t%sTriListCount: %lu\n", tabs, m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; valueIndex < m_triListCount; ++index)
		{
			const unsigned long numIndexes = m_triListData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t\t\t%s    TriList %03lu\n", tabs, index + 1);
			fprintf_s(outTXT, "\t\t\t%s       # of Vertex Indexes: %03lu\n", tabs, numIndexes);
			for (unsigned long vertex = 0; vertex < numIndexes; vertex++)
				fprintf_s(outTXT, "\t\t\t\t%s       Index %03lu: %lu\n", tabs, vertex + 1, m_triListData.m_arrayData[valueIndex++]);
		}
	}
	else if (m_primType == 5)
	{
		fprintf_s(outTXT, "\t\t\t%s TriFanCount: %lu\n", tabs, m_triFanCount);
		if (m_triFanCount)
		{
			unsigned long valueIndex = 0;
			unsigned long initialVertex = m_triFanData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t\t%s   Base TriFan Index: %03lu\n", tabs, initialVertex);
			for (unsigned long index = 0; valueIndex < m_triFanData.m_arraySize; ++index)
			{
				fprintf_s(outTXT, "\t\t\t%s TriFan Group %03lu\n", tabs, index + 1);
				fprintf_s(outTXT, "\t\t\t\t%s      # of Vertices: %lu\n", tabs, m_triFanData.m_arrayData[valueIndex++]);
			}
		}

		fprintf_s(outTXT, "\t\t%s       TriStripCount: %lu\n", tabs, m_triStripCount);
		if (m_triStripCount)
		{
			unsigned long valueIndex = 0;
			unsigned long initialVertex = m_triStripData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t\t%s Base TriStrip Index: %03lu\n", tabs, initialVertex);
			for (unsigned long index = 0; valueIndex < m_triStripData.m_arraySize; ++index)
			{
				fprintf_s(outTXT, "\t\t\t%s TriStrip Group %03lu\n", tabs, index + 1);
				fprintf_s(outTXT, "\t\t\t\t%s      # of Vertices: %lu\n", tabs, m_triStripData.m_arrayData[valueIndex++]);
			}
		}

		if (m_triListCount)
		{
			unsigned long valueIndex = 0;
			fprintf_s(outTXT, "\t\t\t%sTriListCount: %lu\n", tabs, m_triListCount);
			unsigned long initialVertex = m_triListData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t\t%s  Base TriList Index: %03lu\n", tabs, initialVertex);
			for (unsigned long index = 0; valueIndex < m_triListData.m_arraySize; ++index)
			{
				fprintf_s(outTXT, "\t\t\t%s  TriList Group %03lu\n", tabs, index + 1);
				fprintf_s(outTXT, "\t\t\t\t%s      # of Vertices: %lu\n", tabs, m_triListData.m_arrayData[valueIndex++]);
			}
		}
	}
	fprintf_s(outTXT, "\t\t\t%s    CullFunc: ", tabs);
	switch (m_cullFunc)
	{
	case 0:
		fprintf_s(outTXT, "Disabled\n");
		break;
	case 1:
		fprintf_s(outTXT, "Clockwise-winding triangles (CCW is front-facing) [???]\n");
		break;
	case 2:
		fprintf_s(outTXT, "Counterclockwise-winding triangles (CW is front-facing) [???]\n");
	}
	if (m_inputGeometries.size())
	{
		fprintf_s(outTXT, "\t%s       # of Input Geometries: %zu\n", tabs, m_inputGeometries.size());
		for (size_t index = 0; index < m_inputGeometries.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s   %zu. %s\n", tabs, index + 1, m_inputGeometries[index].getPString()->m_pstring);
	}
	if (m_inputMaterials.size())
	{
		fprintf_s(outTXT, "\t\t%s# of Input Materials: %zu\n", tabs, m_inputMaterials.size());
		for (size_t index = 0; index < m_inputMaterials.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s   %zu. %s\n", tabs, index + 1, m_inputMaterials[index].getPString()->m_pstring);
	}
}

int xgDagTransform::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputMatrices.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgDagTransform::create(FILE* outFile, bool full)
{
	PString::push("xgDagTransform", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		for (auto& node : m_inputMatrices)
		{
			PString::push("inputMatrix", outFile);
			node->push(outFile);
			PString::push("outputMatrix", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgDagTransform::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t%s       # of Input Matrices: %zu\n", tabs, m_inputMatrices.size());
	for (size_t index = 0; index < m_inputMatrices.size(); ++index)
		fprintf_s(outTXT, "\t\t\t%s %zu. %s\n", tabs, index + 1, m_inputMatrices[index].getPString()->m_pstring);
}

xgEnvelope::xgEnvelope(const xgEnvelope& env)
	: m_startVertex(env.m_startVertex), m_numweights(env.m_numweights), m_numTargets(env.m_numTargets),
		m_inputMatrices(env.m_inputMatrices), m_inputGeometries(env.m_inputGeometries)
{
	m_weights = new float[m_numweights][4];
	memcpy_s(m_weights, 16ULL * m_numweights, env.m_weights, 16ULL * m_numweights);

	m_vertexTargets = new long[m_numTargets];
	std::copy(env.m_vertexTargets, env.m_vertexTargets + m_numTargets, m_vertexTargets);
}
xgEnvelope::~xgEnvelope()
{
	if (m_weights != nullptr)
		delete[m_numweights] m_weights;

	if (m_vertexTargets != nullptr)
		delete[m_numTargets] m_vertexTargets;
}
int xgEnvelope::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_startVertex, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numweights, 4, 1, inFile);
	m_weights = new float[m_numweights][4];
	if (fread(m_weights, 16, m_numweights, inFile) != m_numweights)
		throw;
	PString::pull(inFile);
	fread(&m_numTargets, 4, 1, inFile);
	m_vertexTargets = new long[m_numTargets];
	if (fread(m_vertexTargets, 4, m_numTargets, inFile) != m_numTargets)
		throw;

	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputMatrix"))
			m_inputMatrices.emplace_back(inFile, nodeList);
		else
			m_inputGeometries.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgEnvelope::create(FILE* outFile, bool full)
{
	PString::push("xgEnvelope", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("startVertex", outFile);
		fwrite(&m_startVertex, 4, 1, outFile);
		PString::push("weights", outFile);
		fwrite(&m_numweights, 4, 1, outFile);
		fwrite(m_weights, 16, m_numweights, outFile);
		PString::push("vertexTargets", outFile);
		fwrite(&m_numTargets, 4, 1, outFile);
		fwrite(m_vertexTargets, 4, m_numTargets, outFile);
		for (size_t index = 0; index < m_inputMatrices.size(); ++index)
		{
			PString::push("inputMatrix" + char(index + 49), outFile);
			m_inputMatrices[index]->push(outFile);
			PString::push("envelopeMatrix", outFile);
		}
		for (auto& node : m_inputGeometries)
		{
			PString::push("inputGeometry", outFile);
			node->push(outFile);
			PString::push("outputGeometry", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgEnvelope::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t%s   Starting Vertex: %lu\n", tabs, m_startVertex);
	fprintf_s(outTXT, "\t\t%s      # of Weights: %lu\n", tabs, m_numweights);
	for (unsigned long index = 0; index < m_numweights; ++index)
		fprintf_s(outTXT, "\t\t\t%s     Weight %03lu: %g, %g, %g, %g\n", tabs, index + 1,
			m_weights[index][0], m_weights[index][1], m_weights[index][2], m_weights[index][3]);

	fprintf_s(outTXT, "\t%s       # of Vertex Targets: %lu\n", tabs, m_numTargets);
	for (unsigned long index = 0, env = 0; index < m_numTargets; ++index, ++env)
	{
		fprintf_s(outTXT, "\t\t\t%sVertex Envelope %03li\n", tabs, env + 1);
		unsigned long vert = 1;
		do
		{
			fprintf_s(outTXT, "\t\t\t%s   Target Mesh Vertex %lu: %li\n", tabs, vert++, m_vertexTargets[index++]);
		} while (index < m_numTargets && m_vertexTargets[index] != -1);
	}

	if (m_inputMatrices.size())
	{
		fprintf_s(outTXT, "\t%s       # of Input Matrices: %zu\n", tabs, m_inputMatrices.size());
		for (size_t index = 0; index < m_inputMatrices.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s %zu. %s\n", tabs, index + 1, m_inputMatrices[index].getPString()->m_pstring);
	}
	if (m_inputGeometries.size())
	{
		fprintf_s(outTXT, "\t%s     # of Input Geometries: %zu\n", tabs, m_inputGeometries.size());
		for (size_t index = 0; index < m_inputGeometries.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s %zu. %s\n", tabs, index + 1, m_inputGeometries[index].getPString()->m_pstring);
	}
}

int xgMaterial::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_blendType, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_shadingType, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_diffuse.red, 4, 1, inFile);
	fread(&m_diffuse.green, 4, 1, inFile);
	fread(&m_diffuse.blue, 4, 1, inFile);
	fread(&m_diffuse.alpha, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_specular.red, 4, 1, inFile);
	fread(&m_specular.green, 4, 1, inFile);
	fread(&m_specular.blue, 4, 1, inFile);
	fread(&m_specular.exponent, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_flags, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_textureEnv, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_uTile, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_vTile, 4, 1, inFile);
	PString test(inFile);
	int sizechange = 0;
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTextures.emplace_back(inFile, nodeList);
		const PString& compare = *m_inputTextures.back().getPString();
		for (size_t i = m_inputTextures.size() - 1; i > 0;)
			if (*m_inputTextures[--i].getPString() == compare)
			{
				sizechange += 28 + compare.m_size;
				m_inputTextures.pop_back();
				break;
			}
		PString::pull(inFile);
		test.fill(inFile);
	}
	return sizechange;
}
void xgMaterial::create(FILE* outFile, bool full)
{
	PString::push("xgMaterial", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("blendType", outFile);
		fwrite(&m_blendType, 4, 1, outFile);
		PString::push("shadingType", outFile);
		fwrite(&m_shadingType, 4, 1, outFile);
		PString::push("diffuse", outFile);
		fwrite(&m_diffuse.red, 4, 1, outFile);
		fwrite(&m_diffuse.green, 4, 1, outFile);
		fwrite(&m_diffuse.blue, 4, 1, outFile);
		fwrite(&m_diffuse.alpha, 4, 1, outFile);
		PString::push("specular", outFile);
		fwrite(&m_specular.red, 4, 1, outFile);
		fwrite(&m_specular.green, 4, 1, outFile);
		fwrite(&m_specular.blue, 4, 1, outFile);
		fwrite(&m_specular.exponent, 4, 1, outFile);
		PString::push("flags", outFile);
		fwrite(&m_flags, 4, 1, outFile);
		PString::push("textureEnv", outFile);
		fwrite(&m_textureEnv, 4, 1, outFile);
		PString::push("uTile", outFile);
		fwrite(&m_uTile, 4, 1, outFile);
		PString::push("vTile", outFile);
		fwrite(&m_vTile, 4, 1, outFile);
		for (auto& node : m_inputTextures)
		{
			PString::push("inputTexture", outFile);
			node->push(outFile);
			PString::push("outputTexture", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgMaterial::writeTXT(FILE* outTXT, const char* tabs)
{
	switch (m_blendType)
	{
	case 1:
		fprintf_s(outTXT, "\t\t\t%s    Blend Type: Additive (+Tex Alpha)\n", tabs); break;
	case 2:
		fprintf_s(outTXT, "\t\t\t%s    Blend Type: Multiply (Ignore Tex Alpha)\n", tabs); break;
	case 3:
		fprintf_s(outTXT, "\t\t\t%s    Blend Type: Subtractive (Ignore Tex Alpha)\n", tabs); break;
	case 4:
		fprintf_s(outTXT, "\t\t\t%s    Blend Type: Unknown\n", tabs); break;
	case 5:
		fprintf_s(outTXT, "\t\t\t%s    Blend Type: Opaque (+Tex Alpha)\n", tabs); break;
	default:
		fprintf_s(outTXT, "\t\t\t%s    Blend Type: Opaque\n", tabs);
	}

	switch (m_shadingType)
	{
	case 1:
		fprintf_s(outTXT, "\t\t\t%s  Shading Type: Shaded (No specular??)\n", tabs); break;
	case 2:
		fprintf_s(outTXT, "\t\t\t%s  Shading Type: Shaded\n", tabs); break;
	case 3:
		fprintf_s(outTXT, "\t\t\t%s  Shading Type: Unshaded, Vertex Colors\n", tabs); break;
	case 4:
		fprintf_s(outTXT, "\t\t\t%s  Shading Type: Shaded, Vertex Colors\n", tabs); break;
	default:
		fprintf_s(outTXT, "\t\t\t%s  Shading Type: Unshaded\n", tabs);
	}

	fprintf_s(outTXT, "\t\t\t%sDiffuse (RGBA): %g, %g, %g, %g\n", tabs, m_diffuse.red, m_diffuse.green, m_diffuse.blue, m_diffuse.alpha);
	fprintf_s(outTXT, "\t\t%s     Specular (RGBExp): %g, %g, %g, %g\n", tabs, m_specular.red, m_specular.green, m_specular.blue, m_specular.exponent);
	fprintf_s(outTXT, "\t\t\t\t%s Flags: %s (Possibly overriden by blend or shading)\n", tabs, m_flags & 1 ? "Use Alpha" : "Ignore Alpha");
	fprintf_s(outTXT, "\t\t%s   Texture Environment: %s\n", tabs, m_textureEnv ? "Sphere/Environment Map" : "UV Coordinates");
	fprintf_s(outTXT, "\t\t\t\t%s UTile: %lu\n", tabs, m_uTile);
	fprintf_s(outTXT, "\t\t\t\t%s VTile: %lu\n", tabs, m_vTile);
	if (m_inputTextures.size())
	{
		fprintf_s(outTXT, "\t\t\t%s # of Textures: %zu\n", tabs, m_inputTextures.size());
		for (size_t index = 0; index < m_inputTextures.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s     %zu. %s\n", tabs, index + 1, m_inputTextures[index].getPString()->m_pstring);
	}
}

int xgMultiPassMaterial::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	PString test;
	int sizechange = 0;
	do
	{
		m_inputMaterials.emplace_back(inFile, nodeList);
		const PString& compare = *m_inputMaterials.back().getPString();
		for (size_t i = m_inputMaterials.size() - 1; i > 0;)
			if (*m_inputMaterials[--i].getPString() == compare)
			{
				sizechange += 30 + compare.m_size;
				m_inputMaterials.pop_back();
				break;
			}
		PString::pull(inFile);
		test.fill(inFile);
	} while (!strchr(test.m_pstring, '}'));
	return sizechange;
}
void xgMultiPassMaterial::create(FILE* outFile, bool full)
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
void xgMultiPassMaterial::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t%s     # of Materials: %zu\n", tabs, m_inputMaterials.size());
	for (size_t index = 0; index < m_inputMaterials.size(); ++index)
		fprintf_s(outTXT, "\t\t%s %zu. %s\n", tabs, index + 1, m_inputMaterials[index].getPString()->m_pstring);
}

xgInterpolator::xgInterpolator(const xgInterpolator& inter) : m_type(inter.m_type), m_inputTimes(inter.m_inputTimes) {}

xgNormalInterpolator::Key& xgNormalInterpolator::Key::operator=(const Key& norm)
{
	m_numNormals = norm.m_numNormals;
	m_normals = new float[m_numNormals][3];
	memcpy_s(m_normals, 12ULL * m_numNormals, norm.m_normals, 12ULL * m_numNormals);
	return *this;
}
xgNormalInterpolator::Key::~Key()
{
	if (m_normals != nullptr)
		delete[m_numNormals] m_normals;
}
xgNormalInterpolator::xgNormalInterpolator(const xgNormalInterpolator& norm)
	: xgInterpolator(norm), m_numtimes(norm.m_numtimes), m_numkeys(norm.m_numkeys), m_numTargets(norm.m_numTargets)
{
	m_name = norm.m_name;
	m_times = new float[m_numtimes];
	std::copy(norm.m_times, norm.m_times + m_numtimes, m_times);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
		m_keys[k] = norm.m_keys[k];
	m_targets = new unsigned long[m_numTargets];
	std::copy(norm.m_targets, norm.m_targets + m_numTargets, m_targets);
}
xgNormalInterpolator::~xgNormalInterpolator()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;

	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;

	if (m_targets != nullptr)
		delete[m_numTargets] m_targets;
}
int xgNormalInterpolator::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_type, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numtimes, 4, 1, inFile);
	m_times = new float[m_numtimes];
	if (fread(m_times, 4, m_numtimes, inFile) != m_numtimes)
		throw;
	PString::pull(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
	{
		fread(&m_keys[k].m_numNormals, 4, 1, inFile);
		m_keys[k].m_normals = new float[m_keys[k].m_numNormals][3];
		fread(m_keys[k].m_normals, 12, m_keys[k].m_numNormals, inFile);
	}
	PString::pull(inFile);
	fread(&m_numTargets, 4, 1, inFile);
	m_targets = new unsigned long[m_numTargets];
	if (fread(m_targets, 4, m_numTargets, inFile) != m_numTargets)
		throw;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgNormalInterpolator::create(FILE* outFile, bool full)
{
	PString::push("xgNormalInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		PString::push("times", outFile);
		fwrite(&m_numtimes, 4, 1, outFile);
		fwrite(m_times, 4, m_numtimes, outFile);
		PString::push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (unsigned long k = 0; k < m_numkeys; ++k)
		{
			fwrite(&m_keys[k].m_numNormals, 4, 1, outFile);
			fwrite(m_keys[k].m_normals, 12, m_keys[k].m_numNormals, outFile);
		}
		PString::push("targets", outFile);
		fwrite(&m_numTargets, 4, 1, outFile);
		fwrite(m_targets, 4, m_numTargets, outFile);
		PString::push("inputTime", outFile);
		m_inputTimes.front()->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgNormalInterpolator::writeTXT(FILE* outTXT, const char* tabs)
{
	if (m_type)
		fprintf_s(outTXT, "\t\t%s Interpolation: TRUE (%lu)\n", tabs, m_type);
	else
		fprintf_s(outTXT, "\t\t%s Interpolation: FALSE\n", tabs);

	fprintf_s(outTXT, "\t\t%s    # of Times: %lu\n", tabs, m_numtimes);
	for (unsigned long index = 0; index < m_numtimes; ++index)
		fprintf_s(outTXT, "\t\t\t%s     Time %lu: %g\n", tabs, index + 1, m_times[index]);

	fprintf_s(outTXT, "\t\t%s     # of Keys: %lu\n", tabs, m_numkeys);
	for (unsigned long index = 0; index < m_numkeys; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s   Key %lu:\n", tabs, index + 1);
		fprintf_s(outTXT, "\t\t\t%s       # of Normals: %lu\n", tabs, m_keys[index].m_numNormals);
		for (unsigned long norm = 0; norm < m_keys[index].m_numNormals; ++norm)
		{
			fprintf_s(outTXT, "\t\t\t\t%s       Normal %lu: %g, %g, %g\n", tabs, norm + 1,
				m_keys[index].m_normals[norm][0], m_keys[index].m_normals[norm][1], m_keys[index].m_normals[norm][2]);
		}
	}

	fprintf_s(outTXT, "\t\t%s  # of Targets: %lu\n", tabs, m_numTargets);
	for (unsigned long index = 0; index < m_numTargets; ++index)
		fprintf_s(outTXT, "\t\t\t%s  Target %lu: %lu\n", tabs, index + 1, m_targets[index]);

	fprintf_s(outTXT, "\t\t%s    Input Time: %s\n", tabs, m_inputTimes.front().getPString()->m_pstring);
}

xgQuatInterpolator::xgQuatInterpolator(const xgQuatInterpolator& quat)
	: xgInterpolator(quat), m_numkeys(quat.m_numkeys)
{
	m_name = quat.m_name;
	m_keys = new float[m_numkeys][4];
	memcpy_s(m_keys, 16ULL * m_numkeys, quat.m_keys, 16ULL * m_numkeys);
}
xgQuatInterpolator::~xgQuatInterpolator()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
int xgQuatInterpolator::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_type, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new float[m_numkeys][4];
	if (fread(m_keys, 16, m_numkeys, inFile) != m_numkeys)
		throw;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgQuatInterpolator::create(FILE* outFile, bool full)
{
	PString::push("xgQuatInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		PString::push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		fwrite(m_keys, 16, m_numkeys, outFile);
		PString::push("inputTime", outFile);
		m_inputTimes.front()->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgQuatInterpolator::writeTXT(FILE* outTXT, const char* tabs)
{
	if (m_type)
		fprintf_s(outTXT, "\t\t%s Interpolation: TRUE (%lu)\n", tabs, m_type);
	else
		fprintf_s(outTXT, "\t\t%s Interpolation: FALSE\n", tabs);

	fprintf_s(outTXT, "\t%s       # of Keys (XYZW): %lu\n", tabs, m_numkeys);
	for (unsigned long index = 0; index < m_numkeys; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s   Key %lu: %g, %g, %g, %g\n", tabs, index + 1,
			m_keys[index][0], m_keys[index][1], m_keys[index][2], m_keys[index][3]);
	}

	fprintf_s(outTXT, "\t\t%s    Input Time: %s\n", tabs, m_inputTimes.front().getPString()->m_pstring);
}

xgShapeInterpolator::Key& xgShapeInterpolator::Key::operator=(const Key& m_key)
{
	m_vertexType = m_key.m_vertexType;
	int size = 0;
	if (m_vertexType & 1)
		size += 4;
	if (m_vertexType & 2)
		size += 3;
	if (m_vertexType & 4)
		size += 4;
	if (m_vertexType & 8)
		size += 2;
	m_numVerts = m_key.m_numVerts;
	m_vertices = new float[m_numVerts * size];
	std::copy(m_key.m_vertices, m_key.m_vertices + m_numVerts * size, m_vertices);
	return *this;
}
xgShapeInterpolator::Key::~Key()
{
	int size = 0;
	if (m_vertexType & 1)
		size += 4;
	if (m_vertexType & 2)
		size += 3;
	if (m_vertexType & 4)
		size += 4;
	if (m_vertexType & 8)
		size += 2;
	delete[m_numVerts * size] m_vertices;
}
xgShapeInterpolator::xgShapeInterpolator(const xgShapeInterpolator& shape)
	: xgInterpolator(shape), m_numtimes(shape.m_numtimes), m_numkeys(shape.m_numkeys)
{
	m_name = shape.m_name;
	m_times = new float[m_numtimes];
	std::copy(shape.m_times, shape.m_times + m_numtimes, m_times);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
		m_keys[k] = shape.m_keys[k];
}
xgShapeInterpolator::~xgShapeInterpolator()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;

	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
int xgShapeInterpolator::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_type, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numtimes, 4, 1, inFile);
	m_times = new float[m_numtimes];
	if (fread(m_times, 4, m_numtimes, inFile) != m_numtimes)
		throw;
	PString::pull(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
	{
		Key& key = m_keys[k];
		fread(&key.m_vertexType, 4, 1, inFile);
		int size = 0;
		if (key.m_vertexType & 1)
			size += 4;
		if (key.m_vertexType & 2)
			size += 3;
		if (key.m_vertexType & 4)
			size += 4;
		if (key.m_vertexType & 8)
			size += 2;
		fread(&key.m_numVerts, 4, 1, inFile);
		key.m_vertices = new float[key.m_numVerts * size];
		fread(key.m_vertices, 4, key.m_numVerts * size, inFile);
	}
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgShapeInterpolator::create(FILE* outFile, bool full)
{
	PString::push("xgShapeInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		PString::push("times", outFile);
		fwrite(&m_numtimes, 4, 1, outFile);
		fwrite(m_times, 4, m_numtimes, outFile);
		PString::push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (unsigned long k = 0; k < m_numkeys; ++k)
		{
			Key& key = m_keys[k];
			fwrite(&key.m_vertexType, 4, 1, outFile);
			int size = 0;
			if (key.m_vertexType & 1)
				size += 4;
			if (key.m_vertexType & 2)
				size += 3;
			if (key.m_vertexType & 4)
				size += 4;
			if (key.m_vertexType & 8)
				size += 2;
			fwrite(&key.m_numVerts, 4, 1, outFile);
			fwrite(key.m_vertices, 4, key.m_numVerts * size, outFile);
		}
		PString::push("inputTime", outFile);
		m_inputTimes.front()->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgShapeInterpolator::writeTXT(FILE* outTXT, const char* tabs)
{
	if (m_type)
		fprintf_s(outTXT, "\t\t%s Interpolation: TRUE (%lu)\n", tabs, m_type);
	else
		fprintf_s(outTXT, "\t\t%s Interpolation: FALSE\n", tabs);

	fprintf_s(outTXT, "\t\t%s    # of Times: %lu\n", tabs, m_numtimes);
	for (unsigned long index = 0; index < m_numtimes; ++index)
		fprintf_s(outTXT, "\t\t\t%s     Time %lu: %g\n", tabs, index + 1, m_times[index]);

	fprintf_s(outTXT, "\t\t%s     # of Keys: %lu\n", tabs, m_numkeys);
	for (unsigned long index = 0; index < m_numkeys; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s   Key %lu:\n", tabs, index + 1);
		fprintf_s(outTXT, "\t\t\t\t%sVertex Type: %lu\n", tabs, m_keys[index].m_vertexType);
		fprintf_s(outTXT, "\t\t\t%s      # of Vertices: %lu\n", tabs, m_keys[index].m_numVerts);
		float* data = m_keys[index].m_vertices;
		for (unsigned long vert = 0; vert < m_keys[index].m_numVerts; ++vert)
		{
			fprintf_s(outTXT, "\t\t\t\t%s       Vertex %lu:\n", tabs, vert + 1);
			// Position
			if (m_type & 1)
			{
				fprintf_s(outTXT, "\t\t\t\t\t\t%s      Position (XYZW): %g, %g, %g, %g\n", tabs, *data, *(data + 1), *(data + 2), *(data + 3));
				data += 4;
			}

			// Normal
			if (m_type & 2)
			{
				fprintf_s(outTXT, "\t\t\t\t\t\t\t%s Normal (XYZ): %g, %g, %g\n", tabs, *data, *(data + 1), *(data + 2));
				data += 3;
			}

			// Color
			if (m_type & 4)
			{
				fprintf_s(outTXT, "\t\t\t\t\t\t\t%s Color (RGBA): %g, %g, %g, %g\n", tabs, *data, *(data + 1), *(data + 2), *(data + 3));
				data += 4;
			}

			// Texture Coordinate
			if (m_type & 8)
			{
				fprintf_s(outTXT, "\t\t\t\t\t%s      Texture Coordinate (ST): %g, %g\n", tabs, *data, *(data + 1));
				data += 2;
			}
		}
	}

	fprintf_s(outTXT, "\t\t%s    Input Time: %s\n", tabs, m_inputTimes.front().getPString()->m_pstring);
}

xgTexCoordInterpolator::Key& xgTexCoordInterpolator::Key::operator=(const Key& m_key)
{
	m_numCoords = m_key.m_numCoords;
	m_texcoords = new float[m_numCoords][2];
	memcpy_s(m_texcoords, 8ULL * m_numCoords, m_key.m_texcoords, 8ULL * m_numCoords);
	return *this;
}
xgTexCoordInterpolator::Key::~Key()
{
	if (m_texcoords != nullptr)
		delete[m_numCoords] m_texcoords;
}
xgTexCoordInterpolator::xgTexCoordInterpolator(const xgTexCoordInterpolator& tex)
	: xgInterpolator(tex), m_numtimes(tex.m_numtimes), m_numkeys(tex.m_numkeys), m_numTargets(tex.m_numTargets)
{
	m_name = tex.m_name;
	m_times = new float[m_numtimes];
	std::copy(tex.m_times, tex.m_times + m_numtimes, m_times);
	m_keys = new Key[m_numkeys];

	for (unsigned long k = 0; k < m_numkeys; ++k)
		m_keys[k] = tex.m_keys[k];

	m_targets = new unsigned long[m_numTargets];
	std::copy(tex.m_targets, tex.m_targets + m_numTargets, m_targets);
}
xgTexCoordInterpolator::~xgTexCoordInterpolator()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;

	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;

	if (m_targets != nullptr)
		delete[m_numTargets] m_targets;
}
int xgTexCoordInterpolator::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_type, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numtimes, 4, 1, inFile);
	m_times = new float[m_numtimes];
	if (fread(m_times, 4, m_numtimes, inFile) != m_numtimes)
		throw;
	PString::pull(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
	{
		fread(&m_keys[k].m_numCoords, 4, 1, inFile);
		m_keys[k].m_texcoords = new float[m_keys[k].m_numCoords][2];
		fread(m_keys[k].m_texcoords, 8, m_keys[k].m_numCoords, inFile);
	}
	PString::pull(inFile);
	fread(&m_numTargets, 4, 1, inFile);
	m_targets = new unsigned long[m_numTargets];
	if (fread(m_targets, 4, m_numTargets, inFile) != m_numTargets)
		throw;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgTexCoordInterpolator::create(FILE* outFile, bool full)
{
	PString::push("xgTexCoordInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		PString::push("times", outFile);
		fwrite(&m_numtimes, 4, 1, outFile);
		fwrite(m_times, 4, m_numtimes, outFile);
		PString::push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (unsigned long k = 0; k < m_numkeys; ++k)
		{
			fwrite(&m_keys[k].m_numCoords, 4, 1, outFile);
			fwrite(m_keys[k].m_texcoords, 8, m_keys[k].m_numCoords, outFile);
		}
		PString::push("targets", outFile);
		fwrite(&m_numTargets, 4, 1, outFile);
		fwrite(m_targets, 4, m_numTargets, outFile);
		PString::push("inputTime", outFile);
		m_inputTimes.front()->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgTexCoordInterpolator::writeTXT(FILE* outTXT, const char* tabs)
{

	if (m_type)
		fprintf_s(outTXT, "\t\t%s Interpolation: TRUE (%lu)\n", tabs, m_type);
	else
		fprintf_s(outTXT, "\t\t%s Interpolation: FALSE\n", tabs);

	fprintf_s(outTXT, "\t\t%s    # of Times: %lu\n", tabs, m_numtimes);
	for (unsigned long index = 0; index < m_numtimes; ++index)
		fprintf_s(outTXT, "\t\t\t%s     Time %lu: %g\n", tabs, index + 1, m_times[index]);

	fprintf_s(outTXT, "\t\t%s     # of Keys: %lu\n", tabs, m_numkeys);
	for (unsigned long index = 0; index < m_numkeys; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s   Key %lu:\n", tabs, index + 1);
		fprintf_s(outTXT, "\t\t\t%s       # of Normals: %lu\n", tabs, m_keys[index].m_numCoords);
		for (unsigned long vert = 0; vert < m_keys[index].m_numCoords; ++vert)
		{
			fprintf_s(outTXT, "\t\t\t\t%s  Vertex %lu (ST): %g, %g\n", tabs, vert + 1,
				m_keys[index].m_texcoords[vert][0], m_keys[index].m_texcoords[vert][1]);
		}
	}

	fprintf_s(outTXT, "\t\t%s  # of Targets: %lu\n", tabs, m_numTargets);
	for (unsigned long index = 0; index < m_numTargets; ++index)
		fprintf_s(outTXT, "\t\t\t%s  Target %lu: %lu\n", tabs, index + 1, m_targets[index]);

	fprintf_s(outTXT, "\t\t%s    Input Time: %s\n", tabs, m_inputTimes.front().getPString()->m_pstring);
}

int xgTexture::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	m_imxName.fill(inFile);
	PString::pull(inFile);
	fread(&m_mipmap_depth, 4, 1, inFile);
	PString::pull(inFile);
	return 0;
}
void xgTexture::create(FILE* outFile, bool full)
{
	PString::push("xgTexture", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("url", outFile);
		m_imxName.push(outFile);
		PString::push("mipmap_depth", outFile);
		fwrite(&m_mipmap_depth, 4, 1, outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgTexture::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t\t%s    Texture: %s\n", tabs, m_imxName.m_pstring);
	fprintf_s(outTXT, "\t\t%s      Mip map depth: %lu\n", tabs, m_mipmap_depth);
}

int xgTime::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_numFrames, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_time, 4, 1, inFile);
	PString::pull(inFile);
	return 0;
}
void xgTime::create(FILE* outFile, bool full)
{
	PString::push("xgTime", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("numFrames", outFile);
		fwrite(&m_numFrames, 4, 1, outFile);
		PString::push("time", outFile);
		fwrite(&m_time, 4, 1, outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgTime::writeTXT(FILE* outTXT, const char* tabs)
{
	fprintf_s(outTXT, "\t\t%s   # of Frames: %g\n", tabs, m_numFrames);
	fprintf_s(outTXT, "\t\t\t%s  Time: %g (Starting point??)\n", tabs, m_time);
}

xgVec3Interpolator::xgVec3Interpolator(const xgVec3Interpolator& norm)
	: xgInterpolator(norm), m_numkeys(norm.m_numkeys)
{
	m_name = norm.m_name;
	m_keys = new float[m_numkeys][3];
	memcpy_s(m_keys, 12ULL * m_numkeys, norm.m_keys, 12ULL * m_numkeys);
}
xgVec3Interpolator::~xgVec3Interpolator()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
int xgVec3Interpolator::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_type, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new float[m_numkeys][3];
	if (fread(m_keys, 12, m_numkeys, inFile) != m_numkeys)
		throw;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgVec3Interpolator::create(FILE* outFile, bool full)
{
	PString::push("xgVec3Interpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		PString::push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		fwrite(m_keys, 12, m_numkeys, outFile);
		PString::push("inputTime", outFile);
		m_inputTimes.front()->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgVec3Interpolator::writeTXT(FILE* outTXT, const char* tabs)
{
	if (m_type)
		fprintf_s(outTXT, "\t\t%s Interpolation: TRUE (%lu)\n", tabs, m_type);
	else
		fprintf_s(outTXT, "\t\t%s Interpolation: FALSE\n", tabs);

	fprintf_s(outTXT, "\t\t%s     # of Keys: %lu\n", tabs, m_numkeys);
	for (unsigned long index = 0; index < m_numkeys; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s   Key %lu: %g, %g, %g\n", tabs, index + 1,
			m_keys[index][0], m_keys[index][1], m_keys[index][2]);
	}

	fprintf_s(outTXT, "\t\t%s    Input Time: %s\n", tabs, m_inputTimes.front().getPString()->m_pstring);
}

xgVertexInterpolator::Key& xgVertexInterpolator::Key::operator=(const Key& m_key)
{
	m_numPositions = m_key.m_numPositions;
	m_positions = new float[m_numPositions][3];
	memcpy_s(m_positions, 12ULL * m_numPositions, m_key.m_positions, 12ULL * m_numPositions);
	return *this;
}
xgVertexInterpolator::Key::~Key()
{
	if (m_positions != nullptr)
		delete[m_numPositions] m_positions;
}
xgVertexInterpolator::xgVertexInterpolator(const xgVertexInterpolator& vert)
	: xgInterpolator(vert), m_times(vert.m_times), m_numkeys(vert.m_numkeys), m_targets(vert.m_targets)
{
	m_name = vert.m_name;
	m_times = new float[m_numtimes];
	std::copy(vert.m_times, vert.m_times + m_numtimes, m_times);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
		m_keys[k] = vert.m_keys[k];

	m_targets = new unsigned long[m_numTargets];
	std::copy(vert.m_targets, vert.m_targets + m_numTargets, m_targets);
}
xgVertexInterpolator::~xgVertexInterpolator()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;

	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;

	if (m_targets != nullptr)
		delete[m_numTargets] m_targets;
}
int xgVertexInterpolator::read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_type, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_numtimes, 4, 1, inFile);
	m_times = new float[m_numtimes];
	if (fread(m_times, 4, m_numtimes, inFile) != m_numtimes)
		throw;
	PString::pull(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (unsigned long k = 0; k < m_numkeys; ++k)
	{
		fread(&m_keys[k].m_numPositions, 4, 1, inFile);
		m_keys[k].m_positions = new float[m_keys[k].m_numPositions][3];
		fread(m_keys[k].m_positions, 12, m_keys[k].m_numPositions, inFile);
	}
	PString::pull(inFile);
	fread(&m_numTargets, 4, 1, inFile);
	m_targets = new unsigned long[m_numTargets];
	if (fread(m_targets, 4, m_numTargets, inFile) != m_numTargets)
		throw;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}
void xgVertexInterpolator::create(FILE* outFile, bool full)
{
	PString::push("xgVertexInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		PString::push("times", outFile);
		fwrite(&m_numtimes, 4, 1, outFile);
		fwrite(m_times, 4, m_numtimes, outFile);
		PString::push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (unsigned long k = 0; k < m_numkeys; ++k)
		{
			fwrite(&m_keys[k].m_numPositions, 4, 1, outFile);
			fwrite(m_keys[k].m_positions, 12, m_keys[k].m_numPositions, outFile);
		}
		PString::push("targets", outFile);
		fwrite(&m_numTargets, 4, 1, outFile);
		fwrite(m_targets, 4, m_numTargets, outFile);
		for (auto& node : m_inputTimes)
		{
			PString::push("inputTime", outFile);
			node->push(outFile);
			PString::push("outputTime", outFile);
		}
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgVertexInterpolator::writeTXT(FILE* outTXT, const char* tabs)
{
	if (m_type)
		fprintf_s(outTXT, "\t\t%s Interpolation: TRUE (%lu)\n", tabs, m_type);
	else
		fprintf_s(outTXT, "\t\t%s Interpolation: FALSE\n", tabs);

	fprintf_s(outTXT, "\t\t%s    # of Times: %lu\n", tabs, m_numtimes);
	for (unsigned long index = 0; index < m_numtimes; ++index)
		fprintf_s(outTXT, "\t\t\t%s     Time %lu: %g\n", tabs, index + 1, m_times[index]);

	fprintf_s(outTXT, "\t\t%s     # of Keys: %lu\n", tabs, m_numkeys);
	for (unsigned long index = 0; index < m_numkeys; ++index)
	{
		fprintf_s(outTXT, "\t\t\t%s   Key %lu:\n", tabs, index + 1);
		fprintf_s(outTXT, "\t\t\t%s       # of Positions?: %lu\n", tabs, m_keys[index].m_numPositions);
		for (unsigned long pos = 0; pos < m_keys[index].m_numPositions; ++pos)
		{
			fprintf_s(outTXT, "\t\t\t\t\t%sPosition %lu: %g, %g, %g\n", tabs, pos + 1,
				m_keys[index].m_positions[pos][0], m_keys[index].m_positions[pos][1], m_keys[index].m_positions[pos][2]);
		}
	}

	fprintf_s(outTXT, "\t\t%s  # of Targets: %lu\n", tabs, m_numTargets);
	for (unsigned long index = 0; index < m_numTargets; ++index)
		fprintf_s(outTXT, "\t\t\t%s  Target %lu: %lu\n", tabs, index + 1, m_targets[index]);

	if (m_inputTimes.size())
	{
		fprintf_s(outTXT, "\t      %s# of Input Times: %zu\n", tabs, m_inputTimes.size());
		for (size_t index = 0; index < m_inputTimes.size(); ++index)
			fprintf_s(outTXT, "\t\t\t%s %zu. %s\n", tabs, index + 1, m_inputTimes[index].getPString()->m_pstring);
	}
}
