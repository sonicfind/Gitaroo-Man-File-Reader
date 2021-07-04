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
#include "FileType.h"
constexpr float s_SAMPLES_PER_MIN = 2880000.0f;

struct SSQRange
{
	float first = 0;
	float last = 0;
};

class SongSection;

class CHC
	: public FileType
{
	friend class CHC_To_CloneHero;
	friend class CloneHero_To_CHC;
	friend class SongSection;
private:
	// Stage number
	int m_stage;
	// Holds header data from original file
	char m_header[36] = { 0 };

	// String for an IMC's location
	char m_imc[256] = { 0 };

	// Win-Loss Animations
	SSQRange m_events[4];

	// Hold data for all 8 audio channels
	struct AudioChannel
	{
		unsigned long volume = 32767;
		unsigned long pan = 16383;
	} m_audio[8];

	// Gameplay speed
	float m_speed;
	// Hold the number of unorganized sections
	unsigned m_unorganized;
	// Saves whether all notes were readjusted for minimal glitching
	bool m_optimized;
	// Holds all life-value data for every player and phase type
	struct EnergyDamage
	{
		float initialEnergy = .2f;
		float chargeInitial = .01f;
		float attackInitial = .015f;
		float guardEnergy = .015f;
		float attackMiss = .05f;
		float guardMiss = .05f;
		float chargeRelease = .025f;
		float attackRelease = .025f;
	} m_energyDamageFactors[4][5];

public:
	// Vector of all sections
	std::vector<SongSection> m_sections;
	CHC();
	CHC(std::string filename);
	CHC(const CHC&);
	CHC& operator=(const CHC&) = default;
	bool create(std::string filename);
	bool write_to_txt();
	bool applyChanges(const bool fix, const bool swap = false, const bool save = false);
	void edit(const bool multi = false);
	bool buildTAS();
	bool exportForCloneHero();
	CHC* importFromCloneHero();
	bool colorCheatTemplate();
	size_t getNumSections() const { return m_sections.size(); }
	bool isPS2Compatible() const { return m_imc[0] > 0; }
	bool isOrganized() const { return m_unorganized == 0; }
	bool isOptimized() const { return m_optimized; }

private:
	// Editor-based functions

	void swapIMC();
	void audioSettings();
	void winLossSettings();
	void adjustSpeed();
	void adjustFactors();

	void fixNotes();
	void organizeAll();
	void PSPToPS2();

	void sectionMassMenu();
	void playerSwapAll();
	void playOrder();
	void rearrange();
	void sectionSelector();

	bool testPathing(const size_t index = 0);
	bool sectionPathTest(const size_t index, bool* isAccessible, bool* canReachEnd);
};

class Chart;

class SongSection
{
	friend class CHC;
public:
	enum class Phase { INTRO, CHARGE, BATTLE, FINAL_AG, HARMONY, END, FINAL_I };
private:
	//Index pulled from the list of SSQs in the CHC
	unsigned long m_index = 0;
	// Section name
	// 16 characters + a null character
	char m_name[17] = { 0 };

	//Name of Audio Section
	// 16 characters + a null character
	char m_audio[17] = { 0 };

	//SSQ frame range
	SSQRange m_frames;
	// Saves whether the subsections are correctly ordered
	unsigned long m_organized = false;
	// Value that holds how the section is swapped
	unsigned long m_swapped = 0;
	// Just junk, saved for consistency
	char m_junk[16] = { 0 };

	// Phase type
	Phase m_battlePhase = Phase::INTRO;
	// BPM for the section
	float m_tempo = 0;
	// Total duration in samples
	unsigned long m_duration = 0;
	// Determines what actions to take after the section
	struct Condition
	{
		unsigned long m_type;
		float m_argument;
		long m_trueEffect;
		long m_falseEffect;
		Condition();
		Condition(FILE* inFile);
		Condition(const Condition& cond) = default;
		void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const CHC* const chc);
	};
	// Vector of all conditions
	std::vector<Condition> m_conditions;
	// Number of players assigned to the section, always 4
	unsigned long m_numPlayers = 4;
	// Number of charts/subsections per player
	unsigned long m_numCharts = 1;
	// Parent CHC
	CHC* m_parent = nullptr;
