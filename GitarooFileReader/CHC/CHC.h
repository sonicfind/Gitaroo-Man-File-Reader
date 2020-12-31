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
#include "Global_Functions.h"
constexpr float s_SAMPLES_PER_MIN = 2880000.0f;

struct SSQ
{
	float first = 0;
	float last = 0;
};

class SongSection;

class CHC
{
	friend class CHC_Main;
	friend class CHC_Editor;
	friend class TAS;
	friend class CH_Exporter;
	friend class CH_Importer;
private:
	//Full filename
	std::string m_filename;
	//Short version
	std::string m_shortname;
	//Stage number
	int m_stage;
	//Holds header data from original file
	char m_header[36] = { 0 };

	//String for an IMC's location
	char m_imc[256] = { 0 };

	//Win-Loss Animations
	SSQ m_events[4];

	//Hold data for all 8 audio channels
	struct AudioChannel
	{
		unsigned long volume = 32767;
		unsigned long pan = 16383;
	} m_audio[8];

	//Gameplay speed
	float m_speed;
	//Hold the number of unorganized sections
	unsigned m_unorganized;
	//Saves whether all notes were readjusted for minimal glitching
	bool m_optimized;
	//Holds all life-value data for every player and phase type
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
	//0 - Not saved
	//1 - Saved
	//2 - Saved at the currently pointed location
	char m_saved;
public:
	//Vector of all sections
	std::vector<SongSection> m_sections;
	CHC();
	CHC(std::string filename);
	CHC(const CHC&) = default;
	void create(std::string filename);
	size_t getNumSections() { return m_sections.size(); }
};

class Chart;

class SongSection
{
	friend class CHC_Main;
	friend class CHC_Editor;
	friend class CHC;
public:
	enum class Phase { INTRO, CHARGE, BATTLE, FINAL_AG, HARMONY, END, FINAL_I };
private:
	//Index pulled from the list of SSQs in the CHC
	unsigned long m_index = 0;
	//Section name
	//16 characters + a null character
	char m_name[17] = { 0 };

	//Name of Audio Section
	//16 characters + a null character
	char m_audio[17] = { 0 };

	//SSQ frame range
	SSQ m_frames;
	//Saves whether the subsections are correctly ordered
	unsigned long m_organized = false;
	//Value that holds how the section is swapped
	unsigned long m_swapped = 0;
	//Total size in bytes
	unsigned long m_size = 384;
	//Just junk, saved for consistency
	char m_junk[16] = { 0 };

	//Phase type
	Phase m_battlePhase = Phase::INTRO;
	//BPM for the section
	float m_tempo = 0;
	//Total duration in samples
	unsigned long m_duration = 0;
	//Determines what actions to take after the section
	struct Condition
	{
		unsigned long m_type;
		float m_argument;
		long m_trueEffect;
		long m_falseEffect;
		Condition();
		Condition(FILE* inFile);
		Condition(const Condition& cond) = default;
	};
	//LinkedList::List of all conditions
	std::vector<Condition> m_conditions;
	//Number of players assigned to the section, always 4
	unsigned long m_numPlayers = 4;
	//Number of charts/subsections per player
	unsigned long m_numCharts = 1;
	//LinkedList::List of all charts/subsections
public:
	std::vector<Chart> m_charts;
	static const long s_SAMPLE_GAP = 1800;
	SongSection();
	SongSection(FILE* inFile);
	SongSection(const SongSection&) = default;
	//Returns name C-string (size: 16)
	char* getName() { return m_name; }
	//Returns audio C-string (size: 16)
	char* getAudio() { return m_audio; }
	//Returns whether the section is organized
	bool getOrganized() const { return m_organized; }
	//Sets organized to the provided value
	void setOrganized(bool org) { m_organized = org; }
	//Returns swap value
	unsigned long getSwapped() const { return m_swapped; }
	//Sets swap value
	void setSwapped(char swap) { m_swapped = swap; }
	//Returns the byte size of the section
	unsigned long getSize() const { return m_size; }
	//Returns a section's phase type
	Phase getPhase() const { return m_battlePhase; }
	//Sets the section's phase type to the provided value
	void setPhase(Phase ph) { m_battlePhase = ph; }
	//Sets the section's phase type to the provided value converted into a phase type
	//If the provided value is 6 or greater, battlePhase is set to FINAL_I
	void setPhase(unsigned long ph)
	{
		if (ph < 6)
			m_battlePhase = static_cast<SongSection::Phase>(ph);
		else
			m_battlePhase = Phase::FINAL_I;
	}
	//Returns the tempo for the section
	float getTempo() const { return m_tempo; }
	//Returns the duration of the section
	unsigned long getDuration() const { return m_duration; }
	//Sets the duration of the section to the provided value
	void setDuration(unsigned long dur) { m_duration = dur; }
	//Returns the number of conditions in the section
	size_t getNumCondtions() const { return m_conditions.size(); }
	//Adds a new condition to the end of the section's condition list
	template<class...Args>
	size_t addCondition(size_t index, Args&&...args)
	{
		if (index > m_conditions.size())
			index = m_conditions.size();
		m_size += 16;
		m_conditions.emplace(index, args...);
		return index;
	}
	Condition& getCondition(size_t index);
	bool removeCondition(size_t);
	//Returns the num of players assigned to this section.
	//Will usually be 4
	unsigned long getNumPlayers() const { return m_numPlayers; }
	//Returns the total number of the charts/subsections in this section
	unsigned long getNumCharts() const { return m_numCharts; }
	void clearConditions();
	void operator++();
	bool operator--();
};

