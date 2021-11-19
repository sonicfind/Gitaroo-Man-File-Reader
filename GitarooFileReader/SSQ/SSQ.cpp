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
using namespace GlobalFunctions;
void flipHand(std::vector<Rotation>& rotations)
{
	float tmp;
	for (auto& rot : rotations)
	{
		tmp = rot.rotation.z;
		rot.rotation.z = rot.rotation.y;
		rot.rotation.y = tmp;
	}
}

GameState g_gameState;

SSQ::SSQ() : FileType(".SSQ") {}

SSQ::SSQ(std::string filename, bool unused)
	: FileType(filename, ".SSQ")
	, m_shadowVAO(0)
	, m_shadowVBO(0)
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
	fread(&m_startFrame, 4, 1, m_filePtr);
	fread(&m_endFrame, 4, 1, m_filePtr);
	fread(&m_currFrame, 4, 1, m_filePtr);
	fread(m_junk, 1, 16, m_filePtr);

	unsigned long numMatrices = 0;
	fread(&numMatrices, 4, 1, m_filePtr);
	m_modelMatrices.reserve(numMatrices);
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
			m_modelSetups.push_back(std::make_unique<ModelSetup>(m_filePtr, m_XGentries[i].m_type, m_modelMatrices[i]));
			break;
		case ModelType::Player1:
		case ModelType::Player2:
		case ModelType::DuetPlayer:
			m_modelSetups.push_back(std::make_unique<PlayerModelSetup>(m_filePtr, m_XGentries[i].m_type, m_modelMatrices[i]));
			break;
		case ModelType::Player1AttDef:
		case ModelType::Player2AttDef:
		case ModelType::DuetPlayerAttDef:
		case ModelType::DuetComboAttack:
			m_modelSetups.push_back(std::make_unique<AttDefModelSetup>(m_filePtr, m_XGentries[i].m_type, m_modelMatrices[i]));
			break;
		case ModelType::Snake:
			m_modelSetups.push_back(std::make_unique<SnakeModelSetup>(m_filePtr, m_XGentries[i].m_type, m_modelMatrices[i]));
		}
	}

	for (size_t i = 0; i < numXG; ++i)
	{
		if (AttDefModelSetup* attDef = dynamic_cast<AttDefModelSetup*>(m_modelSetups[i].get()))
		{
			// Will be filled in reverse order
			auto names = attDef->getConnectedNames();
			if (names.size())
			{
				glm::mat4* matrices[2] = { &m_modelMatrices[i], nullptr };
				for (size_t n = 0; n < names.size(); ++n)
				{
					for (size_t e = 0; e < m_XGentries.size(); ++e)
					{
						if (names[n].compare(m_XGentries[e].m_name) == 0)
						{
							matrices[1 - n] = &m_modelMatrices[e];
							break;
						}
					}
				}
				attDef->setConnectedMatrices(matrices[0], matrices[1]);
			}
		}
	}

	m_camera.read(m_filePtr);
	m_endFrame = m_camera.getLastFrame();

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
	for (auto& entry : m_IMXentries)
		entry.m_imxPtr = m_xgm->getTexture(entry.m_name);

	for (size_t i = 0; i < m_XGentries.size(); ++i)
		if (!m_XGentries[i].m_isClone)
		{
			m_XGentries[i].m_xg = m_xgm->getModel(m_XGentries[i].m_name);
			m_modelSetups[i]->bindXG(m_XGentries[i].m_xg);
		}
		else
			m_modelSetups[i]->bindXG(m_XGentries[m_XGentries[i].m_cloneID].m_xg);

	m_shadowPtr = m_xgm->getTexture("SHADOW.IMX");
	m_skyPtr = m_xgm->getTexture("ST02SKY.IMX");

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
		unsigned long zero[3] = { 0 };
		fwrite(zero, sizeof(unsigned long), 3, m_filePtr);
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

