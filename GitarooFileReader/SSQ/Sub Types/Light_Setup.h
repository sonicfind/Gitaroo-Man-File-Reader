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
class LightSetup
{
	struct BaseValues
	{
		unsigned long m_isActive;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;
		glm::quat m_rotation;
		Val ulong_b;
		Val l_a;
		Val ulong_c;
		Val ulong_d;
		Val ulong_e;
		Val ulong_f;
		Val ulong_g;
		Val ulong_h;
		Val ulong_i;
	} m_baseValues;

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	Val m_junk[4] = { 0 };

	std::vector<Rotation> m_rotations;

	struct LightColors : public Frame
	{
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;
		unsigned long m_doInterpolation;
		unsigned long ulong_b;
		float float_a;
		unsigned long ulong_d;
	};

	std::vector<LightColors> m_colors;

public:
	LightSetup(FILE* inFile);
	void create(FILE* outFile);
};
