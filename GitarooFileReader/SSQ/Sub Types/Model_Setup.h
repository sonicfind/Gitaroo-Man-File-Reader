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
class ModelSetup
{
public:
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
		float m_scalar[3];
		unsigned long m_envMap_maybe;
		unsigned long m_doInterpolation;
		unsigned long ulong_b;
	};

	struct BaseValues
	{
		float m_basePosition[3];
		float m_baseRotation[4];
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
protected:
	char* m_name;

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;
	char m_unk[8] = { 0 };
	Val m_junk[4] = { 0 };
	std::vector<Position> m_positions;
	std::vector<Rotation> m_rotations;
	std::vector<ModelAnim> m_animations;
	std::vector<ModelScalar> m_scalars;
	BaseValues m_baseValues;

public:
	ModelSetup(FILE* inFile, char(&name)[16]);
	virtual void create(FILE* outFile);
};
