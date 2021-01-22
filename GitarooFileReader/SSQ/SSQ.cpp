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
#include <filesystem>

SSQ::SSQ() : FileType(".SSQ") {}

SSQ::SSQ(std::string filename, bool useBanner)
	: FileType(filename, ".SSQ", useBanner)
{
	char test[4];
	fread(test, 1, 4, m_filePtr);
	if (!strstr(test, "GMSX"))
	{
		fclose(m_filePtr);
		throw "Error: No 'GMSX' Tag at byte 0.";
	}

	fread(&m_headerVersion, 4, 1, m_filePtr);
	fread(m_unk, 1, 12, m_filePtr);
	fread(m_junk, 1, 16, m_filePtr);

	unsigned long numXG = 0;
	fread(&numXG, 4, 1, m_filePtr);

	unsigned long numIMX = 0;
	fread(&numIMX, 4, 1, m_filePtr);

	for (unsigned long i = 0; i < numIMX; ++i)
		m_IMXentries.emplace_back(m_filePtr);

	fread(&numXG, 4, 1, m_filePtr);

	for (unsigned long i = 0; i < numXG; ++i)
		m_XGentries.emplace_back(m_filePtr);

	for (unsigned long i = 0; i < numXG; ++i)
	{
		switch (m_XGentries[i].m_type)
		{
		case ModelType::Normal:
			m_modelSetups.push_back(std::make_shared<ModelSetup>(m_filePtr, m_XGentries[i].m_name));
			break;
		case ModelType::Player1:
		case ModelType::Player2:
		case ModelType::DuetPlayer:
			m_modelSetups.push_back(std::make_shared<PlayerModelSetup>(m_filePtr, m_XGentries[i].m_name));
			break;
		case ModelType::Player1AttDef:
		case ModelType::Player2AttDef:
		case ModelType::DuetPlayerAttDef:
		case ModelType::DuetComboAttack:
			m_modelSetups.push_back(std::make_shared<AttDefModelSetup>(m_filePtr, m_XGentries[i].m_name));
			break;
		case ModelType::Snake:
			m_modelSetups.push_back(std::make_shared<SnakeModelSetup>(m_filePtr, m_XGentries[i].m_name));
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

	if (std::filesystem::exists(filename + ".XGM"))
	{
		do
		{
			printf("%sLoad %s.XGM as well? [Y/N][Q = N]\n", g_global.tabs.c_str(), filename.c_str());
			switch (GlobalFunctions::menuChoices("yn"))
			{
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character == 'y')
					m_xgm = std::make_unique<XGM>(filename);
				__fallthrough;
			case GlobalFunctions::ResultType::Quit:
				g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}
}

bool SSQ::create(std::string filename, bool trueSave)
{
	if (FileType::create(filename, true))
	{
		fprintf(m_filePtr, "GMSX");
		fwrite(&m_headerVersion, 4, 1, m_filePtr);
		fwrite(m_unk, 1, 12, m_filePtr);
		fwrite(m_junk, 1, 16, m_filePtr);

		unsigned long size = (unsigned long)m_XGentries.size();
		fwrite(&size, 4, 1, m_filePtr);

		size = (unsigned long)m_IMXentries.size();
		fwrite(&size, 4, 1, m_filePtr);

		for (IMXEntry& imx : m_IMXentries)
			imx.create(m_filePtr);

		size = (unsigned long)m_XGentries.size();
		fwrite(&size, 4, 1, m_filePtr);

		for (XGEntry& xg : m_XGentries)
			xg.create(m_filePtr);

		for (auto& model : m_modelSetups)
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

		if (trueSave)
			m_saved = true;
		return true;
	}
	return false;
}

IMXEntry::IMXEntry(FILE* inFile)
{
	fread(m_name, 1, 16, inFile);
	fread(&m_unused_1, 4, 1, inFile);
	fread(&m_unused_2, 4, 1, inFile);
	fread(m_unused_3, 8, 1, inFile);
}

void IMXEntry::create(FILE* outFile)
{
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_unused_1, 4, 1, outFile);
	fwrite(&m_unused_2, 4, 1, outFile);
	fwrite(m_unused_3, 8, 1, outFile);
}

XGEntry::XGEntry(FILE* inFile)
{
	fread(m_name, 1, 16, inFile);
	fread(&m_isClone, 4, 1, inFile);
	fread(&m_cloneID, 4, 1, inFile);
	fread(&m_unused_1, 4, 1, inFile);
	fread(&m_type, 4, 1, inFile);
	fread(m_unused_2, 1, 16, inFile);
}

void XGEntry::create(FILE* outFile)
{
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_isClone, 4, 1, outFile);
	fwrite(&m_cloneID, 4, 1, outFile);
	fwrite(&m_unused_1, 4, 1, outFile);
	fwrite(&m_type, 4, 1, outFile);
	fwrite(m_unused_2, 1, 16, outFile);
}

ModelSetup::ModelSetup(FILE* inFile, char(&name)[16]): m_name(name)
{
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "GMPX"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMPX' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_size, 4, 1, inFile);
	fread(m_unk, 1, 8, inFile);
	fread(m_junk, 1, 16, inFile);
	unsigned long numpositions, numrotations;
	fread(&numpositions, 4, 1, inFile);
	fread(&numrotations, 4, 1, inFile);

	m_positions.resize(numpositions);
	fread(&m_positions.front(), sizeof(ModelPosition), numpositions, inFile);

	m_rotations.resize(numrotations);
	fread(&m_rotations.front(), sizeof(ModelRotation), numrotations, inFile);

	unsigned long num48;
	fread(&num48, 4, 1, inFile);
	if (num48 > 1)
	{
		m_animations.resize(num48);
		fread(&m_animations.front(), sizeof(ModelAnim), num48, inFile);
	}

	if (m_headerVersion >= 0x1100)
	{
		unsigned long num32;
		fread(&num32, 4, 1, inFile);
		if (num32 > 1)
		{
			m_scaling.resize(num32);
			fread(&m_scaling.front(), sizeof(ModelScaling), num32, inFile);
		}
		fread(&m_64bytes_Opt, sizeof(BaseValues), 1, inFile);
	}
}

void ModelSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMPX");
	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_size, 4, 1, outFile);
	fwrite(m_unk, 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);

	unsigned long numpositions = (unsigned long)m_positions.size(), numrotations = (unsigned long)m_rotations.size();
	
	fwrite(&numpositions, 4, 1, outFile);
	fwrite(&numrotations, 4, 1, outFile);
	fwrite(&m_positions.front(), sizeof(ModelPosition), numpositions, outFile);
	fwrite(&m_rotations.front(), sizeof(ModelRotation), numrotations, outFile);

	unsigned long size = (unsigned long)m_animations.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);
	
	if (size > 1)
		fwrite(&m_animations.front(), sizeof(ModelAnim), size, outFile);

	if (m_headerVersion >= 0x1100)
	{
		size = (unsigned long)m_scaling.size();
		if (!size)
			size = 1;
		fwrite(&size, 4, 1, outFile);

		if (size > 1)
			fwrite(&m_scaling.front(), sizeof(ModelScaling), size, outFile);
		fwrite(&m_64bytes_Opt, sizeof(BaseValues), 1, outFile);
	}
}

