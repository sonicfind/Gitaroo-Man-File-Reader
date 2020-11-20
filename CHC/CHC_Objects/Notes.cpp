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
#include "..\..\Header\pch.h"
#include "Notes.h"
#include <algorithm>
Note& Note::operator=(const Note& note)
{
	m_pivotAlpha = note.m_pivotAlpha;
	return *this;
}

Path::Path(const Note& note) : Note(note)
{
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
		m_duration = path->m_duration;
	else
		m_duration = 1;
}

Note& Path::operator=(const Note& note)
{
	Note::operator=(note);
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
		m_duration = path->m_duration;
	else
		m_duration = 1;
	return *this;
}

bool Path::adjustDuration(long change)
{
	if ((long)m_duration + change >= 1)
	{
		m_duration += change;
		return true;
	}
	else
	{
		m_duration = 1;
		return false;
	}
}

bool Path::changePivotAlpha(const long alpha)
{
	if (alpha < m_pivotAlpha + (long)m_duration)
	{
		m_duration -= alpha - m_pivotAlpha;
		m_pivotAlpha = alpha;
		return true;
	}
	else
		return false;
}

bool Path::changeEndAlpha(const long endAlpha)
{
	if (endAlpha > m_pivotAlpha)
	{
		m_duration = endAlpha - m_pivotAlpha;
		return true;
	}
	else
		return false;
}

Traceline::Traceline(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&m_pivotAlpha, 4, 1, inFile);
		fread(&m_duration, 4, 1, inFile);
		fread(&m_angle, 4, 1, inFile);
		fread(&m_curve, 4, 1, inFile);
	}
	else
		throw "Error creating Trace line: Bruh, open a file first.";
}

Traceline::Traceline(const Note& note) : Path(note)
{
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		m_angle = trace->m_angle;
		m_curve = trace->m_curve;
	}
	else
	{
		m_angle = 0;
		m_curve = false;
	}
}

Note& Traceline::operator=(const Note& note)
{
	Path::operator=(note);
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		m_angle = trace->m_angle;
		m_curve = trace->m_curve;
	}
	else
	{
		m_angle = 0;
		m_curve = false;
	}
	return *this;
}

Phrase::Phrase(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&m_pivotAlpha, 4, 1, inFile);
		fread(&m_duration, 4, 1, inFile);
		fread(&m_start, 4, 1, inFile);
		fread(&m_end, 4, 1, inFile);
		fread(&m_animation, 4, 1, inFile);
		fread(m_junk, 1, 8, inFile);
		if (strstr(m_junk, "NOTECOLR"))
			fread(&m_color, 4, 1, inFile);
		else
		{
			fread(m_junk + 8, 1, 4, inFile);
			m_color = 0;
		}
	}
	else
		throw "Error creating Phrase bar: Bruh, open a file first.";
}

Phrase::Phrase(const Note& note) : Path(note)
{
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		m_start = phrase->m_start;
		m_end = phrase->m_end;
		m_animation = phrase->m_animation;
		std::copy(phrase->m_junk, phrase->m_junk + 12, m_junk);
	}
	else
	{
		m_start = true;
		m_end = true;
		m_animation = 0;
	}
}

Note& Phrase::operator=(const Note& note)
{
	Path::operator=(note);
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		m_start = phrase->m_start;
		m_end = phrase->m_end;
		m_animation = phrase->m_animation;
		std::copy(phrase->m_junk, phrase->m_junk + 12, m_junk);
	}
	else
	{
		m_start = true;
		m_end = true;
		m_animation = 0;
		for (unsigned char i = 0; i < 12; i++)
			m_junk[i] = 0;
	}
	return *this;
}

Guard::Guard(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&m_pivotAlpha, 4, 1, inFile);
		fread(&m_button, 4, 1, inFile);
		fseek(inFile, 8, SEEK_CUR);
	}
	else
		throw "Error creating Guard Mark: Bruh, open a file first.";
}

Guard::Guard(const Note& note) : Note(note)
{
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
		m_button = guard->m_button;
	else
		m_button = 0;
}

Note& Guard::operator=(const Note& note)
{
	Note::operator=(note);
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
		m_button = guard->m_button;
	else
		m_button = 0;
	return *this;
}
