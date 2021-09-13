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
#include <algorithm>

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
