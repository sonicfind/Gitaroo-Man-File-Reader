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
#include "XG_Data.h"
XG_Data::XG_Data(FILE* inFile, unsigned long& filesize)
{
	char test[8] = { 0 };
	fread(test, 1, 8, inFile);
	if (!strstr(test, "XGB"))
	{
		fclose(inFile);
		throw "Error: No 'XGB' tag for model ";
	}

	// For grabbing the node type
	PString type(inFile);
	// For grabbing the name of a node
	PString xgName(inFile);
	// for testing for the ';' character, at least initially
	PString colonTest(inFile);

	do
	{
		try
		{
			// Compares the type string with the strings associated to the various xgNode types
			addNode(type, xgName);
		}
		catch (std::string str)
		{
			const int position = ftell(inFile) - 4;
			fclose(inFile);
			throw str + " [File offset: " + std::to_string(position) + "].";
		}

		type.fill(inFile);
		xgName.fill(inFile);
		colonTest.fill(inFile);
	} while (strchr(colonTest.m_pstring, ';'));

	for (size_t n = 0; n < m_nodes.size(); n++)
	{
		try
		{
			// If a node is found to be un-optimized, it will optimize it and decrement the total filesize accordingly
			filesize -= m_nodes[n]->read(inFile, m_nodes);
		}
		catch (std::string str)
		{
			fclose(inFile);
			throw str;
		}

		if (n + 1 != m_nodes.size())
		{
			type.fill(inFile);
			xgName.fill(inFile);
			colonTest.fill(inFile);
		}
	}

	// Grabs the .dag std::string
	type.fill(inFile);
	// Grabs the { character
	colonTest.fill(inFile);

	// Loops through all dag nodes
	while (true)
	{
		// Will break if the end of the dag map is reached
		// Notated by an extracted '}' character
		try
		{
			m_dagMap.push_back(Dag(inFile, m_nodes, true));
		}
		catch (...)
		{
			break;
		}
	}
}

void XG_Data::addNode(const PString& type, const PString& name)
{
	// Ordered in likelihood of occurance
	if		(xgVec3Interpolator::compare(type))		m_nodes.push_back(std::make_unique<xgVec3Interpolator>(name));
	else if	(xgQuatInterpolator::compare(type))		m_nodes.push_back(std::make_unique<xgQuatInterpolator>(name));
	else if (xgBone::compare(type))					m_nodes.push_back(std::make_unique<xgBone>(name));
	else if (xgBgMatrix::compare(type))				m_nodes.push_back(std::make_unique<xgBgMatrix>(name));
	else if (xgEnvelope::compare(type))				m_nodes.push_back(std::make_unique<xgEnvelope>(name));
	else if (xgMaterial::compare(type))				m_nodes.push_back(std::make_unique<xgMaterial>(name));
	else if (xgTexture::compare(type))				m_nodes.push_back(std::make_unique<xgTexture>(name));
	else if (xgDagMesh::compare(type))				m_nodes.push_back(std::make_unique<xgDagMesh>(name));
	else if (xgBgGeometry::compare(type))			m_nodes.push_back(std::make_unique<xgBgGeometry>(name));
	else if (xgDagTransform::compare(type))			m_nodes.push_back(std::make_unique<xgDagTransform>(name));
	else if (xgMultiPassMaterial::compare(type))	m_nodes.push_back(std::make_unique<xgMultiPassMaterial>(name));
	else if (xgVertexInterpolator::compare(type))	m_nodes.push_back(std::make_unique<xgVertexInterpolator>(name));
	else if (xgNormalInterpolator::compare(type))	m_nodes.push_back(std::make_unique<xgNormalInterpolator>(name));
	else if (xgShapeInterpolator::compare(type))	m_nodes.push_back(std::make_unique<xgShapeInterpolator>(name));
	else if (xgTexCoordInterpolator::compare(type)) m_nodes.push_back(std::make_unique<xgTexCoordInterpolator>(name));
	else if (xgTime::compare(type))
	{
		m_nodes.push_back(std::make_unique<xgTime>(name));
		m_timeNode = m_nodes.back().get();
	}
	else
		throw "Error: Unrecognized xgNode type - " + std::string(type.m_pstring);
}

void XG_Data::create(FILE* outFile)
{
	fwrite("XGBv1.00", 1, 8, outFile);

	// Write node prototypes
	for (auto& node : m_nodes)
		node->create(outFile, false);

	// Write actual node data
	for (auto& node : m_nodes)
		node->create(outFile, true);

	PString::push("dag", outFile);
	PString::push('{', outFile);

	// Construct the dag map
	for (const Dag& dag : m_dagMap)
		dag.create(outFile, true);

	PString::push('}', outFile);
}
