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
#include "CHC.h"
#include <thread>

struct TAS_Frame
{
	unsigned char dpad = 255;
	unsigned char button = 255;
	unsigned char leftStickX = 127;
	unsigned char leftStickY = 127;
	unsigned char rightStickX = 127;
	unsigned char rightStickY = 127;
};

struct Point
{
	int32_t position;
	auto operator<=>(const Point& point) const { return position <=> point.position; }
};

struct TracePoint : public Point
{
	bool last;
	float angle;
	bool curve;
};

struct PhrasePoint : public Point
{
	size_t index;
	bool last;
	uint32_t duration;
};

struct GuardPoint : public Point
{
	size_t index;
	unsigned button;
};

struct SectPoint : public Point
{
	enum class VisualType
	{
		Technical,
		Visual,
		Mixed
	} type;
	int32_t sustainLimit = 0;
};

struct PlayerTrack
{
	// Square - 127
	// X/Cross  - 191
	// Circle - 223
	// Triangle - 239
	static const int s_GUARD_ORIENTATIONS[4][4];
	static const int s_PHRASE_ORIENTATIONS[4];

	std::vector<TAS_Frame> m_frames;
	std::vector<TracePoint> m_tracelines;
	std::vector<PhrasePoint> m_phrases;
	std::vector<GuardPoint> m_guards;
	void convertToFrames(const std::string& filename, const size_t initialFrame, const int orientation);
	operator bool() const { return m_tracelines.size() > 0 || m_guards.size() > 0; }
};

struct TAS
{
	std::string m_filename;
	int m_stage = 0;
	int m_difficulty = 0;
	int m_orientation = 0;
	bool m_multi[2] = { false, false };

	int32_t m_position = 0;
	size_t m_initialFrame = 0;
	int m_notes[2] = { 0, 0 };
	PlayerTrack m_players[4];

	TAS() = default;
	TAS(const std::string& filename, const int stage);
	std::thread convertToFrames(const size_t playerIndex);
	virtual void insertFrames(size_t numFrames) = 0;
	virtual void resultScreen() = 0;
	virtual bool print() = 0;
};

class PCSX2 : public TAS
{
	struct
	{
		const char m_version = 2;
		const char m_emulator[50] = { "PCSX2-1.7.X" };
		char m_author[256] = { 0 };
		const char m_game[256] = { "Gitaroo Man (USA).ISO" };
	} m_header;

public:
	PCSX2(const std::string& filename, const int stage);
	void insertFrames(size_t numFrames);
	void resultScreen();
	bool print();
};

class PPSSPPTAS : public TAS
{
public:
	void insertFrames(size_t numFrames) {}
	void resultScreen() {}
	bool print() { return false; }
};
