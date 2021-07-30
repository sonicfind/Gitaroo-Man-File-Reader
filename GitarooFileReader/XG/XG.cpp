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

XG& XG::operator=(const XG& xg)
{
	if (m_data != xg.m_data)
	{
		m_filename = xg.m_filename;
		std::copy(xg.m_filepath, xg.m_filepath + 256, m_filepath);
		std::copy(xg.m_name, xg.m_name + 16, m_name);
		m_modelIndex = xg.m_modelIndex;
		m_fileSize = xg.m_fileSize;
		m_unk = xg.m_unk;
		m_animations = xg.m_animations;
		m_data = xg.m_data;
		m_saved = xg.m_saved;
	}
	return *this;
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

XG::Animation::Animation(FILE* inFile)
{
	fread(&m_length, 4, 1, inFile);
	fread(&m_keyframe_interval, 4, 1, inFile);
	fread(&m_framerate, 4, 1, inFile);
	fread(&m_starting_keyframe, 4, 1, inFile);
	fread(&m_non_tempo, 4, 1, inFile);
	fread(&m_junk, 4, 3, inFile);
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

bool XG::Animation::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile)
{
	fprintf_s(txtFile, "\t\t\t\t      Playback Length: %g\n", m_length);
	fprintf_s(txtFile, "\t\t\t\t    Keyframe Interval: %g\n", m_keyframe_interval);
	fprintf_s(txtFile, "\t\t\t\t\t    Framerate: %g\n", m_framerate);
	fprintf_s(txtFile, "\t\t\t\t    Starting Keyframe: %g\n", m_starting_keyframe);
	fprintf_s(txtFile, "\t\t\t\t     Not Tempo Linked: %s\n", (m_non_tempo ? "TRUE" : "FALSE"));
	return true;
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

XG_Data::XG_Data(FILE* inFile, unsigned long& filesize)
{
	char test[8] = { 0 };
	static bool(XG_Data:: * types[])(PString&, PString&) =
	{
	&XG_Data::addNode<xgBgGeometry>,
	&XG_Data::addNode<xgBgMatrix>,
	&XG_Data::addNode<xgBone>,
	&XG_Data::addNode<xgDagMesh>,
	&XG_Data::addNode<xgDagTransform>,
	&XG_Data::addNode<xgEnvelope>,
	&XG_Data::addNode<xgMaterial>,
	&XG_Data::addNode<xgMultiPassMaterial>,
	&XG_Data::addNode<xgNormalInterpolator>,
	&XG_Data::addNode<xgQuatInterpolator>,
	&XG_Data::addNode<xgShapeInterpolator>,
	&XG_Data::addNode<xgTexCoordInterpolator>,
	&XG_Data::addNode<xgTexture>,
	&XG_Data::addNode<xgTime>,
	&XG_Data::addNode<xgVec3Interpolator>,
	&XG_Data::addNode<xgVertexInterpolator>,
	nullptr
	};
	fread(test, 1, 8, inFile);
	if (!strstr(test, "XGB"))
	{
		fclose(inFile);
		throw "Error: No 'XGB' tag for model ";
	}

	PString type(inFile);
	PString xgName(inFile);
	PString colonTest(inFile);
	do
	{
		for (bool(XG_Data::* func)(PString&, PString&) : types)
		{
			if (!func)
			{
				fclose(inFile);
				throw "Error: Unrecognized xgNode type - " + std::string(type.m_pstring) + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
			}
			else if ((this->*func)(type, xgName))
				break;
		}

		type.fill(inFile);
		xgName.fill(inFile);
		colonTest.fill(inFile);
	} while (strchr(colonTest.m_pstring, ';'));

	for (size_t n = 0; n < m_nodes.size(); n++)
	{
		try
		{
			filesize -= m_nodes[n]->read(inFile, m_nodes);
			if (n + 1 != m_nodes.size())
			{
				type.fill(inFile);
				xgName.fill(inFile);
				colonTest.fill(inFile);
			}
		}
		catch (std::string str)
		{
			fclose(inFile);
			throw str;
		}
	}

	type.fill(inFile);		// Grabs the .dag std::string
	colonTest.fill(inFile); // Grabs the { character

	std::list<std::vector<XG_Data::DagBase>*> dagStack = { &m_dag };
	xgName.fill(inFile);
	while (!strchr(xgName.m_pstring, '}'))
	{
		if (strchr(xgName.m_pstring, '['))
			dagStack.push_back(&dagStack.back()->back().m_connected);
		else if (strchr(xgName.m_pstring, ']'))
			dagStack.pop_back();
		else
		{
			for (auto& node : m_nodes)
			{
				if (node->m_name == xgName)
				{
					dagStack.back()->emplace_back(node.get());
					break;
				}
			}
		}
		xgName.fill(inFile);
	}
}
XG_Data::XG_Data(XG_Data& xg)
{
	static bool(XG_Data:: * types[])(std::shared_ptr<XGNode>&) =
	{
	&XG_Data::cloneNode<xgBgGeometry>,
	&XG_Data::cloneNode<xgBgMatrix>,
	&XG_Data::cloneNode<xgBone>,
	&XG_Data::cloneNode<xgDagMesh>,
	&XG_Data::cloneNode<xgDagTransform>,
	&XG_Data::cloneNode<xgEnvelope>,
	&XG_Data::cloneNode<xgMaterial>,
	&XG_Data::cloneNode<xgMultiPassMaterial>,
	&XG_Data::cloneNode<xgNormalInterpolator>,
	&XG_Data::cloneNode<xgQuatInterpolator>,
	&XG_Data::cloneNode<xgShapeInterpolator>,
	&XG_Data::cloneNode<xgTexCoordInterpolator>,
	&XG_Data::cloneNode<xgTexture>,
	&XG_Data::cloneNode<xgTime>,
	&XG_Data::cloneNode<xgVec3Interpolator>,
	&XG_Data::cloneNode<xgVertexInterpolator>,
	};

	for (std::shared_ptr<XGNode>& node : xg.m_nodes)
	{
		for (bool(XG_Data::* func)(std::shared_ptr<XGNode>&) : types)
			if ((this->*func)(node))
				break;

		if (m_dag.size() < xg.m_dag.size())
		{
			for (DagBase& dag : xg.m_dag)
			{
				if (dag.m_base->m_name == m_nodes.back()->m_name)
				{
					m_dag.push_back(m_nodes.back().get());
					break;
				}
			}
		}
	}

	for (size_t d = 0; d < xg.m_dag.size(); ++d)
	{
		for (DagBase& base : xg.m_dag[d].m_connected)
		{
			for (std::shared_ptr<XGNode>& node : m_nodes)
			{
				if (node->m_name == base.m_base->m_name)
				{
					m_dag[d].m_connected.push_back(node.get());
					break;
				}
			}
		}
	}
}

void XG_Data::create(FILE* outFile)
{
	fwrite("XGBv1.00", 1, 8, outFile);

	for (std::shared_ptr<XGNode>& node : m_nodes)
		node->create(outFile, false);

	for (std::shared_ptr<XGNode>& node : m_nodes)
		node->create(outFile, true);

	PString::push("dag", outFile);
	PString::push('{', outFile);

	for (DagBase& base : m_dag)
		base.create(outFile, true);

	PString::push('}', outFile);
}

XG_Data::DagBase::DagBase() : m_base(nullptr) {}

XG_Data::DagBase::DagBase(XGNode* m_base) : m_base(m_base) {}

void XG_Data::DagBase::create(FILE* outFile, bool braces)
{
	m_base->push(outFile);
	if (braces || m_connected.size())
		PString::push('[', outFile);

	for (DagBase& dag : m_connected)
		dag.create(outFile);

	if (braces || m_connected.size())
		PString::push(']', outFile);
}
