#pragma once
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
#include "SSQ.h"
float mixFloat(const float a, const float b, const float coefficient)
{
	return a + (b - a) * coefficient;
}

float SSQ::s_frame = 0;

SSQ::SSQ() : FileType(".SSQ") {}

SSQ::SSQ(std::string filename, bool unused)
	: FileType(filename, ".SSQ")
{
	char tmp[5] = { 0 };
	// Block Tag
	fread(tmp, 1, 4, m_filePtr);
	if (!strstr(tmp, "GMSX"))
	{
		fclose(m_filePtr);
		throw "Error: No 'GMSX' Tag at byte 0.";
	}

	fread(&m_headerVersion, 4, 1, m_filePtr);
	fread(m_unk, 1, 12, m_filePtr);
	fread(m_junk, 1, 16, m_filePtr);

	unsigned long numMatrices = 0;
	fread(&numMatrices, 4, 1, m_filePtr);
	m_modelMatrices.resize(numMatrices);

	unsigned long numIMX = 0;
	fread(&numIMX, 4, 1, m_filePtr);

	for (unsigned long i = 0; i < numIMX; ++i)
		m_IMXentries.emplace_back(m_filePtr);

	unsigned long numXG = 0;
	fread(&numXG, 4, 1, m_filePtr);

	for (unsigned long i = 0; i < numXG; ++i)
		m_XGentries.emplace_back(m_filePtr);

	// Setups listed in same order as Entries
	for (unsigned long i = 0; i < numXG; ++i)
	{
		switch (m_XGentries[i].m_type)
		{
		case ModelType::Normal:
			m_modelSetups.push_back(std::make_unique<ModelSetup>(m_filePtr, m_XGentries[i].m_name));
			break;
		case ModelType::Player1:
		case ModelType::Player2:
		case ModelType::DuetPlayer:
			m_modelSetups.push_back(std::make_unique<PlayerModelSetup>(m_filePtr, m_XGentries[i].m_name));
			break;
		case ModelType::Player1AttDef:
		case ModelType::Player2AttDef:
		case ModelType::DuetPlayerAttDef:
		case ModelType::DuetComboAttack:
			m_modelSetups.push_back(std::make_unique<AttDefModelSetup>(m_filePtr, m_XGentries[i].m_name));
			break;
		case ModelType::Snake:
			m_modelSetups.push_back(std::make_unique<SnakeModelSetup>(m_filePtr, m_XGentries[i].m_name));
		}
	}

	m_camera.read(m_filePtr);

	m_sprites.read(m_filePtr);

	if (m_headerVersion >= 0x1100)
	{
		unsigned long numTex;
		fread(&numTex, 4, 1, m_filePtr);
		for (unsigned long i = 0; i < numTex; ++i)
			m_texAnimations.emplace_back(m_filePtr);
	}

	m_pSetup.read(m_filePtr);
	fclose(m_filePtr);
}

bool SSQ::loadXGM()
{
	// Note: Gitaroo Man uses Albumdef.txt to specify an XGM
	m_xgm = std::make_unique<XGM>(m_directory + m_filename);
	for (auto& entry : m_XGentries)
		if (!entry.m_isClone)
			entry.m_xg = m_xgm->getModel(entry.m_name);
	for (auto& texAnim : m_texAnimations)
		texAnim.connectTexture(m_xgm->getTexture(texAnim.getTextureName()));
	return true;
}

bool SSQ::create(std::string filename)
{
	if (FileType::create(filename))
	{
		// Block Tag
		fprintf(m_filePtr, "GMSX");
		fwrite(&m_headerVersion, 4, 1, m_filePtr);
		fwrite(m_unk, 1, 12, m_filePtr);
		fwrite(m_junk, 1, 16, m_filePtr);

		unsigned long size = (unsigned long)m_modelSetups.size();
		fwrite(&size, 4, 1, m_filePtr);

		size = (unsigned long)m_IMXentries.size();
		fwrite(&size, 4, 1, m_filePtr);

		for (IMXEntry& imx : m_IMXentries)
			imx.create(m_filePtr);

		size = (unsigned long)m_modelSetups.size();
		fwrite(&size, 4, 1, m_filePtr);

		for (XGEntry& xg : m_XGentries)
			xg.create(m_filePtr);

		for (const auto& model : m_modelSetups)
			model->create(m_filePtr);

		m_camera.create(m_filePtr);

		m_sprites.create(m_filePtr);

		if (m_headerVersion >= 0x1100)
		{
			size = (unsigned long)m_texAnimations.size();
			fwrite(&size, 4, 1, m_filePtr);
			for (TexAnim& anim : m_texAnimations)
				anim.create(m_filePtr);
		}

		m_pSetup.create(m_filePtr);
		fclose(m_filePtr);

		m_saved = true;
		return true;
	}
	return false;
}

void SSQ::loadbuffers()
{
	for (size_t i = 0; i < m_modelSetups.size(); ++i)
		if (!m_XGentries[i].m_isClone)
			m_XGentries[i].m_xg->initializeViewerState();

	for (auto& texAnim : m_texAnimations)
		texAnim.loadCuts();
}

void SSQ::unloadBuffers()
{
	for (size_t i = 0; i < m_modelSetups.size(); ++i)
		if (!m_XGentries[i].m_isClone)
			m_XGentries[i].m_xg->uninitializeViewerState();
	for (auto& texAnim : m_texAnimations)
		texAnim.unloadCuts();
}

void SSQ::update()
{
	for (size_t i = 0; i < m_modelSetups.size(); ++i)
	{
		auto& entry = m_XGentries[i];
		XG* xg = entry.m_isClone ? m_XGentries[entry.m_cloneID].m_xg : entry.m_xg;
		if (m_modelSetups[i]->animate(xg, s_frame))
		{
			entry.m_isActive = 1;
			m_modelMatrices[i] = m_modelSetups[i]->getModelMatrix(s_frame);
		}
		else
			entry.m_isActive = 0;
	}

	for (auto& texAnim : m_texAnimations)
		texAnim.substitute(s_frame);

	// Insert Light stuff here
}

glm::mat4 SSQ::getViewMatrix() const
{
	return m_camera.getViewMatrix(s_frame);
}

glm::mat4 SSQ::getProjectionMatrix(unsigned int width, unsigned int height) const
{
	return m_camera.getProjectionMatrix(s_frame, width, height);
}

glm::vec4 SSQ::getClearColor() const
{
	return m_camera.getClearColor(s_frame);
}

void SSQ::draw(const glm::mat4 view, const bool showNormals, const bool doTransparents)
{
	static glm::mat4 matrixBuffer[32];
	for (size_t i = 0; i < m_modelSetups.size(); ++i)
	{
		auto& entry = m_XGentries[i];
		if (!entry.m_isClone && entry.m_xg->getInstanceCount())
		{
			unsigned count = 0;
			if (entry.m_isActive)
				matrixBuffer[count++] = m_modelMatrices[i];

			for (size_t j = i + 1; count < entry.m_xg->getInstanceCount(); ++j)
			{
				if (m_XGentries[j].m_isClone
					&& m_XGentries[j].m_cloneID == i
					&& m_XGentries[j].m_isActive)
					matrixBuffer[count++] = m_modelMatrices[j];
			}
			entry.m_xg->draw(view, matrixBuffer, showNormals, doTransparents);
		}
	}
}

void SSQ::setFrame(float frame)
{
	s_frame = frame;
}

void SSQ::adjustFrame(float delta)
{
	s_frame += 30 * delta;
}
