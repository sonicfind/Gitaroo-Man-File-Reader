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
	void adJustAngle(long double change) { m_angle = float(m_angle + change); }
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
