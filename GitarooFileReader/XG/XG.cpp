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
	, m_instanceCount(0)
	, m_fromXGM(false) {}

XG::XG(FILE* inFile, const std::string& directory, std::vector<IMX>& textures)
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
	fread(&m_instanceCount, 4, 1, inFile);
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
	m_data->connectTextures(textures);
}

XG::XG(std::string filename)
	: FileType(filename, ".XG")
	, m_modelIndex(0)
	, m_instanceCount(0)
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

	if (!m_saved)
	{
		while (true)
		{
			printf_tab("All poorly constructed/unoptimized XGNodes have been fixed.\n");
			printf_tab("It is recommended to save these changes to a separate XG file. Do so now? [Y/(N or Q)]\n");
			switch (menuChoices("yn"))
			{
			case ResultType::Success:
				if (g_global.answer.character == 'y')
				{
					string ext = "_Optimized";
					while (true)
					{
						switch (fileOverwriteCheck(filename + ext + m_extension))
						{
						case ResultType::No:
							ext += "_T";
							break;
						case ResultType::Yes:
							create(filename + ext);
							m_filename += ext;
						case ResultType::Quit:
							return;
						}
					}

				}
				__fallthrough;
			case ResultType::Quit:
				return;
			}
		}
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
	fwrite(&m_instanceCount, 4, 1, outFile);
	fwrite(&m_animations[0], sizeof(Animation), size, outFile);
	m_data->create(outFile);
	m_saved = 1;
}

bool XG::create(string filename)
{
	if (FileType::create(filename))
	{
		m_data->create(m_filePtr);
		fclose(m_filePtr);
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
		size_t index = 0;
		for (const auto& node : m_data->m_nodes)
		{
			fprintf_s(txtFile, "\t Node %03zu - ", index + 1);
			fprintf_s(simpleTxtFile, "\t Node %03zu - ", index + 1);
			node->write_to_simple_txt(simpleTxtFile);
			node->write_to_txt(txtFile);
			fflush(txtFile);
			++index;
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
	size_t index = 0;
	for (const auto& node : m_data->m_nodes)
	{
		fprintf_s(txtFile, "\t\t\t\t     Node %03zu - ", index + 1);
		fprintf_s(simpleTxtFile, "\t\t\t\t Node %03zu - ", index + 1);
		node->write_to_simple_txt(simpleTxtFile);
		node->write_to_txt(txtFile, "\t\t\t    ");
		fflush(txtFile);
		++index;
	}
	return true;
}

bool XG::write_to_obj(std::string newDirectory)
{
	GlobalFunctions::banner(" " + m_filename + " - Model Export ");
	string file = newDirectory.length() ? newDirectory + m_filename + ".obj" : m_directory + m_filename + ".obj";
	FILE* objFile = nullptr;
	if (!fopen_s(&objFile, file.c_str(), "w") && objFile)
	{
		
		vector<pair<size_t, xgBgGeometry*>> history;
		history.push_back({ 1, nullptr });
		for (const auto& dag : m_data->m_dagMap)
			dag.queue_for_obj(history);

		fprintf(objFile, "# This file uses centimeters as units for non-parametric coordinates.\n\nmtllib u1.mtl\n");

		for (size_t i = 0; i < history.size() - 1; ++i)
			history[i].second->positions_to_obj(objFile);

		for (size_t i = 0; i < history.size() - 1; ++i)
			history[i].second->texCoords_to_obj(objFile);

		for (size_t i = 0; i < history.size() - 1; ++i)
			history[i].second->normals_to_obj(objFile);

		for (const auto& dag : m_data->m_dagMap)
			dag.faces_to_obj(objFile, history);

		fclose(objFile);
		printf("%sExported to %s%s.OBJ\n", g_global.tabs.c_str(), m_directory.c_str(), m_filename.c_str());
		return true;
	}
	else
	{
		printf("Error: %s could not be created.", file.c_str());
		return false;
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

// Constructs all the vertex and uniform buffers for use in the OpenGL viewer
void XG::initializeViewerState()
{
	m_data->initializeViewerState();
}

// Deletes all the vertex and uniform buffers created for the OpenGL viewer
void XG::uninitializeViewerState()
{
	m_data->uninitializeViewerState();
}

// Returns the total duration of the chosen animation in seconds 
float XG::getAnimationLength(size_t index)
{
	return m_animations[index].getTotalTime();
}

// Sets all vertex and bone matrix values to their defaults
void XG::restPose() const
{
	m_data->restPose();
}

// Updates all data to the current frame
void XG::animate(float frame, size_t index)
{
	// Calculates the current keyframe from the current animation
	const float key = m_animations[index].getTime(frame);
	// Increment count for if another instance is needed
	m_data->animate(key, m_instanceCount++);
}

// Draws all vertex data to the current framebuffer
void XG::draw(const glm::mat4 view, const glm::mat4* models, const bool showNormals, const bool doTransparents, const bool isAnimated)
{
	m_data->draw(view, models, m_instanceCount, showNormals, doTransparents, isAnimated);
	if (doTransparents)
		m_instanceCount = 0;
}
}
