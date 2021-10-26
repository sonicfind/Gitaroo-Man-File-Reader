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
		uint32_t volume = 32767;
		uint32_t pan = 16383;
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
	bool create(std::string filename, bool trueSave = true);

	bool write_to_txt();
	bool applyChanges(const bool fix, const bool swap = false, const bool save = false);
	bool edit(const bool multi = false);
	bool buildTAS();
	bool exportForCloneHero();
	bool importFromCloneHero(bool doSave = false);
	bool colorCheatTemplate();
	size_t getNumSections() const { return m_sections.size(); }
	bool isPS2Compatible() const { return m_imc[0] > 0; }
	bool isOrganized() const { return m_unorganized == 0; }
	bool isOptimized() const { return m_optimized; }

	bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension);
	bool functionSelection(const char choice, bool isMulti);
	static void displayMultiChoices();
	static void displayMultiHelp();
	static const std::string multiChoiceString;

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
	uint32_t m_index = 0;
	// Section name
	// 16 characters + a null character
	char m_name[17] = { 0 };

	//Name of Audio Section
	// 16 characters + a null character
	char m_audio[17] = { 0 };

	//SSQ frame range
	SSQRange m_frames;
	// Saves whether the subsections are correctly ordered
	uint32_t m_organized = false;
	// Value that holds how the section is swapped
	uint32_t m_swapped = 0;
	// Just junk, saved for consistency
	char m_junk[16] = { 0 };

	// Phase type
	Phase m_battlePhase = Phase::INTRO;
	// BPM for the section
	float m_tempo = 0;
	// Total duration in samples
	uint32_t m_duration = 0;
	// Determines what actions to take after the section
	struct Condition
	{
		uint32_t m_type;
		float m_argument;
		int32_t m_trueEffect;
		int32_t m_falseEffect;
		Condition();
		Condition(FILE* inFile);
		Condition(const Condition& cond) = default;
		void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const CHC* const chc);
	};
	// Vector of all conditions
	std::vector<Condition> m_conditions;
	// Number of players assigned to the section, always 4
	uint32_t m_numPlayers = 4;
	// Number of charts/subsections per player
	uint32_t m_numCharts = 1;
	// Parent CHC
	CHC* m_parent = nullptr;
public:
	// Vector of all charts/subsections
	std::vector<Chart> m_charts;
	static const int32_t s_SAMPLE_GAP = 1800;
	SongSection(CHC* parent);
	SongSection();
	SongSection(CHC* parent, FILE* inFile);
	SongSection(const SongSection&) = default;
	SongSection& operator=(const SongSection& section);
	// Fills in rest of values from the given file
	// Returns if the section is organized
	void continueRead(FILE* inFile, const size_t index, const int stage, bool isDuet);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
	uint32_t getIndex() const { return m_index; }
	// Returns name C-string (size: 16)
	char* getName() { return m_name; }
	// Returns audio C-string (size: 16)
	char* getAudio() { return m_audio; }
	// Returns whether the section is organized
	bool getOrganized() const { return m_organized; }
	// Sets organized to the provided value
	void setOrganized(uint32_t org);
	// Returns swap value
	uint32_t getSwapped() const { return m_swapped; }
	// Sets swap value
	void setSwapped(char swap) { m_swapped = swap; }
	// Returns the byte size of the section
	uint32_t getSize() const;
	// Returns a section's phase type
	Phase getPhase() const { return m_battlePhase; }
	// Sets the section's phase type to the provided value
	void setPhase(Phase ph) { m_battlePhase = ph; }
	// Sets the section's phase type to the provided value converted into a phase type
	// If the provided value is 6 or greater, battlePhase is set to FINAL_I
	void setPhase(uint32_t ph)
	{
		if (ph < 6)
			m_battlePhase = static_cast<SongSection::Phase>(ph);
		else
			m_battlePhase = Phase::FINAL_I;
	}
	// Returns the tempo for the section
	float getTempo() const { return m_tempo; }
	// Returns the duration of the section
	uint32_t getDuration() const { return m_duration; }
	// Sets the duration of the section to the provided value
	void setDuration(uint32_t dur) { m_duration = dur; }
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
	uint32_t getNumPlayers() const { return m_numPlayers; }
	// Returns the total number of the charts/subsections in this section
	uint32_t getNumCharts() const { return m_numCharts; }
	void clearConditions();
	void operator++();
	bool operator--();

private:
	// Editor-based functions

	bool reorganize(const bool isPs2, const int stage);
	void playerSwap(const bool isPs2, const bool allSections = false);

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
	int32_t m_pivotTime = 0;
	// Optional value noting when to transition to another chart/subsection
	// Unused in the game
	int32_t m_endTime = 0;