PlayerModelSetup::PlayerModelSetup(FILE* inFile, char(&name)[16]) : ModelSetup(inFile, name)
{
	if (m_headerVersion >= 0x1300)
	{
		unsigned long numControllable;
		fread(&numControllable, 4, 1, inFile);
		m_player_controllable.resize(numControllable);
		fread(&m_player_controllable.front(), sizeof(Struct48_2f), numControllable, inFile);
		m_player_read4Entry.resize(numControllable);
		for (auto& r4e : m_player_read4Entry)
		{
			unsigned long num;
			fread(&num, 4, 1, inFile);
			if (num)
			{
				r4e.m_vals.resize(num);
				fread(&r4e.m_vals.front(), 4, num, inFile);
			}
		}
		m_player_ulongs.resize(numControllable);
		fread(&m_player_ulongs.front(), 4, numControllable, inFile);
	}
}

void PlayerModelSetup::create(FILE* outFile)
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1300)
	{
		unsigned long size = (unsigned long)m_player_controllable.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(&m_player_controllable.front(), sizeof(Struct48_2f), size, outFile);
		for (auto& r4e : m_player_read4Entry)
		{
			unsigned long num = (unsigned long)r4e.m_vals.size();
			fwrite(&num, 4, 1, outFile);
			if (num)
				fwrite(&r4e.m_vals.front(), 4, num, outFile);
		}
		fwrite(&m_player_ulongs.front(), 4, size, outFile);
	}
}

