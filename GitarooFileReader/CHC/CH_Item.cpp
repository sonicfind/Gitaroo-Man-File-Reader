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
#include "CH_Item.h"
using namespace std;

SyncTrack::SyncTrack(float pos, unsigned long ts, unsigned long tempo, bool writeTimeSig, string egth) : CHItem(pos), m_bpm(tempo), m_eighth(egth)
{
	if (writeTimeSig)
		m_timeSig = ts;
	else
		m_timeSig = 0;
}

SyncTrack::SyncTrack(FILE* inFile) : m_timeSig(0), m_bpm(0), m_eighth("")
{
	char ignore[30];
	fscanf_s(inFile, " %f%[^BT]%c", &m_position, ignore, 30, ignore, 1);
	if (ignore[0] == 'T')
	{
		fseek(inFile, 2, SEEK_CUR);
		fscanf_s(inFile, "%lu", &m_timeSig);
		fscanf_s(inFile, "%c", ignore, 1);
		if (ignore[0] == ' ')
		{
			fscanf_s(inFile, "%c", ignore, 1);
			m_eighth = ignore[0];
		}
	}
	else if (ignore[0] == 'B')
		fscanf_s(inFile, " %lu", &m_bpm);
}

void SyncTrack::write(FILE* outFile)
{
	if (m_timeSig)
		fprintf(outFile, "  %lu = TS %lu%s\n", (unsigned long)round(m_position), m_timeSig, m_eighth.c_str());
	if (m_bpm)
		fprintf(outFile, "  %lu = B %lu\n", (unsigned long)round(m_position), m_bpm);
};

Event::Event(FILE* inFile)
{
	char bagel[101] = { 0 };
	fscanf_s(inFile, " %f%[^\"]s", &m_position, bagel, 4);
	fscanf_s(inFile, " %[^\n]s", bagel, 100);
	m_name = bagel;
	if (m_name[0] == '\"')
		m_name = m_name.substr(1, m_name.length() - 2); //Gets rid of ""
};

void Event::write(FILE* outFile)
{
	fprintf(outFile, "  %lu = E \"%s\"\n", (unsigned long)round(m_position), m_name.c_str());
};

CHNote::CHNote(FILE* inFile)
{
	char type = 0;
	//Only the second "m_type" insertion matters; First one is '=' character removal
	fscanf_s(inFile, " %f %c %c", &m_position, &type, 1, &type, 1);
	if (type == 'E')
	{
		char bagel[101] = { 0 };
		fscanf_s(inFile, " %[^\n]s", bagel, 100);
		m_name = bagel;
		this->m_type = NoteType::EVENT;
		m_mod = Modifier::NORMAL;
	}
	else
	{
		m_name = "";
		if (type == 'S')
			this->m_type = NoteType::STAR;
		else
			this->m_type = NoteType::NOTE;
		fscanf_s(inFile, " %u", &m_fret.m_lane);
		if (m_fret.m_lane == 6)
		{
			m_mod = Modifier::TAP;
			m_fret.m_lane = 0;
		}
		else if (m_fret.m_lane == 5)
		{
			m_mod = Modifier::FORCED;
			m_fret.m_lane = 0;
		}
		else
		{
			m_mod = Modifier::NORMAL;
			if (m_fret.m_lane == 7)
				m_fret.m_lane = 5;
		}
		fscanf_s(inFile, " %f", &m_fret.m_sustain);
	}
};

void CHNote::write(FILE* outFile)
{
	switch (m_type)
	{
	case CHNote::NoteType::STAR:
		fprintf(outFile, "  %lu = S 2 %lu\n", (unsigned long)round(m_position), (unsigned long)round(m_fret.m_sustain));
		break;
	case CHNote::NoteType::NOTE:
		fprintf(outFile, "  %lu = N %u ", (unsigned long)round(m_position), m_fret.m_lane);
		if (m_fret.m_writeSustain)
			fprintf(outFile, "%lu\n", (unsigned long)round(m_fret.m_sustain));
		else
			fprintf(outFile, "%lu\n", 0UL);
		switch (m_mod)
		{
		case CHNote::Modifier::FORCED:
			fprintf(outFile, "  %lu = N 5 0\n", (unsigned long)round(m_position));
			break;
		case CHNote::Modifier::TAP:
			fprintf(outFile, "  %lu = N 6 0\n", (unsigned long)round(m_position));
		}
		if (m_name.length() <= 1)
			break;
	case CHNote::NoteType::EVENT:
		fprintf(outFile, "  %lu = E %s\n", (unsigned long)round(m_position), m_name.c_str());
	}
};

