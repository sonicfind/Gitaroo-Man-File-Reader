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
#include "Notes.h"
Note& Note::operator=(const Note& note)
{
	pivotAlpha = note.pivotAlpha;
	return *this;
}

Path::Path(const Note& note)
{
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
	{
		pivotAlpha = path->pivotAlpha;
		duration = path->duration;
	}
	else
	{
		Note(note);
		duration = 1;
	}
}

Note& Path::operator=(const Note& note)
{
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
	{
		pivotAlpha = path->pivotAlpha;
		duration = path->duration;
	}
	else
	{
		Note::operator=(note);
		duration = 1;
	}
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

Traceline::Traceline(const Note& note)
{
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		pivotAlpha = trace->pivotAlpha;
		duration = trace->duration;
		angle = trace->angle;
		curve = trace->curve;
	}
	else
	{
		Path(note);
		angle = 0;
		curve = false;
	}
}

Note& Traceline::operator=(const Note& note)
{
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		pivotAlpha = trace->pivotAlpha;
		duration = trace->duration;
		angle = trace->angle;
		curve = trace->curve;
	}
	else
	{
		Path::operator=(note);
		angle = 0;
		curve = false;
	}
	return *this;
}

Phrase::Phrase() : Path(), start(true), end(true), animation(0), color(-1)
{
	for (unsigned char i = 0; i < 12; i++)
		junk[i] = 0;
}

Phrase::Phrase(long alpha, unsigned long dur, bool st, bool ed, unsigned long anim, char* jnk, long clr) : Path(alpha, dur), start(st), end(ed), animation(anim), color(clr)
{
	if (jnk == nullptr)
	{
		for (unsigned char i = 0; i < 12; i++)
			junk[i] = 0;
	}
	else
	{
		for (unsigned char i = 0; i < 12; i++)
			junk[i] = jnk[i];
	}
}

Phrase::Phrase(const Note& note)
{
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		pivotAlpha = phrase->pivotAlpha;
		duration = phrase->duration;
		start = phrase->start;
		end = phrase->end;
		animation = phrase->animation;
		for (unsigned char i = 0; i < 12; i++) junk[i] = phrase->junk[i];
	}
	else
	{
		Path(note);
		start = true;
		end = true;
		animation = 0;
		for (unsigned char i = 0; i < 12; i++) junk[i] = 0;
	}
}

Note& Phrase::operator=(const Note& note)
{
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		pivotAlpha = phrase->pivotAlpha;
		duration = phrase->duration;
		start = phrase->start;
		end = phrase->end;
		animation = phrase->animation;
		for (unsigned char i = 0; i < 12; i++) junk[i] = phrase->junk[i];
	}
	else
	{
		Path::operator=(note);
		start = true;
		end = true;
		animation = 0;
		for (unsigned char i = 0; i < 12; i++) junk[i] = 0;
	}
	return *this;
}

Guard::Guard(const Note& note)
{
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
	{
		pivotAlpha = guard->pivotAlpha;
		button = guard->button;
	}
	else
	{
		Note(note);
		button = 0;
	}
}

Note& Guard::operator=(const Note& note)
{
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
	{
		pivotAlpha = guard->pivotAlpha;
		button = guard->button;
	}
	else
	{
		Note::operator=(note);
		button = 0;
	}
	return *this;
}