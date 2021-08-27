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
#include "XG.h"
#include <filesystem>
using namespace std;
using namespace GlobalFunctions;
XG::XG()
	: FileType(".XG")
	, m_modelIndex(0)
	, m_fileSize(0)
	, m_unk(0)
	, m_fromXGM(false) {}

XG::XG(FILE* inFile, const std::string& directory)
	: FileType(".XG", true)
	, m_fromXGM(true)
{
	m_directory = directory;
	fread(m_filepath, 1, 256, inFile);
	fread(m_name, 1, 16, inFile);
	m_filename = m_name;
	m_filename.erase(m_filename.length() - 3, 3);
	fread(&m_modelIndex, 4, 1, inFile);
	fread(&m_fileSize, 4, 1, inFile);
	unsigned long numAnims;
	fread(&numAnims, 4, 1, inFile);
	fread(&m_unk, 4, 1, inFile);
	for (size_t a = 0; a < numAnims; ++a)
		m_animations.emplace_back(inFile);

	try
	{
		unsigned long tmp = m_fileSize;
		m_data = std::make_shared<XG_Data>(inFile, m_fileSize);
		if (tmp != m_fileSize)
			m_saved = 0;
	}
	catch (const char* str)
	{
		throw str + std::string(m_name) + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
	catch (std::string str)
	{
		throw str + std::string(m_name) + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
}

XG::XG(std::string filename, bool useBanner)
	: FileType(filename, ".XG", useBanner)
	, m_modelIndex(0)
	, m_unk(false)
{
	try
	{
		unsigned long tmp = m_fileSize;
		m_data = std::make_shared<XG_Data>(m_filePtr, m_fileSize);
		if (tmp != m_fileSize)
			m_saved = 0;
		fclose(m_filePtr);
	}
	catch (const char* str)
	{
		long pos = ftell(m_filePtr) - 4;
		fclose(m_filePtr);
		throw str + m_filename + ".XG [File offset: " + to_string(pos) + "]";
	}
	catch (std::string str)
	{
		long pos = ftell(m_filePtr) - 4;
		fclose(m_filePtr);
		throw str + m_filename + ".XG [File offset: " + to_string(pos) + "]";
	}
}

void XG::create(FILE* outFile)
{
	fwrite(m_filepath, 1, 256, outFile);
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_modelIndex, 4, 1, outFile);
	fwrite(&m_fileSize, 4, 1, outFile);
	const unsigned long size = (unsigned long)m_animations.size();
	fwrite(&size, 4, 1, outFile);
	fwrite(&m_unk, 4, 1, outFile);
	fwrite(&m_animations[0], sizeof(Animation), size, outFile);
	m_data->create(outFile);
	m_saved = 1;
}

bool XG::create(string filename, bool useBanner)
{
	if (FileType::create(filename, true))
	{
		m_data->create(m_filePtr);
		fclose(m_filePtr);
		if (useBanner)
			m_saved = true;
		return true;
	}
	return false;
}

bool XG::write_to_txt()
{
	FILE* txtFile, * simpleTxtFile;
	if (FileType::write_to_txt(txtFile, simpleTxtFile))
	{
		fprintf_s(txtFile, "# of Nodes: %zu\n", m_data->m_nodes.size());
		fprintf_s(simpleTxtFile, "# of Nodes: %zu\n", m_data->m_nodes.size());
		for (size_t index = 0; index < m_data->m_nodes.size(); ++index)
		{
			shared_ptr<XGNode>& node = m_data->m_nodes[index];
			fprintf_s(txtFile, "\t Node %03zu - %s: %s\n", index + 1, node->getType(), node->m_name.m_pstring);
			fprintf_s(simpleTxtFile, "\t Node %03zu - %-18s: %s\n", index + 1, node->getType(), node->m_name.m_pstring);
			node->writeTXT(txtFile);
			fflush(txtFile);
		}
		fclose(txtFile);
		fclose(simpleTxtFile);
		return true;
	}
	return false;
}

bool XG::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile)
{
	fprintf_s(txtFile, "\t\t    Original File Path: %s\n", m_filepath);
	fprintf_s(txtFile, "\t\t\t     File Size: %lu bytes\n", m_fileSize);
	fprintf_s(simpleTxtFile, "\t\t\tFile Size: %lu bytes\n", m_fileSize);
	fprintf_s(txtFile, "\t\t       # of Animations: %zu\n", m_animations.size());
	fprintf_s(simpleTxtFile, "\t\t  # of Animations: %zu\n", m_animations.size());
	for (size_t index = 0; index < m_animations.size(); index++)
	{
		fprintf_s(txtFile, "\t\t\t    Animation Entry %02zu\n", index + 1);
		m_animations[index].write_to_txt(txtFile, simpleTxtFile);
	}

	fprintf_s(txtFile, "\t\t            # of Nodes: %zu\n", m_data->m_nodes.size());
	fprintf_s(simpleTxtFile, "\t\t       # of Nodes: %zu\n", m_data->m_nodes.size());
	for (size_t index = 0; index < m_data->m_nodes.size(); index++)
	{
		std::shared_ptr<XGNode>& node = m_data->m_nodes[index];
		fprintf_s(txtFile, "\t\t\t\t     Node %03zu - %s: %s\n", index + 1, node->getType(), node->m_name.m_pstring);
		fprintf_s(simpleTxtFile, "\t\t\t\t     Node %03zu - %-18s: %s\n", index + 1, node->getType(), node->m_name.m_pstring);
		node->writeTXT(txtFile, "\t\t\t    ");
		fflush(txtFile);
	}
	return true;
}

