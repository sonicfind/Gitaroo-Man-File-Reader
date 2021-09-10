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

class Animation
{
	float m_length = 0;
	float m_keyframe_interval = 0;
	float m_framerate = 60;
	float m_starting_keyframe = 0;
	unsigned long m_non_tempo = true;
	union
	{
		char c[4];
		float f;
		unsigned long ul;
		long l;
	} m_junk[3] = { 0 };

	static float s_tempo;
public:
	Animation(FILE* inFile);
	bool write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
	static float getTempo();
	static bool setTempo();

	const float getTotalTime() const;
	const float getTotalCoefficient() const;
	const float getTime(const float numSeconds) const;
};