AttDefModelSetup::AttDefModelSetup(FILE* inFile, char(&name)[16]) : ModelSetup(inFile, name)
{
	if (m_headerVersion >= 0x1200)
	{
		fread(&m_attdef_float32, 4, 1, inFile);
		fread(&m_attdef_64bytes, sizeof(Struct64_9f), 1, inFile);
	}
}

void AttDefModelSetup::create(FILE* outFile)
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1200)
	{
		fwrite(&m_attdef_float32, 4, 1, outFile);
		fwrite(&m_attdef_64bytes, sizeof(Struct64_9f), 1, outFile);
	}
}

SnakeModelSetup::SnakeModelSetup(FILE* inFile, char(&name)[16]) : ModelSetup(inFile, name) {}
void SnakeModelSetup::create(FILE* outFile)
{
	ModelSetup::create(outFile);
}

void CameraSetup::read(FILE* inFile)
{
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "GMPX"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMPX' Tag (Camera) at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_size, 4, 1, inFile);
	fread(m_unk, 1, 8, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(&m_64bytes, sizeof(Struct64_9f), 1, inFile);

	unsigned long numpositions, numrotations;
	fread(&numpositions, 4, 1, inFile);
	fread(&numrotations, 4, 1, inFile);

	m_positions.resize(numpositions);
	fread(&m_positions.front(), sizeof(CamPosition), numpositions, inFile);

	m_rotations.resize(numrotations);
	fread(&m_rotations.front(), sizeof(CamRotation), numrotations, inFile);

	unsigned long unk1;
	fread(&unk1, 4, 1, inFile);
	if (unk1 > 1)
	{
		m_32bytes_1.resize(unk1);
		fread(&m_32bytes_1.front(), sizeof(Struct32_6f), unk1, inFile);
	}

	unsigned long numShadeColors;
	fread(&numShadeColors, 4, 1, inFile);
	if (numShadeColors > 1)
	{
		m_shadeColors.resize(numShadeColors);
		fread(&m_shadeColors.front(), sizeof(ShadeColor), numShadeColors, inFile);
	}

	unsigned long numlights;
	fread(&numlights, 4, 1, inFile);

	for (unsigned long i = 0; i < numlights; ++i)
		m_lights.emplace_back(inFile);

	if (m_headerVersion >= 0x1200)
	{
		unsigned long unk3;
		fread(&unk3, 4, 1, inFile);
		if (unk3 > 1)
		{
			m_64bytes_v.resize(unk3);
			fread(&m_64bytes_v.front(), sizeof(Struct64_7f), unk3, inFile);
		}
	}
}

void CameraSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMPX");
	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_size, 4, 1, outFile);
	fwrite(m_unk, 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_64bytes, sizeof(Struct64_9f), 1, outFile);

	unsigned long numpositions = (unsigned long)m_positions.size(), numrotations = (unsigned long)m_rotations.size();
	
	fwrite(&numpositions, 4, 1, outFile);
	fwrite(&numrotations, 4, 1, outFile);
	fwrite(&m_positions.front(), sizeof(CamPosition), numpositions, outFile);
	fwrite(&m_rotations.front(), sizeof(CamRotation), numrotations, outFile);

	unsigned long size = (unsigned long)m_32bytes_1.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);
	if (size > 1)
		fwrite(&m_32bytes_1.front(), sizeof(Struct32_6f), size, outFile);

	size = (unsigned long)m_shadeColors.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);
	if (size > 1)
		fwrite(&m_shadeColors.front(), sizeof(ShadeColor), size, outFile);

	size = (unsigned long)m_lights.size();
	fwrite(&size, 4, 1, outFile);

	for (auto& light: m_lights)
		light.create(outFile);

	if (m_headerVersion >= 0x1200)
	{
		size = (unsigned long)m_64bytes_v.size();
		if (!size)
			size = 1;
		fwrite(&size, 4, 1, outFile);
		if (size > 1)
			fwrite(&m_64bytes_v.front(), sizeof(Struct64_7f), size, outFile);
	}
}

