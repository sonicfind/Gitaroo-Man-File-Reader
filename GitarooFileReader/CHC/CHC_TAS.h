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
#include "CHC.h"

class TAS
{
	static struct FrameFile
	{
		std::string name = "VALUES.P2M2V";
		bool use = false;
		long samples[2][13][6] =
		{ { { 0, 0, 0, 0, 0, 0 }, { 44, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 } },
		  { { 0, 0, 0, 0, 0, 0 }, { 44, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 } } };
		long frames[2][13][6] =
		{ { {373, 399, 0, 0, 0, 0}, {646, 639, 646, 720, 735, 739},
			{553, 553, 547, 680, 693, 696}, {712, 705, 711, 755, 767, 772},
			{304, 298, 304, 0, 0, 0}, {741, 733, 739, 871, 885, 887},
			{419, 414, 420, 0, 0, 0}, {695, 673, 682, 818, 827, 833},
			{622, 615, 622, 735, 746, 749}, {595, 589, 595, 0, 0, 0},
			{656, 650, 657, 523, 0, 0}, {256, 580, 574, 0, 0, 0},
			{241, 280, 406, 0, 0, 0} },
		  { {373, 399, 0, 0, 0, 0}, {646, 639, 646, 720, 735, 739},
			{553, 553, 547, 680, 693, 696}, {712, 705, 711, 755, 767, 772},
			{304, 298, 304, 0, 0, 0}, {741, 733, 739, 871, 885, 887},
			{419, 414, 420, 0, 0, 0}, {695, 673, 682, 818, 827, 833},
			{622, 615, 622, 735, 746, 749}, {595, 589, 595, 0, 0, 0},
			{656, 650, 657, 523, 0, 0}, {256, 580, 574, 0, 0, 0},
			{241, 280, 406, 0, 0, 0} } };
	} frameValues;

	struct TAS_Frame
	{
		unsigned char dpad = 255;
		unsigned char button = 255;
		unsigned char leftStickX = 127;
		unsigned char leftStickY = 127;
		unsigned char rightStickX = 127;
		unsigned char rightStickY = 127;
	};

	struct PCSX2TAS
	{
		friend class TAS;
	private:
		char m_version = 2;
		char m_emulator[50] = { "PCSX2-1.7.X" };
		char m_author[256] = { 0 };
		char m_game[256] = { "Gitaroo Man (USA).ISO" };
		float m_framerate = 59.94f;
		std::vector<TAS_Frame> m_players[4];
	public:
		size_t insertFrames(const int stage, const int orientation, const int difficulty, const bool(&multi)[2], size_t numFrames);
		void resultScreen(const int stage, const int notes, const bool singleplayer, const bool(&multi)[2]);
		void print(std::string filename);
	} m_pcsx2;

	struct PPSSPPTAS
	{
		friend class TAS;
		std::vector<TAS_Frame> players[4];
	} m_ppsspp;

	struct NotePoint
	{
		long position;
		Note* note;
		size_t index;
		bool last;
		NotePoint(long pos, Note* note = nullptr, size_t index = 0, bool last = false) : position(pos), note(note), index(index), last(last) {}
	};
	struct SectPoint
	{
		long position;
		enum class VisualType
		{
			Technical,
			Visual,
			Mixed
		} type;
		long sustainLimit = 0;
		SectPoint(long pos, size_t visuals = 0, long sus = 0) : position(pos), type(static_cast<VisualType>(visuals)), sustainLimit(sus) {}
	};
	std::vector<NotePoint> timeline[4];
	std::vector<SectPoint> markers;
	CHC* m_tutorialStageB = nullptr;
public:
	bool build(CHC song);
	bool loadValues(std::string file = frameValues.name);
	~TAS()
	{
		if (m_tutorialStageB)
			delete m_tutorialStageB;
	}
};
