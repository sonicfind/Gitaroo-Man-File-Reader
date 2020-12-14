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

constexpr float s_TICKS_PER_BEAT = 480.0f;
struct CHItem
{
	float m_position;
	CHItem() : m_position(0) {}
	CHItem(float pos) : m_position(pos) {}
	bool operator==(const CHItem& item) const { return m_position == item.m_position; }
	bool operator<(const CHItem& item) const { return m_position < item.m_position; }
	bool operator<=(const CHItem& item) const { return operator==(item) || operator<(item); }
	bool operator>(const CHItem& item) const { return m_position > item.m_position; }
	bool operator>=(const CHItem& item) const { return operator==(item) || operator>(item); }
};

struct SyncTrack : public CHItem
{
	unsigned long m_timeSig;
	unsigned long m_bpm;
	std::string m_eighth;
	SyncTrack() : CHItem(), m_timeSig(4), m_bpm(120), m_eighth("") {}
	SyncTrack(FILE* inFile);
	SyncTrack(float pos, unsigned long ts = 4, unsigned long tempo = 120, bool writeTimeSig = true, std::string egth = "");
	void write(FILE* outFile);
};

struct Event : public CHItem
{
	std::string m_name;
	Event() : CHItem(), m_name("") {}
	Event(FILE* inFile);
	Event(float pos, std::string nam = "") : CHItem(pos), m_name(nam) {}
	void write(FILE* outFile);
};

struct CHNote : public CHItem
{
	enum class NoteType { NOTE, STAR, EVENT };
	enum class Modifier { NORMAL, FORCED, TAP };
	struct Fret
	{
		unsigned m_lane;
		float m_sustain;
		bool m_writeSustain;
		Fret() : m_lane(0), m_sustain(0), m_writeSustain(true) {}
		Fret(unsigned lane, float sus = 0, bool write = true) : m_lane(lane), m_sustain(sus), m_writeSustain(write) {}
		Fret(const Fret& fret) : m_lane(fret.m_lane), m_sustain(fret.m_sustain), m_writeSustain(fret.m_writeSustain) {}
	};
	Fret m_fret;
	Modifier m_mod;
	NoteType m_type;
	std::string m_name;
	CHNote() : CHItem(), m_mod(Modifier::NORMAL), m_type(NoteType::NOTE), m_name("") {}
	CHNote(FILE* inFile);
	CHNote(float pos, unsigned lane = 0, float sus = 0, bool write = true, Modifier md = Modifier::NORMAL, NoteType tp = NoteType::NOTE, std::string nam = "")
		: CHItem(pos), m_fret(lane, sus, write), m_mod(md), m_type(tp), m_name(nam) {}
	CHNote(const CHNote& note) = default;
	float setEndPoint(float endTick)
	{
		m_fret.m_sustain = endTick - m_position;
		return m_fret.m_sustain;
	}
	void write(FILE* outFile);
	bool operator==(const CHNote& note) const;
	bool operator<(const CHNote& note) const;
	bool operator<=(const CHNote& note) const { return operator==(note) || operator<(note); }
	bool operator>(const CHNote& note) const;
	bool operator>=(const CHNote& note) const { return operator==(note) || operator>(note); }
};
