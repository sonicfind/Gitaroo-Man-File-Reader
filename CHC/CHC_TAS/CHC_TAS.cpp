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
#include "..\..\Header\pch.h"
#include "Global_Functions.h"
#include "CHC_TAS.h"
PCSX2TAS::Framefile PCSX2TAS::frameValues;
using namespace std;

bool chcTAS(CHC& song)
{
	TAS tas(song);
	return tas.buildTAS();
}

bool PCSX2TAS::loadValues(string filename)
{
	size_t pos = filename.find_last_of("\\"); string shortname = filename.substr(pos != string::npos ? pos + 1 : 0);
	FILE* p2m2v;
	(filename, ios::binary);
	if (!fopen_s(&p2m2v, filename.c_str(), "r"))
	{
		frameValues.name = filename;
		char ignore[300];
		bool error = false, failed = true;
		for (int framerate = 0; framerate < 3; framerate++)
		{
			fscanf_s(p2m2v, " %[^;]s", ignore, 300);
			for (int stage = 0; stage < 13; stage++)
			{
				for (int difficulty = 0; difficulty < 6; difficulty++)
				{
					if (!feof(p2m2v))
					{
						switch (valueInsertFromFile(p2m2v, frameValues.initialDisplacements[framerate][stage][difficulty], true))
						{
						case 1:
							failed = false;
							break;
						case -4:
							cout << global.tabs << "Error: Invalid input (" << global.invalid << ")\n";
						case 0:
							cout << global.tabs << "Read stopped at (& not including): " << (!framerate ? "59.94 " : (framerate & 1 ? "50.00 " : "Custom "));
							cout << "FPS [Initial Displacements] - Stage " << stage + 1;
							switch (difficulty)
							{
							case 0: cout << " (Hard) [1]\n" << global.tabs << endl; break;
							case 1: cout << " (Normal) [2]\n" << global.tabs << endl; break;
							case 2: cout << " (Easy) [3]\n" << global.tabs << endl; break;
							default: cout << " (Multiplayer) [" << difficulty + 1 << "]\n" << global.tabs << endl;
							}
							fclose(p2m2v);
							global.quit = true;
						}
					}
					else
					{
						cout << global.tabs << "Read stopped at (& not including): " << (!framerate ? "59.94 " : (framerate & 1 ? "50.00 " : "Custom "));
						cout << "FPS [Initial Displacements] - Stage " << stage + 1;
						switch (difficulty)
						{
						case 0: cout << " (Hard) [1]\n" << global.tabs << endl; break;
						case 1: cout << " (Normal) [2]\n" << global.tabs << endl; break;
						case 2: cout << " (Easy) [3]\n" << global.tabs << endl; break;
						default: cout << " (Multiplayer) [" << difficulty + 1 << "]\n" << global.tabs << endl;
						}
						fclose(p2m2v);
						global.quit = true;
					}
					if (global.quit)
						break;
				}
				if (global.quit)
					break;
			}
			if (global.quit)
				break;
			fscanf_s(p2m2v, " %[^;]s", ignore, 300);
			for (int stage = 0; stage < 13; stage++)
			{
				for (int difficulty = 0; difficulty < 6; difficulty++)
				{
					if (!feof(p2m2v))
					{
						switch (valueInsertFromFile(p2m2v, frameValues.frames[framerate][stage][difficulty]))
						{
						case 1:

							break;
						case -1:
							cout << global.tabs << "Error: No Negative values\n";
							cout << global.tabs << "Skipping " << (!framerate ? "59.94 " : (framerate & 1 ? "50.00 " : "Custom "));
							cout << "FPS: Stage " << stage + 1 << " - Value " << difficulty + 1 << endl << global.tabs << endl;
							error = true;
							break;
						case -4:
							cout << global.tabs << "Error: Invalid input (" << global.invalid << "\n";
						case 0:
							cout << global.tabs << "Read stopped at (& not including): " << (!framerate ? "59.94 " : (framerate & 1 ? "50.00 " : "Custom "));
							cout << "FPS [Frame Displacements] - Stage " << stage + 1;
							switch (difficulty)
							{
							case 0: cout << " (Hard) [1]\n" << global.tabs << endl; break;
							case 1: cout << " (Normal) [2]\n" << global.tabs << endl; break;
							case 2: cout << " (Easy) [3]\n" << global.tabs << endl; break;
							default: cout << " (Multiplayer) [" << difficulty + 1 << "]\n" << global.tabs << endl;
							}
							fclose(p2m2v);
							global.quit = true;
						}
					}
					else
					{
						cout << global.tabs << "Read stopped at (& not including): " << (!framerate ? "59.94 " : (framerate & 1 ? "50.00 " : "Custom "));
						cout << "FPS [Frame Displacements] - Stage " << stage + 1;
						switch (difficulty)
						{
						case 0: cout << " (Hard) [1]\n" << global.tabs << endl; break;
						case 1: cout << " (Normal) [2]\n" << global.tabs << endl; break;
						case 2: cout << " (Easy) [3]\n" << global.tabs << endl; break;
						default: cout << " (Multiplayer) [" << difficulty + 1 << "]\n" << global.tabs << endl;
						}
						fclose(p2m2v);
						global.quit = true;
					}
					if (global.quit)
						break;
				}
				if (global.quit)
					break;
			}
			if (global.quit)
				break;
		}
		fclose(p2m2v);
		if (!failed)
		{
			cout << global.tabs << shortname << " Loaded " << (error ? "with error(s)" : "") << endl;
			frameValues.use = true;
			return true;
		}
		else
		{
			cout << global.tabs << shortname << " Failed to Load" << endl;
			return false;
		}
	}
	else
	{
		cout << global.tabs << "\"" << shortname << "\" could not be found\n";
		return false;
	}
}