public:
	// Vector of all charts/subsections
	std::vector<Chart> m_charts;
	static const long s_SAMPLE_GAP = 1800;
	SongSection(CHC* parent);
	SongSection();
	SongSection(CHC* parent, FILE* inFile);
	SongSection(const SongSection&) = default;
	SongSection& operator=(const SongSection& section);
	// Fills in rest of values from the given file
	// Returns if the section is organized
	void continueRead(FILE* inFile, const size_t index, const int stage, bool isDuet);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const CHC* const chc);

	// Returns name C-string (size: 16)
	char* getName() { return m_name; }
	// Returns audio C-string (size: 16)
	char* getAudio() { return m_audio; }
	// Returns whether the section is organized
	bool getOrganized() const { return m_organized; }
	// Sets organized to the provided value
	void setOrganized(unsigned long org);
	// Returns swap value
	unsigned long getSwapped() const { return m_swapped; }
	// Sets swap value
	void setSwapped(char swap) { m_swapped = swap; }
	// Returns the byte size of the section
	unsigned long getSize() const;
	// Returns a section's phase type
	Phase getPhase() const { return m_battlePhase; }
	// Sets the section's phase type to the provided value
	void setPhase(Phase ph) { m_battlePhase = ph; }
	// Sets the section's phase type to the provided value converted into a phase type
	// If the provided value is 6 or greater, battlePhase is set to FINAL_I
	void setPhase(unsigned long ph)
	{
		if (ph < 6)
			m_battlePhase = static_cast<SongSection::Phase>(ph);
		else
			m_battlePhase = Phase::FINAL_I;
	}
	// Returns the tempo for the section
	float getTempo() const { return m_tempo; }
	// Returns the duration of the section
	unsigned long getDuration() const { return m_duration; }
	// Sets the duration of the section to the provided value
	void setDuration(unsigned long dur) { m_duration = dur; }
	// Returns the number of conditions in the section
	size_t getNumCondtions() const { return m_conditions.size(); }
	// Adds a new condition to the end of the section's condition list
	template<class...Args>
	size_t addCondition(size_t index, Args&&...args)
	{
		if (index > m_conditions.size())
			index = m_conditions.size();
		m_conditions.emplace(index, args...);
		return index;
	}
	bool removeCondition(size_t);
	// Returns the num of players assigned to this section.
	// Will usually be 4
	unsigned long getNumPlayers() const { return m_numPlayers; }
	// Returns the total number of the charts/subsections in this section
	unsigned long getNumCharts() const { return m_numCharts; }
	void clearConditions();
	void operator++();
	bool operator--();

private:
	// Editor-based functions

	bool reorganize(const bool isPs2, const int stage);
	void playerSwap(const bool isPs2);

	void menu(const bool isPs2, const int stage);
	void changeName();
	void changeAudio();
	void changeFrames();
	void switchPhase();
	void adjustTempo();
	void adjustDuration();
	void conditionMenu() {}
};

class Note;
class Traceline;
class Phrase;
class Guard;

class Chart
{
	friend class CHC;
private:
	// Just junk, saved for consistency
	char m_junk[16] = { 0 };

	// 'Center' point that all notes revolve around
	long m_pivotTime = 0;
	// Optional value noting when to transition to another chart/subsection
	// Unused in the game
	long m_endTime = 0;
public:
	// Vector of all trace lines
	//
	// DO NOT ADD OR REMOVE DIRECTLY FROM THIS LIST!!
	// USE emplaceTracline()!!
	std::vector<Traceline> m_tracelines;
	// Vector of all phrase bars
	//
	// DO NOT ADD OR REMOVE DIRECTLY FROM THIS LIST!!
	// USE emplacePhrase()!!
	std::vector<Phrase> m_phrases;
	// Vector of all guard marks
	//
	// DO NOT ADD OR REMOVE DIRECTLY FROM THIS LIST!!
	// USE emplaceGuard()!!
	std::vector<Guard> m_guards;

	Chart(const bool addTraceline);
	Chart();
	Chart(FILE* inFile);
	Chart(const Chart&) = default;

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
	// Returns the byte size of the chart/subsection
	unsigned long getSize() const;
	// Returns junk C-string (size: 16)
	char* getJunk() { return m_junk; }
	// Copies C-string newJunk to junk
	// Size "count" will be limited to a max of 16
	void setJunk(char* newJunk, rsize_t count = 16);
	// Returns the pivot time for the chart/subsection
	long getPivotTime() const { return m_pivotTime; }
	// Sets the pivot time for the chart/subsection to the provided value
	void setPivotTime(long piv) { m_pivotTime = piv; }
	// Adjusts the pivot time for the chart/subsection by the provided value
	void adjustPivotTime(long difference) { m_pivotTime += difference; }
	// Returns the end time for the chart/subsection
	long getEndTime() const { return m_endTime; }
	// Sets the end time for the chart/subsection to the provided value
	void setEndTime(long piv) { m_endTime = piv; }
	// Returns the number of the trace lines in the chart/subsection
	size_t getNumTracelines() const { return m_tracelines.size(); }
	template<class... Args>
	void emplaceTraceline(Args&&... args)
	{
		GlobalFunctions::emplace_ordered(m_tracelines, args...);
	}

	// Returns the number of the phrase bars in the chart/subsection
	size_t getNumPhrases() const { return m_phrases.size(); }
	template<class... Args>
	void emplacePhrase(Args&&... args)
	{
		GlobalFunctions::emplace_ordered(m_phrases, args...);
	}

	// Returns the number of the guard marks in the chart/subsection
	size_t getNumGuards() const { return m_guards.size(); }
	template<class... Args>
	void emplaceGuard(Args&&... args)
	{
		GlobalFunctions::emplace_ordered(m_guards, args...);
	}