public:
	// Vector of all trace lines
	std::vector<Traceline> m_tracelines;
	// Vector of all phrase bars
	std::vector<Phrase> m_phrases;
	// Vector of all guard marks
	std::vector<Guard> m_guards;

	Chart(const bool addTraceline);
	Chart();
	Chart(FILE* inFile);
	Chart(const Chart&) = default;

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
	// Returns the byte size of the chart/subsection
	uint32_t getSize() const;
	// Returns junk C-string (size: 16)
	char* getJunk() { return m_junk; }
	// Copies C-string newJunk to junk
	// Size "count" will be limited to a max of 16
	void setJunk(char* newJunk, rsize_t count = 16);
	// Returns the pivot time for the chart/subsection
	int32_t getPivotTime() const { return m_pivotTime; }
	// Sets the pivot time for the chart/subsection to the provided value
	void setPivotTime(int32_t piv) { m_pivotTime = piv; }
	// Adjusts the pivot time for the chart/subsection by the provided value
	void adjustPivotTime(int32_t difference) { m_pivotTime += difference; }
	// Returns the end time for the chart/subsection
	int32_t getEndTime() const { return m_endTime; }
	// Sets the end time for the chart/subsection to the provided value
	void setEndTime(int32_t piv) { m_endTime = piv; }
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
	int32_t m_pivotAlpha;
	Note();
	Note(int32_t alpha);
	Note(FILE* inFile);
	Note(const Note& note) = default;
	virtual Note& operator=(const Note& note);
	virtual void create(FILE* outFile);
	// Adjusts the note's pivot alpha by the provided value
	void adjustPivotAlpha(int32_t change) { m_pivotAlpha += change; }
	bool operator==(const Note& other) const { return m_pivotAlpha == other.m_pivotAlpha; }
	auto operator<=>(const Note& other) const { return m_pivotAlpha <=> other.m_pivotAlpha; }
};

class Path : public Note
{
	friend class CHC;
public:
	// Duration of the note in samples
	uint32_t m_duration;
	Path();
	Path(int32_t alpha, uint32_t dur = 1);
	Path(FILE* inFile);
	Path(const Note& note);
	Note& operator=(const Note& note);
	void create(FILE* outFile);
	// Adjusts the path note's duration to the provided value
	// Returns whether the resulting duration is >= 1
	// If not, duration gets set to 1 and false is returned
	bool adjustDuration(int32_t change);
	// Returns the result of adding the note's pivot alpha with its duration
	int32_t getEndAlpha() const { return m_pivotAlpha + m_duration; }
	// Attempts to set the path note's pivot alpha to the provided value.
	// Will stretch or shrink the note's duration to maintain the same end alpha.
	// If the result would be a duration < 1, then no changes are made and false is returned.
	// Otherwise, return true.
	bool changePivotAlpha(const int32_t alpha);
	// Attempts to set the path note's end alpha to the provided value.
	// Automatically adjusts duration to match.
	// If the result would be a duration < 1, then no changes are made and false is returned.
	// Otherwise, return true.
	bool changeEndAlpha(const int32_t endAlpha);
	// Returns whether the provided alpha value lands inside the path note's range of values
	bool contains(const int32_t alpha) const { return alpha >= m_pivotAlpha && alpha < m_pivotAlpha + (int32_t)m_duration; }
	// Returns whether the pivot alpha of the provided note lands inside the path note's range of values
	bool contains(Note& note) const { return note.m_pivotAlpha >= m_pivotAlpha && note.m_pivotAlpha < m_pivotAlpha + (int32_t)m_duration; }
};

class Traceline : public Path
{
	friend class CHC;
public:
	// Direction that the trace line points (flipped 180 degrees)
	// Held in Radians
	float m_angle;
	// Holds whether the entire trace line is curved
	uint32_t m_curve;
	Traceline();
	Traceline(int32_t alpha, uint32_t dur = 1, float ang = 0, uint32_t cur = false);
	Traceline(FILE* inFile);
	Traceline(const Note& note);
	Note& operator=(const Note& note);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const int32_t pivotTime);
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
	uint32_t m_color;
public:
	// Boolean for whether the phrase bar has a start cap
	uint32_t m_start;
	// Boolean for whether the phrase bar has an end cap
	uint32_t m_end;
	// Which character animation to play when hit (if "start" is true)
	uint32_t m_animation;
	Phrase();
	Phrase(int32_t alpha, uint32_t dur = 1, uint32_t start = true, uint32_t end = true, uint32_t anim = 0, uint32_t color = 0);
	Phrase(FILE* inFile);
	Phrase(const Note& note);
	Note& operator=(const Note& note);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const int32_t pivotTime);
	// Returns the color value of the phrase bar
	uint32_t getColor() const { return m_color; }
	// Sets the color value of the phrase bar
	void setColor(uint32_t col) { m_color = col; }
	// Sets the color value of the phrase bar
	void addColor(uint32_t col) { m_color |= col; }
	// Sets the color value of the phrase bar
	void removeColor(uint32_t col) { m_color &= ~col; }
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
	uint32_t m_button = 0;
	Guard();
	Guard(int32_t alpha, uint32_t but = 0);
	Guard(FILE* inFile);
	Guard(const Note& note);
	Note& operator=(const Note& note);

	void create(FILE* outFile);
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const int32_t pivotTime);
};
