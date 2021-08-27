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
