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
#include "Animation.h"
#include "Global_Functions.h"
float Animation::s_tempo = 120;

Animation::Animation(FILE* inFile)
{
	fread(&m_length, 4, 1, inFile);
	fread(&m_keyframe_interval, 4, 1, inFile);
	fread(&m_framerate, 4, 1, inFile);
	fread(&m_starting_keyframe, 4, 1, inFile);
	fread(&m_non_tempo, 4, 1, inFile);
	fread(&m_junk, 4, 3, inFile);
}

bool Animation::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile)
{
	fprintf_s(txtFile, "\t\t\t\t      Playback Length: %g\n", m_length);
	fprintf_s(txtFile, "\t\t\t\t    Keyframe Interval: %g\n", m_keyframe_interval);
	fprintf_s(txtFile, "\t\t\t\t\t    Framerate: %g\n", m_framerate);
	fprintf_s(txtFile, "\t\t\t\t    Starting Keyframe: %g\n", m_starting_keyframe);
	fprintf_s(txtFile, "\t\t\t\t     Not Tempo Linked: %s\n", (m_non_tempo ? "TRUE" : "FALSE"));
	return true;
}

float Animation::getTempo()
{
	return s_tempo;
}

bool Animation::setTempo()
{
	while (true)
	{
		GlobalFunctions::printf_tab("Current BPM: %g ['B' to leave unchanged]\n", s_tempo);
		GlobalFunctions::printf_tab("Input: ");
		switch (GlobalFunctions::valueInsert(s_tempo, false, "b"))
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::SpecialCase:
		case GlobalFunctions::ResultType::Success:
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

// Returns the total length in seconds
const float Animation::getTotalTime() const
{
	if (m_non_tempo)
		return m_length / 30;
	else
		return (15 * m_length) / (s_tempo * 2);
}

// Generates the keyframe value to give to the xgTime node
// 
// @param frame - The current time in terms of frames
const float Animation::getTime(const float frame) const
{
	if (m_non_tempo)
		return frame / m_keyframe_interval + m_starting_keyframe;
	else
		return frame * s_tempo / (225 * m_keyframe_interval) + m_starting_keyframe;
}
