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
void XGNode::read(FILE* inFile)
{
	throw "Error: Unrecognized XGNode type [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
}

xgBgGeometry::Vertices::Vertices(const Vertices& verts)
{
	m_vertexFlags = verts.m_vertexFlags;
	m_numVerts = verts.m_numVerts;
	m_vertices = new float* [m_numVerts];
	size_t size = 0;
	if (m_vertexFlags & 1)
		size += 4;
	if (m_vertexFlags & 2)
		size += 3;
	if (m_vertexFlags & 4)
		size += 4;
	if (m_vertexFlags & 8)
		size += 2;
	for (size_t v = 0; v < m_numVerts; v++)
	{
		m_vertices[v] = new float[size];
		std::copy(verts.m_vertices[v], verts.m_vertices[v] + size, m_vertices[v]);
	}
}
xgBgGeometry::Vertices::~Vertices()
{
	size_t size = 0;
	if (m_vertexFlags & 1)
		size += 4;
	if (m_vertexFlags & 2)
		size += 3;
	if (m_vertexFlags & 4)
		size += 4;
	if (m_vertexFlags & 8)
		size += 2;
	for (size_t v = 0; v < m_numVerts; v++)
		delete[size] m_vertices[v];
	delete[m_numVerts] m_vertices;
}
void xgBgGeometry::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_density, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_vertexData.m_vertexFlags, 4, 1, inFile);
	fread(&m_vertexData.m_numVerts, 4, 1, inFile);
	m_vertexData.m_vertices = new float* [m_vertexData.m_numVerts];
	size_t size = 0;
	if (m_vertexData.m_vertexFlags & 1)
		size += 4;
	if (m_vertexData.m_vertexFlags & 2)
		size += 3;
	if (m_vertexData.m_vertexFlags & 4)
		size += 4;
	if (m_vertexData.m_vertexFlags & 8)
		size += 2;
	for (size_t v = 0; v < m_vertexData.m_numVerts; v++)
	{
		m_vertexData.m_vertices[v] = new float[size];
		fread(m_vertexData.m_vertices[v], 4, size, inFile);
	}
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputGeometryNames.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgBgGeometry::create(FILE* outFile, bool full)
{
	PString buffer("xgBgGeometry", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("density", outFile);
		fwrite(&m_density, 4, 1, outFile);
		buffer.push("vertices", outFile);
		fwrite(&m_vertexData.m_vertexFlags, 4, 1, outFile);
		fwrite(&m_vertexData.m_numVerts, 4, 1, outFile);
		size_t size = 0;
		if (m_vertexData.m_vertexFlags & 1)
			size += 4;
		if (m_vertexData.m_vertexFlags & 2)
			size += 3;
		if (m_vertexData.m_vertexFlags & 4)
			size += 4;
		if (m_vertexData.m_vertexFlags & 8)
			size += 2;
		for (size_t v = 0; v < m_vertexData.m_numVerts; v++)
			fwrite(m_vertexData.m_vertices[v], 4, size, outFile);
		for (size_t inp = 0; inp < m_inputGeometryNames.size(); inp++)
		{
			buffer.push("inputGeometry", outFile);
			m_inputGeometryNames[inp].push(outFile);
			buffer.push("outputGeometry", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgBgGeometry::writeTXT(FILE* outTXT, bool fromXgm)
{
	fprintf_s(outTXT, "\t                     Density: %f\n", m_density);
	fprintf_s(outTXT, "\t                Vertex Flags: %lu\n", m_vertexData.m_vertexFlags);
	fprintf_s(outTXT, "\t               # of Vertices: %lu\n", m_vertexData.m_numVerts);
	for (unsigned long index = 0; index < m_vertexData.m_numVerts; index++)
	{
		fprintf_s(outTXT, "\t                    Vertex %03lu:\n", index);
		float* data = m_vertexData.m_vertices[index];
		if (m_vertexData.m_vertexFlags & 1) // Position
			fprintf_s(outTXT, "\t						Position (XYZ+Unk): %f, %f, %f, %f\n", *(data++), *(data++), *(data++), *(data++));
		if (m_vertexData.m_vertexFlags & 2) // Normal
			fprintf_s(outTXT, "\t							  Normal (XYZ): %f, %f, %f\n", *(data++), *(data++), *(data++));
		if (m_vertexData.m_vertexFlags & 4) // Color
			fprintf_s(outTXT, "\t							  Color (RGBA): %f, %f, %f, %f\n", *(data++), *(data++), *(data++), *(data++));
		if (m_vertexData.m_vertexFlags & 8) // Texture Coordinate
			fprintf_s(outTXT, "\t				   Texture Coordinate (ST): %f, %f\n", *(data++), *(data++));
	}
	if (m_inputGeometryNames.size())
	{
		fprintf_s(outTXT, "\t       # of Input Geometries: %zu\n", m_inputGeometryNames.size());
		for (size_t index = 0; index < m_inputGeometryNames.size(); index++)
			fprintf_s(outTXT, "\t\t\t   %zu. %s\n", index, m_inputGeometryNames[index].m_pstring);
	}
}

void xgBgMatrix::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(m_position, 4, 3, inFile);
	piece.fill(inFile);
	fread(m_rotation, 4, 4, inFile);
	piece.fill(inFile);
	fread(m_scale, 4, 3, inFile);
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputPosition"))
			m_inputPosition.fill(inFile);
		else if (strstr(test.m_pstring, "inputRotation"))
			m_inputRotation.fill(inFile);
		else if (strstr(test.m_pstring, "inputScale"))
			m_inputScale.fill(inFile);
		else
			m_inputParentMatrix.fill(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgBgMatrix::create(FILE* outFile, bool full)
{
	PString buffer("xgBgMatrix", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("position", outFile);
		fwrite(m_position, 4, 3, outFile);
		buffer.push("rotation", outFile);
		fwrite(m_rotation, 4, 4, outFile);
		buffer.push("scale", outFile);
		fwrite(m_scale, 4, 3, outFile);
		if (m_inputPosition.m_size)
		{
			buffer.push("inputPosition", outFile);
			m_inputPosition.push(outFile);
			buffer.push("outputVec3", outFile);
		}
		if (m_inputRotation.m_size)
		{
			buffer.push("inputRotation", outFile);
			m_inputRotation.push(outFile);
			buffer.push("outputQuat", outFile);
		}
		if (m_inputScale.m_size)
		{
			buffer.push("inputScale", outFile);
			m_inputScale.push(outFile);
			buffer.push("outputVec3", outFile);
		}
		if (m_inputParentMatrix.m_size)
		{
			buffer.push("inputParentMatrix", outFile);
			m_inputParentMatrix.push(outFile);
			buffer.push("outputMatrix", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgBgMatrix::writeTXT(FILE* outTXT, bool fromXgm)
{
	fprintf_s(outTXT, "\t         Grid Position (XYZ): %f, %f, %f\n", m_position[0], m_position[1], m_position[2]);
	fprintf_s(outTXT, "\t        Grid Rotation (XYZW): %f, %f, %f, %f\n", m_rotation[0], m_rotation[1], m_rotation[2], m_rotation[3]);
	fprintf_s(outTXT, "\t            Grid Scale (XYZ): %f, %f, %f\n", m_scale[0], m_scale[1], m_scale[2]);
	if (m_inputPosition.m_size > 0)
		fprintf_s(outTXT, "\t    Position offset from: %s\n", m_inputPosition.m_pstring);
	if (m_inputRotation.m_size > 0)
		fprintf_s(outTXT, "\t    Rotation offset from: %s\n", m_inputPosition.m_pstring);
	if (m_inputScale.m_size > 0)
		fprintf_s(outTXT, "\t       Scale offset from: %s\n", m_inputPosition.m_pstring);
	if (m_inputParentMatrix.m_size > 0)
		fprintf_s(outTXT, "\t     Parent Input Matrix: %s\n", m_inputPosition.m_pstring);
}

void xgBone::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(m_restMatrix, 4, 16, inFile);
	piece.fill(inFile);
	m_inputMatrix.fill(inFile);
	piece.fill(inFile);
	test.fill(inFile);
}
void xgBone::create(FILE* outFile, bool full)
{
	PString buffer("xgBone", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("restMatrix", outFile);
		fwrite(m_restMatrix, 4, 16, outFile);
		buffer.push("inputMatrix", outFile);
		m_inputMatrix.push(outFile);
		buffer.push("outputMatrix", outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgBone::writeTXT(FILE* outTXT, bool fromXgm)
{
	fprintf_s(outTXT, "\t                 Rest Matrix:\n");
	for (size_t index = 0; index < 16; index++)
		fprintf_s(outTXT, "\t                      Value %02zu: %f\n", index, m_restMatrix[index]);
	fprintf_s(outTXT, "\t           Input Matrix from: %s\n", m_inputMatrix.m_pstring);
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
void xgDagMesh::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_primType, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_primCount, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_primData.m_arraySize, 4, 1, inFile);
	if (m_primData.m_arraySize > 0)
	{
		m_primData.m_arrayData = new unsigned long[m_primData.m_arraySize];
		fread(m_primData.m_arrayData, 4, m_primData.m_arraySize, inFile);
	}
	piece.fill(inFile);
	fread(&m_triFanCount, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_triFanData.m_arraySize, 4, 1, inFile);
	if (m_triFanData.m_arraySize > 0)
	{
		m_triFanData.m_arrayData = new unsigned long[m_triFanData.m_arraySize];
		fread(m_triFanData.m_arrayData, 4, m_triFanData.m_arraySize, inFile);
	}
	piece.fill(inFile);
	fread(&m_triStripCount, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_triStripData.m_arraySize, 4, 1, inFile);
	if (m_triStripData.m_arraySize > 0)
	{
		m_triStripData.m_arrayData = new unsigned long[m_triStripData.m_arraySize];
		fread(m_triStripData.m_arrayData, 4, m_triStripData.m_arraySize, inFile);
	}
	piece.fill(inFile);
	fread(&m_triListCount, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_triListData.m_arraySize, 4, 1, inFile);
	if (m_triListData.m_arraySize > 0)
	{
		m_triListData.m_arrayData = new unsigned long[m_triListData.m_arraySize];
		fread(m_triListData.m_arrayData, 4, m_triListData.m_arraySize, inFile);
	}
	piece.fill(inFile);
	fread(&m_cullFunc, 4, 1, inFile);
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputGeometry"))
			m_inputGeometry.emplace_back(inFile);
		else
			m_inputMaterial.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgDagMesh::create(FILE* outFile, bool full)
{
	PString buffer("xgDagMesh", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("primType", outFile);
		fwrite(&m_primType, 4, 1, outFile);
		buffer.push("primCount", outFile);
		fwrite(&m_primCount, 4, 1, outFile);
		buffer.push("primData", outFile);
		fwrite(&m_primData.m_arraySize, 4, 1, outFile);
		fwrite(m_primData.m_arrayData, 4, m_primData.m_arraySize, outFile);
		buffer.push("triFanCount", outFile);
		fwrite(&m_triFanCount, 4, 1, outFile);
		buffer.push("triFanData", outFile);
		fwrite(&m_triFanData.m_arraySize, 4, 1, outFile);
		fwrite(m_triFanData.m_arrayData, 4, m_triFanData.m_arraySize, outFile);
		buffer.push("triStripCount", outFile);
		fwrite(&m_triStripCount, 4, 1, outFile);
		buffer.push("triStripData", outFile);
		fwrite(&m_triStripData.m_arraySize, 4, 1, outFile);
		fwrite(m_triStripData.m_arrayData, 4, m_triStripData.m_arraySize, outFile);
		buffer.push("triListCount", outFile);
		fwrite(&m_triListCount, 4, 1, outFile);
		buffer.push("triListData", outFile);
		fwrite(&m_triListData.m_arraySize, 4, 1, outFile);
		fwrite(m_triListData.m_arrayData, 4, m_triListData.m_arraySize, outFile);
		buffer.push("cullFunc", outFile);
		fwrite(&m_cullFunc, 4, 1, outFile);
		for (size_t geo = 0; geo < m_inputGeometry.size(); geo++)
		{
			buffer.push("inputGeometry", outFile);
			m_inputGeometry[geo].push(outFile);
			buffer.push("outputGeometry", outFile);
		}
		for (size_t mat = 0; mat < m_inputMaterial.size(); mat++)
		{
			buffer.push("inputMaterial", outFile);
			m_inputMaterial[mat].push(outFile);
			buffer.push("outputMaterial", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgDagMesh::writeTXT(FILE* outTXT, bool fromXgm)
{
	fprintf_s(outTXT, "\t                    PrimType: ");
	if (m_primType == 4)
		fprintf_s(outTXT, "Kick vertices separately\n");
	else if (m_primType == 5)
		fprintf_s(outTXT, "Kick vertices in groups\n");
	else
		fprintf_s(outTXT, "Who the heck knows\n");
	if (m_primType == 4)
	{
		fprintf_s(outTXT, "\t                # of TriFans: %lu\n", m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; index < m_triFanCount; index++)
		{
			const unsigned long numIndexes = m_triFanData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t					TriFan %03lu\n", index);
			fprintf_s(outTXT, "\t					    # of Vertex Indexes: %03lu\n", numIndexes);
			for (unsigned long vertex = 0; vertex < numIndexes; vertex++, valueIndex++)
			fprintf_s(outTXT, "\t					            Index %03lu: %lu\n", vertex, m_triFanData.m_arrayData[valueIndex]);
		}
		fprintf_s(outTXT, "\t              # of TriStrips: %lu\n", m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; index < m_triStripCount; index++)
		{
			const unsigned long numIndexes = m_triStripData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t					TriStrip %03lu\n", index);
			fprintf_s(outTXT, "\t					    # of Vertex Indexes: %03lu\n", numIndexes);
			for (unsigned long vertex = 0; vertex < numIndexes; vertex++, valueIndex++)
				fprintf_s(outTXT, "\t					            Index %03lu: %lu\n", vertex, m_triStripData.m_arrayData[valueIndex]);
		}
		fprintf_s(outTXT, "\t               # of TriLists: %lu\n", m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; index < m_triListCount; index++)
		{
			const unsigned long numIndexes = m_triListData.m_arrayData[valueIndex++];
			fprintf_s(outTXT, "\t					TriList %03lu\n", index);
			fprintf_s(outTXT, "\t					    # of Vertex Indexes: %03lu\n", numIndexes);
			for (unsigned long vertex = 0; vertex < numIndexes; vertex++, valueIndex++)
				fprintf_s(outTXT, "\t					            Index %03lu: %lu\n", vertex, m_triListData.m_arrayData[valueIndex]);
		}
	}
	else if (m_primType == 5)
	{
		fprintf_s(outTXT, "\t                 TriFanCount: %lu\n", m_triFanCount);
		for (unsigned long index = 0, valueIndex = 0; index < m_triFanCount; index++)
		{
			fprintf_s(outTXT, "\t					TriFan %03lu\n", index);
			fprintf_s(outTXT, "\t					    Initial Vertex Index: %03lu\n", m_triFanData.m_arrayData[valueIndex++]);
			fprintf_s(outTXT, "\t					           # of Vertices: %lu\n", m_triFanData.m_arrayData[valueIndex++]);
		}
		fprintf_s(outTXT, "\t               TriStripCount: %lu\n", m_triStripCount);
		for (unsigned long index = 0, valueIndex = 0; index < m_triStripCount; index++)
		{
			fprintf_s(outTXT, "\t					TriStrip %03lu\n", index);
			fprintf_s(outTXT, "\t					    Initial Vertex Index: %03lu\n", m_triStripData.m_arrayData[valueIndex++]);
			fprintf_s(outTXT, "\t					           # of Vertices: %lu\n", m_triStripData.m_arrayData[valueIndex++]);
		}
		fprintf_s(outTXT, "\t                TriListCount: %lu\n", m_triListCount);
		for (unsigned long index = 0, valueIndex = 0; index < m_triListCount; index++)
		{
			fprintf_s(outTXT, "\t					TriList %03lu\n", index);
			fprintf_s(outTXT, "\t					    Initial Vertex Index: %03lu\n", m_triListData.m_arrayData[valueIndex++]);
			fprintf_s(outTXT, "\t					           # of Vertices: %lu\n", m_triListData.m_arrayData[valueIndex++]);
		}
	}
	fprintf_s(outTXT, "\t                    CullFunc: ");
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
	if (m_inputGeometry.size())
	{
		fprintf_s(outTXT, "\t       # of Input Geometries: %zu\n", m_inputGeometry.size());
		for (size_t index = 0; index < m_inputGeometry.size(); index++)
			fprintf_s(outTXT, "\t\t\t   %zu. %s\n", index, m_inputGeometry[index].m_pstring);
	}
	if (m_inputMaterial.size())
	{
		fprintf_s(outTXT, "\t        # of Input Materials: %zu\n", m_inputMaterial.size());
		for (size_t index = 0; index < m_inputMaterial.size(); index++)
			fprintf_s(outTXT, "\t\t\t   %zu. %s\n", index, m_inputMaterial[index].m_pstring);
	}
}

void xgDagTransform::read(FILE* inFile)
{
	PString piece, test;
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputMatrix.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgDagTransform::create(FILE* outFile, bool full)
{
	PString buffer("xgDagTransform", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		for (size_t mat = 0; mat < m_inputMatrix.size(); mat++)
		{
			buffer.push("inputMatrix", outFile);
			m_inputMatrix[mat].push(outFile);
			buffer.push("outputMatrix", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgDagTransform::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgEnvelope::Key::Key(const Key& m_key)
{
	m_numweights = m_key.m_numweights;
	m_weights = new float[m_numweights][4];
	memcpy_s(m_weights, 16ULL * m_numweights, m_key.m_weights, 16ULL * m_numweights);
}
xgEnvelope::Key::~Key()
{
	if (m_weights != nullptr)
		delete[m_numweights] m_weights;
}
xgEnvelope::Targets::Targets(const Targets& m_targets)
{
	m_numTargets = m_targets.m_numTargets;
	m_vertexTargets = new long[m_numTargets];
	std::copy(m_targets.m_vertexTargets, m_targets.m_vertexTargets + m_numTargets, m_vertexTargets);
}
xgEnvelope::Targets::~Targets()
{
	if (m_vertexTargets != nullptr)
		delete[m_numTargets] m_vertexTargets;
}
void xgEnvelope::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_startVertex, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_key.m_numweights, 4, 1, inFile);
	m_key.m_weights = new float[m_key.m_numweights][4];
	fread(m_key.m_weights, 16, m_key.m_numweights, inFile);
	piece.fill(inFile);
	fread(&m_vertices.m_numTargets, 4, 1, inFile);
	m_vertices.m_vertexTargets = new long[m_vertices.m_numTargets];
	fread(m_vertices.m_vertexTargets, 4, m_vertices.m_numTargets, inFile);
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputMatrix1"))
			m_inputMatrix1.emplace_back(inFile);
		else
			m_inputGeometry.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgEnvelope::create(FILE* outFile, bool full)
{
	PString buffer("xgEnvelope", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("startVertex", outFile);
		fwrite(&m_startVertex, 4, 1, outFile);
		buffer.push("weights", outFile);
		fwrite(&m_key.m_numweights, 4, 1, outFile);
		fwrite(m_key.m_weights, 16, m_key.m_numweights, outFile);
		buffer.push("vertexTargets", outFile);
		fwrite(&m_vertices.m_numTargets, 4, 1, outFile);
		fwrite(m_vertices.m_vertexTargets, 4, m_vertices.m_numTargets, outFile);
		for (size_t mat = 0; mat < m_inputMatrix1.size(); mat++)
		{
			buffer.push("inputMatrix1", outFile);
			m_inputMatrix1[mat].push(outFile);
			buffer.push("envelopeMatrix", outFile);
		}
		for (size_t geo = 0; geo < m_inputGeometry.size(); geo++)
		{
			buffer.push("inputGeometry", outFile);
			m_inputGeometry[geo].push(outFile);
			buffer.push("outputGeometry", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgEnvelope::writeTXT(FILE* outTXT, bool fromXgm)
{

}

void xgMaterial::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_blendType, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_shadingType, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_diffuse.red, 4, 1, inFile);
	fread(&m_diffuse.green, 4, 1, inFile);
	fread(&m_diffuse.blue, 4, 1, inFile);
	fread(&m_diffuse.alpha, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_specular.red, 4, 1, inFile);
	fread(&m_specular.green, 4, 1, inFile);
	fread(&m_specular.blue, 4, 1, inFile);
	fread(&m_specular.exponent, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_flags, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_textureEnv, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_uTile, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_vTile, 4, 1, inFile);
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTexture.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgMaterial::create(FILE* outFile, bool full)
{
	PString buffer("xgMaterial", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("blendType", outFile);
		fwrite(&m_blendType, 4, 1, outFile);
		buffer.push("shadingType", outFile);
		fwrite(&m_shadingType, 4, 1, outFile);
		buffer.push("diffuse", outFile);
		fwrite(&m_diffuse.red, 4, 1, outFile);
		fwrite(&m_diffuse.green, 4, 1, outFile);
		fwrite(&m_diffuse.blue, 4, 1, outFile);
		fwrite(&m_diffuse.alpha, 4, 1, outFile);
		buffer.push("specular", outFile);
		fwrite(&m_specular.red, 4, 1, outFile);
		fwrite(&m_specular.green, 4, 1, outFile);
		fwrite(&m_specular.blue, 4, 1, outFile);
		fwrite(&m_specular.exponent, 4, 1, outFile);
		buffer.push("flags", outFile);
		fwrite(&m_flags, 4, 1, outFile);
		buffer.push("textureEnv", outFile);
		fwrite(&m_textureEnv, 4, 1, outFile);
		buffer.push("uTile", outFile);
		fwrite(&m_uTile, 4, 1, outFile);
		buffer.push("vTile", outFile);
		fwrite(&m_vTile, 4, 1, outFile);
		for (size_t tex = 0; tex < m_inputTexture.size(); tex++)
		{
			buffer.push("inputTexture", outFile);
			m_inputTexture[tex].push(outFile);
			buffer.push("outputTexture", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgMaterial::writeTXT(FILE* outTXT, bool fromXgm)
{

}

void xgMultiPassMaterial::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	do
	{
		m_inputMaterial.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	} while (!strchr(test.m_pstring, '}'));
}
void xgMultiPassMaterial::create(FILE* outFile, bool full)
{
	PString buffer("xgMultiPassMaterial", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		for (size_t mat = 0; mat < m_inputMaterial.size(); mat++)
		{
			buffer.push("inputMaterial", outFile);
			m_inputMaterial[mat].push(outFile);
			buffer.push("outputMaterial", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgMultiPassMaterial::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgNormalInterpolator::Time::Time(const Time& time)
{
	m_numtimes = time.m_numtimes;
	m_times = new float[m_numtimes];
	std::copy(time.m_times, time.m_times + m_numtimes, m_times);
}
xgNormalInterpolator::Time::~Time()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;
}
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
xgNormalInterpolator::Targets::Targets(const Targets& target)
{
	m_numTargets = target.m_numTargets;
	m_targets = new unsigned long[m_numTargets];
	std::copy(target.m_targets, target.m_targets + m_numTargets, m_targets);
}
xgNormalInterpolator::Targets::~Targets()
{
	if (m_targets != nullptr)
		delete[m_numTargets] m_targets;
}
xgNormalInterpolator::xgNormalInterpolator(xgNormalInterpolator& norm)
	: XGNode(norm.m_name), m_type(norm.m_type), m_times(norm.m_times), m_numkeys(norm.m_numkeys), m_targets(norm.m_targets), m_inputTime(norm.m_inputTime)
{
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
		m_keys[k] = norm.m_keys[k];
}
void xgNormalInterpolator::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_type, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_times.m_numtimes, 4, 1, inFile);
	m_times.m_times = new float[m_times.m_numtimes];
	fread(m_times.m_times, 4, m_times.m_numtimes, inFile);
	piece.fill(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
	{
		fread(&m_keys[k].m_numNormals, 4, 1, inFile);
		m_keys[k].m_normals = new float[m_keys[k].m_numNormals][3];
		fread(m_keys[k].m_normals, 12, m_keys[k].m_numNormals, inFile);
	}
	piece.fill(inFile);
	fread(&m_targets.m_numTargets, 4, 1, inFile);
	m_targets.m_targets = new unsigned long[m_targets.m_numTargets];
	fread(m_targets.m_targets, 4, m_targets.m_numTargets, inFile);
	piece.fill(inFile);
	m_inputTime.fill(inFile);
	piece.fill(inFile);
	test.fill(inFile);
}
void xgNormalInterpolator::create(FILE* outFile, bool full)
{
	PString buffer("xgNormalInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		buffer.push("times", outFile);
		fwrite(&m_times.m_numtimes, 4, 1, outFile);
		fwrite(m_times.m_times, 4, m_times.m_numtimes, outFile);
		buffer.push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (size_t k = 0; k < m_numkeys; k++)
		{
			fwrite(&m_keys[k].m_numNormals, 4, 1, outFile);
			fwrite(m_keys[k].m_normals, 12, m_keys[k].m_numNormals, outFile);
		}
		buffer.push("targets", outFile);
		fwrite(&m_targets.m_numTargets, 4, 1, outFile);
		fwrite(m_targets.m_targets, 4, m_targets.m_numTargets, outFile);
		buffer.push("inputTime", outFile);
		m_inputTime.push(outFile);
		buffer.push("outputTime", outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
xgNormalInterpolator::~xgNormalInterpolator()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
void xgNormalInterpolator::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgQuatInterpolator::Key::Key(const Key& m_key)
{
	m_numkeys = m_key.m_numkeys;
	m_keys = new float[m_numkeys][4];
	memcpy_s(m_keys, 16ULL * m_numkeys, m_key.m_keys, 16ULL * m_numkeys);
}
xgQuatInterpolator::Key::~Key()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
void xgQuatInterpolator::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_type, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_keyStruct.m_numkeys, 4, 1, inFile);
	m_keyStruct.m_keys = new float[m_keyStruct.m_numkeys][4];
	fread(m_keyStruct.m_keys, 16, m_keyStruct.m_numkeys, inFile);
	piece.fill(inFile);
	m_inputTime.fill(inFile);
	piece.fill(inFile);
	test.fill(inFile);
}
void xgQuatInterpolator::create(FILE* outFile, bool full)
{
	PString buffer("xgQuatInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		buffer.push("keys", outFile);
		fwrite(&m_keyStruct.m_numkeys, 4, 1, outFile);
		fwrite(m_keyStruct.m_keys, 16, m_keyStruct.m_numkeys, outFile);
		buffer.push("inputTime", outFile);
		m_inputTime.push(outFile);
		buffer.push("outputTime", outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgQuatInterpolator::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgShapeInterpolator::Time::Time(const Time& time)
{
	m_numtimes = time.m_numtimes;
	m_times = new float[m_numtimes];
	std::copy(time.m_times, time.m_times + m_numtimes, m_times);
}
xgShapeInterpolator::Time::~Time()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;
}
xgShapeInterpolator::Key& xgShapeInterpolator::Key::operator=(const Key& m_key)
{
	m_vertexType = m_key.m_vertexType;
	m_numVerts = m_key.m_numVerts;
	m_vertices = new float* [m_numVerts];
	size_t size = 0;
	if (m_vertexType & 1)
		size += 4;
	if (m_vertexType & 2)
		size += 3;
	if (m_vertexType & 4)
		size += 4;
	if (m_vertexType & 8)
		size += 2;
	for (size_t v = 0; v < m_numVerts; v++)
	{
		m_vertices[v] = new float[size];
		std::copy(m_key.m_vertices[v], m_key.m_vertices[v] + size, m_vertices[v]);
	}
	return *this;
}
xgShapeInterpolator::Key::~Key()
{
	size_t size = 0;
	if (m_vertexType & 1)
		size += 4;
	if (m_vertexType & 2)
		size += 3;
	if (m_vertexType & 4)
		size += 4;
	if (m_vertexType & 8)
		size += 2;
	for (size_t v = 0; v < m_numVerts; v++)
		delete[size] m_vertices[v];
	delete[m_numVerts] m_vertices;
}
xgShapeInterpolator::xgShapeInterpolator(xgShapeInterpolator& shape)
	: XGNode(shape.m_name), m_type(shape.m_type), m_times(shape.m_times), m_numkeys(shape.m_numkeys), m_inputTime(shape.m_inputTime)
{
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
		m_keys[k] = shape.m_keys[k];
}
void xgShapeInterpolator::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_type, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_times.m_numtimes, 4, 1, inFile);
	m_times.m_times = new float[m_times.m_numtimes];
	fread(m_times.m_times, 4, m_times.m_numtimes, inFile);
	piece.fill(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
	{
		Key& key = m_keys[k];
		fread(&key.m_vertexType, 4, 1, inFile);
		size_t size = 0;
		if (key.m_vertexType & 1)
			size += 4;
		if (key.m_vertexType & 2)
			size += 3;
		if (key.m_vertexType & 4)
			size += 4;
		if (key.m_vertexType & 8)
			size += 2;
		fread(&key.m_numVerts, 4, 1, inFile);
		key.m_vertices = new float* [key.m_numVerts];
		for (size_t v = 0; v < key.m_numVerts; v++)
		{
			key.m_vertices[v] = new float[size];
			fread(key.m_vertices[v], 4, size, inFile);
		}
	}
	piece.fill(inFile);
	m_inputTime.fill(inFile);
	piece.fill(inFile);
	test.fill(inFile);
}
void xgShapeInterpolator::create(FILE* outFile, bool full)
{
	PString buffer("xgShapeInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		buffer.push("times", outFile);
		fwrite(&m_times.m_numtimes, 4, 1, outFile);
		fwrite(m_times.m_times, 4, m_times.m_numtimes, outFile);
		buffer.push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (size_t k = 0; k < m_numkeys; k++)
		{
			Key& key = m_keys[k];
			fwrite(&key.m_vertexType, 4, 1, outFile);
			size_t size = 0;
			if (key.m_vertexType & 1)
				size += 4;
			if (key.m_vertexType & 2)
				size += 3;
			if (key.m_vertexType & 4)
				size += 4;
			if (key.m_vertexType & 8)
				size += 2;
			fwrite(&key.m_numVerts, 4, 1, outFile);
			for (size_t v = 0; v < key.m_numVerts; v++)
				fwrite(key.m_vertices[v], 4, size, outFile);
		}
		buffer.push("inputTime", outFile);
		m_inputTime.push(outFile);
		buffer.push("outputTime", outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
xgShapeInterpolator::~xgShapeInterpolator()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
void xgShapeInterpolator::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgTexCoordInterpolator::Time::Time(const Time& time)
{
	m_numtimes = time.m_numtimes;
	m_times = new float[m_numtimes];
	std::copy(time.m_times, time.m_times + m_numtimes, m_times);
}
xgTexCoordInterpolator::Time::~Time()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;
}
xgTexCoordInterpolator::Key& xgTexCoordInterpolator::Key::operator=(const Key& m_key)
{
	m_numVerts = m_key.m_numVerts;
	m_texcoords = new float[m_numVerts][2];
	memcpy_s(m_texcoords, 8ULL * m_numVerts, m_key.m_texcoords, 8ULL * m_numVerts);
	return *this;
}
xgTexCoordInterpolator::Key::~Key()
{
	if (m_texcoords != nullptr)
		delete[m_numVerts] m_texcoords;
}
xgTexCoordInterpolator::Targets::Targets(const Targets& target)
{
	m_numTargets = target.m_numTargets;
	m_targets = new unsigned long[m_numTargets];
	std::copy(target.m_targets, target.m_targets + m_numTargets, m_targets);
}
xgTexCoordInterpolator::Targets::~Targets()
{
	if (m_targets != nullptr)
		delete[m_numTargets] m_targets;
}
xgTexCoordInterpolator::xgTexCoordInterpolator(xgTexCoordInterpolator& tex)
	: XGNode(tex.m_name), m_type(tex.m_type), m_times(tex.m_times), m_numkeys(tex.m_numkeys), m_targets(tex.m_targets), m_inputTime(tex.m_inputTime)
{
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
		m_keys[k] = tex.m_keys[k];
}
void xgTexCoordInterpolator::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_type, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_times.m_numtimes, 4, 1, inFile);
	m_times.m_times = new float[m_times.m_numtimes];
	fread(m_times.m_times, 4, m_times.m_numtimes, inFile);
	piece.fill(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
	{
		fread(&m_keys[k].m_numVerts, 4, 1, inFile);
		m_keys[k].m_texcoords = new float[m_keys[k].m_numVerts][2];
		fread(m_keys[k].m_texcoords, 8, m_keys[k].m_numVerts, inFile);
	}
	piece.fill(inFile);
	fread(&m_targets.m_numTargets, 4, 1, inFile);
	m_targets.m_targets = new unsigned long[m_targets.m_numTargets];
	fread(m_targets.m_targets, 4, m_targets.m_numTargets, inFile);
	piece.fill(inFile);
	m_inputTime.fill(inFile);
	piece.fill(inFile);
	test.fill(inFile);
}
void xgTexCoordInterpolator::create(FILE* outFile, bool full)
{
	PString buffer("xgTexCoordInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		buffer.push("times", outFile);
		fwrite(&m_times.m_numtimes, 4, 1, outFile);
		fwrite(m_times.m_times, 4, m_times.m_numtimes, outFile);
		buffer.push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (size_t k = 0; k < m_numkeys; k++)
		{
			fwrite(&m_keys[k].m_numVerts, 4, 1, outFile);
			fwrite(m_keys[k].m_texcoords, 8, m_keys[k].m_numVerts, outFile);
		}
		buffer.push("targets", outFile);
		fwrite(&m_targets.m_numTargets, 4, 1, outFile);
		fwrite(m_targets.m_targets, 4, m_targets.m_numTargets, outFile);
		buffer.push("inputTime", outFile);
		m_inputTime.push(outFile);
		buffer.push("outputTime", outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
xgTexCoordInterpolator::~xgTexCoordInterpolator()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
void xgTexCoordInterpolator::writeTXT(FILE* outTXT, bool fromXgm)
{

}

void xgTexture::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	m_imxName.fill(inFile);
	piece.fill(inFile);
	fread(&m_mipmap_depth, 4, 1, inFile);
	test.fill(inFile);
}
void xgTexture::create(FILE* outFile, bool full)
{
	PString buffer("xgTexture", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("url", outFile);
		m_imxName.push(outFile);
		buffer.push("mipmap_depth", outFile);
		fwrite(&m_mipmap_depth, 4, 1, outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgTexture::writeTXT(FILE* outTXT, bool fromXgm)
{

}

void xgTime::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_numFrames, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_time, 4, 1, inFile);
	test.fill(inFile);
}
void xgTime::create(FILE* outFile, bool full)
{
	PString buffer("xgTime", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("numFrames", outFile);
		fwrite(&m_numFrames, 4, 1, outFile);
		buffer.push("time", outFile);
		fwrite(&m_time, 4, 1, outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgTime::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgVec3Interpolator::Key::Key(const Key& m_key)
{
	m_numkeys = m_key.m_numkeys;
	m_keys = new float[m_numkeys][3];
	memcpy_s(m_keys, 12ULL * m_numkeys, m_key.m_keys, 12ULL * m_numkeys);
}
xgVec3Interpolator::Key::~Key()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
void xgVec3Interpolator::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_type, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_keyStruct.m_numkeys, 4, 1, inFile);
	m_keyStruct.m_keys = new float[m_keyStruct.m_numkeys][3];
	fread(m_keyStruct.m_keys, 12, m_keyStruct.m_numkeys, inFile);
	piece.fill(inFile);
	m_inputTime.fill(inFile);
	piece.fill(inFile);
	test.fill(inFile);
}
void xgVec3Interpolator::create(FILE* outFile, bool full)
{
	PString buffer("xgVec3Interpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		buffer.push("keys", outFile);
		fwrite(&m_keyStruct.m_numkeys, 4, 1, outFile);
		fwrite(m_keyStruct.m_keys, 12, m_keyStruct.m_numkeys, outFile);
		buffer.push("inputTime", outFile);
		m_inputTime.push(outFile);
		buffer.push("outputTime", outFile);
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
void xgVec3Interpolator::writeTXT(FILE* outTXT, bool fromXgm)
{

}

xgVertexInterpolator::Time::Time(const Time& time)
{
	m_numtimes = time.m_numtimes;
	m_times = new float[m_numtimes];
	std::copy(time.m_times, time.m_times + m_numtimes, m_times);
}
xgVertexInterpolator::Time::~Time()
{
	if (m_times != nullptr)
		delete[m_numtimes] m_times;
}
xgVertexInterpolator::Key& xgVertexInterpolator::Key::operator=(const Key& m_key)
{
	m_numsize = m_key.m_numsize;
	m_nums = new float[m_numsize][3];
	memcpy_s(m_nums, 12ULL * m_numsize, m_key.m_nums, 12ULL * m_numsize);
	return *this;
}
xgVertexInterpolator::Key::~Key()
{
	if (m_nums != nullptr)
		delete[m_numsize] m_nums;
}
xgVertexInterpolator::Targets::Targets(const Targets& target)
{
	m_numTargets = target.m_numTargets;
	m_targets = new unsigned long[m_numTargets];
	std::copy(target.m_targets, target.m_targets + m_numTargets, m_targets);
}
xgVertexInterpolator::Targets::~Targets()
{
	if (m_targets != nullptr)
		delete[m_numTargets] m_targets;
}
xgVertexInterpolator::xgVertexInterpolator(xgVertexInterpolator& vert)
	: XGNode(vert.m_name), m_type(vert.m_type), m_times(vert.m_times), m_numkeys(vert.m_numkeys), m_targets(vert.m_targets), m_inputTimes(vert.m_inputTimes)
{
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
		m_keys[k] = vert.m_keys[k];
}
void xgVertexInterpolator::read(FILE* inFile)
{
	PString piece, test;
	piece.fill(inFile);
	fread(&m_type, 4, 1, inFile);
	piece.fill(inFile);
	fread(&m_times.m_numtimes, 4, 1, inFile);
	m_times.m_times = new float[m_times.m_numtimes];
	fread(m_times.m_times, 4, m_times.m_numtimes, inFile);
	piece.fill(inFile);
	fread(&m_numkeys, 4, 1, inFile);
	m_keys = new Key[m_numkeys];
	for (size_t k = 0; k < m_numkeys; k++)
	{
		fread(&m_keys[k].m_numsize, 4, 1, inFile);
		m_keys[k].m_nums = new float[m_keys[k].m_numsize][3];
		fread(m_keys[k].m_nums, 12, m_keys[k].m_numsize, inFile);
	}
	piece.fill(inFile);
	fread(&m_targets.m_numTargets, 4, 1, inFile);
	m_targets.m_targets = new unsigned long[m_targets.m_numTargets];
	fread(m_targets.m_targets, 4, m_targets.m_numTargets, inFile);
	test.fill(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		m_inputTimes.emplace_back(inFile);
		piece.fill(inFile);
		test.fill(inFile);
	}
}
void xgVertexInterpolator::create(FILE* outFile, bool full)
{
	PString buffer("xgVertexInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		buffer.push("{", outFile);
		buffer.push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
		buffer.push("times", outFile);
		fwrite(&m_times.m_numtimes, 4, 1, outFile);
		fwrite(m_times.m_times, 4, m_times.m_numtimes, outFile);
		buffer.push("keys", outFile);
		fwrite(&m_numkeys, 4, 1, outFile);
		for (size_t k = 0; k < m_numkeys; k++)
		{
			fwrite(&m_keys[k].m_numsize, 4, 1, outFile);
			fwrite(m_keys[k].m_nums, 12, m_keys[k].m_numsize, outFile);
		}
		buffer.push("targets", outFile);
		fwrite(&m_targets.m_numTargets, 4, 1, outFile);
		fwrite(m_targets.m_targets, 4, m_targets.m_numTargets, outFile);
		for (size_t time = 0; time < m_inputTimes.size(); time++)
		{
			buffer.push("inputTime", outFile);
			m_inputTimes[time].push(outFile);
			buffer.push("outputTime", outFile);
		}
		buffer.push("}", outFile);
	}
	else
		buffer.push(";", outFile);
}
xgVertexInterpolator::~xgVertexInterpolator()
{
	if (m_keys != nullptr)
		delete[m_numkeys] m_keys;
}
void xgVertexInterpolator::writeTXT(FILE* outTXT, bool fromXgm)
{

}
