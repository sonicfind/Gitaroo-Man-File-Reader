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
Note& Note::operator=(const Note& note)
{
	pivotAlpha = note.pivotAlpha;
	return *this;
}

Path::Path(const Note& note) : Note(note)
{
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
		duration = path->duration;
	else
		duration = 1;
}

Note& Path::operator=(const Note& note)
{
	Note::operator=(note);
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
		duration = path->duration;
	else
		duration = 1;
	return *this;
}

bool Path::adjustDuration(long change)
{
	if ((long)duration + change >= 1)
	{
		duration += change;
		return true;
	}
	else
	{
		duration = 1;
		return false;
	}
}

bool Path::changePivotAlpha(const long alpha)
{
	if (alpha < pivotAlpha + (long)duration)
	{
		duration -= alpha - pivotAlpha;
		pivotAlpha = alpha;
		return true;
	}
	else
		return false;
}

bool Path::changeEndAlpha(const long endAlpha)
{
	if (endAlpha > pivotAlpha)
	{
		duration = endAlpha - pivotAlpha;
		return true;
	}
	else
		return false;
}

Traceline::Traceline(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&pivotAlpha, 4, 1, inFile);
		fread(&duration, 4, 1, inFile);
		fread(&angle, 4, 1, inFile);
		fread(&curve, 4, 1, inFile);
	}
	else
		throw "Error creating Trace line: Bruh, open a file first.";
}

Traceline::Traceline(const Note& note) : Path(note)
{
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		angle = trace->angle;
		curve = trace->curve;
	}
	else
	{
		angle = 0;
		curve = false;
	}
}

Note& Traceline::operator=(const Note& note)
{
	Path::operator=(note);
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		angle = trace->angle;
		curve = trace->curve;
	}
	else
	{
		angle = 0;
		curve = false;
	}
	return *this;
}

Phrase::Phrase(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&pivotAlpha, 4, 1, inFile);
		fread(&duration, 4, 1, inFile);
		fread(&start, 4, 1, inFile);
		fread(&end, 4, 1, inFile);
		fread(&animation, 4, 1, inFile);
		fread(junk, 1, 12, inFile);
		if (strstr(junk, "NOTECOLR"))
			memcpy_s((char*)&color, 4, junk + 8, 4);
		else
			color = 0;
	}
	else
		throw "Error creating Phrase bar: Bruh, open a file first.";
}

Phrase::Phrase(const Note& note) : Path(note)
{
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		start = phrase->start;
		end = phrase->end;
		animation = phrase->animation;
		for (unsigned char i = 0; i < 12; i++)
			junk[i] = phrase->junk[i];
	}
	else
	{
		start = true;
		end = true;
		animation = 0;
	}
}

Note& Phrase::operator=(const Note& note)
{
	Path::operator=(note);
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		start = phrase->start;
		end = phrase->end;
		animation = phrase->animation;
		for (unsigned char i = 0; i < 12; i++)
			junk[i] = phrase->junk[i];
	}
	else
	{
		start = true;
		end = true;
		animation = 0;
		for (unsigned char i = 0; i < 12; i++)
			junk[i] = 0;
	}
	return *this;
}

Guard::Guard(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&pivotAlpha, 4, 1, inFile);
		fread(&button, 4, 1, inFile);
		fseek(inFile, 8, SEEK_CUR);
	}
	else
		throw "Error creating Guard Mark: Bruh, open a file first.";
}

Guard::Guard(const Note& note) : Note(note)
{
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
		button = guard->button;
	else
		button = 0;
}

Note& Guard::operator=(const Note& note)
{
	Note::operator=(note);
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
		button = guard->button;
	else
		button = 0;
	return *this;
}