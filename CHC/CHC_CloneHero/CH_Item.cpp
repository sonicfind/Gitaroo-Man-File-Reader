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
#include "CH_Item.h"
using namespace std;

SyncTrack::SyncTrack(double pos, unsigned long ts, unsigned long tempo, bool writeTimeSig, string egth) : CHItem(pos), bpm(tempo), eighth(egth)
{
	if (writeTimeSig)
		timeSig = ts;
	else
		timeSig = 0;
}

SyncTrack::SyncTrack(FILE* inFile) : timeSig(0), bpm(0), eighth("")
{
	char ignore[30];
	fscanf_s(inFile, " %lf%[^BT]%c", &position, ignore, 30, ignore, 1);
	if (ignore[0] == 'T')
	{
		fseek(inFile, 2, SEEK_CUR);
		fscanf_s(inFile, "%lu", &timeSig);
		fscanf_s(inFile, "%c", ignore, 1);
		if (ignore[0] == ' ')
		{
			fscanf_s(inFile, "%c", ignore, 1);
			eighth = ignore[0];
		}
	}
	else if (ignore[0] == 'B')
		fscanf_s(inFile, " %lu", &bpm);
}

void SyncTrack::write(FILE* outFile)
{
	if (timeSig)
		fprintf(outFile, "  %lu = TS %lu%s\n", (unsigned long)round(position), timeSig, eighth.c_str());
	if (bpm)
		fprintf(outFile, "  %lu = B %lu\n", (unsigned long)round(position), bpm);
};

Event::Event(FILE* inFile)
{
	char bagel[101] = { 0 };
	fscanf_s(inFile, " %lf%[^\"]s", &position, bagel, 4);
	fscanf_s(inFile, " %[^\n]s", bagel, 100);
	name = bagel;
	if (name[0] == '\"')
		name = name.substr(1, name.length() - 2); //Gets rid of ""
};

void Event::write(FILE* outFile)
{
	fprintf(outFile, "  %lu = E \"%s\"\n", (unsigned long)round(position), name.c_str());
};

CHNote::CHNote(FILE* inFile)
{
	char type = 0;
	//Only the second "type" insertion matters; First one is '=' character removal
	fscanf_s(inFile, " %lf %c %c", &position, &type, 1, &type, 1);
	if (type == 'E')
	{
		char bagel[101] = { 0 };
		fscanf_s(inFile, " %[^\n]s", bagel, 100);
		name = bagel;
		this->type = NoteType::EVENT;
		mod = Modifier::NORMAL;
	}
	else
	{
		name = "";
		if (type == 'S')
			this->type = NoteType::STAR;
		else
			this->type = NoteType::NOTE;
		fscanf_s(inFile, " %zu", &fret.lane);
		if (fret.lane == 6)
		{
			mod = Modifier::TAP;
			fret.lane = 0;
		}
		else if (fret.lane == 5)
		{
			mod = Modifier::FORCED;
			fret.lane = 0;
		}
		else
		{
			mod = Modifier::NORMAL;
			if (fret.lane == 7)
				fret.lane = 5;
		}
		fscanf_s(inFile, " %lf", &fret.sustain);
	}
};

void CHNote::write(FILE* outFile)
{
	switch (type)
	{
	case CHNote::NoteType::STAR:
		fprintf(outFile, "  %lu = S 2 %lu\n", (unsigned long)round(position), (unsigned long)round(fret.sustain));
		break;
	case CHNote::NoteType::NOTE:
		fprintf(outFile, "  %lu = N %zu ", (unsigned long)round(position), fret.lane);
		if (fret.writeSustain)
			fprintf(outFile, "%lu\n", (unsigned long)round(fret.sustain));
		else
			fprintf(outFile, "%lu\n", 0UL);
		switch (mod)
		{
		case CHNote::Modifier::FORCED:
			fprintf(outFile, "  %lu = N 5 0\n", (unsigned long)round(position));
			break;
		case CHNote::Modifier::TAP:
			fprintf(outFile, "  %lu = N 6 0\n", (unsigned long)round(position));
		}
		if (name.length() <= 1)
			break;
	case CHNote::NoteType::EVENT:
		fprintf(outFile, "  %lu = E %s\n", (unsigned long)round(position), name.c_str());
	}
};

bool CHNote::operator==(const CHNote& note) const
{
	if (position == note.position && type == note.type)
	{
		switch (type)
		{
		case NoteType::NOTE:
			return fret.lane == note.fret.lane && mod == note.mod;
		case NoteType::EVENT:
		{
			if (name.length() == note.name.length())
			{
				const size_t length = name.length();
				for (size_t index = 0; index < length; index++)
					if (tolower(name[index]) != tolower(note.name[index]))
						return false;
				return true;
			}
			else
				return false;
		}
		default:
			return true;
		}
	}
	else
		return false;
}

bool CHNote::operator<(const CHNote& note) const
{
	if (position < note.position)
		return true;
	else if (position > note.position)
		return false;
	else
	{
		switch (type)
		{
		case NoteType::NOTE:
			switch (note.type)
			{
			case NoteType::NOTE:
				switch (fret.lane)
				{
				case 6:
					return false;
				case 5:
					return note.fret.lane == 6;
				case 7:
					return note.fret.lane == 5 || note.fret.lane == 6;
				default:
					if (fret.lane < note.fret.lane)
						return true;
					else if (fret.lane > note.fret.lane)
						return false;
					else
						return mod < note.mod;
				}
			default:
				return true;
			}
		case NoteType::STAR:
			switch (note.type)
			{
			case NoteType::EVENT:
				return true;
			default:
				return false;
			}
		default:
			switch (note.type)
			{
			case NoteType::EVENT:
			{
				const size_t len1 = name.length(), len2 = note.name.length();
				for (size_t index = 0; index < len1 && index < len2; index++)
				{
					if (tolower(name[index]) < tolower(note.name[index]))
						return true;
					else if (tolower(name[index]) > tolower(note.name[index]))
						return false;
				}
				if (len1 < len2)
					return true;
				else
					return false;
			}
			default:
				return false;

			}
		}
	}
}

bool CHNote::operator>(const CHNote& note) const
{
	if (position > note.position)
		return true;
	else if (position < note.position)
		return false;
	else
	{
		switch (type)
		{
		case NoteType::NOTE:
			switch (note.type)
			{
			case NoteType::NOTE:
				switch (fret.lane)
				{
				case 6:
					return note.fret.lane <= 5 || note.fret.lane == 7;
				case 5:
					return note.fret.lane < 5 || note.fret.lane == 7;
				case 7:
					return note.fret.lane < 5;
				default:
					if (fret.lane > note.fret.lane)
						return true;
					else if (fret.lane < note.fret.lane)
						return false;
					else
						return mod > note.mod;
				}
			default:
				return false;
			}
		case NoteType::STAR:
			switch (note.type)
			{
			case NoteType::NOTE:
				return true;
			default:
				return false;
			}
		default:
			switch (note.type)
			{
			case NoteType::EVENT:
			{
				const size_t len1 = name.length(), len2 = note.name.length();
				for (size_t index = 0; index < len1 && index < len2; index++)
				{
					if (tolower(name[index]) > tolower(note.name[index]))
						return true;
					else if (tolower(name[index]) < tolower(note.name[index]))
						return false;
				}
				if (len1 > len2)
					return true;
				else
					return false;
			}
			default:
				return true;

			}
		}
	}
}