LightSetup::LightSetup(FILE* inFile)
{

	fread(&m_80bytes, sizeof(Struct80_7f), 1, inFile);

	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "GMLT"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMLT' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_size, 4, 1, inFile);
	fread(m_unk, 1, 8, inFile);
	fread(m_junk, 1, 16, inFile);

	unsigned long num32, num48;
	fread(&num32, 4, 1, inFile);
	fread(&num48, 4, 1, inFile);

	if (num32 > 1)
	{
		m_32bytes.resize(num32);
		fread(&m_32bytes.front(), sizeof(Struct32_6f), num32, inFile);
	}

	if (num48 > 1)
	{
		m_48bytes.resize(num48);
		fread(&m_48bytes.front(), sizeof(Struct48_8f), num48, inFile);
	}
}

void LightSetup::create(FILE* outFile)
{
	fwrite(&m_80bytes, sizeof(Struct80_7f), 1, outFile);
	fprintf(outFile, "GMLT");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_size, 4, 1, outFile);
	fwrite(m_unk, 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);

	unsigned long num32 = (unsigned long)m_32bytes.size(), num48 = (unsigned long)m_48bytes.size();
	if (!num32)
		num32 = 1;

	if (!num48)
		num48 = 1;

	fwrite(&num32, 4, 1, outFile);
	fwrite(&num48, 4, 1, outFile);

	if (num32 > 1)
		fwrite(&m_32bytes.front(), sizeof(Struct32_6f), num32, outFile);

	if (num48 > 1)
		fwrite(&m_48bytes.front(), sizeof(Struct48_8f), num48, outFile);
}

void SpritesSetup::read(FILE* inFile)
{
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "GMSP"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMSP' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);

	unsigned long fixed, unk1, unk2;
	fread(&fixed, 4, 1, inFile);
	fread(&unk1, 4, 1, inFile);
	fread(&unk2, 4, 1, inFile);
	fread(&m_unused, 4, 1, inFile);

	if (fixed)
		m_fixedSpriteSetup.read(inFile);

	if (unk1)
		m_unk1SpriteSetup.read(inFile);

	if (unk2)
		m_unk2SpriteSetup.read(inFile);
}

void SpritesSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMSP");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);

	unsigned long fixed = m_fixedSpriteSetup.used(), unk1 = m_unk1SpriteSetup.used(), unk2 = m_unk2SpriteSetup.used();
	fwrite(&fixed, 4, 1, outFile);
	fwrite(&unk1, 4, 1, outFile);
	fwrite(&unk2, 4, 1, outFile);
	fwrite(&m_unused, 4, 1, outFile);

	if (fixed)
		m_fixedSpriteSetup.create(outFile);

	if (unk1)
		m_unk1SpriteSetup.create(outFile);

	if (unk2)
		m_unk2SpriteSetup.create(outFile);
}

void FixedSpriteSetup::read(FILE* inFile)
{
	m_used = true;
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "GMF0"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMF0' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);

	unsigned long fixed;
	fread(&fixed, 4, 1, inFile);

	m_80bytes.resize(fixed);
	fread(&m_80bytes.front(), sizeof(Struct80_7f), fixed, inFile);

	for (size_t i = 0; i < fixed; ++i)
		m_fixedSprites.emplace_back(inFile);
}

void FixedSpriteSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMF0");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);

	unsigned long fixed = (unsigned long)m_80bytes.size();
	fwrite(&fixed, 4, 1, outFile);
	fwrite(&m_80bytes.front(), sizeof(Struct80_7f), fixed, outFile);

	for (auto& fixed : m_fixedSprites)
		fixed.create(outFile);
}

void Unk1SpriteSetup::read(FILE* inFile)
{
	m_used = true;
}

