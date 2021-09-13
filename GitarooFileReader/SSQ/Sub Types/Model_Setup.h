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
	const char* m_name;
	XG* m_xg = nullptr;
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

protected:
	unsigned long m_headerVersion;

public:
	ModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;
	void loadXG(XG* xg);
};

class PlayerModelSetup : public ModelSetup
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
	PlayerModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;
};

class AttDefModelSetup : public ModelSetup
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
	AttDefModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;
};

class SnakeModelSetup : public ModelSetup
{
public:
	SnakeModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;
};
