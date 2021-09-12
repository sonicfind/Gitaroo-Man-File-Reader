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
#include "SSQ_BaseStructs.h"
#include "XG/XG.h"
class ModelSetup_Sub
{
public:
	virtual void create(FILE* outFile) const = 0;
	virtual void use() = 0;
};

class ModelSetup
{
	struct ModelAnim : public Frame
	{
		unsigned long m_animIndex;
		unsigned long m_startOverride;
		unsigned long m_noDrawing;
		unsigned long m_firstAnimofSection_maybe;
		unsigned long m_loop;
		unsigned long ulong_f;
		unsigned long m_holdLastFrame;
		unsigned long ulong_h;
		unsigned long m_unknown;
		unsigned long m_otherPos;
	};

	struct ModelScalar : public Frame
	{
		glm::vec3 m_scalar;
		unsigned long m_envMap_maybe;
		unsigned long m_doInterpolation;
		unsigned long ulong_b;
	};

	struct BaseValues
	{
		glm::vec3 m_basePosition;
		glm::quat m_baseRotation;
		unsigned long m_baseAnimIndex_maybe;
		unsigned long ulong_b;
		unsigned long m_depthTest;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		float float_h;
		float float_i;
		unsigned long ulong_g;
	};

	enum class ModelType
	{
		Normal,
		Player1,
		Player2,
		DuetPlayer,
		Player1AttDef = 5,
		Player2AttDef,
		DuetPlayerAttDef,
		DuetComboAttack,
		Snake
	};

	struct XGEntry
	{
		char m_name[16] = { 0 };
		unsigned long m_isClone;
		unsigned long m_cloneID;
		// Unused in the actual game, but is going to be used here
		unsigned long m_instanceIndex;
		ModelType m_type = ModelType::Normal;
		float m_length;
		float m_speed;
		float m_framerate;
		char m_junk[4];

		void read(FILE* inFile, unsigned long& instance);
		void create(FILE* outFile) const;
	} m_entry;

	XG* m_xg = nullptr;

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;
	char m_unk[8] = { 0 };
	Val m_junk[4] = { 0 };
	std::vector<Position> m_positions;
	size_t m_posIndex;
	std::vector<Rotation> m_rotations;
	size_t m_rotIndex;
	std::vector<ModelAnim> m_animations;
	size_t m_animIndex;
	std::vector<ModelScalar> m_scalars;
	size_t m_sclIndex;
	BaseValues m_baseValues;
	std::unique_ptr<ModelSetup_Sub> m_subType;

public:
	void readEntry(FILE* inFile, unsigned long& instance);
	void read(FILE* inFile);
	void createEntry(FILE* outFile) const;
	void create(FILE* outFile) const;
	const char* getName() { return m_entry.m_name; }
	void loadXG(XG* xg);
};

class PlayerModelSetup : public ModelSetup_Sub
{
	struct Struct48_2f
	{
		float float_a;
		float float_b;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long ulong_i;
		unsigned long ulong_j;
	};

	std::vector<Struct48_2f> m_player_controllable;

	struct Read4Entry
	{
		unsigned long m_size = 0;
		std::vector<unsigned long> m_vals;
	};

	std::vector<Read4Entry> m_player_read4Entry;

	std::vector<unsigned long> m_player_ulongs;

public:
	PlayerModelSetup(FILE* inFile);
	void create(FILE* outFile) const;
	void use() {}
};

class AttDefModelSetup : public ModelSetup_Sub
{
	float m_attdef_float32;

	struct Struct64_9f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		float float_h;
		unsigned long ulong_a;
		unsigned long ulong_b_maybe;
		unsigned long ulong_c_maybe;
		unsigned long ulong_d_maybe;
		unsigned long ulong_e;
		unsigned long ulong_f_maybe;
		unsigned long ulong_g_maybe;
		unsigned long ulong_h_maybe;
	} m_attdef_64bytes;

public:
	AttDefModelSetup(FILE* inFile);
	void create(FILE* outFile) const;
	void use() {}
};

class SnakeModelSetup : public ModelSetup_Sub
{
public:
	SnakeModelSetup(FILE* inFile);
	void create(FILE* outFile) const;
	void use() {}
};
