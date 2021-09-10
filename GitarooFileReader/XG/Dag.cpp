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
#include "Dag.h"
Dag::Dag(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList, bool isRootBranch)
	: m_matrix(1.0f)
{
	PString pstr(inFile);
	if (pstr.m_pstring[0] == '}' || pstr.m_pstring[0] == ']')
		throw "End of branch";

	// Fill base with actual node
	m_base.fill(pstr, nodeList);

	if (isRootBranch || m_base.get<xgDagTransform>())
	{
		// Extract '[' character
		pstr.fill(inFile);
		// Loops through all connected nodes
		while (true)
		{
			// Will break if the end of the branch is reached
			// Notated by an extracted ']' character
			try
			{
				m_connected.push_back(Dag(inFile, nodeList));
			}
			catch (...)
			{
				break;
			}
		}
	}
}

void Dag::create(FILE* outFile, bool isRootBranch) const
{
	m_base->push(outFile);
	if (isRootBranch || m_connected.size())
		PString::push('[', outFile);

	for (const Dag& dag : m_connected)
		dag.create(outFile);

	if (isRootBranch || m_connected.size())
		PString::push(']', outFile);
}

void Dag::faces_to_obj(FILE* objFile, std::vector<std::pair<size_t, xgBgGeometry*>>& history) const
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->faces_to_obj(objFile, history);
	else
		for (const auto& dag : m_connected)
			dag.faces_to_obj(objFile, history);
}

void Dag::queue_for_obj(std::vector<std::pair<size_t, xgBgGeometry*>>& history) const
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->queue_for_obj(history);
	else
		for (const auto& dag : m_connected)
			dag.queue_for_obj(history);
}

void Dag::connectTextures(std::vector<IMX>& textures)
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->connectTextures(textures);
	else
		for (auto& dag : m_connected)
			dag.connectTextures(textures);
}

void Dag::initializeViewerState()
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->intializeBuffers();
	else
		for (auto& dag : m_connected)
			dag.initializeViewerState();
}

void Dag::uninitializeViewerState()
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->deleteBuffers();
	else
		for (auto& dag : m_connected)
			dag.uninitializeViewerState();
}

void Dag::restPose()
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->restPose();
	else
	{
		m_matrix = glm::identity<glm::mat4>();
		for (auto& dag : m_connected)
			dag.restPose();
	}
}

void Dag::animate()
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->animate();
	else
	{
		m_matrix = m_base.get<xgDagTransform>()->getModelMatrix();
		for (auto& dag : m_connected)
			dag.animate();
	}
}

void Dag::draw(const glm::mat4 view, const glm::mat4 model, const bool showNormals, const bool doTransparents) const
{
	if (auto mesh = m_base.get<xgDagMesh>())
		mesh->draw(view, model, showNormals, doTransparents);
	else
	{
		const glm::mat4 transform = model * m_matrix;
		for (const auto& dag : m_connected)
			dag.draw(view, transform, showNormals, doTransparents);
	}
}
