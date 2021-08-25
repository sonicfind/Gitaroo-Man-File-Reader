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

union Val
{
	char c[4];
	float f;
	unsigned long ul;
	long l;
};

struct Frame
{
	float m_frame;
	float m_coefficient;
};

struct Position : public Frame
{
	float m_position[3];
	unsigned long m_doInterpolation;
	// Essentially m_frame * 160
	unsigned long m_otherPos;
	unsigned long ulong_c;
};

struct Rotation : public Frame
{
	float m_rotation[4];
	unsigned long m_doInterpolation;
	// Essentially m_frame * 160
	unsigned long m_otherPos;
};