class Note;
class Traceline;
class Phrase;
class Guard;

class Chart
{
	friend class CHC_Editor;
	friend class CHC;
	friend class CH_Importer;
private:
	//Total size in bytes
	unsigned long m_size = 76;
	//Just junk, saved for consistency
	char m_junk[16] = { 0 };

	//'Center' point that all notes revolve around
	long m_pivotTime = 0;
	//Optional value noting when to transition to another chart/subsection
	//Unused in the game
	long m_endTime = 0;
public:
	//Linked list of all trace lines
	std::vector<Traceline> m_tracelines;
	//Linked list of all phrase bars
	std::vector<Phrase> m_phrases;
	//Linked list of all guard marks
	std::vector<Guard> m_guards;

	Chart(const bool tracelines);
	Chart();
	Chart(const Chart&) = default;
	//Returns the byte size of the chart/subsection
	unsigned long getSize() const { return m_size; }
	//Sets the byte size of the chart/subsection to the provided value
	void setSize(unsigned long siz) { m_size = siz; }
	//Adjusts the byte size of the chart/subsection by the provided value
	void adjustSize(long difference);
	//Returns junk C-string (size: 16)
	char* getJunk() { return m_junk; }
	//Copies C-string newJunk to junk
	//Size "count" will be limited to a max of 16
	void setJunk(char* newJunk, rsize_t count = 16);
	//Returns the pivot time for the chart/subsection
	long getPivotTime() const { return m_pivotTime; }
	//Sets the pivot time for the chart/subsection to the provided value
	void setPivotTime(long piv) { m_pivotTime = piv; }
	//Adjusts the pivot time for the chart/subsection by the provided value
	void adjustPivotTime(long difference) { m_pivotTime += difference; }
	//Returns the end time for the chart/subsection
	long getEndTime() const { return m_endTime; }
	//Sets the end time for the chart/subsection to the provided value
	void setEndTime(long piv) { m_endTime = piv; }
	//Returns the number of the trace lines in the chart/subsection
	size_t getNumTracelines() const { return m_tracelines.size(); }
	template<class... Args>
	size_t emplaceTraceline(Args&&... args)
	{
		m_size += 16;
		return GlobalFunctions::emplace_ordered(m_tracelines, args...);
	}

	//Returns the number of the phrase bars in the chart/subsection
	size_t getNumPhrases() const { return m_phrases.size(); }
	template<class... Args>
	size_t emplacePhrase(Args&&... args)
	{
		m_size += 32;
		return GlobalFunctions::emplace_ordered(m_phrases, args...);
	}

	//Returns the number of the guard marks in the chart/subsection
	size_t getNumGuards() const { return m_guards.size(); }
	template<class... Args>
	size_t emplaceGuard(Args&&... args)
	{
		m_size += 16;
		return GlobalFunctions::emplace_ordered(m_guards, args...);
	}

