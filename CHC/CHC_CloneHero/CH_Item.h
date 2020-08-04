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

constexpr long double TICKS_PER_BEAT = 480.0;
struct CHItem
{
	double position;
	CHItem() : position(0) {}
	CHItem(double pos) : position(pos) {}
	bool operator==(const CHItem& item) const { return position == item.position; }
	bool operator<(const CHItem& item) const { return position < item.position; }
	bool operator<=(const CHItem& item) const { return operator==(item) || operator<(item); }
	bool operator>(const CHItem& item) const { return position > item.position; }
	bool operator>=(const CHItem& item) const { return operator==(item) || operator>(item); }
};

struct SyncTrack : public CHItem
{
	unsigned long timeSig;
	unsigned long bpm;
	std::string eighth;
	SyncTrack() : CHItem(), timeSig(4), bpm(120), eighth("") {}
	SyncTrack(FILE* inFile);
	SyncTrack(double pos, unsigned long ts = 4, unsigned long tempo = 120, bool writeTimeSig = true, std::string egth = "");
	void write(FILE* outFile);
};

struct Event : public CHItem
{
	std::string name;
	Event() : CHItem(), name("") {}
	Event(FILE* inFile);
	Event(double pos, std::string nam = "") : CHItem(pos), name(nam) {}
	void write(FILE* outFile);
};

struct CHNote : public CHItem
{
	enum class NoteType { NOTE, STAR, EVENT };
	enum class Modifier { NORMAL, FORCED, TAP };
	struct Fret
	{
		size_t lane;
		double sustain;
		bool writeSustain;
		Fret() : lane(0), sustain(0), writeSustain(true) {}
		Fret(size_t lane, double sus = 0, bool write = true) : lane(lane), sustain(sus), writeSustain(write) {}
		Fret(const Fret& fret) : lane(fret.lane), sustain(fret.sustain), writeSustain(fret.writeSustain) {}
	};
	Fret fret;
	Modifier mod;
	NoteType type;
	std::string name;
	CHNote() : CHItem(), mod(Modifier::NORMAL), type(NoteType::NOTE), name("") {}
	CHNote(FILE* inFile);
	CHNote(double pos, size_t lane = 0, double sus = 0, bool write = true, Modifier md = Modifier::NORMAL, NoteType tp = NoteType::NOTE, std::string nam = "")
		: CHItem(pos), fret(lane, sus, write), mod(md), type(tp), name(nam) {}
	CHNote(CHNote& note) : CHItem(note.position), fret(note.fret), mod(note.mod), type(note.type), name(note.name) {}
	double setEndPoint(double endTick)
	{
		fret.sustain = endTick - position;
		return fret.sustain;
	}
	void write(FILE* outFile);
	bool operator==(const CHNote& note) const;
	bool operator<(const CHNote& note) const;
	bool operator<=(const CHNote& note) const { return operator==(note) || operator<(note); }
	bool operator>(const CHNote& note) const;
	bool operator>=(const CHNote& note) const { return operator==(note) || operator>(note); }
};
