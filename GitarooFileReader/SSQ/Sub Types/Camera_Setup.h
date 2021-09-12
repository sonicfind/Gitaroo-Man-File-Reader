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
#include "Light_Setup.h"
class CameraSetup
{
public:
	struct BaseGlobalValues
	{
		glm::vec4 m_clearColor;
		glm::u32vec3 m_baseAmbience;
		unsigned long m_useDiffuse;
		glm::u32vec3 m_vertColorDiffuse;
		float m_fov;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;
		unsigned long ulong_h;
	};

	struct Projection : public Frame
	{
		float m_fov;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;
		unsigned long ulong_a;
		unsigned long m_doInterpolation;
	};

	struct AmbientColor : public Frame
	{
		glm::vec3 m_color;
		unsigned long m_doInterpolation;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
		// Seems to match m_otherPos
		unsigned long ulong_c;
	};

	struct Struct64_7f : public Frame
	{
		unsigned long m_cameraRelated;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long ulong_i;
		unsigned long ulong_j;
		unsigned long ulong_k;
		unsigned long ulong_l;
		unsigned long ulong_m;
		unsigned long m_doInterpolation;
	};
private:

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	Val m_junk[4] = { 0 };

public:
	BaseGlobalValues m_baseGlobalValues;

private:
	std::vector<Position> m_positions;
	std::vector<Rotation> m_rotations;
	std::vector<Projection> m_projections;
	std::vector<AmbientColor> m_ambientColors;
	std::vector<LightSetup> m_lights;
	std::vector<Struct64_7f> m_64bytes_v;

public:
	void read(FILE* inFile);
	void create(FILE* outFile);
};