	size_t add(Note*);
	bool resize(long numElements, char type = 't');
	bool removeTraceline(size_t index);
	bool removePhraseBar(size_t index);
	bool removeGuardMark(size_t index);
	void clearTracelines();
	void clearPhrases();
	void clearGuards();
	void clear();
	//Takes the notes from the source chart and moves them into
	//the current chart (replacing any old notes that would overlap
	long insertNotes(Chart* source);
};

class Note
{
	friend class CHC_Editor;
	friend class CHC;
protected:
	//Displacement from chart pivot time in samples
	long m_pivotAlpha;
public:
	Note() { m_pivotAlpha = 0; }
	Note(long alpha) { m_pivotAlpha = alpha; }
	Note(const Note& note) : m_pivotAlpha(note.m_pivotAlpha) {}
	virtual Note& operator=(const Note& note);
	virtual ~Note() {};
	//Returns the note's pivot alpha
	long getPivotAlpha() const { return m_pivotAlpha; }
	//Sets the note's pivot alpha to the provided value
	void setPivotAlpha(long alpha) { m_pivotAlpha = alpha; }
	//Adjusts the note's pivot alpha by the provided value
	void adjustPivotAlpha(long change) { m_pivotAlpha += change; }
	//Returns whether pivot alphas are equal
	bool operator==(const Note& note) const { return m_pivotAlpha == note.m_pivotAlpha; }
	//Returns whether pivot alphas are not equal
	bool operator!=(const Note& note) const { return m_pivotAlpha != note.m_pivotAlpha; }
	//Returns whether pivot alpha is >= the pivot alpha of the provided note
	bool operator>=(const Note& note) const { return m_pivotAlpha >= note.m_pivotAlpha; }
	//Returns whether pivot alpha is > the pivot alpha of the provided note
	bool operator>(const Note& note) const { return m_pivotAlpha > note.m_pivotAlpha; }
	//Returns whether pivot alpha is <= the pivot alpha of the provided note
	bool operator<=(const Note& note) const { return m_pivotAlpha <= note.m_pivotAlpha; }
	//Returns whether pivot alpha is < the pivot alpha of the provided note
	bool operator<(const Note& note) const { return m_pivotAlpha < note.m_pivotAlpha; }
};

class Path : public Note
{
	friend class CHC_Editor;
	friend class CHC;
protected:
	//Duration of the note in samples
	unsigned long m_duration;
public:
	Path() : Note(), m_duration(1) {}
	Path(long alpha, unsigned long dur = 1) : Note(alpha), m_duration(dur) {}
	Path(const Note& note);
	virtual Note& operator=(const Note& note);
	virtual ~Path() {};
	//Returns the path note's duration
	unsigned long getDuration() const { return m_duration; }
	//Sets the path note's duration to the provided value
	void setDuration(unsigned long dur) { m_duration = dur; }
	//Adjusts the path note's duration to the provided value
	//Returns whether the resulting duration is >= 1
	//If not, duration gets set to 1 and false is returned
	bool adjustDuration(long change);
	//Returns the result of adding the note's pivot alpha with its duration
	long getEndAlpha() const { return m_pivotAlpha + m_duration; }
	//Attempts to set the path note's pivot alpha to the provided value.
	//Will stretch or shrink the note's duration to maintain the same end alpha.
	//If the result would be a duration < 1, then no changes are made and false is returned.
	//Otherwise, return true.
	bool changePivotAlpha(const long alpha);
	//Attempts to set the path note's end alpha to the provided value.
	//Automatically adjusts duration to match.
	//If the result would be a duration < 1, then no changes are made and false is returned.
	//Otherwise, return true.
	bool changeEndAlpha(const long endAlpha);
	//Returns whether the provided alpha value lands inside the path note's range of values
	bool contains(const long alpha) const { return alpha >= m_pivotAlpha && alpha < m_pivotAlpha + (long)m_duration; }
	//Returns whether the pivot alpha of the provided note lands inside the path note's range of values
	bool contains(Note& note) const { return note.getPivotAlpha() >= m_pivotAlpha && note.getPivotAlpha() < m_pivotAlpha + (long)m_duration; }
};