	void add(Note*);
	void add(Traceline*);
	void add(Phrase*);
	void add(Guard*);
	bool removeTraceline(size_t index);
	bool removePhraseBar(size_t index);
	bool removeGuardMark(size_t index);
	void clear();
	// Converts all note positioning from songsection space to chart space
	void finalizeNotes();
};

class Note
{
	friend class CHC;
public:
	// Displacement from chart pivot time in samples
	long m_pivotAlpha;
	Note();
	Note(long alpha);
	Note(FILE* inFile);
	Note(const Note& note) = default;
	virtual Note& operator=(const Note& note);
	virtual void create(FILE* outFile);
	// Adjusts the note's pivot alpha by the provided value
	void adjustPivotAlpha(long change) { m_pivotAlpha += change; }
	bool operator==(const Note& other) const { return m_pivotAlpha == other.m_pivotAlpha; }
	auto operator<=>(const Note& other) const { return m_pivotAlpha <=> other.m_pivotAlpha; }
};

class Path : public Note
{
	friend class CHC;
public:
	// Duration of the note in samples
	unsigned long m_duration;
	Path();
	Path(long alpha, unsigned long dur = 1);
	Path(FILE* inFile);
	Path(const Note& note);
	Note& operator=(const Note& note);
	void create(FILE* outFile);
	// Adjusts the path note's duration to the provided value
	// Returns whether the resulting duration is >= 1
	// If not, duration gets set to 1 and false is returned
	bool adjustDuration(long change);
	// Returns the result of adding the note's pivot alpha with its duration
	long getEndAlpha() const { return m_pivotAlpha + m_duration; }
	// Attempts to set the path note's pivot alpha to the provided value.
	// Will stretch or shrink the note's duration to maintain the same end alpha.
	// If the result would be a duration < 1, then no changes are made and false is returned.
	// Otherwise, return true.
	bool changePivotAlpha(const long alpha);
	// Attempts to set the path note's end alpha to the provided value.
	// Automatically adjusts duration to match.
	// If the result would be a duration < 1, then no changes are made and false is returned.
	// Otherwise, return true.
	bool changeEndAlpha(const long endAlpha);
	// Returns whether the provided alpha value lands inside the path note's range of values
	bool contains(const long alpha) const { return alpha >= m_pivotAlpha && alpha < m_pivotAlpha + (long)m_duration; }
	// Returns whether the pivot alpha of the provided note lands inside the path note's range of values
	bool contains(Note& note) const { return note.m_pivotAlpha >= m_pivotAlpha && note.m_pivotAlpha < m_pivotAlpha + (long)m_duration; }
};

class Traceline : public Path
{
	friend class CHC;
public:
	// Direction that the trace line points (flipped 180 degrees)
	// Held in Radians
	float m_angle;
	// Holds whether the entire trace line is curved
	unsigned long m_curve;
	Traceline();
	Traceline(long alpha, unsigned long dur = 1, float ang = 0, unsigned long cur = false);
	Traceline(FILE* inFile);
	Traceline(const Note& note);
	Note& operator=(const Note& note);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const long pivotTime);
	// Adjusts the radian angle value of the trace line by the provided value
	void adJustAngle(float change) { m_angle += change; }
};

class Phrase : public Path
{
	friend class CHC;
private:
	// Just junk, saved for consistency
	// May hold data for "color" generated from a .chart import
	char m_junk[12] = { 0 };
	// Color value used in Clone Hero & color patch exporting
	// 0 = not used
	unsigned long m_color;
public:
	// Boolean for whether the phrase bar has a start cap
	unsigned long m_start;
	// Boolean for whether the phrase bar has an end cap
	unsigned long m_end;
	// Which character animation to play when hit (if "start" is true)
	unsigned long m_animation;
	Phrase();
	Phrase(long alpha, unsigned long dur = 1, unsigned long start = true, unsigned long end = true, unsigned long anim = 0, unsigned long color = 0);
	Phrase(FILE* inFile);
	Phrase(const Note& note);
	Note& operator=(const Note& note);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const long pivotTime);
	// Returns the color value of the phrase bar
	unsigned long getColor() const { return m_color; }
	// Sets the color value of the phrase bar
	void setColor(unsigned long col) { m_color = col; }
	// Sets the color value of the phrase bar
	void addColor(unsigned long col) { m_color |= col; }
	// Sets the color value of the phrase bar
	void removeColor(unsigned long col) { m_color &= ~col; }
	// Returns junk C-string (size: 12)
	char* getJunk() { return m_junk; }
};

class Guard : public Note
{
	friend class CHC;
public:
	// Direction to appear from
	// 0 - []
	// 1 - X
	// 2 - O
	// 3 - /\.
	unsigned long m_button = 0;
	Guard();
	Guard(long alpha, unsigned long but = 0);
	Guard(FILE* inFile);
	Guard(const Note& note);
	Note& operator=(const Note& note);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const long pivotTime);
};