void Unk1SpriteSetup::create(FILE* outFile)
{
	
}

void Unk2SpriteSetup::read(FILE* inFile)
{
	m_used = true;
}

void Unk2SpriteSetup::create(FILE* outFile)
{
	
}

FixedSprite::FixedSprite(FILE* inFile)
{
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "GMSP"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMSP' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(&m_64bytes, sizeof(Struct64_7f), 1, inFile);

	unsigned long num48, pair1, numFrames;
	fread(&num48, 4, 1, inFile);
	fread(&pair1, 4, 1, inFile);
	fread(&numFrames, 4, 1, inFile);

	if (num48 > 1)
	{
		m_48bytes.resize(num48);
		fread(&m_48bytes.front(), sizeof(Struct48_8f), num48, inFile);
	}

	if (pair1 > 1)
	{
		m_32Pair_1.resize(pair1);
		fread(&m_32Pair_1.front(), sizeof(Struct32_6f), pair1, inFile);
	}

	if (numFrames > 1)
	{
		m_spriteFrames.resize(numFrames);
		fread(&m_spriteFrames.front(), sizeof(SpriteFrame), numFrames, inFile);
	}
}

void FixedSprite::create(FILE* outFile)
{
	fprintf(outFile, "GMSP");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_64bytes, sizeof(Struct64_7f), 1, outFile);

	unsigned long num48 = (unsigned long)m_48bytes.size(), pair1 = (unsigned long)m_32Pair_1.size(), numFrames = (unsigned long)m_spriteFrames.size();
	if (!num48)
		num48 = 1;

	if (!pair1)
		pair1 = 1;

	if (!numFrames)
		numFrames = 1;

	fwrite(&num48, 4, 1, outFile);
	fwrite(&pair1, 4, 1, outFile);
	fwrite(&numFrames, 4, 1, outFile);

	if (num48 > 1)
		fwrite(&m_48bytes.front(), sizeof(Struct48_8f), num48, outFile);

	if (pair1 > 1)
		fwrite(&m_32Pair_1.front(), sizeof(Struct32_6f), pair1, outFile);

	if (numFrames > 1)
		fwrite(&m_spriteFrames.front(), sizeof(SpriteFrame), numFrames, outFile);
}

TexAnim::TexAnim(FILE* inFile)
{
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "\0\0\0\0"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No '\\0\\0\\0\\0' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk1, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(&m_unk2, 4, 1, inFile);
	fread(&m_unk3, 4, 1, inFile);
	fread(m_texture, 1, 24, inFile);

	unsigned long numCutOuts, numTexFrames;
	fread(&numCutOuts, 4, 1, inFile);
	m_cutOuts.resize(numCutOuts);
	fread(&m_cutOuts.front(), sizeof(CutOut), numCutOuts, inFile);

	fread(&numTexFrames, 4, 1, inFile);
	m_textureFrames.resize(numTexFrames);
	fread(&m_textureFrames.front(), sizeof(TexFrame), numTexFrames, inFile);
}

void TexAnim::create(FILE* outFile)
{
	fwrite("\0\0\0\0", 1, 4, outFile);

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk1, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_unk2, 4, 1, outFile);
	fwrite(&m_unk3, 4, 1, outFile);
	fwrite(m_texture, 1, 24, outFile);

	unsigned long numCutOuts = (unsigned long)m_cutOuts.size(), numTexFrames = (unsigned long)m_textureFrames.size();
	fwrite(&numCutOuts, 4, 1, outFile);
	fwrite(&m_cutOuts.front(), sizeof(CutOut), numCutOuts, outFile);

	fwrite(&numTexFrames, 4, 1, outFile);
	fwrite(&m_textureFrames.front(), sizeof(TexFrame), numTexFrames, outFile);
}

void PSetup::read(FILE* inFile)
{
	char test[4];
	fread(test, 1, 4, inFile);
	if (!strstr(test, "PSTP"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'PSTP' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);

	fread(&m_numMystery, 4, 1, inFile);
}

void PSetup::create(FILE* outFile)
{
	fprintf(outFile, "PSTP");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);

	fwrite(&m_numMystery, 4, 1, outFile);
}