bool SSQ::viewSequence()
{
	while (true)
	{
		banner(" " + m_filename + ".SSQ - Sequence Viewer ");
		printf_tab("V - Enter the Viewer\n");
		printf_tab("B - Change BPM for tempo-base animations: %g\n", Animation::getTempo());
		printf_tab("A - Switch aspect ratio: %s\n", getAspectRatioString());
		printf_tab("H - Change viewer resolution, Height: %u\n", s_screenHeight);
		printf_tab("D - Toggle window border: %s\n", s_borderless ? "FALSE" : "TRUE");
		printf_tab("S - Change Starting Frame: %g\n", m_startFrame);
		printf_tab("E - Change Ending Frame: %g\n", m_endFrame);
		if (m_skyPtr)
			printf_tab("K - Toggle Sky Background: %s\n", m_doSkyBackground ? "TRUE" : "FALSE");
		printf_tab("? - Show list of controls\n");
		switch (menuChoices("vbahdsek"))
		{
		case ResultType::Help:
			printf_tab("\n");
			printf_tab("P - Pause/Play\n");
			printf_tab("R (Hold) - Reset to the beginning of the sequence\n");
			printf_tab("N - Toggle displaying vertex normal vectors\n");
			printf_tab("F - Toggle FreeCam movement\n");
			printf_tab("U - Toggle Shading\n");
			if (m_skyPtr)
				printf_tab("K - Toggle Sky Background\n");
			printf_tab("ESC - exit\n");
			printf_tab("\n");

			printf_tab("5/6/7/8 - Toggle models that are bound to attack events of the corresponding type\n");
			printf_tab("\t5 - Player 1 Attack\n");
			printf_tab("\t6 - Player 2 Attack\n");
			printf_tab("\t7 - Duet Player Attack (currently unapplicable)\n");
			printf_tab("\t8 - Duet Combo Attack (currently unapplicable)\n");
			printf_tab("\n");

			printf_tab("With FreeCam mode active:\n");
			printf_tab("\tWASD - Move\n");
			printf_tab("\tSpace/Left Shift - Ascend/Descend\n");
			printf_tab("\tUp/Down - Increase/Decrease movement speed\n");
			printf_tab("\tMouse - Camera Aiming\n");
			printf_tab("\tMouse Scroll - Increase/Decrease zoom\n");
			printf_tab("\t'M' - Release/Reattach mouse control from/to the window (while in FreeCam mode)\n");
			
			printf_tab("\n");
			printf_tab("Press 'Enter' when you're done reading\n");
			printf_tab("");
			clearIn();
			testForMulti();
			break;
		case ResultType::Quit:
			return true;
		case ResultType::Success:
			switch (g_global.answer.character)
			{
			case 'v':
				m_viewerControls = new ViewerControls_SSQ;
				srand(unsigned int(time(0)));
				startDisplay(m_filename.c_str());
				break;
			case 'b':
				if (Animation::setTempo())
					return true;
				break;
			case 'a':
				Viewer::switchAspectRatio();
				break;
			case 'h':
				if (Viewer::changeHeight())
					return true;
				break;
			case 'd':
				s_borderless = !s_borderless;
				break;
			case 's':
				if (changeStartFrame())
					return true;
				break;
			case 'e':
				if (changeEndFrame())
					return true;
				break;
			case 'k':
				if (m_skyPtr)
					m_doSkyBackground = !m_doSkyBackground;
			}
		}
	}
}

bool SSQ::changeStartFrame()
{
	while (true)
	{
		GlobalFunctions::printf_tab("Current Starting Fame: %g ['B' to leave unchanged]\n", m_startFrame);
		GlobalFunctions::printf_tab("Input: ");
		switch (GlobalFunctions::valueInsert(m_startFrame, false, 0.0f, m_endFrame, "b"))
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::MaxExceeded:
			m_startFrame = m_endFrame;
			__fallthrough;
		case GlobalFunctions::ResultType::Success:
		case GlobalFunctions::ResultType::SpecialCase:
			return false;
		case GlobalFunctions::ResultType::InvalidNegative:
			GlobalFunctions::printf_tab("Value must be positive.\n");
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::Failed:
			GlobalFunctions::printf_tab("\"%s\" is not a valid response.\n", g_global.invalid.c_str());
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
		}
	}
}

bool SSQ::changeEndFrame()
{
	const float last = m_camera.getLastFrame();
	while (true)
	{
		GlobalFunctions::printf_tab("Current Ending Fame: %g ['B' to leave unchanged]\n", m_endFrame);
		GlobalFunctions::printf_tab("Input: ");
		switch (GlobalFunctions::valueInsert(m_endFrame, false, m_startFrame, last, "b"))
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::MaxExceeded:
			m_endFrame = last;
			__fallthrough;
		case GlobalFunctions::ResultType::Success:
		case GlobalFunctions::ResultType::SpecialCase:
			return false;
		case GlobalFunctions::ResultType::MinExceeded:
			m_endFrame = m_startFrame;
			return false;
		case GlobalFunctions::ResultType::InvalidNegative:
			GlobalFunctions::printf_tab("Value must be positive.\n");
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::Failed:
			GlobalFunctions::printf_tab("\"%s\" is not a valid response.\n", g_global.invalid.c_str());
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
		}
	}
}

void SSQ::setFrame(const float frame)
{
	m_currFrame = frame;
	for (auto& model : m_modelSetups)
		model->jumpToFrame(frame);
}