bool XG::write_to_obj(std::string newDirectory)
{
	GlobalFunctions::banner(" " + m_filename + " - Model Export ");
	string file = newDirectory.length() ? newDirectory + m_filename + ".obj" : m_directory + m_filename + ".obj";
	struct Position
	{
		float x, y, z;
	};
	struct TexCoord
	{
		float s, t;
	};
	struct Normal
	{
		float i, j, k;
	};
	struct Face
	{
		unsigned long pos[3] = { 0 };
		unsigned long tex[3] = { 0 };
		unsigned long nor[3] = { 0 };
	};
	vector<Position> vertices;
	vector<TexCoord> texCoords;
	vector<Normal> vertNormals;
	vector<Face> faces;
	for (shared_ptr<XGNode>& node : m_data->m_nodes)
	{
		if (dynamic_pointer_cast<xgDagMesh>(node))
		{
			xgDagMesh* mesh = (xgDagMesh*)node.get();
			for (auto& input : mesh->m_inputGeometries)
			{
				for (shared_ptr<XGNode>& node2 : m_data->m_nodes)
				{
					if (input.m_node == node2.get())
					{
						xgBgGeometry* geo = (xgBgGeometry*)node2.get();
						unsigned long vertexSize = 0;
						if (geo->m_vertexFlags & 1) // Position
							vertexSize += 4;
						if (geo->m_vertexFlags & 2) // Normal
							vertexSize += 3;
						if (geo->m_vertexFlags & 4) // Color
							vertexSize += 4;
						if (geo->m_vertexFlags & 8) // Texture Coordinate
							vertexSize += 2;

						if (mesh->m_primType == 4)
						{
							if (mesh->m_triFanData.m_elementCount)
							{
								for (unsigned long valueIndex = 0; valueIndex < mesh->m_triFanData.m_arraySize;)
								{
									const unsigned long vertIndex = (unsigned long)vertices.size();
									const unsigned long normIndex = (unsigned long)vertNormals.size();
									const unsigned long coordIndex = (unsigned long)texCoords.size();
									const unsigned long numIndexes = mesh->m_triFanData.m_arrayData[valueIndex++];
									{
										unsigned long firstIndex = mesh->m_triFanData.m_arrayData[valueIndex++] * vertexSize;
										unsigned long secondIndex = mesh->m_triFanData.m_arrayData[valueIndex++] * vertexSize;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertices.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertNormals.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 3;
											secondIndex += 3;
										}
										if (geo->m_vertexFlags & 4) // Color
										{
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1] });
											texCoords.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1] });
										}
									}
									for (unsigned long vertex = 2; vertex < numIndexes; ++vertex)
									{
										unsigned long index = mesh->m_triFanData.m_arrayData[valueIndex++] * vertexSize;
										Face face;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.pos[0] = vertIndex;
											face.pos[1] = vertIndex + vertex - 1;
											face.pos[2] = vertIndex + vertex;
											index += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.nor[0] = normIndex;
											face.nor[1] = normIndex + vertex - 1;
											face.nor[2] = normIndex + vertex;
											index += 3;
										}
										if (geo->m_vertexFlags & 4)
											index += 4;
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1] });
											face.tex[0] = coordIndex;
											face.tex[1] = coordIndex + vertex - 1;
											face.tex[2] = coordIndex + vertex;
										}
										faces.push_back(face);
									}
								}
							}

							if (mesh->m_triStripData.m_elementCount)
							{
								for (unsigned long valueIndex = 0; valueIndex < mesh->m_triStripData.m_arraySize;)
								{
									const unsigned long vertIndex = (unsigned long)vertices.size();
									const unsigned long normIndex = (unsigned long)vertNormals.size();
									const unsigned long coordIndex = (unsigned long)texCoords.size();
									const unsigned long numIndexes = mesh->m_triStripData.m_arrayData[valueIndex++];
									{
										unsigned long firstIndex = mesh->m_triStripData.m_arrayData[valueIndex++] * vertexSize;
										unsigned long secondIndex = mesh->m_triStripData.m_arrayData[valueIndex++] * vertexSize;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertices.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertNormals.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 3;
											secondIndex += 3;
										}
										if (geo->m_vertexFlags & 4) // Color
										{
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1] });
											texCoords.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1] });
										}
									}
									for (unsigned long vertex = 2; vertex < numIndexes; ++vertex)
									{
										unsigned long index = mesh->m_triStripData.m_arrayData[valueIndex++] * vertexSize;
										Face face;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.pos[0] = vertIndex + vertex - 2;
											if (vertex & 1)
											{
												face.pos[1] = vertIndex + vertex;
												face.pos[2] = vertIndex + vertex - 1;
											}
											else
											{
												face.pos[1] = vertIndex + vertex - 1;
												face.pos[2] = vertIndex + vertex;
											}
											index += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.nor[0] = normIndex + vertex - 2;
											if (vertex & 1)
											{
												face.nor[1] = normIndex + vertex;
												face.nor[2] = normIndex + vertex - 1;
											}
											else
											{
												face.nor[1] = normIndex + vertex - 1;
												face.nor[2] = normIndex + vertex;
											}
											index += 3;
										}
										if (geo->m_vertexFlags & 4)
											index += 4;
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1] });
											face.tex[0] = coordIndex + vertex - 2;
											if (vertex & 1)
											{
												face.tex[1] = coordIndex + vertex;
												face.tex[2] = coordIndex + vertex - 1;
											}
											else
											{
												face.tex[1] = coordIndex + vertex - 1;
												face.tex[2] = coordIndex + vertex;
											}
										}
										faces.push_back(face);
									}
								}
							}

							if (mesh->m_triListData.m_elementCount)
							{
								for (unsigned long valueIndex = 0; valueIndex < mesh->m_triListData.m_arraySize;)
								{
									const unsigned long numIndexes = mesh->m_triListData.m_arrayData[valueIndex++];
									for (unsigned long vertex = 0; vertex < numIndexes; vertex += 3)
									{
										unsigned long index1 = mesh->m_triListData.m_arrayData[valueIndex++] * vertexSize;
										unsigned long index2 = mesh->m_triListData.m_arrayData[valueIndex++] * vertexSize;
										unsigned long index3 = mesh->m_triListData.m_arrayData[valueIndex++] * vertexSize;
										Face face;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[index1],
																geo->m_vertices[index1 + 1],
																geo->m_vertices[index1 + 2] });
											face.pos[0] = (unsigned long)vertices.size() - 1;

											vertices.push_back({ geo->m_vertices[index2],
																geo->m_vertices[index2 + 1],
																geo->m_vertices[index2 + 2] });
											face.pos[1] = (unsigned long)vertices.size() - 1;

											vertices.push_back({ geo->m_vertices[index3],
																geo->m_vertices[index3 + 1],
																geo->m_vertices[index3 + 2] });
											face.pos[2] = (unsigned long)vertices.size() - 1;
											index1 += 4;
											index2 += 4;
											index3 += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[index1],
																geo->m_vertices[index1 + 1],
																geo->m_vertices[index1 + 2] });
											face.nor[0] = (unsigned long)vertNormals.size() - 1;

											vertNormals.push_back({ geo->m_vertices[index2],
																geo->m_vertices[index2 + 1],
																geo->m_vertices[index2 + 2] });
											face.nor[1] = (unsigned long)vertNormals.size() - 1;

											vertNormals.push_back({ geo->m_vertices[index3],
																geo->m_vertices[index3 + 1],
																geo->m_vertices[index3 + 2] });
											face.nor[2] = (unsigned long)vertNormals.size() - 1;
											index1 += 3;
											index2 += 3;
											index3 += 3;
										}
										if (geo->m_vertexFlags & 4)
										{
											index1 += 4;
											index2 += 4;
											index3 += 4;
										}
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[index1],
																geo->m_vertices[index1 + 1] });
											face.tex[0] = (unsigned long)texCoords.size() - 1;

											texCoords.push_back({ geo->m_vertices[index2],
																geo->m_vertices[index2 + 1] });
											face.tex[1] = (unsigned long)texCoords.size() - 1;

											texCoords.push_back({ geo->m_vertices[index3],
																geo->m_vertices[index3 + 1] });
											face.tex[2] = (unsigned long)texCoords.size() - 1;
										}
										faces.push_back(face);
									}
								}
							}
						}
						else if (mesh->m_primType == 5)
						{
							if (mesh->m_triFanData.m_elementCount)
							{
								unsigned long valueIndex = 0;
								for (unsigned long startIndex = mesh->m_triFanData.m_arrayData[valueIndex++];
									valueIndex < mesh->m_triFanData.m_arraySize;)
								{
									const unsigned long vertIndex = (unsigned long)vertices.size();
									const unsigned long normIndex = (unsigned long)vertNormals.size();
									const unsigned long coordIndex = (unsigned long)texCoords.size();
									const unsigned long numVerts = mesh->m_triFanData.m_arrayData[valueIndex++];
									{
										unsigned long firstIndex = startIndex * vertexSize;
										unsigned long secondIndex = firstIndex + vertexSize;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertices.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertNormals.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 3;
											secondIndex += 3;
										}
										if (geo->m_vertexFlags & 4) // Color
										{
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1] });
											texCoords.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1] });
										}
									}

									for (unsigned long vertex = 2; vertex < numVerts; ++vertex)
									{
										unsigned long index = (startIndex + vertex) * vertexSize;
										Face face;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.pos[0] = vertIndex;
											face.pos[1] = vertIndex + vertex - 1;
											face.pos[2] = vertIndex + vertex;
											index += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.nor[0] = normIndex;
											face.nor[1] = normIndex + vertex - 1;
											face.nor[2] = normIndex + vertex;
											index += 3;
										}
										if (geo->m_vertexFlags & 4)
											index += 4;
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1] });
											face.tex[0] = coordIndex;
											face.tex[1] = coordIndex + vertex - 1;
											face.tex[2] = coordIndex + vertex;
										}
										faces.push_back(face);
									}
									startIndex += numVerts;
								}
							}

							if (mesh->m_triStripData.m_elementCount)
							{
								unsigned long valueIndex = 0;
								for (unsigned long startIndex = mesh->m_triStripData.m_arrayData[valueIndex++]; valueIndex < mesh->m_triStripData.m_arraySize;)
								{
									const unsigned long vertIndex = (unsigned long)vertices.size();
									const unsigned long normIndex = (unsigned long)vertNormals.size();
									const unsigned long coordIndex = (unsigned long)texCoords.size();
									const unsigned long numVerts = mesh->m_triStripData.m_arrayData[valueIndex++];
									{
										unsigned long firstIndex = startIndex * vertexSize;
										unsigned long secondIndex = firstIndex + vertexSize;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertices.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1],
																geo->m_vertices[firstIndex + 2] });
											vertNormals.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1],
																geo->m_vertices[secondIndex + 2] });
											firstIndex += 3;
											secondIndex += 3;
										}
										if (geo->m_vertexFlags & 4) // Color
										{
											firstIndex += 4;
											secondIndex += 4;
										}
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[firstIndex],
																geo->m_vertices[firstIndex + 1] });
											texCoords.push_back({ geo->m_vertices[secondIndex],
																geo->m_vertices[secondIndex + 1] });
										}
									}

									for (unsigned long vertex = 2; vertex < numVerts; ++vertex)
									{
										Face face;
										unsigned long index = (startIndex + vertex) * vertexSize;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.pos[0] = vertIndex + vertex - 2;
											if (vertex & 1)
											{
												face.pos[1] = vertIndex + vertex;
												face.pos[2] = vertIndex + vertex - 1;
											}
											else
											{
												face.pos[1] = vertIndex + vertex - 1;
												face.pos[2] = vertIndex + vertex;
											}
											index += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1],
																geo->m_vertices[index + 2] });
											face.nor[0] = normIndex + vertex - 2;
											if (vertex & 1)
											{
												face.nor[1] = normIndex + vertex;
												face.nor[2] = normIndex + vertex - 1;
											}
											else
											{
												face.nor[1] = normIndex + vertex - 1;
												face.nor[2] = normIndex + vertex;
											}
											index += 3;
										}
										if (geo->m_vertexFlags & 4)
											index += 4;
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[index],
																geo->m_vertices[index + 1] });
											face.tex[0] = coordIndex + vertex - 2;
											if (vertex & 1)
											{
												face.tex[1] = coordIndex + vertex;
												face.tex[2] = coordIndex + vertex - 1;
											}
											else
											{
												face.tex[1] = coordIndex + vertex - 1;
												face.tex[2] = coordIndex + vertex;
											}
										}
										faces.push_back(face);
									}
									startIndex += numVerts;
								}
							}

							if (mesh->m_triListData.m_elementCount)
							{
								unsigned long valueIndex = 0;
								for (unsigned long startIndex = mesh->m_triListData.m_arrayData[valueIndex++];
									valueIndex < mesh->m_triListData.m_arraySize;)
								{
									const unsigned long numVerts = mesh->m_triListData.m_arrayData[valueIndex++];
									for (unsigned long vertex = 0; vertex < numVerts; vertex += 3)
									{
										unsigned long index1 = (startIndex + vertex) * vertexSize;
										unsigned long index2 = index1 + vertexSize;
										unsigned long index3 = index2 + vertexSize;
										Face face;
										if (geo->m_vertexFlags & 1) // Position
										{
											vertices.push_back({ geo->m_vertices[index1],
																geo->m_vertices[index1 + 1],
																geo->m_vertices[index1 + 2] });
											face.pos[0] = (unsigned long)vertices.size() - 1;

											vertices.push_back({ geo->m_vertices[index2],
																geo->m_vertices[index2 + 1],
																geo->m_vertices[index2 + 2] });
											face.pos[1] = (unsigned long)vertices.size() - 1;

											vertices.push_back({ geo->m_vertices[index3],
																geo->m_vertices[index3 + 1],
																geo->m_vertices[index3 + 2] });
											face.pos[2] = (unsigned long)vertices.size() - 1;
											index1 += 4;
											index2 += 4;
											index3 += 4;
										}
										if (geo->m_vertexFlags & 2) // Normal
										{
											vertNormals.push_back({ geo->m_vertices[index1],
																geo->m_vertices[index1 + 1],
																geo->m_vertices[index1 + 2] });
											face.nor[0] = (unsigned long)vertNormals.size() - 1;

											vertNormals.push_back({ geo->m_vertices[index2],
																geo->m_vertices[index2 + 1],
																geo->m_vertices[index2 + 2] });
											face.nor[1] = (unsigned long)vertNormals.size() - 1;

											vertNormals.push_back({ geo->m_vertices[index3],
																geo->m_vertices[index3 + 1],
																geo->m_vertices[index3 + 2] });
											face.nor[2] = (unsigned long)vertNormals.size() - 1;
											index1 += 3;
											index2 += 3;
											index3 += 3;
										}
										if (geo->m_vertexFlags & 4)
										{
											index1 += 4;
											index2 += 4;
											index3 += 4;
										}
										if (geo->m_vertexFlags & 8) // Texture Coordinate
										{
											texCoords.push_back({ geo->m_vertices[index1],
																geo->m_vertices[index1 + 1] });
											face.tex[0] = (unsigned long)texCoords.size() - 1;

											texCoords.push_back({ geo->m_vertices[index2],
																geo->m_vertices[index2 + 1] });
											face.tex[1] = (unsigned long)texCoords.size() - 1;

											texCoords.push_back({ geo->m_vertices[index3],
																geo->m_vertices[index3 + 1] });
											face.tex[2] = (unsigned long)texCoords.size() - 1;
										}
										faces.push_back(face);
									}
									startIndex += numVerts;
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	FILE* objFile = nullptr;
	if (fopen_s(&objFile, file.c_str(), "w"))
	{
		printf("Error: %s could not be created.", file.c_str());
		return false;
	}
	else
	{
		fprintf(objFile, "# This file uses centimeters as units for non-parametric coordinates.\n\nmtllib u1.mtl\n");
		for (Position& pos : vertices)
			fprintf(objFile, "v %f %f %f\n", pos.x, pos.y, pos.z);
		for (TexCoord& tex : texCoords)
			fprintf(objFile, "vt %f %f\n", tex.s, tex.t);
		for (Normal& nor : vertNormals)
			fprintf(objFile, "vn %f %f %f\n", nor.i, nor.j, nor.k);
		fprintf(objFile, "s off\n");
		for (Face& face : faces)
			fprintf(objFile, "f %lu/%lu/%lu %lu/%lu/%lu %lu/%lu/%lu\n", face.pos[0] + 1, face.tex[0] + 1, face.nor[0] + 1,
				face.pos[1] + 1, face.tex[1] + 1, face.nor[1] + 1,
				face.pos[2] + 1, face.tex[2] + 1, face.nor[2] + 1);
		fclose(objFile);
		printf("%sExported to %s%s.OBJ\n", g_global.tabs.c_str(), m_directory.c_str(), m_filename.c_str());
		return true;
	}

}

bool XG::importOBJ()
{
	GlobalFunctions::banner(" " + m_filename + " - Model Import ");
	const string initialName = m_directory + m_filename + ".OBJ";
	string objName;
	bool found = false;
	if (filesystem::exists(m_directory + '\\' + m_filename + ".OBJ"))
		objName = m_directory + '\\' + m_filename + ".OBJ";
	else
	{
		do
		{
			printf("%sProvide the name of the .OBJ file you wish to import (Or 'Q' to exit): ", g_global.tabs.c_str());
			objName.clear();
			switch (GlobalFunctions::stringInsertion(objName))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::Success:
				if (objName.find(".OBJ") == string::npos && objName.find(".obj") == string::npos)
					objName += ".OBJ";
				if (filesystem::exists(objName))
					g_global.quit = true;
				else
				{
					size_t pos = objName.find_last_of('\\');
					if (pos != string::npos)
						printf("%s\"%s\" is not a valid file of extension \".OBJ\"\n", g_global.tabs.c_str(), objName.substr(pos + 1).c_str());
					else
						printf("%s\"%s\" is not a valid file of extension \".OBJ\"\n", g_global.tabs.c_str(), objName.c_str());
				}
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}

	return false;
}

{
}

}

{
}

{


}