void PCSX2TAS::resultScreen(size_t stage, size_t notes, bool singleplayer, bool multi[2])
{
	unsigned toResult = 0;
	switch (stage)
	{
	case 0: 
		toResult = 65;
		break;
	case 1:
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
		players[0].resize(players[0].size() + toResult + notes + 306);
	else
		players[0].resize(players[0].size() + toResult);
	if (!singleplayer)
	{
		players[1].resize(players[0].size());
		if (multi[0]) players[2].resize(players[0].size());
		if (multi[1]) players[3].resize(players[0].size());
	}
}

void PCSX2TAS::print(string filename)
{
	FILE* outp2m2;
	fopen_s(&outp2m2, (filename + ".p2m2").c_str(), "wb");
	char multi[8] = { true, 0, 0, 0, players[1].size() > 0, players[2].size() > 0, players[3].size() > 0, 0 };
	fputc(version, outp2m2);
	fwrite(emulator, 1, 50, outp2m2);
	fwrite(author, 1, 255, outp2m2);
	fwrite(game, 1, 255, outp2m2);
	fwrite("\0", 1, 1, outp2m2);
	fwrite(multi, 1, 8, outp2m2);
	fwrite(&players[0].size(), 4, 1, outp2m2);
	fwrite("\0\0\0\0", 1, 4, outp2m2);
	for (unsigned index = 0; index < players[0].size(); index++)
	{
		for (long player = 0; player < 4; player++)
		{
			if (players[player].size())
			{
				TAS_Frame frame = players[player][index];
				unsigned char* sequence = new unsigned char[18]();
				if (!(frame.dpad & 128)) sequence[7] = 0xFF;
				if (!(frame.dpad & 64)) sequence[9] = 0xFF;
				if (!(frame.dpad & 32)) sequence[6] = 0xFF;
				if (!(frame.dpad & 16)) sequence[8] = 0xFF;
				if (!(frame.button & 128)) sequence[13] = 0xFF;
				if (!(frame.button & 64)) sequence[12] = 0xFF;
				if (!(frame.button & 32)) sequence[11] = 0xFF;
				if (!(frame.button & 16)) sequence[10] = 0xFF;
				sequence[0] = frame.startButton < 255 ? frame.startButton : frame.dpad;
				sequence[1] = frame.button;
				sequence[2] = frame.rightStickX;
				sequence[3] = frame.rightStickY;
				sequence[4] = frame.leftStickX;
				sequence[5] = frame.leftStickY;
				fwrite(sequence, 1, 18, outp2m2);
				delete[18] sequence;
			}
		}
	}
	fclose(outp2m2);
	cout << global.tabs << "PCSX2 TAS Completed." << endl;
}

size_t PCSX2TAS::insertFrames(size_t stage, size_t orientation, size_t difficulty, bool multi[2], size_t numFrames)
{
	if (framerate >= 60.00f)
	{
		if (!stage)
		{
			players[0].resize(546 + frameValues.frames[2][0][0] + numFrames);
			players[0][255].startButton = 247;	//Start - Title Screen
			players[0][545].button &= 191;			//X-button - Start Tutorial
		}
		else
		{
			players[0].resize(1390 + 6ULL * multi[0] + frameValues.frames[2][stage][difficulty + multi[0] + multi[1]] + numFrames);
			size_t index = 253 + 2ULL * multi[0];
			players[0][index++].startButton = 247;	index += 283ULL + multi[0];	//Start - Title Screen
			players[0][index++].dpad &= 239;										//D-pad Up - To Settings option
			players[0][index++].button &= 191; index += 207;						//X-button - Select Settings option
			players[0][index++].dpad &= 223; index++;								//D-pad Right - To Difficulty option
			players[0][index++].dpad &= 223;										//D-pad Right - To Load Game option
			players[0][index++].button &= 191;	index += 150ULL + multi[0];			//X-button - Select Load Game
			players[0][index++].button &= 191; index += 161;						//X-button - Select Memory Slot 1
			players[0][index++].button &= 191;	index += 49;						//X-button - Select Save File 1
			players[0][index++].button &= 191;	index += 164ULL + multi[0];			//X-button - Exit Loading Module
			if (difficulty == 1)
			{
				players[0].resize(players[0].size() + 4);
				players[0][index++].dpad &= 223;					//D-pad Right - To Difficulty option
				players[0][index++].dpad &= 191;					//D-pad Down - Difficulty to "Hard"
				players[0][index++].dpad &= 127;	index++;		//D-pad Left - To Vibration option
			}
			if (orientation)
			{
				players[0].resize(players[0].size() + 87);
				players[0][index++].dpad &= 127; index++;			//D-pad Left - To Volume option
				players[0][index++].dpad &= 127;					//D-pad Left - To Controller option
				players[0][index++].button &= 191;	index += 34;	//X-button - Select Controller option
				switch (orientation)
				{
				case 1:
					index += 3;		//Do nothing on the Controller screen
					break;
				case 2:
					players[0][index++].dpad &= 223; index += 2;	//D-pad Right - To Orientation 2
					break;
				case 3:
					players[0][index++].dpad &= 223; index++;		//D-pad Right - To Orientation 2
					players[0][index++].dpad &= 223;				//D-pad Right - To Orientation 3
				}
				players[0][index++].button &= 239;	index += 45;	// /\-button - Exit Controller screen
			}
			players[0][index++].button &= 239;	index += 111ULL + multi[0];	// /\-button - Exit Settings menu / To Main menu
			switch (difficulty)
			{
			case 0:		//Master
				players[0].resize(players[0].size() + 1);
				players[0][index++].dpad &= 191;				//D-pad Down - To Master Play option
			case 1:		//Hard		Stay on Single Play option
			case 2:		//Normal	Stay on Single Play option
				break;
			case 3:
				players[0].resize(players[0].size() + 3);
				players[0][index++].dpad &= 191; index++;		//D-pad Down - To Master Play option
				players[0][index++].dpad &= 191;				//D-pad Down - To VS Play option
			}
			players[0][index++].button &= 191;					//X-button - Select Current option
			size_t stageSelectIndex = players[0].size();
			if (difficulty != 3)
			{
				size_t stValue = (stage < 11 ? stage - 1 : stage - 11);
				players[0].resize(players[0].size() + 143 + 2 * stValue); index += 143;
				for (size_t stageIndex = 0; stageIndex < stValue; stageIndex++)
				{
					players[0][index++].dpad &= 223;	//D-pad Right - Stage Scrolling
					if (stage - stageIndex - 2) index++;
				}
			}
			else
			{
				players[0].resize(players[0].size() + 203ULL + 33ULL * multi[0] + 32ULL * multi[1] - 4 * ((stage - 1) / 3) + 2 * (stage / 10));
				players[0][index++].button &= 191; index += 48;	//X-button - MP Intro Screen
				for (size_t stageIndex = 0, mStage = stage - ((stage - 1) / 3) - 1; stageIndex < mStage; stageIndex++)
				{
					players[0][index++].dpad &= 191;				//D-pad Down - Stage Scrolling
					if (mStage - stageIndex - 1) index++;
				}
				players[0][index++].button &= 191; index += 2 * (9 - stage + 16 * ((unsigned long long)multi[0] + multi[1]) - ((stage - 1) / 3) + (stage / 10)) + 1;		//X-button - Stage Selection
				players[0][index++].button &= 191; index++;		//X-button - Character Selection
				players[0][index++].button &= 191;	index++;		//X-button - Health Handicap Selection
			}
			players[0][index].button &= 191;						//X-button - Stage Start
			if (difficulty == 3) //If Multiplayer
			{
				players[1].resize(players[0].size()); index = stageSelectIndex + 203 + 33ULL * multi[0] + 32ULL * multi[1] - 4 * ((stage - 1) / 3) + 2 * (stage / 10);
				players[1][index++].button &= 191; index++;		//X-button - Character Selection
				players[1][index].button &= 191;					//X-button - Health Handicap Selection
				if (multi[1]) //If 4-Player
				{
					players[2] = players[1];
					players[3] = players[1];
				}
				else if (multi[0]) //If 3-Player
				{
					players[2] = players[1];
				}
			}
		}
	}
	else if (framerate >= 59.94f)
	{
		if(!stage)
		{
			players[0].resize(546 + frameValues.frames[0][0][0] + numFrames);
			players[0][255].startButton = 247;	//Start - Title Screen
			players[0][545].button &= 191;			//X-button - Start Tutorial
		}
		else
		{
			players[0].resize(1429 + 16ULL * multi[0] + frameValues.frames[0][stage][difficulty + multi[0] + multi[1]] + numFrames);
			size_t index = 255ULL + 3ULL * multi[0];
			players[0][index++].startButton = 247;	index += 289 + 2ULL * multi[0];	//Start - Title Screen
			players[0][index++].dpad &= 239;										//D-pad Up - To Settings option
			players[0][index++].button &= 191; index += 215 + 3ULL * multi[0];						//X-button - Select Settings option
			players[0][index++].dpad &= 223; index++;								//D-pad Right - To Difficulty option
			players[0][index++].dpad &= 223;										//D-pad Right - To Load Game option
			players[0][index++].button &= 191;	index += 156 + 3ULL * multi[0];			//X-button - Select Load Game
			players[0][index++].button &= 191; index += 161;						//X-button - Select Memory Slot 1
			players[0][index++].button &= 191;	index += 49;						//X-button - Select Save File 1
			players[0][index++].button &= 191;	index += 173 + 3ULL * multi[0];			//X-button - Exit Loading Module
			if (difficulty == 1)
			{
				players[0].resize(players[0].size() + 4);
				players[0][index++].dpad &= 223;					//D-pad Right - To Difficulty option
				players[0][index++].dpad &= 191;					//D-pad Down - Difficulty to "Hard"
				players[0][index++].dpad &= 127;	index++;		//D-pad Left - To Vibration option
			}
			if (orientation)
			{
				players[0].resize(players[0].size() + 87);
				players[0][index++].dpad &= 127; index++;			//D-pad Left - To Volume option
				players[0][index++].dpad &= 127;					//D-pad Left - To Controller option
				players[0][index++].button &= 191;	index += 34;	//X-button - Select Controller option
				switch (orientation)
				{
				case 1:
					index += 3;		//Do nothing on the Controller screen
					break;
				case 2:
					players[0][index++].dpad &= 223; index += 2;	//D-pad Right - To Orientation 2
					break;
				case 3:
					players[0][index++].dpad &= 223; index++;		//D-pad Right - To Orientation 2
					players[0][index++].dpad &= 223;				//D-pad Right - To Orientation 3
				}
				players[0][index++].button &= 239;	index += 45;	// /\-button - Exit Controller screen
			}
			players[0][index++].button &= 239;	index += 117 + 2ULL * multi[0];	// /\-button - Exit Settings menu / To Main menu
			if (stage < 11)
			{
				switch (difficulty)
				{
				case 0:		//Master
					players[0].resize(players[0].size() + 1);
					players[0][index++].dpad &= 191;				//D-pad Down - To Master Play option
				case 1:		//Hard		Stay on Single Play option
				case 2:		//Normal	Stay on Single Play option
					break;
				case 3:		//Multi-
					players[0].resize(players[0].size() + 3);
					players[0][index++].dpad &= 191; index++;		//D-pad Down - To Master Play option
					players[0][index++].dpad &= 191;				//D-pad Down - To VS Play option
				}
			}
			players[0][index++].button &= 191;					//X-button - Select Current option
			size_t stageSelectIndex = index;
			if (difficulty != 3) //Single Player
			{
				size_t stValue = (stage < 11 ? stage - 1 : stage - 11);
				if (stage != 8)
					players[0].resize(players[0].size() + 152 + 2 * stValue);
				else
					players[0].resize(players[0].size() + 153 + 2 * stValue);
				index += 152;
				for (size_t stageIndex = 0; stageIndex < stValue; stageIndex++)
				{
					players[0][index++].dpad &= 223;	//D-pad Right - Stage Scrolling
					if (stage - stageIndex - 2) index++;
				}
				if (stage == 8)
					index++;
				players[0][index].button &= 191;		//X-button - Stage Start
			}
			else //Multiplayer
			{
				size_t scrollVal = stage - ((stage - 1) / 3);
				if (multi[1])
					players[0].resize(players[0].size() + 270U);
				else if (multi[0])
					players[0].resize(players[0].size() + 238U);
				else if (stage == 10)
					players[0].resize(players[0].size() + 205U);
				else
					players[0].resize(players[0].size() + 203U);
				index += 141 + 3ULL * multi[0];
				players[0][index++].button &= 191; index += 47;	//X-button - MP Intro Screen
				for (size_t stageIndex = 0; stageIndex < scrollVal; stageIndex++)
				{
					index++;
					if (stageIndex + 1 != scrollVal)
						players[0][index++].dpad &= 191;			//D-pad Down - Stage Scrolling
				}
				players[0][index++].button &= 191;		//X-button - Stage Selection
				if (multi[1])
					index += 74 - (scrollVal << 1) - (stage > 1);
				else if (multi[0])
					index += 42 - (scrollVal << 1) - (stage > 1);
				else if (stage < 5)
					index += 10 - (scrollVal << 1) - (stage > 1);
				else
					index++;
				players[0][index++].button &= 191; index++;		//X-button - Multiplayer Character Selection
				players[0][index++].button &= 191; index++;		//X-button - Health Handicap Selection/Stage Start
			}
			//If Multiplayer
			if (difficulty == 3)
			{
				players[1].resize(players[0].size());
				index = stageSelectIndex + 191;
				if (multi[1])
					index += 75;
				else if (multi[0])
					index += 43;
				else if (stage > 5)
					index += (stage - ((stage - 1) / 3)) << 1;
				else
					index += 8;
				players[1][index++].button &= 191;		//X-button - Multiplayer Character Selection
				players[1][++index].button &= 191;		//X-button - Health Handicap Selection
				if (multi[0]) //If Player 3
				{
					players[2] = players[1];
					if (multi[1])	//If Player 4
						players[3] = players[1];
				}
			}
		}
	}
	return players[0].size() - numFrames;
}

bool TAS::buildTAS()
{
	if (song.imc[0] == 0)
	{
		cout << global.tabs << song.shortname << ".CHC is not compatible for PS2" << endl;
		return false;
	}
	banner(" " + song.shortname + " - TAS creation ");
	auto load = [&]()
	{
		do
		{
			string p2m2vTemp = "";
			cout << global.tabs << "Drag & drop a valid P2M2V file (or type only 'Q' to back out or only 'B' for base values): ";
			switch (filenameInsertion(p2m2vTemp, "b"))
			{
			case -1:
				return false;
			case 1:
				global.quit = true;
				break;
			case 0:
				if (p2m2vTemp.find(".P2M2V") == string::npos)
					p2m2vTemp += ".P2M2V";
				if (pcsx2.loadValues(p2m2vTemp))
					global.quit = true;
			}
		} while (!global.quit);
		global.quit = false;
		return true;
	};
	if (!PCSX2TAS::frameValues.use)
	{
		FILE* p2m2v;
		if (!fopen_s(&p2m2v, PCSX2TAS::frameValues.name.c_str(), "r"))
		{
			fclose(p2m2v);
			size_t pos = PCSX2TAS::frameValues.name.find_last_of("\\");
			string shortname = PCSX2TAS::frameValues.name.substr(pos != string::npos ? pos + 1 : 0);
			do
			{
				cout << global.tabs << "Use \"" << shortname << "\"? [Y/N]\n";
				switch (menuChoices("yn"))
				{
				case 0:
					pcsx2.loadValues();
					global.quit = true;
					break;
				case 1:
					if (load())
					{
						global.quit = true;
						break;
					}
				case -1:
					cout << global.tabs << endl;
					cout << global.tabs << "TAS creation cancelled" << endl;
					return false;
				}
			} while (!global.quit);
			global.quit = false;
		}
	}
	else if (!pcsx2.loadValues() && !load())
		return false;
	do
	{
		cout << global.tabs << "Select a framerate ('Q' to back out to Main Menu)\n";
		cout << global.tabs << "0 - NTSC (59.94)\n";
		cout << global.tabs << "1 - PAL  (50.00) [NOT YET SUPPORTED]\n";
		cout << global.tabs << "2 - NTSC-C (60.00)\n";
		cout << global.tabs << "3 - NTSC-Custom\n";
		switch (menuChoices("0123"))
		{
		case 0:
			cout << global.tabs << endl;
			global.quit = true;
			break;
		case 1:
			cout << global.tabs << "I do not own the PAl version, so optimizing for it is not something I can do just yet.\n";
			cout << global.tabs << endl;
			/*
			pcsx2.framerate = 50.00;
			global.quit = true;*/
			break;
		case 2:
			cout << global.tabs << endl;
			pcsx2.framerate = 60.00f;
			global.quit = true;
			break;
		case 3:
			cout << global.tabs << endl;
			do
			{
				cout << global.tabs << "Provide an FPS value (minimum: 59.94) ('Q' to back out to Main Menu)\n";
				cout << global.tabs << "Input: ";
				switch (valueInsert(pcsx2.framerate, false, 59.94f))
				{
				case 0:
					cout << global.tabs << endl;
					cout << global.tabs << "TAS creation cancelled" << endl;
					return true;
				case -1:
					cout << global.tabs << "What the hell is a negative FPS?!?\n" << global.tabs << endl;
					break;
				case -2:
					cout << global.tabs << "Given FPS is lower than the 59.94 FPS minimum\n" << global.tabs << endl;
					break;
				case -4:
					cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
				}
				cout << global.tabs << endl;
			} while (!global.quit);
			break;
		case -1:
			cout << global.tabs << endl;
			cout << global.tabs << "TAS creation cancelled" << endl;
			return true;
		}
	} while (!global.quit);
	global.quit = false;
	size_t stage = 0;
	size_t difficulty = 0;
	if (song.shortname.find("ST00") != string::npos)
	{
		stage = 0;
		difficulty = 0;
	}
	else if (song.shortname.find("ST01") != string::npos)
	{
		stage = 1;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST02") != string::npos)
	{
		stage = 2;
		if (song.shortname.find("HE") != string::npos)
			difficulty = 1;
		else if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST03") != string::npos)
	{
		stage = 3;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST04") != string::npos)
	{
		stage = 4;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST05") != string::npos)
	{
		stage = 5;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST06") != string::npos)
	{
		stage = 6;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST07") != string::npos)
	{
		stage = 7;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST08") != string::npos)
	{
		stage = 8;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST09") != string::npos)
	{
		stage = 9;
		if (song.shortname.find("H2") != string::npos)
			difficulty = 1;
		else if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST10") != string::npos)
	{
		stage = 10;
		if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("E") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST11") != string::npos)
	{
		stage = 11;
		if (song.shortname.find("E") != string::npos || song.shortname.find("HA") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("HB") != string::npos)
			difficulty = 1;
		else if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else if (song.shortname.find("ST12") != string::npos)
	{
		stage = 12;
		if (song.shortname.find("E") != string::npos || song.shortname.find("HA") != string::npos)
			difficulty = 2;
		else if (song.shortname.find("HB") != string::npos)
			difficulty = 1;
		else if (song.shortname.find("H") != string::npos)
			difficulty = 0;
		else if (song.shortname.find("M") != string::npos)
			difficulty = 3;
		else
			difficulty = 1;
	}
	else
	{
		do
		{
			cout << global.tabs << "Which stage will be replaced? [1-10] ('Q' to back out to Main Menu)\n";
			cout << global.tabs << "Input: ";
			switch (valueInsert(stage, false, (size_t)1, (size_t)10))
			{
			case 1:
				global.quit = true;
				break;
			case -1:
			case -2:
				cout << global.tabs << "Given value cannot be less than 1\n" << global.tabs << endl;
				break;
			case -3:
				cout << global.tabs << "Given value cannot be greater than 10\n" << global.tabs << endl;
				break;
			case -4:
				cout << global.tabs << "\"" << global.invalid << "\" is not a valid response\n" << global.tabs << endl;
				break;
			case 0:
				cout << global.tabs << endl;
				cout << global.tabs << "TAS creation cancelled" << endl;
				return true;
			}
		} while (!global.quit);
		global.quit = false;
		if (toupper(song.shortname.back()) == 'E')
			difficulty = 2;
		else if (toupper(song.shortname.back()) == 'N')
			difficulty = 1;
		else if (toupper(song.shortname.back()) == 'H')
			difficulty = 0;
		else if (toupper(song.shortname.back()) == 'M')
			difficulty = 3;
		else
		{
			do
			{
				cout << global.tabs << "Which difficulty is this chart for? ('Q' to back out to Main Menu)\n";
				cout << global.tabs << " 0 - Hard/Master Play\n";
				cout << global.tabs << " 1 - Normal\n";
				cout << global.tabs << " 2 - Easy\n";
				cout << global.tabs << " 3 - Multiplayer\n";
				difficulty = menuChoices("0123");
				switch (difficulty)
				{
				case -1:
					cout << global.tabs << endl;
					cout << global.tabs << "TAS creation cancelled" << endl;
					return true;
				case -3:
					cout << global.tabs << endl;
				case -2:
					break;
				default:
					cout << global.tabs << endl;
					global.quit = true;
				}
			} while (!global.quit);
			global.quit = false;
		}
	}
	bool multi[2] = { false, false };
	if (difficulty == 3 && stage != 10)
	{
		do
		{
			cout << global.tabs << "How many players for this multiplayer TAS [2/3/4]? ('Q' to back out to Main Menu)\n";
			switch (menuChoices("234"))
			{
			case 2:
				multi[1] = true;
			case 1:
				multi[0] = true;
			case 0:
				cout << global.tabs << endl;
				global.quit = true;
				break;
			case -1:
				cout << global.tabs << endl;
				cout << global.tabs << "TAS creation cancelled" << endl;
				return true;
			}
		} while (!global.quit);
		global.quit = false;
	}
	cout << global.tabs << "Stage " << stage << " - Diff. " << (unsigned int)difficulty << ": ";
	char framerateIndex;
	if (pcsx2.framerate == 59.94f)
		framerateIndex = 0;
	else if (pcsx2.framerate == 50.00f)
		framerateIndex = 1;
	else
		framerateIndex = 2;
	cout << PCSX2TAS::frameValues.frames[framerateIndex][stage][difficulty + multi[0] + multi[1]] << " frames |";
	cout << " Displacement: " << PCSX2TAS::frameValues.initialDisplacements[framerateIndex][stage][difficulty + multi[0] + multi[1]] << " samples\n" << global.tabs << endl;
	cout << global.tabs << "Type the name of the author [255 character limit] (';' to notate the end of the name [for multi-step usage]) ('Q' to back out to Main Menu)\n";
	cout << global.tabs << "Input: ";
	if (global.multi)
	{
		cin.putback(global.input);
		cout << '*';
	}
	cin >> ws >> global.input;
	if (tolower(global.input) == 'q' && cin.peek() == '\n')
	{
		cout << global.tabs << '\n' << global.tabs << "TAS creation cancelled" << endl;
		return true;
	}
	else
	{
		unsigned char index = 0;
		for (; global.input != '\n' && global.input != ';'; index++)
		{
			if (index != 255)
			{
				pcsx2.author[index] = global.input;
				cin >> global.input;
			}
			else
			{
				pcsx2.author[254] = 0;
				if (global.multi)
					cout << pcsx2.author << endl;
				global.multi = false;
				cout << endl;
				cout << global.tabs << "Author insertion overflow - Input buffer flushed\n";
				cin.sync();
				break;
			}
		}
		pcsx2.author[254] = 0;
		if (global.multi)
			cout << pcsx2.author << endl;
		cout << global.tabs << endl;
		if (global.input == '\n')
			global.multi = false;
		else
		{
			while (global.input == ';')
			{
				do
				{
					cin >> global.input;
				} while (global.input == ' ');
				if (global.input != '\n')
					global.multi = true;
				else
					global.multi = false;
			}
		}
	}
	
	size_t orientation;
	do
	{
		cout << global.tabs << "Which orientation for all guard phrases? ('Q' to back out to Main Menu)\n";
		cout << global.tabs << "0 - No Orientation Change\n";
		cout << global.tabs << "1 - No Change - Open Config Menu\n";
		cout << global.tabs << "2 - Change to Orientation 2\n";
		cout << global.tabs << "3 - Change to Orientation 3\n";
		orientation = menuChoices("0123");
		switch (orientation)
		{
		case -1:
			cout << global.tabs << endl;
			cout << global.tabs << "TAS creation cancelled" << endl;
			return true;
		case -2:
			break;
		default:
			cout << global.tabs << endl;
			global.quit = true;
		}
	} while (!global.quit);
	global.quit = false;
	List<unsigned long> sectionIndexes;
	do
	{
		cout << global.tabs << "Type the number for each section that you wish to TAS - in chronological order and w/ spaces in-between\n";
		for (unsigned sectIndex = 0; sectIndex < song.numSections; sectIndex++)
			cout << global.tabs << sectIndex << " - " << song.sections[sectIndex].getName() << '\n';
		if (sectionIndexes.size())
		{
			cout << global.tabs << "Current List: ";
			for (unsigned index = 0; index < sectionIndexes.size(); index++)
				cout << song.sections[sectionIndexes[index]].getName() << " ";
			cout << '\n';
		}
		switch (vectorValueInsert(sectionIndexes, "yntmv", song.numSections))
		{
		case -1:

			break;
		case 0:
			cout << global.tabs << endl;
			cout << global.tabs << "TAS creation cancelled" << endl;
			return true;
		case 2:
		case 4:
		case 5:
		case 6:
			if (sectionIndexes.size())
			{
				global.quit = true;
				break;
			}
		case 1:
			if (!sectionIndexes.size())
			{
				cout << global.tabs << endl;
				do
				{
					cout << global.tabs << "No sections have been selected. Quit TAS creation? [Y/N]\n";
					switch (menuChoices("yn"))
					{
					case -1:
					case 0:
						cout << global.tabs << endl;
						cout << global.tabs << "TAS creation cancelled" << endl;
						return true;
					case 1:
						cout << global.tabs << endl;
						global.quit = true;
					}
				} while (!global.quit);
				global.quit = false;
			}
			else
				global.quit = true;
			break;
		case 3:
			cout << global.tabs << endl;
			cout << global.tabs << "Ok... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n";
			cout << global.tabs << endl;
		}
	} while (!global.quit);
	global.quit = false;
	string filename = song.name.substr(0, song.name.length() - 4);
	if (difficulty == 3)
		filename += "_" + to_string(int(pcsx2.framerate)) + "_" + to_string(orientation) + "_" + to_string(multi[0] + multi[1] + 2) + 'P';
	else
		filename += "_" + to_string(int(pcsx2.framerate)) + "_" + to_string(orientation) + "_SP";
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
		long position = 0;
		//0 - Technical
		//1 - Visuals
		//2 - Mixed
		size_t visualType = 0;
		long sustainLimit = 0;
		SectPoint(long pos, size_t visuals = 0, long sus = 0) : position(pos), visualType(visuals), sustainLimit(sus) {}
	};
	List<NotePoint> timeline[4];
	List<SectPoint> markers[4];
	const long double SAMPLES_PER_FRAME = 48000.0L / (pcsx2.framerate == 59.94f ? 59.94L : pcsx2.framerate);
	unsigned long totalDuration = 0;
	bool endReached = false;
	unsigned short notes[4] = { 0, 0, 0, 0 };
	long position = PCSX2TAS::frameValues.initialDisplacements[framerateIndex][stage][difficulty + multi[0] + multi[1]];
	//Places every single note that will appear in all chosen sections
	//into one huge timeline.
	for (unsigned sectIndex = 0; sectIndex < sectionIndexes.size(); sectIndex++)
	{
		SongSection& section = song.sections[sectionIndexes[sectIndex]];
		cout << global.tabs << section.getName() << endl;
		if (!stage || (section.getPhase() != SongSection::Phase::INTRO && !strstr(section.getName(), "BRK"))) //If not INTRO phase or BRK section
		{
			if (section.getPhase() == SongSection::Phase::END || sectionIndexes[sectIndex] + 1 == song.numSections)
				endReached = true; // If END phase or last section
			{
				//0 - Technical
				//1 - Visuals
				//2 - Mixed
				size_t visualType = 0;
				float sustainCoeffienct = 1; //Sustain limit set to a base of 1 beat
				for (size_t playerIndex = 0; playerIndex < section.getNumPlayers() && !global.quit; playerIndex++)
				{
					if (!(playerIndex & 1) || difficulty == 3)
					{
						for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && !global.quit; chartIndex++)
						{
							if (section.getChart(playerIndex * (size_t)section.getNumCharts() + chartIndex).getNumPhrases())
							{
								do
								{
									cout << global.tabs << "How should " << section.getName() << "'s phrase bars be played?\n";
									cout << global.tabs << "T - Technicality (Release at the end of all sustains)\n";
									cout << global.tabs << "V - Visually (Hold past/through the end of all sustains)\n";
									cout << global.tabs << "M - Mixed (Release only on sustain that exceeds a defined length)\n";
									visualType = menuChoices("tvm");
									switch (visualType)
									{
									case -1:
										cout << global.tabs << endl;
										cout << global.tabs << "TAS creation cancelled" << endl;
										return true;
									case -2:
										break;
									default:
										cout << global.tabs << endl;
										if (visualType == 2)
										{
											do
											{
												cout << global.tabs << "Provide a value for the sustain limit coeffienct.\n";
												cout << global.tabs << "AKA, how many beats must a sustain be before the TAS programs a sustain-release at the end of the note?\n";
												cout << global.tabs << "Value can range from 0.5 to 4.0 [Default is 1].\n";
												cout << global.tabs << "Input: ";
												switch (valueInsert(sustainCoeffienct, false, 0.5f, 4.0f))
												{
												case 1:
													global.quit = true;
													break;
												case 0:
													cout << global.tabs << endl;
													cout << global.tabs << "TAS creation cancelled" << endl;
													return true;
												case -1:
												case -2:
													cout << global.tabs << "Provided value *must* be greater than or equal to 0.5.\n" << global.tabs << endl;
													break;
												case -3:
													cout << global.tabs << "Provided value *must* be less than or equal to 4.0.\n" << global.tabs << endl;
													break;
												case -4:
													cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
													cin.clear();
												}
											} while (!global.quit);
										}
										global.quit = true;
									}
								} while (!global.quit);
							}
						}
					}
				}
				global.quit = false;
				markers[0].emplace_back(position, visualType, (long)round(sustainCoeffienct * SAMPLES_PER_MIN / section.getTempo()));
				if (difficulty == 3)
				{
					markers[1].emplace_back(position, visualType, (long)round(sustainCoeffienct * SAMPLES_PER_MIN / section.getTempo()));
					if (multi[0])
						markers[2].emplace_back(position, visualType, (long)round(sustainCoeffienct * SAMPLES_PER_MIN / section.getTempo()));
					if (multi[1])
						markers[3].emplace_back(position, visualType, (long)round(sustainCoeffienct * SAMPLES_PER_MIN / section.getTempo()));
				}
			}
			//Marking where in each list the current section starts
			size_t startIndex[4] = { timeline[0].size(), timeline[1].size(), timeline[2].size(), timeline[3].size() };
			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); playerIndex++)
				{
					Chart& chart = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
					//Player 1 will always be TAS'd, it just depends on how
					if (!(playerIndex & 1))
						currentPlayer = (multi[0] ? playerIndex : 0);
					else if (difficulty == 3)
						currentPlayer = (multi[1] ? playerIndex : 1);
					else
						continue;
					List<NotePoint>& player = timeline[currentPlayer];
					size_t index = startIndex[currentPlayer];
					for (size_t i = 0; i < chart.getNumGuards(); i++)
					{
						long pos = chart.getGuard(i).getPivotAlpha() + chart.getPivotTime() + position;
						while (index < player.size())
						{
							if (pos <= player[index].position)
								break;
							else
								index++;
						}
						player.emplace(index, 1, pos, &chart.getGuard(i), i, i + 1 == chart.getNumGuards());
						notes[currentPlayer]++;
						index++;
					}
					index = startIndex[currentPlayer];
					for (size_t i = 0; i < chart.getNumPhrases(); i++)
					{
						Phrase& phrase = chart.getPhrase(i);
						long pos = phrase.getPivotAlpha() + chart.getPivotTime() + position;
						while (index < player.size())
						{
							if (pos <= player[index].position)
								break;
							else
								index++;
						}
						//Combine all pieces into one Note
						while (i < chart.getNumPhrases())
						{
							if (!chart.getPhrase(i).getEnd())
								i++;
							else
							{
								if (i + 1 != chart.getNumPhrases())
								{
									if (markers[currentPlayer].back().visualType == 1)
									{
										if (chart.getPhrase(i + 1).getPivotAlpha() - chart.getPhrase(i).getEndAlpha() < markers[currentPlayer].back().sustainLimit)
											phrase.changeEndAlpha(chart.getPhrase(i + 1).getPivotAlpha() - long(SAMPLES_PER_FRAME));
										else
											phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha() + long(2 * SAMPLES_PER_FRAME));
									}
									else if (markers[currentPlayer].back().visualType == 2
											 && chart.getPhrase(i + 1).getPivotAlpha() - phrase.getPivotAlpha() < markers[currentPlayer].back().sustainLimit)
										phrase.changeEndAlpha(chart.getPhrase(i + 1).getPivotAlpha() - long(SAMPLES_PER_FRAME));
									else
										phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
								}
								else if (markers[currentPlayer].back().visualType != 1
										|| !phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha() - long(5.0 * SAMPLES_PER_FRAME)))
									phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
								break;
							}
						}
						player.emplace(index, 1, pos, &phrase, i, i + 1 == chart.getNumPhrases());
						notes[currentPlayer]++;
						index++;
					}
					index = startIndex[currentPlayer];
					if (chart.getNumTracelines() > 1)
					{
						for (size_t i = 0; i < chart.getNumTracelines(); i++)
						{
							long pos = chart.getTraceline(i).getPivotAlpha() + chart.getPivotTime() + position;
							while (index < player.size())
							{
								if (pos <= player[index].position)
									break;
								else
									index++;
							}
							player.emplace(index, 1, pos, &chart.getTraceline(i), i, i + 1 == chart.getNumTracelines());
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
	if (stage == 0 && song.shortname.find("ST00B") == string::npos)
	{
		endReached = false;
		string tutorialName = song.name.substr(0, song.name.length() - 5) + 'B';
		try
		{
			tutorial = new CHC(tutorialName);
		}
		catch (...)
		{
			do
			{
				tutorialName.clear();
				cout << global.tabs << "Provide the ST00B.CHC file to use for this TAS (Or 'N' to only TAS ST00A): ";
				switch (filenameInsertion(tutorialName, "n"))
				{
				case -1:
					cout << global.tabs << endl;
					cout << global.tabs << "TAS creation cancelled" << endl;
					return false;
				case 1:
					global.quit = true;
					break;
				case 0:
					if (tutorialName.find("ST00B") != string::npos)
					{
						if (tutorialName.find(".CHC") != string::npos)
							tutorialName = tutorialName.substr(0, tutorialName.length() - 4);
						try
						{
							tutorial = new CHC(tutorialName);
							cout << global.tabs << endl;
							global.quit = true;
						}
						catch (...)
						{
							cout << global.tabs << "The given ST00B could not be successfully read." << endl;
						}
					}
					else
						cout << global.tabs << "Only an ST00B.CHC file can be accepted." << endl;
				}
			} while (!global.quit);
			global.quit = false;
		}
		
		if (tutorial != nullptr)
		{
			cout << global.tabs << "Stage ST00B: ";
			cout << PCSX2TAS::frameValues.frames[framerateIndex][0][1] << " frames during intermission |";
			cout << " Post-intermission Displacement: " << PCSX2TAS::frameValues.initialDisplacements[framerateIndex][0][1] << " samples\n" << global.tabs << endl;
			sectionIndexes.clear();
			position += long(((long)PCSX2TAS::frameValues.frames[framerateIndex][0][1] - 1000) * SAMPLES_PER_FRAME) + PCSX2TAS::frameValues.initialDisplacements[framerateIndex][0][1];
			do
			{
				cout << global.tabs << "Type the number for each section that you wish to TAS from ST00B - in chronological order and w/ spaces in-between\n";
				for (unsigned sectIndex = 0; sectIndex < tutorial->numSections; sectIndex++)
					cout << global.tabs << sectIndex << " - " << tutorial->sections[sectIndex].getName() << '\n';
				if (sectionIndexes.size())
				{
					cout << global.tabs << "Current List: ";
					for (unsigned index = 0; index < sectionIndexes.size(); index++)
						cout << tutorial->sections[sectionIndexes[index]].getName() << " ";
					cout << '\n';
				}
				switch (vectorValueInsert(sectionIndexes, "yntmv", tutorial->numSections))
				{
				case -1:

					break;
				case 0:
					cout << global.tabs << endl;
					cout << global.tabs << "TAS creation cancelled" << endl;
					return true;
				case 2:
				case 4:
				case 5:
				case 6:
					if (sectionIndexes.size())
					{
						global.quit = true;
						break;
					}
				case 1:
					if (!sectionIndexes.size())
					{
						cout << global.tabs << endl;
						do
						{
							cout << global.tabs << "No sections have been selected. Quit TAS creation? [Y/N]\n";
							switch (menuChoices("yn"))
							{
							case -1:
							case 0:
								cout << global.tabs << endl;
								cout << global.tabs << "TAS creation cancelled" << endl;
								return true;
							case 1:
								cout << global.tabs << endl;
								global.quit = true;
							}
						} while (!global.quit);
						global.quit = false;
					}
					else
						global.quit = true;
					break;
				case 3:
					cout << global.tabs << endl;
					cout << global.tabs << "Ok... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n";
					cout << global.tabs << endl;
				}
			} while (!global.quit);
			global.quit = false;
			for (unsigned sectIndex = 0; sectIndex < sectionIndexes.size(); sectIndex++)
			{
				SongSection& section = tutorial->sections[sectionIndexes[sectIndex]];
				cout << global.tabs << section.getName() << endl;
				if (sectionIndexes[sectIndex] + 1 == tutorial->numSections)
					endReached = true; // If END phase or last section
				{
					//0 - Technical
					//1 - Visuals
					//2 - Mixed
					size_t visualType = 0;
					float sustainCoeffienct = 1; //Sustain limit set to a base of 1 beat
					for (size_t playerIndex = 0; playerIndex < section.getNumPlayers() && !global.quit; playerIndex += 2)
					{
						for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && !global.quit; chartIndex++)
						{
							if (section.getChart(playerIndex * (size_t)section.getNumCharts() + chartIndex).getNumPhrases())
							{
								do
								{
									cout << global.tabs << "How should " << section.getName() << "'s phrase bars be played?\n";
									cout << global.tabs << "T - Technicality (Release at the end of all sustains)\n";
									cout << global.tabs << "V - Visually (Hold past/through the end of all sustains)\n";
									cout << global.tabs << "M - Mixed (Release only on sustain that exceeds a defined length)\n";
									visualType = menuChoices("tvm");
									switch (visualType)
									{
									case -1:
										cout << global.tabs << endl;
										cout << global.tabs << "TAS creation cancelled" << endl;
										return true;
									case -2:
										break;
									default:
										cout << global.tabs << endl;
										if (visualType == 2)
										{
											do
											{
												cout << global.tabs << "Provide a value for the sustain limit coeffienct.\n";
												cout << global.tabs << "AKA, how many beats must a sustain be before the TAS programs a sustain-release at the end of the note?\n";
												cout << global.tabs << "Value can range from 0.5 to 4.0 [Default is 1].\n";
												cout << global.tabs << "Input: ";
												switch (valueInsert(sustainCoeffienct, false, 0.5f, 4.0f))
												{
												case 1:
													global.quit = true;
													break;
												case 0:
													cout << global.tabs << endl;
													cout << global.tabs << "TAS creation cancelled" << endl;
													return true;
												case -1:
												case -2:
													cout << global.tabs << "Provided value *must* be greater than or equal to 0.5.\n" << global.tabs << endl;
													break;
												case -3:
													cout << global.tabs << "Provided value *must* be less than or equal to 4.0.\n" << global.tabs << endl;
													break;
												case -4:
													cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
													cin.clear();
												}
											} while (!global.quit);
										}
										global.quit = true;
									}
								} while (!global.quit);
							}
						}
					}
					global.quit = false;
					markers[0].emplace_back(position, visualType, (long)round(sustainCoeffienct * SAMPLES_PER_MIN / section.getTempo()));
				}
				size_t startIndex = timeline[0].size();
				for (unsigned chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
				{
					for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); playerIndex++)
					{
						Chart& chart = section.getChart(chartIndex);
						size_t index = startIndex;
						for (size_t i = 0; i < chart.getNumGuards(); i++)
						{
							long pos = chart.getGuard(i).getPivotAlpha() + chart.getPivotTime() + position;
							while (index < timeline[0].size())
							{
								if (pos <= timeline[0][index].position)
									break;
								else
									index++;
							}
							timeline[0].emplace(index, 1, pos, &chart.getGuard(i), i, i + 1 == chart.getNumGuards());
							notes[currentPlayer]++;
							index++;
						}
						index = startIndex;
						for (size_t i = 0; i < chart.getNumPhrases(); i++)
						{
							Phrase& phrase = chart.getPhrase(i);
							long pos = phrase.getPivotAlpha() + chart.getPivotTime() + position;
							while (index < timeline[0].size())
							{
								if (pos <= timeline[0][index].position)
									break;
								else
									index++;
							}
							//Combine all pieces into one Note
							while (i < chart.getNumPhrases())
							{
								if (!chart.getPhrase(i).getEnd())
									i++;
								else
								{
									if (i + 1 != chart.getNumPhrases())
									{
										if (markers[currentPlayer].back().visualType == 1)
										{
											if (chart.getPhrase(i + 1).getPivotAlpha() - chart.getPhrase(i).getEndAlpha() < markers[currentPlayer].back().sustainLimit)
												phrase.changeEndAlpha(chart.getPhrase(i + 1).getPivotAlpha() - long(SAMPLES_PER_FRAME));
											else
												phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha() + long(2 * SAMPLES_PER_FRAME));
										}
										else if (markers[currentPlayer].back().visualType == 2
											&& chart.getPhrase(i + 1).getPivotAlpha() - phrase.getPivotAlpha() < markers[currentPlayer].back().sustainLimit)
											phrase.changeEndAlpha(chart.getPhrase(i + 1).getPivotAlpha() - long(SAMPLES_PER_FRAME));
										else
											phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
									}
									else if (markers[currentPlayer].back().visualType != 1
										|| !phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha() - long(5.0 * SAMPLES_PER_FRAME)))
										phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
									break;
								}
							}
							timeline[0].emplace(index, 1, pos, &phrase, i, i + 1 == chart.getNumPhrases());
							notes[currentPlayer]++;
							index++;
						}
						index = startIndex;
						if (chart.getNumTracelines() > 1)
						{
							for (size_t i = 0; i < chart.getNumTracelines(); i++)
							{
								long pos = chart.getTraceline(i).getPivotAlpha() + chart.getPivotTime() + position;
								while (index < timeline[0].size())
								{
									if (pos <= timeline[0][index].position)
										break;
									else
										index++;
								}
								timeline[0].emplace(index, 1, pos, &chart.getTraceline(i), i, i + 1 == chart.getNumTracelines());
								index++;
							}
						}
					}
				}
				position += section.getDuration();
				totalDuration += section.getDuration();
			}
			frameStart = pcsx2.insertFrames(stage, orientation, difficulty, multi, (size_t)ceil(totalDuration / SAMPLES_PER_FRAME) + PCSX2TAS::frameValues.frames[framerateIndex][0][1] - 1000);
		}
	}
	else
		frameStart = pcsx2.insertFrames(stage, orientation, difficulty, multi, (size_t)ceil(totalDuration / SAMPLES_PER_FRAME));
	for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
	{
		if (markers[playerIndex].size())
		{
			unsigned long sectIndex = 0;
			bool inTrace = false;
			bool connected = false;
			FILE* taslog;
			if (difficulty != 3)
				fopen_s(&taslog, (filename + ".txt").c_str(), "w");
			else
				fopen_s(&taslog, (filename + "_P" + to_string(playerIndex + 1) + ".txt").c_str(), "w");
			fprintf(taslog, "Samples per frame: %Lf\n", SAMPLES_PER_FRAME);
			fprintf(taslog, "////Section Marker 1 at sample %li\n", markers[playerIndex][sectIndex].position);
			NotePoint* prevPhrase = nullptr;
			for (unsigned long noteIndex = 0; noteIndex < timeline[playerIndex].size(); noteIndex++)
			{
				NotePoint& point = timeline[playerIndex][noteIndex];
				while (sectIndex + 1ULL != markers[playerIndex].size() && point.position >= markers[playerIndex][sectIndex + 1ULL].position)
				{
					sectIndex++;
					fprintf(taslog, "////Section Marker %lu at sample %li", sectIndex + 1, markers[playerIndex][sectIndex].position);
				}
				if (dynamic_cast<Guard*>(timeline[playerIndex][noteIndex].note) != nullptr)
				{
					size_t grdFrame = frameStart + (unsigned long)round(point.position / SAMPLES_PER_FRAME);
					//If the next note, if it exists, is a guard mark
					if (noteIndex + 1ULL != timeline[playerIndex].size() && dynamic_cast<Guard*>(timeline[playerIndex][noteIndex + 1ULL].note) != nullptr)
					{
						size_t distance = frameStart + (unsigned long)round(timeline[playerIndex][noteIndex + 1ULL].position / SAMPLES_PER_FRAME) - grdFrame;
						//If two guard marks are within two frames of each other
						if (distance < 2)
						{
							//Pull the current mark forwards one frame
							grdFrame--;
							//If they *were* within a single frame
							if (distance == 0)
							{
								//Push the second mark back one frame
								timeline[playerIndex][noteIndex + 1ULL].position += unsigned long(SAMPLES_PER_FRAME);
								//If pushing it back places it behind the note that *was* after it, fix the order
								if (noteIndex + 2ULL != timeline[playerIndex].size() && timeline[playerIndex][noteIndex + 1ULL].position >= timeline[playerIndex][noteIndex + 2ULL].position)
									timeline[playerIndex].moveElements(noteIndex + 1ULL, noteIndex + 3ULL);
							}
						}
					}
					//Value for setting the button to use
					char exponent = 7;
					//Square - 239
					//X/Cross  - 223
					//Circle - 191
					//Triangle - 127
					switch (orientation)	//Determine button based on orientation
					{
					case 0:
					case 1:
						switch (static_cast<Guard*>(point.note)->getButton())
						{
						case 3:
							exponent -= 3;
							break;
						case 2:
							exponent -= 2;
							break;
						case 1:
							exponent--;
						}
						break;
					case 2:
						switch (static_cast<Guard*>(point.note)->getButton())
						{
						case 2:
							exponent -= 3;
							break;
						case 1:
							exponent -= 2;
							break;
						case 0:
							exponent--;
						}
						break;
					case 3:
						switch (static_cast<Guard*>(point.note)->getButton())
						{
						case 0:
							exponent -= 3;
							break;
						case 3:
							exponent -= 2;
							break;
						case 2:
							exponent--;
						}
					}
					char safety = exponent;
					for (unsigned i = 0; i < 4; i++)
					{
						//The button is already in use on the previous frame
						if (!(pcsx2.players[playerIndex][grdFrame - 1].button & (1 << safety)))
						{
							//Swap to the next available button
							if (safety != 4)
								safety--;
							else
								safety = 7;
						}
						else
						{
							//Add a button press of the current type (described above) to the previous frame
							pcsx2.players[playerIndex][grdFrame - 1].button &= 255 - (1 << safety);
							break;
						}
					}
					//Clear the button on the previous frame, then add it on the current frame
					pcsx2.players[playerIndex][grdFrame - 1].button |= (1 << exponent);
					pcsx2.players[playerIndex][grdFrame].button &= 255 - (1 << exponent);
					fprintf(taslog, "Guard Mark %03zu-  Landing at sample %li | Frame #%zu\n", point.index, point.position, grdFrame - frameStart);
				}
				else if (dynamic_cast<Traceline*>(timeline[playerIndex][noteIndex].note) != nullptr)
				{
					for (unsigned long testIndex = noteIndex + 1; testIndex < timeline[playerIndex].size(); testIndex++)
					{
						if (dynamic_cast<Guard*>(timeline[playerIndex][testIndex].note) != nullptr && point.last)
						{
							inTrace = false;
							break;
						}
						else if (dynamic_cast<Traceline*>(timeline[playerIndex][testIndex].note) != nullptr)
						{
							if (!point.last || 10 * (timeline[playerIndex][testIndex].position - (long double)point.position) <= 3.0L * song.speed * SAMPLES_PER_FRAME)
							{
								if (point.last || timeline[playerIndex][testIndex].index - 1 == point.index)
								{
									if (point.last)
										connected = true;
									inTrace = true;
									size_t currentFrame = frameStart + (unsigned long)round(point.position / SAMPLES_PER_FRAME);
									size_t endFrame = frameStart + (unsigned)round(timeline[playerIndex][testIndex].position / SAMPLES_PER_FRAME);
									if (endFrame - currentFrame > 0)
									{
										long double currentAngle = static_cast<Traceline*>(point.note)->getAngle();
										long double angleDif = 0;
										if (!point.last)
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
										if (!static_cast<Traceline*>(point.note)->getCurve()) //If curve is false
										{
											//Iterate through all frames with a straight trace line, if any
											for (; 20 * (endFrame - currentFrame - 1) > song.speed; currentFrame++)
											{
												//Only Orientation 2 uses the right stick for trace lines
												switch (orientation)
												{
												case 2:
													pcsx2.players[playerIndex][currentFrame].rightStickX = (unsigned)round(127 - 127 * cos(currentAngle));
													pcsx2.players[playerIndex][currentFrame].rightStickY = (unsigned)round(127 + 127 * sin(currentAngle));
													break;
												default:
													pcsx2.players[playerIndex][currentFrame].leftStickX = (unsigned)round(127 - 127 * cos(currentAngle));
													pcsx2.players[playerIndex][currentFrame].leftStickY = (unsigned)round(127 + 127 * sin(currentAngle));
												}
											}
										}
										long double angleIncrement = angleDif / (endFrame - currentFrame);
										for (; currentFrame < endFrame; currentFrame++)
										{
											//Only Orientation 2 uses the right stick for trace lines
											switch (orientation)
											{
											case 2:
												pcsx2.players[playerIndex][currentFrame].rightStickX = (unsigned)round(127 - 127 * cos(currentAngle));
												pcsx2.players[playerIndex][currentFrame].rightStickY = (unsigned)round(127 + 127 * sin(currentAngle));
												break;
											default:
												pcsx2.players[playerIndex][currentFrame].leftStickX = (unsigned)round(127 - 127 * cos(currentAngle));
												pcsx2.players[playerIndex][currentFrame].leftStickY = (unsigned)round(127 + 127 * sin(currentAngle));
											}
											currentAngle += angleIncrement;
										}
									}
									break;
								}
								else
									timeline[playerIndex].erase(testIndex);
							}
							else
							{
								inTrace = false;
								connected = false;
								break;
							}
						}
					}
				}
				else if (inTrace) //only go here if the confirmed to be Phrase bar is inside a trace line
				{
					size_t phraseStart;
					//If this phrase bar isn't the first one in the song but is the first in its subsection
					//&
					//if the current & following trace lines are close enough together without interruptions
					if (prevPhrase != nullptr && !point.index && connected)
					{
						unsigned long prevsectIndex = sectIndex;
						//Check if the previous PB is close enough to justify connecting its
						//sustain button press to the current PB
						while (prevPhrase->position < markers[playerIndex][prevsectIndex].position)
								prevsectIndex--;
						if ((markers[playerIndex][prevsectIndex].visualType == 2 && point.position - prevPhrase->position < markers[playerIndex][prevsectIndex].sustainLimit)
							|| (markers[playerIndex][prevsectIndex].visualType == 1
								&& point.position - prevPhrase->position < markers[playerIndex][prevsectIndex].sustainLimit + long(static_cast<Phrase*>(prevPhrase->note)->getDuration())))
						{
							phraseStart = frameStart + (size_t)round((point.position - SAMPLES_PER_FRAME) / SAMPLES_PER_FRAME);
							fprintf(taslog, "\t      - Extended to sample %li | Frame #%zu\n", point.position, phraseStart - frameStart);
							size_t phraseEnd = frameStart
							+ (size_t)ceil((SAMPLES_PER_FRAME + prevPhrase->position + static_cast<Phrase*>(prevPhrase->note)->getDuration()) / SAMPLES_PER_FRAME);
							for (; phraseEnd < phraseStart - 1; phraseEnd++)
							{
								//No need to check for button slots being occupied
								switch (orientation)
								{
								case 0:
								case 1:
									pcsx2.players[playerIndex][phraseEnd].button &= 191;
									break;
								case 2:
									pcsx2.players[playerIndex][phraseEnd].button &= 223;
									break;
								case 3:
									pcsx2.players[playerIndex][phraseEnd].button &= 127;
								}
							}
						}
					}
					phraseStart = frameStart + (size_t)round(point.position / SAMPLES_PER_FRAME);
					size_t phraseEnd = frameStart + (size_t)ceil(((double)point.position + static_cast<Phrase*>(point.note)->getDuration()) / SAMPLES_PER_FRAME);
					fprintf(taslog, "Phrase Bar %03zu- Starting at sample %li | Frame #%zu\n", point.index, point.position, phraseStart - frameStart);
					pcsx2.players[playerIndex][phraseStart - 1].button = 255;
					/*
					//Square - 239
					//X/Cross  - 223
					//Circle - 191
					//Triangle - 127
					*/
					for (; phraseStart <= phraseEnd; phraseStart++)
					{
						char exponent = 7; //Triangle (base of orientation 3)
						switch (orientation)
						{
						case 0:
						case 1:
							exponent -= 3; //Square
							break;
						case 2:
							exponent -= 2; //X/Cross
						}
						if (phraseStart + 1 != pcsx2.players[playerIndex].size())
						{
							for (unsigned i = 0; i < 4; i++)
							{
								//If the button is already taken in the next frame
								if (!(pcsx2.players[playerIndex][phraseStart + 1].button & (1 << exponent)))
								{
									if (exponent != 4)
										exponent--;
									else
										exponent = 7;
								}
								else
								{
									pcsx2.players[playerIndex][phraseStart].button &= 255 - (1 << exponent);
									break;
								}
							}
						}
						else
							pcsx2.players[playerIndex][phraseStart].button &= 255 - (1 << exponent);
					}
					fprintf(taslog, "\t      -   Ending at sample %li | Frame #%zu\n" , point.position + static_cast<Phrase*>(point.note)->getDuration(), phraseEnd - frameStart);
					prevPhrase = &point;
				}
			}
			fclose(taslog);
		}
	}
	if (endReached)
	{
		if (notes[0] + notes[2] > notes[1] + notes[3])
			pcsx2.resultScreen(stage, (unsigned long long)notes[0] + notes[2], difficulty != 3, multi);
		else
			pcsx2.resultScreen(stage, (unsigned long long)notes[1] + notes[3], false, multi);
	}
	while (true)
	{
		switch (fileOverwriteCheck(filename + ".p2m2"))
		{
		case 0:
			cout << global.tabs << endl;
			filename += "_T";
			break;
		case 1:
			pcsx2.print(filename);
			return true;
		case -1:
			cout << global.tabs << endl;
			cout << global.tabs << "PCSX2 TAS was completed, but the file generation was cancelled." << endl;
			return false;
		}
	}
}