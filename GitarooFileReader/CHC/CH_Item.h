#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020-2021 Gitaroo Pals
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
struct CHObject
{
	float m_position_ticks;
	CHObject();
	CHObject(float pos_ticks);
	bool operator==(const CHObject& item) const { return m_position_ticks == item.m_position_ticks; }
	auto operator<=>(const CHObject& item) const { return m_position_ticks <=> item.m_position_ticks; }
};

struct SyncTrack : public CHObject
{
	unsigned long m_timeSigNumerator = 4;
	unsigned long m_timeSigDenomSelection = 2;
	unsigned long m_bpm = 120;
	SyncTrack(FILE* inFile);
	SyncTrack(float pos, unsigned long ts, unsigned long tempo = 0, unsigned long denom = 2);
	void write(FILE* outFile);
};

struct Event : public CHObject
{
	std::string m_name;
	Event(FILE* inFile);
	Event(float pos, std::string nam = "") : CHObject(pos), m_name(nam) {}
	void write(FILE* outFile);
};

class CHNote : public CHObject
{
private:
	// Sets the parents of all the frets
	CHNote(float pos);

public:
	struct Fret
	{
		bool m_active = false;
		bool m_writeSustain = true;
		float m_sustain = 0;
		CHNote* m_parent = nullptr;
		Fret& operator=(const Fret& note);
		void setEndPoint(float endTick, float threshold = 0);
		bool operator!=(const Fret& fret) const { return *m_parent != *fret.m_parent; }
		auto operator<(const Fret& fret) const { return m_parent->m_position_ticks < fret.m_parent->m_position_ticks; }
	};

	// Order: Green Red Yellow Blue Orange Open
	Fret m_colors[6];
	bool m_isForced = false;
	bool m_isTap = false;
	Fret m_star;
	std::list<std::string> m_events;
	
	// Pull values from a .chart file
	CHNote(FILE* inFile);

	// Note constructor
	CHNote(float pos, int frets, float sustain = 0, bool write = true);

	// Event constructor
	CHNote(float pos, std::string& ev);

	// Star Power constructor
	CHNote(float start, float end);
	CHNote& operator=(const CHNote& note);
	CHNote(const CHNote& note);
	void write(FILE* outFile) const;
	bool hasColors() const;
};