bool CHNote::operator==(const CHNote& note) const
{
	if (m_position == note.m_position && m_type == note.m_type)
	{
		switch (m_type)
		{
		case NoteType::NOTE:
			return m_fret.m_lane == note.m_fret.m_lane && m_mod == note.m_mod;
		case NoteType::EVENT:
			if (m_name.length() == note.m_name.length())
			{
				for (std::string::const_iterator iter1 = m_name.begin(), iter2 = note.m_name.begin();
					iter1 != m_name.end(); ++iter1, ++iter2)
				{
					if (tolower(*iter1) != tolower(*iter2))
						return false;
				}
				return true;
			}
			else
				return false;
		default:
			return true;
		}
	}
	else
		return false;
}

bool CHNote::operator<(const CHNote& note) const
{
	if (m_position < note.m_position)
		return true;
	else if (m_position > note.m_position)
		return false;
	else
	{
		switch (m_type)
		{
		case NoteType::NOTE:
			switch (note.m_type)
			{
			case NoteType::NOTE:
				switch (m_fret.m_lane)
				{
				case 6:
					return false;
				case 5:
					return note.m_fret.m_lane == 6;
				case 7:
					return note.m_fret.m_lane == 5 || note.m_fret.m_lane == 6;
				default:
					if (m_fret.m_lane < note.m_fret.m_lane)
						return true;
					else if (m_fret.m_lane > note.m_fret.m_lane)
						return false;
					else
						return m_mod < note.m_mod;
				}
			default:
				return true;
			}
		case NoteType::STAR:
			switch (note.m_type)
			{
			case NoteType::EVENT:
				return true;
			default:
				return false;
			}
		default:
			switch (note.m_type)
			{
			case NoteType::EVENT:
				for (std::string::const_iterator iter1 = m_name.begin(), iter2 = note.m_name.begin();
					iter1 != m_name.end() && iter2 != note.m_name.end(); ++iter1, ++iter2)
				{
					if (tolower(*iter1) < tolower(*iter2))
						return true;
					else if (tolower(*iter1) > tolower(*iter2))
						return false;
				}
				if (m_name.length() < note.m_name.length())
					return true;
				else
					return false;
			default:
				return false;

			}
		}
	}
}

bool CHNote::operator>(const CHNote& note) const
{
	if (m_position > note.m_position)
		return true;
	else if (m_position < note.m_position)
		return false;
	else
	{
		switch (m_type)
		{
		case NoteType::NOTE:
			switch (note.m_type)
			{
			case NoteType::NOTE:
				switch (m_fret.m_lane)
				{
				case 6:
					return note.m_fret.m_lane <= 5 || note.m_fret.m_lane == 7;
				case 5:
					return note.m_fret.m_lane < 5 || note.m_fret.m_lane == 7;
				case 7:
					return note.m_fret.m_lane < 5;
				default:
					if (m_fret.m_lane > note.m_fret.m_lane)
						return true;
					else if (m_fret.m_lane < note.m_fret.m_lane)
						return false;
					else
						return m_mod > note.m_mod;
				}
			default:
				return false;
			}
		case NoteType::STAR:
			switch (note.m_type)
			{
			case NoteType::NOTE:
				return true;
			default:
				return false;
			}
		default:
			switch (note.m_type)
			{
			case NoteType::EVENT:
				for (std::string::const_iterator iter1 = m_name.begin(), iter2 = note.m_name.begin();
					iter1 != m_name.end() && iter2 != note.m_name.end(); ++iter1, ++iter2)
				{
					if (tolower(*iter1) > tolower(*iter2))
						return true;
					else if (tolower(*iter1) < tolower(*iter2))
						return false;
				}
				if (m_name.length() > note.m_name.length())
					return true;
				else
					return false;
			default:
				return true;

			}
		}
	}
}