class Traceline : public Path
{
	friend class CHC_Editor;
	friend class CHC;
private:
	//Direction that the trace line points (flipped 180 degrees)
	//Held in Radians
	float m_angle;
	//Holds whether the entire trace line is curved
	unsigned long m_curve;
public:
	Traceline() : Path(), m_angle(0), m_curve(false) {}
	Traceline(FILE* inFile);
	Traceline(long alpha, unsigned long dur = 1, float ang = 0, bool cur = false) : Path(alpha, dur), m_angle(ang), m_curve(cur) {}
	Traceline(const Note& note);
	Note& operator=(const Note& note);
	~Traceline() {};
	//Returns the radian angle value of the trace line
	float getAngle() const { return m_angle; }
	//Sets the radian angle value of the trace line to the provided value
	void setAngle(float ang) { m_angle = ang; }
	//Adjusts the radian angle value of the trace line by the provided value
	void adJustAngle(float change) { m_angle += change; }
	//Returns whether the trace line is set to curve
	bool getCurve() const { return m_curve; }
	//Sets the trace line's curve value to the provided value
	void setCurve(bool cur) { m_curve = cur; }
};

class Phrase : public Path
{
	friend class CHC_Editor;
	friend class CHC;
private:
	//Boolean for whether the phrase bar has a start cap
	unsigned long m_start;
	//Boolean for whether the phrase bar has an end cap
	unsigned long m_end;
	//Which character animation to play when hit (if "start" is true)
	unsigned long m_animation;
	//Just junk, saved for consistency
	//May hold data for "color" on file read
	char m_junk[12] = { 0 };
	//Color value used Clone Hero & color patch exporting
	//-1 = not used
	unsigned long m_color;
public:
	Phrase() : Path(), m_start(true), m_end(true), m_animation(0), m_color(0) {}
	Phrase(FILE* inFile);
	Phrase(long alpha, unsigned long dur = 1, bool st = true, bool ed = true, unsigned long anim = 0, unsigned long clr = 0)
		: Path(alpha, dur), m_start(st), m_end(ed), m_animation(anim), m_color(clr) {}
	Phrase(const Note& note);
	Note& operator=(const Note& note);
	~Phrase() {};
	//Returns whether the phrase bar has a start cap
	bool getStart() const { return m_start; }
	//Sets whether the phrase bar has a start cap
	void setStart(bool newStart) { m_start = newStart; }
	//Returns whether the phrase bar has an end cap
	bool getEnd() const { return m_end; }
	//Sets whether the phrase bar has an end cap
	void setEnd(bool newEnd) { m_end = newEnd; }
	//Returns the animation type of the phrase bar
	unsigned long getAnimation() const { return m_animation; }
	//Sets the animation type of the phrase bar to the provided value
	void setAnimation(unsigned long anim) { m_animation = anim; }
	//Returns the color value of the phrase bar
	unsigned long getColor() const { return m_color; }
	//Sets the color value of the phrase bar
	void setColor(unsigned long col) { m_color = col; }
	//Sets the color value of the phrase bar
	void addColor(unsigned long col) { m_color |= col; }
	//Sets the color value of the phrase bar
	void removeColor(unsigned long col) { m_color &= ~col; }
	//Returns junk C-string (size: 12)
	char* getJunk() { return m_junk; }
};

class Guard : public Note
{
	friend class CHC_Editor;
	friend class CHC;
private:
	//Direction to appear from
	//0 - []
	//1 - X
	//2 - O
	//3 - /\.
	unsigned long m_button = 0;
public:
	Guard() : Note(), m_button(0) {}
	Guard(FILE* inFile);
	Guard(long alpha, unsigned long but = 0) : Note(alpha), m_button(but)
	{
		if (m_button > 3)
			throw "Error: Invalid button choice for Guard Mark";
	}
	~Guard() {};
	Guard(const Note& note);
	Note& operator=(const Note& note);
	//Returns the button value of the guard mark
	unsigned long getButton() const { return m_button; }
	//Sets the button value of the guard mark to the provided value
	void setButton(unsigned long butt) { m_button = butt; }
};