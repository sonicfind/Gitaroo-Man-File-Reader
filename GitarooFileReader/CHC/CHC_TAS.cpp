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
#include "Global_Functions.h"
#include "CHC_TAS.h"
#include <fstream>

TAS::FrameFile TAS::frameValues;
using namespace std;

bool TAS::loadValues(string filename)
{
	size_t pos = filename.find_last_of("\\");
#pragma warning(suppress : 4996)
	std::ifstream p2m2v(filename.c_str());
	if (p2m2v.is_open())
	{
		frameValues.name = filename;
		bool error = false, failed = true;
		for (int framerate = 0; framerate < 2; framerate++)
		{
			if (!p2m2v.eof())
			{
				p2m2v.ignore(500, ';');
				while (!p2m2v.eof())
				{
					char line[128];
					p2m2v.getline(line, 128);
					if (strchr(line, ';'))
						break;
					std::strstream str;
					str << line;
					unsigned int stage;
					str >> stage;
					if (stage <= 12)
					{
						str.ignore(1);
						for (int difficulty = 0; !str.eof() && difficulty < 6; difficulty++)
						{
							switch (GlobalFunctions::insertFromStream(str, frameValues.samples[framerate][stage][difficulty], true))
							{
							case GlobalFunctions::ResultType::Success:
								failed = false;
								break;
							case GlobalFunctions::ResultType::Failed:
								printf("%sError: Invalid input (%s)\n", g_global.tabs.c_str(), g_global.invalid.c_str());
								printf("%sAt %s", g_global.tabs.c_str(), framerate == 0 ? "59.94 " : "50.00 ");
								printf("FPS [Sample Offsets] - Stage %u", stage);
								switch (difficulty)
								{
								case 0: printf(" (Hard) [1]\n%s\n", g_global.tabs.c_str()); break;
								case 1: printf(" (Normal) [2]\n%s\n", g_global.tabs.c_str()); break;
								case 2: printf(" (Easy) [3]\n%s\n", g_global.tabs.c_str()); break;
								default: printf(" (Multiplayer) [%u]\n%s\n", difficulty + 1, g_global.tabs.c_str());
								}
								printf("%sRead ended early\n", g_global.tabs.c_str());
							case GlobalFunctions::ResultType::Quit:
								goto Finish_Read;
							}
						}

						if (!str.eof() && strchr(line, 'q') || strchr(line, 'Q'))
							goto Finish_Read;
					}
				}

				if (p2m2v.eof())
				{
					printf("%sRead stopped before %s FPS's frame offsets\n", g_global.tabs.c_str(), framerate == 0 ? "59.94 " : "50.00 ");
					goto Finish_Read;
				}

				while (!p2m2v.eof())
				{
					char line[128];
					p2m2v.getline(line, 128);
					if (strchr(line, ';'))
						break;
					std::strstream str;
					str << line;
					unsigned stage;
					str >> stage;
					if (stage <= 12)
					{
						str.ignore(1);
						for (int difficulty = 0; !str.eof() && difficulty < 6; difficulty++)
						{
							switch (GlobalFunctions::insertFromStream(str, frameValues.frames[framerate][stage][difficulty], true))
							{
							case GlobalFunctions::ResultType::Success:
								failed = false;
								break;
							case GlobalFunctions::ResultType::Failed:
								printf("%sError: Invalid input (%s)\n", g_global.tabs.c_str(), g_global.invalid.c_str());
								printf("%sAt %s", g_global.tabs.c_str(), framerate == 0 ? "59.94 " : "50.00 ");
								printf("FPS [Frame Offsets] - Stage %u", stage);
								switch (difficulty)
								{
								case 0: printf(" (Hard) [1]\n%s\n", g_global.tabs.c_str()); break;
								case 1: printf(" (Normal) [2]\n%s\n", g_global.tabs.c_str()); break;
								case 2: printf(" (Easy) [3]\n%s\n", g_global.tabs.c_str()); break;
								default: printf(" (Multiplayer) [%u]\n%s\n", difficulty + 1, g_global.tabs.c_str());
								}
								printf("%sRead ended early\n", g_global.tabs.c_str());
							case GlobalFunctions::ResultType::Quit:
								goto Finish_Read;
							}
						}

						if (!str.eof() && strchr(line, 'q') || strchr(line, 'Q'))
							goto Finish_Read;
					}
				}
			}
			else
			{
				printf("%sRead stopped before FPS %s\n", g_global.tabs.c_str(), framerate == 0 ? "59.94 " : "50.00 ");
				break;
			}
		}

	Finish_Read:
		p2m2v.close();
		if (!failed)
		{
			printf("%s%s loaded%s\n", g_global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str(), error ? " with error(s)" : "");
			frameValues.use = true;
			return true;
		}
		else
		{
			printf("%s%s failed to load\n", g_global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
			return false;
		}
	}
	else
	{
		printf("%s%s could not be located\n", g_global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
		return false;
	}
}

void TAS::PCSX2TAS::resultScreen(const int stage, const int notes, const bool singleplayer, const bool(&multi)[2])
{
	size_t toResult = 0;
	switch (stage)
	{
	case 'q': 
		toResult = 65;
		break;
	case '!':
		toResult = 611;
		break;
	case 2:
		singleplayer ? toResult = 790 : toResult = 500;
		break;
	case 3:
		toResult = 790;
		break;
	case 4:
		toResult = 790;
		break;
	case 5:
		singleplayer ? toResult = 790 : toResult = 500;
		break;
	case 6:
		toResult = 340;
		break;
	case 7:
		singleplayer ? toResult = 1130 : toResult = 500;
		break;
	case 8:
		toResult = 175;
		break;
	case 9:
		toResult = 1125;
		break;
	case 10:
		toResult = 790;
		break;
	case 11:
		toResult = 790;
		break;
	case 12:
		toResult = 790;
	}
	if (stage != 0)
		m_players[0].resize(m_players[0].size() + toResult + notes + 306);
	else
		m_players[0].resize(m_players[0].size() + toResult);
	if (!singleplayer)
	{
		m_players[1].resize(m_players[0].size());
		if (multi[0]) m_players[2].resize(m_players[0].size());
		if (multi[1]) m_players[3].resize(m_players[0].size());
	}
}

void TAS::PCSX2TAS::print(string filename)
{
#pragma warning(suppress : 4996) 
	FILE* outp2m2 = fopen((filename + ".p2m2").c_str(), "wb");
	char multi[8] = { true, 0, 0, 0, m_players[1].size() > 0, m_players[2].size() > 0, m_players[3].size() > 0, 0 };
	fputc(m_version, outp2m2);
	fwrite(m_emulator, 1, 50, outp2m2);
	fwrite(m_author, 1, 255, outp2m2);
	fwrite(m_game, 1, 255, outp2m2);
	fwrite("\0", 1, 1, outp2m2);
	fwrite(multi, 1, 8, outp2m2);
	const size_t size = m_players[0].size();
	fwrite(&size, 4, 1, outp2m2);
	fwrite("\0\0\0\0", 1, 4, outp2m2);
	unsigned char sequence[18] = { 0 };
	for (size_t index = 0; index < m_players[0].size(); index++)
	{
		for (long player = 0; player < 4; player++)
		{
			if (m_players[player].size())
			{
				TAS_Frame& frame = m_players[player][index];
				sequence[0] = frame.dpad;
				sequence[1] = frame.button;
				sequence[2] = frame.rightStickX;
				sequence[3] = frame.rightStickY;
				sequence[4] = frame.leftStickX;
				sequence[5] = frame.leftStickY;
				frame.dpad = ~frame.dpad;
				if (frame.dpad & 128) sequence[7] = 0xFF;	// Left
				if (frame.dpad & 64) sequence[9] = 0xFF;	// Down
				if (frame.dpad & 32) sequence[6] = 0xFF;	// Right
				if (frame.dpad & 16) sequence[8] = 0xFF;	// Up
				frame.button = ~frame.button;
				if (frame.button & 128) sequence[13] = 0xFF;
				if (frame.button & 64) sequence[12] = 0xFF;	// Cross
				if (frame.button & 32) sequence[11] = 0xFF;
				if (frame.button & 16) sequence[10] = 0xFF;	// Triangle
				fwrite(sequence, 1, 18, outp2m2);
				memset(sequence, 0, 14);
			}
		}
		fflush(outp2m2);
	}
	fclose(outp2m2);
	printf("%sPCSX2 TAS Completed.\n", g_global.tabs.c_str());
}

size_t TAS::PCSX2TAS::insertFrames(const int stage, const int orientation, const int difficulty, const bool(&multi)[2], size_t numFrames)
{
	if (m_framerate >= 60.00f)
	{
		if (!stage)
		{
			m_players[0].resize(size_t(546) + frameValues.frames[2][0][0] + numFrames);
			m_players[0][253].dpad &= 247;	// Start - Title Screen
			m_players[0][545].button &= 191;	// X-button - Start Tutorial
		}
		else if (stage <= 10)
		{
			m_players[0].resize(1390 + size_t(6) * multi[0] + frameValues.frames[2][stage][difficulty + multi[0] + multi[1]] + numFrames);
			size_t index = 253 + size_t(2) * multi[0];
			m_players[0][index++].dpad &= 247;		index += size_t(283) + multi[0];	// Start - Title Screen
			m_players[0][index++].dpad &= 239;											// D-pad Up - To Settings option
			m_players[0][index++].button &= 191;	index += 207;						// X-button - Select Settings option
			m_players[0][index++].dpad &= 223;		index++;							// D-pad Right - To Difficulty option
			m_players[0][index++].dpad &= 223;											// D-pad Right - To Load Game option
			m_players[0][index++].button &= 191;	index += size_t(150) + multi[0];	// X-button - Select Load Game
			m_players[0][index++].button &= 191;	index += 161;						// X-button - Select Memory Slot 1
			m_players[0][index++].button &= 191;	index += 49;						// X-button - Select Save File 1
			m_players[0][index++].button &= 191;	index += size_t(164) + multi[0];	// X-button - Exit Loading Module
			if (difficulty == 1)
			{
				m_players[0].resize(m_players[0].size() + 4);
				m_players[0][index++].dpad &= 223;					// D-pad Right - To Difficulty option
				m_players[0][index++].dpad &= 191;					// D-pad Down - Difficulty to "Hard"
				m_players[0][index++].dpad &= 127;	index++;		// D-pad Left - To Vibration option
			}
			if (orientation)
			{
				m_players[0].resize(m_players[0].size() + 87);
				m_players[0][index++].dpad &= 127;		index++;		// D-pad Left - To Volume option
				m_players[0][index++].dpad &= 127;						// D-pad Left - To Controller option
				m_players[0][index++].button &= 191;	index += 34;	// X-button - Select Controller option
				switch (orientation)
				{
				case 1:
					index += 3;		// Do nothing on the Controller screen
					break;
				case 2:
					m_players[0][index++].dpad &= 223;	index += 2;	// D-pad Right - To Orientation 2
					break;
				case 3:
					m_players[0][index++].dpad &= 223;	index++;	// D-pad Right - To Orientation 2
					m_players[0][index++].dpad &= 223;				// D-pad Right - To Orientation 3
				}
				m_players[0][index++].button &= 239;	index += 45;	// /\-button - Exit Controller screen
			}
			m_players[0][index++].button &= 239;	index += size_t(111) + multi[0];	// /\-button - Exit Settings menu / To Main menu
			switch (difficulty)
			{
			case 0:		// Master
				m_players[0].resize(m_players[0].size() + 1);
				m_players[0][index++].dpad &= 191;				// D-pad Down - To Master Play option
			case 1:		// Hard		Stay on Single Play option
			case 2:		// Normal	Stay on Single Play option
				break;
			case 3:
				m_players[0].resize(m_players[0].size() + 3);
				m_players[0][index++].dpad &= 191;	 index++;	// D-pad Down - To Master Play option
				m_players[0][index++].dpad &= 191;				// D-pad Down - To VS Play option
			}
			m_players[0][index++].button &= 191;				// X-button - Select Current option
			size_t stageSelectIndex = m_players[0].size();
			if (difficulty != 3)
			{
				size_t stValue = (stage < 11 ? stage - 1 : stage - 11);
				m_players[0].resize(m_players[0].size() + 143 + 2 * stValue);
				index += 143;
				for (size_t stageIndex = 0; stageIndex < stValue; stageIndex++)
				{
					m_players[0][index++].dpad &= 223;	// D-pad Right - Stage Scrolling
					if (stage - stageIndex - 2)
						index++;
				}
				m_players[0][index].button &= 191;						// X-button - Stage Start
			}
			else
			{
				size_t scrollVal = stage - size_t((stage - 1) / 3);
				if (multi[1])
					m_players[0].resize(m_players[0].size() + 268U);
				else if (multi[0])
					m_players[0].resize(m_players[0].size() + 236U);
				else if (stage == 10)
					m_players[0].resize(m_players[0].size() + 205U);
				else
					m_players[0].resize(m_players[0].size() + 203U);
				index += 138 + 3ULL * multi[0];
				const size_t val1 = (stage - 1) / 3, val2 = stage == 10, val3 = size_t(multi[0]) + multi[1];
				m_players[0].resize(m_players[0].size() + 203 + (val3 << 5) + multi[0] - 2 * (2 * val1 + val2));
				m_players[0][index++].button &= 191;	index += 48;	// X-button - MP Intro Screen
				if (stage != 0)
				{
					m_players[0][index++].dpad &= 191;		// D-pad Down - Stage Scrolling
					for (size_t stageIndex = 1, mStage = stage - val1 - 1; stageIndex < mStage; stageIndex++)
					{
						index++;
						m_players[0][index++].dpad &= 191;		// D-pad Down - Stage Scrolling
					}
				}
				m_players[0][index++].button &= 191;				// X-button - Stage Selection
				index += 2 * (size_t(9) - stage + (val3 << 4) - val1 + val2) + 1;
				m_players[0][index++].button &= 191;	index++;	// X-button - Character Selection
				m_players[0][index++].button &= 191;	index++;	// X-button - Health Handicap Selection
				m_players[0][index].button &= 191;					// X-button - Stage Start

				m_players[1].resize(m_players[0].size()); index = stageSelectIndex + 203 + (val3 << 5) + multi[0] - 2 * (2 * val1 + val2);
				m_players[1][index++].button &= 191;	index++;	// X-button - Character Selection
				m_players[1][index].button &= 191;					// X-button - Health Handicap Selection
				if (multi[0]) // If Player 3
				{
					m_players[2] = m_players[1];
					if (multi[1])	// If Player 4
						m_players[3] = m_players[1];
				}
			}
		}
		else
		{
			m_players[0].resize(543 + 3ULL * multi[0] + frameValues.frames[2][stage][difficulty + multi[0] + multi[1]] + numFrames);
			size_t index = 253 + 2ULL * multi[0];
			m_players[0][index++].dpad &= 247;	index += 283ULL + multi[0];		// Start - Title Screen
			m_players[0][index++].dpad &= 239;	index++;						// D-pad Up - To Settings option
			m_players[0][index++].dpad &= 239;									// D-pad Up - To Collection option
			if (stage == 11)
				m_players[0][(++index)++].dpad &= 239;							// D-pad Up - To Theater option
			m_players[0][index].button &= 191;									// X-button - Select Theater or Collection option
		}
	}
	else if (m_framerate >= 59.94f)
	{
		if (!stage)
		{
			m_players[0].resize(size_t(546) + frameValues.frames[0][0][0] + numFrames);
			m_players[0][255].dpad &= 247;		// Start - Title Screen
			m_players[0][545].button &= 191;	// X-button - Start Tutorial
		}
		else if (stage <= 10)
		{
			m_players[0].resize(1429 + 16ULL * multi[0] + frameValues.frames[0][stage][difficulty + multi[0] + multi[1]] + numFrames);
			size_t index = 255ULL + 3ULL * multi[0];
			m_players[0][index++].dpad &= 247;	index += 289 + 2ULL * multi[0];		// Start - Title Screen
			m_players[0][index++].dpad &= 239;										// D-pad Up - To Settings option
			m_players[0][index++].button &= 191;	index += 215 + 3ULL * multi[0];	// X-button - Select Settings option
			m_players[0][index++].dpad &= 223;	index++;							// D-pad Right - To Difficulty option
			m_players[0][index++].dpad &= 223;										// D-pad Right - To Load Game option
			m_players[0][index++].button &= 191;	index += 156 + 3ULL * multi[0];	// X-button - Select Load Game
			m_players[0][index++].button &= 191;	index += 161;					// X-button - Select Memory Slot 1
			m_players[0][index++].button &= 191;	index += 49;					// X-button - Select Save File 1
			m_players[0][index++].button &= 191;	index += 173 + 3ULL * multi[0];	// X-button - Exit Loading Module
			if (difficulty == 1)
			{
				m_players[0].resize(m_players[0].size() + 4);
				m_players[0][index++].dpad &= 223;				// D-pad Right - To Difficulty option
				m_players[0][index++].dpad &= 191;				// D-pad Down - Difficulty to "Hard"
				m_players[0][index++].dpad &= 127;	index++;	// D-pad Left - To Vibration option
			}
			if (orientation)
			{
				m_players[0].resize(m_players[0].size() + 87);
				m_players[0][index++].dpad &= 127;		index++;	// D-pad Left - To Volume option
				m_players[0][index++].dpad &= 127;					// D-pad Left - To Controller option
				m_players[0][index++].button &= 191;	index += 34;// X-button - Select Controller option
				switch (orientation)
				{
				case 1:
					index += 3;		// Do nothing on the Controller screen
					break;
				case 2:
					m_players[0][index++].dpad &= 223; index += 2;	// D-pad Right - To Orientation 2
					break;
				case 3:
					m_players[0][index++].dpad &= 223; index++;		// D-pad Right - To Orientation 2
					m_players[0][index++].dpad &= 223;				// D-pad Right - To Orientation 3
				}
				m_players[0][index++].button &= 239;	index += 45;// /\-button - Exit Controller screen
			}
			m_players[0][index++].button &= 239;	index += 117 + 2ULL * multi[0];	// /\-button - Exit Settings menu / To Main menu
			switch (difficulty)
			{
			case 0:		// Master
				m_players[0].resize(m_players[0].size() + 1);
				m_players[0][index++].dpad &= 191;				// D-pad Down - To Master Play option
			case 1:		// Hard		Stay on Single Play option
			case 2:		// Normal	Stay on Single Play option
				break;
			case 3:		// Multi-
				m_players[0].resize(m_players[0].size() + 3);
				m_players[0][index++].dpad &= 191; index++;		// D-pad Down - To Master Play option
				m_players[0][index++].dpad &= 191;				// D-pad Down - To VS Play option
			}
			m_players[0][index++].button &= 191;				// X-button - Select Current option
			size_t stageSelectIndex = index;
			if (difficulty != 3) // Single Player
			{
				if (stage != 8)
					m_players[0].resize(m_players[0].size() + 150 + size_t(2) * stage);
				else
					m_players[0].resize(m_players[0].size() + 151 + size_t(2) * stage);
				index += 152;
				for (size_t stageIndex = 0; stageIndex < stage - size_t(1); stageIndex++)
				{
					m_players[0][index++].dpad &= 223;	// D-pad Right - Stage Scrolling
					if (stage - stageIndex - 2)
						index++;
				}
				if (stage == 8)
					index++;
				m_players[0][index].button &= 191;	// X-button - Stage Start
			}
			else // Multiplayer
			{
				size_t scrollVal = stage - size_t((stage - 1) / 3);
				if (multi[1])
					m_players[0].resize(m_players[0].size() + 270U);
				else if (multi[0])
					m_players[0].resize(m_players[0].size() + 238U);
				else if (stage == 10)
					m_players[0].resize(m_players[0].size() + 205U);
				else
					m_players[0].resize(m_players[0].size() + 203U);
				index += 141 + 3ULL * multi[0];
				m_players[0][index++].button &= 191;	index += 47;	// X-button - MP Intro Screen
				for (size_t stageIndex = 0; stageIndex < scrollVal; stageIndex++)
				{
					index++;
					if (stageIndex + 1 != scrollVal)
						m_players[0][index++].dpad &= 191;	// D-pad Down - Stage Scrolling
				}
				m_players[0][index++].button &= 191;		// X-button - Stage Selection
				if (multi[1])
					index += 74 - (scrollVal << 1) - (stage > 1);
				else if (multi[0])
					index += 42 - (scrollVal << 1) - (stage > 1);
				else if (stage < 5)
					index += 10 - (scrollVal << 1) - (stage > 1);
				else
					index++;
				m_players[0][index++].button &= 191;	index++;	// X-button - Multiplayer Character Selection
				m_players[0][index++].button &= 191;	index++;	// X-button - Health Handicap Selection/Stage Start
			}
			// If Multiplayer
			if (difficulty == 3)
			{
				m_players[1].resize(m_players[0].size());
				index = stageSelectIndex + 191;
				if (multi[1])
					index += 75;
				else if (multi[0])
					index += 43;
				else if (stage > 5)
					index += (stage - size_t((stage - 1) / 3)) << 1;
				else
					index += 8;
				m_players[1][index++].button &= 191;	index++;	// X-button - Multiplayer Character Selection
				m_players[1][index].button &= 191;					// X-button - Health Handicap Selection
				if (multi[0]) // If Player 3
				{
					m_players[2] = m_players[1];
					if (multi[1])	// If Player 4
						m_players[3] = m_players[1];
				}
			}
		}
		else
		{
			m_players[0].resize(551 + 5ULL * multi[0] + frameValues.frames[0][stage][difficulty + multi[0] + multi[1]] + numFrames);
			size_t index = 255 + 3ULL * multi[0];
			m_players[0][index++].dpad &= 247; index += 289ULL + 2ULL * multi[0];	// Start - Title Screen
			m_players[0][index++].dpad &= 239; index++;								// D-pad Up - To Settings option
			m_players[0][index++].dpad &= 239;										// D-pad Up - To Collection option
			if (stage == 11)
				m_players[0][(++index)++].dpad &= 239;								// D-pad Up - To Theater option
			m_players[0][index].button &= 191;										// X-button - Select Theater or Collection option
		}
	}
	return m_players[0].size() - numFrames;
}

bool TAS::build(CHC& song)
{
	if (song.m_imc[0] == 0)
	{
		printf("%s%s.CHC is not compatible for PS2\n", g_global.tabs.c_str(), song.m_shortname.c_str());
		return false;
	}
	GlobalFunctions::banner(" " + song.m_shortname + " - TAS creation ");
	auto load = [&]()
	{
		do
		{
			string p2m2vTemp = "";
			printf("%sDrag & drop a valid P2M2V file (or type only 'Q' to back out or only 'B' for base values):", g_global.tabs.c_str());
			switch (GlobalFunctions::stringInsertion(p2m2vTemp, "b"))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::SpecialCase:
				g_global.quit = true;
				break;
			case GlobalFunctions::ResultType::Success:
				if (p2m2vTemp.find(".P2M2V") == string::npos)
					p2m2vTemp += ".P2M2V";
				if (loadValues(p2m2vTemp))
					g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
		return true;
	};
	if (!frameValues.use)
	{
		FILE* p2m2v;
		if (!fopen_s(&p2m2v, frameValues.name.c_str(), "r"))
		{
			fclose(p2m2v);
			size_t pos = frameValues.name.find_last_of("\\");
			do
			{
				printf("%sUse \"%s\"? [Y/N]\n", g_global.tabs.c_str(), frameValues.name.substr(pos != string::npos ? pos + 1 : 0).c_str());
				switch (GlobalFunctions::menuChoices("yn"))
				{
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'y')
					{
						loadValues();
						g_global.quit = true;
						break;
					}
					else if (load())
					{
						g_global.quit = true;
						break;
					}
					__fallthrough;
				case GlobalFunctions::ResultType::Quit:
					printf("%s\n", g_global.tabs.c_str());
					printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
					return false;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		else if (!load())
			return false;
	}
	else if (!loadValues() && !load())
		return false;
	do
	{
		printf("%sSelect a framerate ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
		printf("%s0 - NTSC (59.94)\n", g_global.tabs.c_str());
		printf("%s1 - PAL  (50.00) [NOT YET SUPPORTED]\n", g_global.tabs.c_str());
		printf("%s2 - NTSC-C (60.00)\n", g_global.tabs.c_str());
		printf("%s3 - NTSC-Custom\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("0123"))
		{
		case GlobalFunctions::ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
			return false;
		case GlobalFunctions::ResultType::Success:
			switch (g_global.answer.character)
			{
			case '0':
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
				break;
			case '1':
				printf("%sI do not own the PAL version, so optimizing for it is not something I can do just yet.\n", g_global.tabs.c_str());
				printf("%s\n", g_global.tabs.c_str());
				/*
				m_pcsx2.m_framerate = 50.00;
				g_global.quit = true;*/
				break;
			case '2':
				printf("%s\n", g_global.tabs.c_str());
				m_pcsx2.m_framerate = 60.00f;
				g_global.quit = true;
				break;
			case '3':
				printf("%s\n", g_global.tabs.c_str());
				do
				{
					printf("%sProvide an FPS value (minimum: 59.94) ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
					printf("%sInput: ", g_global.tabs.c_str());
					switch (GlobalFunctions::valueInsert(m_pcsx2.m_framerate, false, 59.94f))
					{
					case GlobalFunctions::ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
						return false;
					case GlobalFunctions::ResultType::InvalidNegative:
						printf("%sWhat the hell is a negative FPS?!?\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
						break;
					case GlobalFunctions::ResultType::MinExceeded:
						printf("%sGiven FPS is lower than the 59.94 FPS minimum\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
						break;
					case GlobalFunctions::ResultType::Failed:
						printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
					}
					printf("%s\n", g_global.tabs.c_str());
				} while (!g_global.quit);
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	int stage = song.m_stage;
	int difficulty;
	bool multi[2] = { false, false };
	if (stage == 0)
		difficulty = 0;
	else if (stage < 0 || stage > 12)
	{
		do
		{
			printf("%sWhich stage will be replaced? [1-12] ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
			printf("%sInput: ", g_global.tabs.c_str());
			switch (GlobalFunctions::valueInsert(stage, false, 1, 12))
			{
			case GlobalFunctions::ResultType::Success:
				g_global.quit = true;
				break;
			case GlobalFunctions::ResultType::InvalidNegative:
			case GlobalFunctions::ResultType::MinExceeded:
				printf("%sGiven value cannot be less than 1\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::MaxExceeded:
				printf("%sGiven value cannot be greater than 10\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::Failed:
				printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::Quit:
				printf("%s\n", g_global.tabs.c_str());
				printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
				return false;
			}
		} while (!g_global.quit);
		g_global.quit = false;
		if (song.m_stage > 12)
		{
			if (song.m_shortname.find("H") != string::npos)
				difficulty = 0;
			else if (song.m_shortname.find("E") != string::npos)
				difficulty = 2;
			else if (song.m_shortname.find("M") != string::npos)
				difficulty = 3;
			else
				difficulty = 1;
		}
		else
		{
			do
			{
				printf("%sWhich difficulty is this chart for? ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
				printf("%s 0 - Hard/Master Play\n", g_global.tabs.c_str());
				printf("%s 1 - Normal\n", g_global.tabs.c_str());
				printf("%s 2 - Easy\n", g_global.tabs.c_str());
				printf("%s 3 - Multiplayer\n", g_global.tabs.c_str());
				switch (GlobalFunctions::menuChoices("0123", true))
				{
				case GlobalFunctions::ResultType::Quit:
					printf("%s\n", g_global.tabs.c_str());
					printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
					return false;
				case GlobalFunctions::ResultType::Help:
					printf("%s\n", g_global.tabs.c_str());
				case GlobalFunctions::ResultType::Failed:
					break;
				default:
					difficulty = (int)g_global.answer.index;
					printf("%s\n", g_global.tabs.c_str());
					g_global.quit = true;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
	}
	else
	{
		if (song.m_shortname.find("HE") != string::npos || song.m_shortname.find("H2") != string::npos ||
			song.m_shortname.find("HB") != string::npos)
			difficulty = 1;
		else if (song.m_shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.m_shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.m_shortname.find("M") != string::npos)
		{
			difficulty = 3;
			if (stage != 10)
			{
				do
				{
					printf("%sHow many players for this multiplayer TAS [2/3/4]? ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
					switch (GlobalFunctions::menuChoices("234", true))
					{
					case GlobalFunctions::ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
						return false;
					case GlobalFunctions::ResultType::Success:
						switch (g_global.answer.index)
						{
						case 2:
							multi[1] = true;
						case 1:
							multi[0] = true;
						default:
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
						}
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}
		}
		else
			difficulty = 1;
	}
	
	char framerateIndex = 2;
	if (m_pcsx2.m_framerate == 59.94f)
		framerateIndex = 0;
	else if (m_pcsx2.m_framerate == 50.00f)
		framerateIndex = 1;
	printf("%sStage %i - Diff. %i: ", g_global.tabs.c_str(), stage, difficulty);
	printf("%lu frames | ", frameValues.frames[framerateIndex][stage][difficulty + multi[0] + multi[1]]);
	printf("Displacement: %li samples\n%s\n", frameValues.initialDisplacements[framerateIndex][stage][difficulty + multi[0] + multi[1]], g_global.tabs.c_str());
	printf("%sType the name of the author [255 character limit] (';' to notate the end of the name [for multi-step usage]) ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
	printf("%sInput: ", g_global.tabs.c_str());
	if (GlobalFunctions::charArrayInsertion(m_pcsx2.m_author, 255) == GlobalFunctions::ResultType::Quit)
	{
		printf("%s\n%sTAS creation cancelled\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		return false;
	}
	
	int orientation;
	do
	{
		printf("%sWhich orientation for all guard phrases? ('Q' to back out to Main Menu)\n", g_global.tabs.c_str());
		printf("%s0 - No Orientation Change\n", g_global.tabs.c_str());
		printf("%s1 - No Change - Open Config Menu\n", g_global.tabs.c_str());
		printf("%s2 - Change to Orientation 2\n", g_global.tabs.c_str());
		printf("%s3 - Change to Orientation 3\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("0123", true))
		{
		case  GlobalFunctions::ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
			return false;
		case  GlobalFunctions::ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			orientation = (int)g_global.answer.index;
			printf("%s\n", g_global.tabs.c_str());
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	std::vector<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to TAS - in chronological order and w/ spaces in-between\n", g_global.tabs.c_str());
		for (size_t index = 0; index < song.m_sections.size(); ++index)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), index, song.m_sections[index].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent List: ", g_global.tabs.c_str());
			for (const size_t index : sectionIndexes)
				printf("%s ", song.m_sections[index].getName());
			putchar('\n');
		}
		switch (GlobalFunctions::insertIndexValues(sectionIndexes, "yntmv", song.m_sections.size()))
		{
		case  GlobalFunctions::ResultType::Help:

			break;
		case  GlobalFunctions::ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
			return false;
		case GlobalFunctions::ResultType::SpecialCase:
			if (g_global.answer.character == 'n')
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sOk... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n", g_global.tabs.c_str());
				printf("%s\n", g_global.tabs.c_str());
				break;
			}
			else if(sectionIndexes.size())
			{
				g_global.quit = true;
				break;
			}
			else
				__fallthrough;
		case GlobalFunctions::ResultType::Success:
			if (!sectionIndexes.size())
			{
				printf("%s\n", g_global.tabs.c_str());
				do
				{
					printf("%sNo sections have been selected. Quit TAS creation? [Y/N]\n", g_global.tabs.c_str());
					switch (GlobalFunctions::menuChoices("yn"))
					{
					case GlobalFunctions::ResultType::Success:
						if (g_global.answer.character == 'n')
						{
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
							break;
						}
						__fallthrough;
					case GlobalFunctions::ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
						return false;
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}
			else
				g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	string filename = song.m_filename.substr(0, song.m_filename.length() - 4);
	if (difficulty == 3)
		filename += "_" + to_string(int(m_pcsx2.m_framerate)) + "_" + to_string(orientation) + "_" + to_string(multi[0] + multi[1] + 2) + 'P';
	else
		filename += "_" + to_string(int(m_pcsx2.m_framerate)) + "_" + to_string(orientation) + "_SP";
	
	
	const float SAMPLES_PER_FRAME = 48000.0f / m_pcsx2.m_framerate;
	unsigned long totalDuration = 0;
	bool endReached = false;
	int notes[4] = { 0, 0, 0, 0 };
	long position = frameValues.initialDisplacements[framerateIndex][stage][difficulty + multi[0] + multi[1]];
	// Places every single note that will appear in all chosen sections
	// into one huge timeline.
	for (const size_t sectIndex : sectionIndexes)
	{
		SongSection& section = song.m_sections[sectIndex];
		printf("%s%s\n", g_global.tabs.c_str(), section.getName());
		if (!stage ||
			(section.getPhase() != SongSection::Phase::INTRO && !strstr(section.getName(), "BRK"))) // If not INTRO phase or BRK section
		{
			if (section.getPhase() == SongSection::Phase::END || sectIndex + 1 == song.m_sections.size())
				endReached = true; // If END phase or last section

			{
				// t - Technical
				// v - Visuals
				// m - Mixed
				for (size_t playerIndex = 0;
					playerIndex < section.getNumPlayers() && !g_global.quit;
					difficulty == 3 ? ++playerIndex : playerIndex += 2)
				{
					for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && !g_global.quit; chartIndex++)
					{
						if (section.m_charts[playerIndex * section.getNumCharts() + chartIndex].getNumPhrases())
						{
							do
							{
								printf("%sHow should %s's phrase bars be played?\n", g_global.tabs.c_str(), section.getName());
								printf("%sT - Technicality (Release at the end of all sustains)\n", g_global.tabs.c_str());
								printf("%sV - Visually (Hold past/through the end of all sustains)\n", g_global.tabs.c_str());
								printf("%sM - Mixed (Release only on sustain that exceeds a defined length)\n", g_global.tabs.c_str());
								switch (GlobalFunctions::menuChoices("tvm", true))
								{
								case GlobalFunctions::ResultType::Quit:
									printf("%s\n", g_global.tabs.c_str());
									printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
									return false;
								case GlobalFunctions::ResultType::Help:
									printf("%s\n", g_global.tabs.c_str());
								case GlobalFunctions::ResultType::Failed:
									break;
								default:
									printf("%s\n", g_global.tabs.c_str());
									if (g_global.answer.index == 2)
									{
										do
										{
											printf("%sProvide a value for the sustain limit coeffienct.\n", g_global.tabs.c_str());
											printf("%sAKA, how many beats must a sustain be before the TAS programs a sustain-release at the end of the note?\n", g_global.tabs.c_str());
											printf("%sValue can range from 0.5 to 4.0 [Default is 1].\n", g_global.tabs.c_str());
											printf("%sInput: ", g_global.tabs.c_str());
											float sustainCoeffienct = 1; // Sustain limit set to a base of 1 beat
											switch (GlobalFunctions::valueInsert(sustainCoeffienct, false, 0.5f, 4.0f))
											{
											case GlobalFunctions::ResultType::Success:
												markers.emplace_back(position, 2, (long)round(sustainCoeffienct * s_SAMPLES_PER_MIN / section.getTempo()));
												g_global.quit = true;
												break;
											case GlobalFunctions::ResultType::Quit:
												printf("%s\n", g_global.tabs.c_str());
												printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
												return false;
											case GlobalFunctions::ResultType::InvalidNegative:
											case GlobalFunctions::ResultType::MinExceeded:
												printf("%sProvided value *must* be greater than or equal to 0.5.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
												break;
											case GlobalFunctions::ResultType::MaxExceeded:
												printf("%sProvided value *must* be less than or equal to 4.0.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
												break;
											case GlobalFunctions::ResultType::Failed:
												printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
												GlobalFunctions::clearIn();
											}
										} while (!g_global.quit);
									}
									else
										markers.emplace_back(position, g_global.answer.index, (long)round(s_SAMPLES_PER_MIN / section.getTempo()));
									g_global.quit = true;
								}
							} while (!g_global.quit);
						}
					}
				}
				g_global.quit = false;
				
			}
			// Marking where in each list the current section starts
			size_t startIndex[4] = { timeline[0].size(), timeline[1].size(), timeline[2].size(), timeline[3].size() };
			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); difficulty == 3 ? ++playerIndex : playerIndex += 2)
				{
					Chart& chart = section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
					currentPlayer = (multi[playerIndex & 1] ? playerIndex : playerIndex & 1);
					std::vector<NotePoint>& player = timeline[currentPlayer];
					size_t index = startIndex[currentPlayer];
					for (size_t i = 0; i < chart.getNumGuards(); i++)
					{
						long pos = chart.m_guards[i].getPivotAlpha() + chart.getPivotTime() + position;
						while (index < player.size() && pos > player[index].position)
							index++;
						player.emplace(player.begin() + index, pos, &chart.m_guards[i], i, i + 1 == chart.getNumGuards());
						notes[currentPlayer]++;
						index++;
					}
					index = startIndex[currentPlayer];
					for (size_t i = 0; i < chart.getNumPhrases(); i++)
					{
						Phrase& phrase = chart.m_phrases[i];
						long pos = phrase.getPivotAlpha() + chart.getPivotTime() + position;
						while (index < player.size() && pos > player[index].position)
							index++;
						// Combine all pieces into one Note
						while (i < chart.getNumPhrases())
						{
							if (!chart.m_phrases[i].getEnd())
								i++;
							else
							{
								if (i + 1 != chart.getNumPhrases())
								{
									switch (markers.back().type)
									{
									case SectPoint::VisualType::Visual:
										if (chart.m_phrases[i + 1].getPivotAlpha() - chart.m_phrases[i].getEndAlpha() < markers.back().sustainLimit)
											phrase.changeEndAlpha(chart.m_phrases[i + 1].getPivotAlpha() - long(SAMPLES_PER_FRAME));
										else
											phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() + long(2 * SAMPLES_PER_FRAME));
										break;
									case SectPoint::VisualType::Mixed:
										if (chart.m_phrases[i + 1].getPivotAlpha() - phrase.getPivotAlpha() < markers.back().sustainLimit)
										{
											phrase.changeEndAlpha(chart.m_phrases[i + 1].getPivotAlpha() - long(SAMPLES_PER_FRAME));
											break;
										}
									default:
										phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());
									}
								}
								else if (markers.back().type != SectPoint::VisualType::Visual ||
										!phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() - long(5 * SAMPLES_PER_FRAME)))
									phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());
								break;
							}
						}
						player.emplace(player.begin() + index, pos, &phrase, i, i + 1 == chart.getNumPhrases());
						notes[currentPlayer]++;
						index++;
					}
					index = startIndex[currentPlayer];
					if (chart.getNumTracelines() > 1)
					{
						for (size_t i = 0; i < chart.getNumTracelines(); i++)
						{
							long pos = chart.m_tracelines[i].getPivotAlpha() + chart.getPivotTime() + position;
							while (index < player.size() && pos > player[index].position)
								index++;
							player.emplace(player.begin() + index, pos, &chart.m_tracelines[i], i, i + 1 == chart.getNumTracelines());
							index++;
						}
					}
				}
			}
		}
		position += section.getDuration();
		totalDuration += section.getDuration();
	}
	size_t frameStart;
	if (stage == 0 && song.m_shortname.compare("ST00B") != 0)
	{
		endReached = false;
		try
		{
			m_tutorialStageB = new CHC(song.m_filename.substr(0, song.m_filename.length() - 5) + 'B');
		}
		catch (...)
		{
			do
			{
				string filename;
				printf("%sProvide the ST00B.CHC file to use for this TAS (Or 'N' to only TAS ST00A): ", g_global.tabs.c_str());
				switch (GlobalFunctions::stringInsertion(filename, "n"))
				{
				case GlobalFunctions::ResultType::Quit:
					printf("%s\n", g_global.tabs.c_str());
					printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
					return false;
				case GlobalFunctions::ResultType::SpecialCase:
					g_global.quit = true;
					break;
				case GlobalFunctions::ResultType::Success:
					if (filename.find("ST00B") != string::npos)
					{
						if (filename.find(".CHC") != string::npos)
							filename = filename.substr(0, filename.length() - 4);
						try
						{
							m_tutorialStageB = new CHC(filename);
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
						}
						catch (...)
						{
							printf("%sThe given ST00B could not be successfully read.\n", g_global.tabs.c_str());
						}
					}
					else
						printf("%sOnly an ST00B.CHC file can be accepted.\n", g_global.tabs.c_str());
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		
		if (m_tutorialStageB != nullptr)
		{
			printf("%sStage ST00B: ", g_global.tabs.c_str());
			printf("%lu frames during intermission |", frameValues.frames[framerateIndex][0][1]);
			printf(" Post-intermission Displacement: %li samples\n%s\n", frameValues.initialDisplacements[framerateIndex][0][1], g_global.tabs.c_str());
			sectionIndexes.clear();
			position += long((frameValues.frames[framerateIndex][0][1] - 1000) * SAMPLES_PER_FRAME) + frameValues.initialDisplacements[framerateIndex][0][1];
			do
			{
				printf("%sType the number for each section that you wish to TAS from ST00B - in chronological order and w/ spaces in-between\n", g_global.tabs.c_str());
				for (size_t index = 0; index < m_tutorialStageB->m_sections.size(); ++index)
					printf("%s%zu - %s\n", g_global.tabs.c_str(), index, m_tutorialStageB->m_sections[index].getName());
				if (sectionIndexes.size())
				{
					printf("%sCurrent List: ", g_global.tabs.c_str());
					for (const size_t index : sectionIndexes)
						printf("%s ", song.m_sections[index].getName());
					putchar('\n');
				}
				switch (GlobalFunctions::insertIndexValues(sectionIndexes, "yntmv", song.m_sections.size()))
				{
				case GlobalFunctions::ResultType::Help:

					break;
				case GlobalFunctions::ResultType::Quit:
					printf("%s\n", g_global.tabs.c_str());
					printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
					delete m_tutorialStageB;
					return false;
				case GlobalFunctions::ResultType::SpecialCase:
					if (g_global.answer.character == 'n')
					{
						printf("%s\n", g_global.tabs.c_str());
						printf("%sOk... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n", g_global.tabs.c_str());
						printf("%s\n", g_global.tabs.c_str());
						break;
					}
					else if (sectionIndexes.size())
					{
						g_global.quit = true;
						break;
					}
				case GlobalFunctions::ResultType::Success:
					if (!sectionIndexes.size())
					{
						printf("%s\n", g_global.tabs.c_str());
						do
						{
							printf("%sNo sections have been selected. Quit TAS creation? [Y/N]\n", g_global.tabs.c_str());
							switch (GlobalFunctions::menuChoices("yn"))
							{
							case GlobalFunctions::ResultType::Quit:
							case GlobalFunctions::ResultType::Success:
								if (g_global.answer.character == 'y')
								{
									printf("%s\n", g_global.tabs.c_str());
									printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
									delete m_tutorialStageB;
									return false;
								}
								else
								{
									printf("%s\n", g_global.tabs.c_str());
									g_global.quit = true;
								}
							}
						} while (!g_global.quit);
						g_global.quit = false;
					}
					else
						g_global.quit = true;
				}
			} while (!g_global.quit);
			g_global.quit = false;
			for (const size_t sectIndex : sectionIndexes)
			{
				SongSection& section = m_tutorialStageB->m_sections[sectIndex];
				printf("%s%s\n", g_global.tabs.c_str(), section.getName());
				if (sectIndex + 1 == m_tutorialStageB->m_sections.size())
					endReached = true; // If END phase or last section
				{
					// 0 - Technical
					// 1 - Visuals
					// 2 - Mixed
					for (size_t playerIndex = 0; !g_global.quit && playerIndex < section.getNumPlayers(); playerIndex += 2)
					{
						for (size_t chartIndex = 0; !g_global.quit && chartIndex < section.getNumCharts(); chartIndex++)
						{
							if (section.m_charts[playerIndex * (size_t)section.getNumCharts() + chartIndex].getNumPhrases())
							{
								do
								{
									printf("%sHow should %s's phrase bars be played?\n", g_global.tabs.c_str(), section.getName());
									printf("%sT - Technicality (Release at the end of all sustains)\n", g_global.tabs.c_str());
									printf("%sV - Visually (Hold past/through the end of all sustains)\n", g_global.tabs.c_str());
									printf("%sM - Mixed (Release only on sustain that exceeds a defined length)\n", g_global.tabs.c_str());
									switch (GlobalFunctions::menuChoices("tvm", true))
									{
									case GlobalFunctions::ResultType::Quit:
										printf("%s\n", g_global.tabs.c_str());
										printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
										delete m_tutorialStageB;
										return false;
									case GlobalFunctions::ResultType::Help:
										printf("%s\n", g_global.tabs.c_str());
									case GlobalFunctions::ResultType::Failed:
										break;
									default:
										printf("%s\n", g_global.tabs.c_str());
										if (g_global.answer.index == 2)
										{
											do
											{
												printf("%sProvide a value for the sustain limit coeffienct.\n", g_global.tabs.c_str());
												printf("%sAKA, how many beats must a sustain be before the TAS programs a sustain-release at the end of the note?\n", g_global.tabs.c_str());
												printf("%sValue can range from 0.5 to 4.0 [Default is 1].\n", g_global.tabs.c_str());
												printf("%sInput: ", g_global.tabs.c_str());
												float sustainCoeffienct = 1; // Sustain limit set to a base of 1 beat
												switch (GlobalFunctions::valueInsert(sustainCoeffienct, false, 0.5f, 4.0f))
												{
												case GlobalFunctions::ResultType::Success:
													markers.emplace_back(position, 2, (long)round(sustainCoeffienct * s_SAMPLES_PER_MIN / section.getTempo()));
													g_global.quit = true;
													break;
												case GlobalFunctions::ResultType::Quit:
													printf("%s\n", g_global.tabs.c_str());
													printf("%sTAS creation cancelled\n", g_global.tabs.c_str());
													delete m_tutorialStageB;
													return false;
												case GlobalFunctions::ResultType::InvalidNegative:
												case GlobalFunctions::ResultType::MinExceeded:
													printf("%sProvided value *must* be greater than or equal to 0.5.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
													break;
												case GlobalFunctions::ResultType::MaxExceeded:
													printf("%sProvided value *must* be less than or equal to 4.0.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
													break;
												case GlobalFunctions::ResultType::Failed:
													printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
													GlobalFunctions::clearIn();
												}
											} while (!g_global.quit);
										}
										else
											markers.emplace_back(position, g_global.answer.index, (long)round(s_SAMPLES_PER_MIN / section.getTempo()));
										g_global.quit = true;
									}
								} while (!g_global.quit);
							}
						}
					}
				}
				size_t startIndex = timeline[0].size();
				for (unsigned chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
				{
					for (unsigned playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); playerIndex++)
					{
						Chart& chart = section.m_charts[chartIndex];
						size_t index = startIndex;
						for (size_t i = 0; i < chart.getNumGuards(); i++)
						{
							long pos = chart.m_guards[i].getPivotAlpha() + chart.getPivotTime() + position;
							while (index < timeline[0].size() && pos > timeline[0][index].position)
									index++;
							timeline[0].emplace(timeline[0].begin() + index, pos, &chart.m_guards[i], i, i + 1 == chart.getNumGuards());
							notes[currentPlayer]++;
							index++;
						}
						index = startIndex;
						for (size_t i = 0; i < chart.getNumPhrases(); i++)
						{
							Phrase& phrase = chart.m_phrases[i];
							long pos = phrase.getPivotAlpha() + chart.getPivotTime() + position;
							while (index < timeline[0].size() && pos > timeline[0][index].position)
								index++;
							// Combine all pieces into one Note
							while (i < chart.getNumPhrases())
							{
								if (!chart.m_phrases[i].getEnd())
									i++;
								else
								{
									if (i + 1 != chart.getNumPhrases())
									{
										switch (markers.back().type)
										{
										case SectPoint::VisualType::Visual:
											if (chart.m_phrases[i + 1].getPivotAlpha() - chart.m_phrases[i].getEndAlpha() < markers.back().sustainLimit)
												phrase.changeEndAlpha(chart.m_phrases[i + 1].getPivotAlpha() - long(SAMPLES_PER_FRAME));
											else
												phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() + long(2 * SAMPLES_PER_FRAME));
											break;
										case SectPoint::VisualType::Mixed:
											if (chart.m_phrases[i + 1].getPivotAlpha() - phrase.getPivotAlpha() < markers.back().sustainLimit)
											{
												phrase.changeEndAlpha(chart.m_phrases[i + 1].getPivotAlpha() - long(SAMPLES_PER_FRAME));
												break;
											}
										default:
											phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());
										}
									}
									else if (markers.back().type != SectPoint::VisualType::Visual ||
											!phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() - long(5 * SAMPLES_PER_FRAME)))
										phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());
									break;
								}
							}
							timeline[0].emplace(timeline[0].begin() + index, pos, &phrase, i, i + 1 == chart.getNumPhrases());
							notes[currentPlayer]++;
							index++;
						}
						index = startIndex;
						if (chart.getNumTracelines() > 1)
						{
							for (size_t i = 0; i < chart.getNumTracelines(); i++)
							{
								long pos = chart.m_tracelines[i].getPivotAlpha() + chart.getPivotTime() + position;
								while (index < timeline[0].size() && pos > timeline[0][index].position)
									index++;
								timeline[0].emplace(timeline[0].begin() + index, pos, &chart.m_tracelines[i], i, i + 1 == chart.getNumTracelines());
								index++;
							}
						}
					}
				}
				position += section.getDuration();
				totalDuration += section.getDuration();
			}
			frameStart = m_pcsx2.insertFrames(stage, orientation, difficulty, multi, (size_t)ceilf(totalDuration / SAMPLES_PER_FRAME) + frameValues.frames[framerateIndex][0][1] - 1000);
		}
		else
			frameStart = m_pcsx2.insertFrames(stage, orientation, difficulty, multi, (size_t)ceilf(totalDuration / SAMPLES_PER_FRAME));
	}
	else
		frameStart = m_pcsx2.insertFrames(stage, orientation, difficulty, multi, (size_t)ceilf(totalDuration / SAMPLES_PER_FRAME));
	bool results[4] = { false };
	auto convertToFrames = [&](const size_t playerIndex)
	{
		FILE* taslog;
		if (difficulty != 3)
			fopen_s(&taslog, (filename + ".txt").c_str(), "w");
		else
			fopen_s(&taslog, (filename + "_P" + to_string(playerIndex + 1) + ".txt").c_str(), "w");

		fprintf(taslog, "Samples per frame: %g", SAMPLES_PER_FRAME);
		NotePoint* prevPhrase = nullptr;
		for (size_t markIndex = 0, noteIndex = 0; markIndex < markers.size(); ++markIndex)
		{
			fprintf(taslog, "\n//// Section Marker %zu at sample %li\n\n", markIndex + 1, markers[markIndex].position);
			for (; noteIndex < timeline[playerIndex].size() &&
				(markIndex + 1 == markers.size() || timeline[playerIndex][noteIndex].position < markers[markIndex + 1].position);
				++noteIndex)
			{
				if (dynamic_cast<Guard*>(timeline[playerIndex][noteIndex].note) != nullptr)
				{
					size_t grdFrame = frameStart + (size_t)round(timeline[playerIndex][noteIndex].position / SAMPLES_PER_FRAME);
					// If the next note, if it exists, is a guard mark
					if (noteIndex + 1 != timeline[playerIndex].size() && dynamic_cast<Guard*>(timeline[playerIndex][noteIndex + 1].note) != nullptr)
					{

						switch (frameStart + (size_t)round(timeline[playerIndex][noteIndex + 1].position / SAMPLES_PER_FRAME) - grdFrame)
						{
						case 0:
							// Push the second mark back one frame
							timeline[playerIndex][noteIndex + 1].position += long(SAMPLES_PER_FRAME);
							// If pushing it back places it behind the note that *was* after it, fix the order
							if (noteIndex + 1 != timeline[playerIndex].size() && timeline[playerIndex][noteIndex + 1].position >= timeline[playerIndex][noteIndex + 2].position)
								GlobalFunctions::moveElements(timeline[playerIndex], noteIndex + 1, noteIndex + 3);
							__fallthrough;
						case 1:
							// Pull the current mark forwards one frame
							grdFrame--;
						}
					}
					// Clear any buttons from the prior frame
					m_pcsx2.m_players[playerIndex][grdFrame - 1].button = 255;

					// Square - 239
					// X/Cross  - 223
					// Circle - 191
					// Triangle - 127
					static const int orientationSets[4][4] = { {239, 223, 191, 127}, {239, 223, 191, 127}, {223, 191, 127, 239}, {127, 239, 223, 191} };
					m_pcsx2.m_players[playerIndex][grdFrame].button = orientationSets[orientation][static_cast<Guard*>(timeline[playerIndex][noteIndex].note)->getButton()];

					fprintf(taslog, "Guard Mark %03zu: ", timeline[playerIndex][noteIndex].index);
					switch (m_pcsx2.m_players[playerIndex][grdFrame].button)
					{
					case 239:
						fprintf(taslog, "[Square] - Landing at sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].position, grdFrame - frameStart);
						break;
					case 223:
						fprintf(taslog, "[X/Cross] - Landing at sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].position, grdFrame - frameStart);
						break;
					case 191:
						fprintf(taslog, "[Circle] - Landing at sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].position, grdFrame - frameStart);
						break;
					case 127:
						fprintf(taslog, "[Triangle] - Landing at sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].position, grdFrame - frameStart);
					}

				}
				else if (dynamic_cast<Traceline*>(timeline[playerIndex][noteIndex].note) != nullptr)
				{
					for (size_t testIndex = noteIndex + 1; testIndex < timeline[playerIndex].size(); ++testIndex)
					{
						if (dynamic_cast<Guard*>(timeline[playerIndex][testIndex].note) != nullptr && timeline[playerIndex][noteIndex].last)
							break;
						else if (dynamic_cast<Traceline*>(timeline[playerIndex][testIndex].note) != nullptr)
						{
							if (!timeline[playerIndex][noteIndex].last || 10 * (timeline[playerIndex][testIndex].position - (float)timeline[playerIndex][noteIndex].position) <= 3.0f * song.m_speed * SAMPLES_PER_FRAME)
							{
								if (timeline[playerIndex][noteIndex].last || timeline[playerIndex][testIndex].index - 1 == timeline[playerIndex][noteIndex].index)
								{
									size_t currentFrame = frameStart + (size_t)round(timeline[playerIndex][noteIndex].position / SAMPLES_PER_FRAME);
									size_t endFrame = frameStart + (size_t)round(timeline[playerIndex][testIndex].position / SAMPLES_PER_FRAME);
									if (endFrame - currentFrame > 0)
									{
										float currentAngle = static_cast<Traceline*>(timeline[playerIndex][noteIndex].note)->getAngle();
										float angleDif = 0;
										if (!timeline[playerIndex][noteIndex].last)
										{
											if (!timeline[playerIndex][testIndex].last)
											{
												angleDif = static_cast<Traceline*>(timeline[playerIndex][testIndex].note)->getAngle() - currentAngle;
												if (angleDif > M_PI)
													angleDif -= 2 * M_PI;
												else if (angleDif < -M_PI)
													angleDif += 2 * M_PI;
											}
										}
										else
											currentAngle = static_cast<Traceline*>(timeline[playerIndex][testIndex].note)->getAngle();

										if (orientation == 2)
											currentAngle += .5 * M_PI;
										else if (orientation == 3)
											currentAngle -= .5 * M_PI;

										if (!static_cast<Traceline*>(timeline[playerIndex][noteIndex].note)->getCurve()) // If curve is false
										{
											// Iterate through all frames with a straight trace line, if any
											for (size_t compare = 20 * (endFrame - currentFrame - 1); compare > song.m_speed; ++currentFrame, compare -= 20)
											{
												// Only Orientation 2 uses the right stick for trace lines
												if (orientation == 2)
												{
													m_pcsx2.m_players[playerIndex][currentFrame].rightStickX = (unsigned)round(127 - 127 * cos(currentAngle));
													m_pcsx2.m_players[playerIndex][currentFrame].rightStickY = (unsigned)round(127 + 127 * sin(currentAngle));
												}
												else
												{
													m_pcsx2.m_players[playerIndex][currentFrame].leftStickX = (unsigned)round(127 - 127 * cos(currentAngle));
													m_pcsx2.m_players[playerIndex][currentFrame].leftStickY = (unsigned)round(127 + 127 * sin(currentAngle));
												}
											}
										}
										float angleIncrement = angleDif / (endFrame - currentFrame);
										for (; currentFrame < endFrame; ++currentFrame)
										{
											// Only Orientation 2 uses the right stick for trace lines
											if (orientation == 2)
											{
												m_pcsx2.m_players[playerIndex][currentFrame].rightStickX = (unsigned)round(127 - 127 * cos(currentAngle));
												m_pcsx2.m_players[playerIndex][currentFrame].rightStickY = (unsigned)round(127 + 127 * sin(currentAngle));
											}
											else
											{
												m_pcsx2.m_players[playerIndex][currentFrame].leftStickX = (unsigned)round(127 - 127 * cos(currentAngle));
												m_pcsx2.m_players[playerIndex][currentFrame].leftStickY = (unsigned)round(127 + 127 * sin(currentAngle));
											}
											currentAngle += angleIncrement;
										}
									}
									break;
								}
								else
									timeline[playerIndex].erase(timeline[playerIndex].begin() + testIndex);
							}
							else
								break;
						}
					}
				}
				else
				{
					// Square - 239
					// X/Cross  - 223
					// Triangle - 127
					static const int orientationsSet[4] = { 239, 239, 223, 127 };
					size_t startIndex = frameStart + (size_t)round(timeline[playerIndex][noteIndex].position / SAMPLES_PER_FRAME) - 1;

					// If this phrase bar isn't the first one in the song but is the first in its subsection
					// &
					// if the current & following trace lines are close enough together without interruptions
					if (m_pcsx2.m_players[playerIndex][startIndex].leftStickX != 127 || m_pcsx2.m_players[playerIndex][startIndex].leftStickY != 127)
					{
						if (!timeline[playerIndex][noteIndex].index && prevPhrase != nullptr)
						{
							size_t prevSect = markIndex - 1;
							// Check if the previous PB is close enough to justify connecting its
							// sustain button press to the current PB
							while (prevPhrase->position < markers[prevSect].position)
								--prevSect;
							if ((markers[prevSect].type == SectPoint::VisualType::Mixed &&
										timeline[playerIndex][noteIndex].position - prevPhrase->position < markers[prevSect].sustainLimit) ||
								(markers[prevSect].type == SectPoint::VisualType::Visual &&
										timeline[playerIndex][noteIndex].position - prevPhrase->position <
												markers[prevSect].sustainLimit + long(static_cast<Phrase*>(prevPhrase->note)->getDuration())))
							{
								fprintf(taslog, "\t      - Extended to sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].position, startIndex - frameStart);
								size_t phraseEnd = frameStart + (size_t)ceilf(((SAMPLES_PER_FRAME + prevPhrase->position +
																					static_cast<Phrase*>(prevPhrase->note)->getDuration()) / SAMPLES_PER_FRAME));

								--startIndex;
								while (phraseEnd < startIndex)
									m_pcsx2.m_players[playerIndex][phraseEnd++].button = orientationsSet[orientation];
								++startIndex;
							}
						}
					}

					++startIndex;
					if (m_pcsx2.m_players[playerIndex][startIndex].leftStickX != 127 || m_pcsx2.m_players[playerIndex][startIndex].leftStickY != 127)
					{
						const size_t phraseEnd = frameStart + (size_t)ceilf((timeline[playerIndex][noteIndex].position + static_cast<Phrase*>(timeline[playerIndex][noteIndex].note)->getDuration()) / SAMPLES_PER_FRAME);
						fprintf(taslog, "Phrase Bar %03zu- Starting at sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].index, timeline[playerIndex][noteIndex].position, startIndex - frameStart);
						m_pcsx2.m_players[playerIndex][startIndex - 1].button = 255;
						while (startIndex <= phraseEnd)
							m_pcsx2.m_players[playerIndex][startIndex++].button = orientationsSet[orientation];
						fprintf(taslog, "\t      -   Ending at sample %li | Frame #%zu\n", timeline[playerIndex][noteIndex].position + static_cast<Phrase*>(timeline[playerIndex][noteIndex].note)->getDuration(), phraseEnd - frameStart);
						prevPhrase = &timeline[playerIndex][noteIndex];
					}
				}
			}

			fflush(taslog);
		}
		fclose(taslog);
		results[playerIndex] = true;
	};

	if (difficulty == 3 && processor_count > 1)
	{
		struct threadControl
		{
			size_t playerIndex;
			std::thread thisThread;
			threadControl(const size_t index) : playerIndex(index) {}
		};
		std::vector<threadControl> threads;
		threads.reserve(processor_count - 2);
		for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
		{
			if (timeline[playerIndex].size())
			{
				while (threads.size() == processor_count - 1)
				{
					for (size_t thr = 0; thr < threads.size();)
					{
						if (results[threads[thr].playerIndex])
						{
							threads[thr].thisThread.join();
							threads.erase(threads.begin() + thr);
						}
						else
							++thr;
					}
				}
				threads.emplace_back(playerIndex);
				threads.back().thisThread = std::thread(convertToFrames, playerIndex);
			}
		}
		for (threadControl& thr : threads)
			thr.thisThread.join();
	}
	else
	{
		for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
			if (timeline[playerIndex].size())
				convertToFrames(playerIndex);
	}

	if (endReached)
	{
		if (notes[0] + notes[2] > notes[1] + notes[3])
			m_pcsx2.resultScreen(stage, notes[0] + notes[2], difficulty != 3, multi);
		else
			m_pcsx2.resultScreen(stage, notes[1] + notes[3], false, multi);
	}
	while (true)
	{
		switch (GlobalFunctions::fileOverwriteCheck(filename + ".p2m2"))
		{
		case GlobalFunctions::ResultType::No:
			printf("%s\n", g_global.tabs.c_str());
			filename += "_T";
			break;
		case GlobalFunctions::ResultType::Yes:
			m_pcsx2.print(filename);
			return true;
		case GlobalFunctions::ResultType::Quit:
			printf("%s\n%sPCSX2 TAS was completed, but the file generation was cancelled.\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			return false;
		}
	}
}
