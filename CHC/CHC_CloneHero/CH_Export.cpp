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
#include "CH_Export.h"
using namespace std;

bool CH_Exporter::exportChart()
{
	List<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to chart - w/ spaces inbetween.\n", global.tabs.c_str());
		for (size_t sectIndex = 0; sectIndex < song->sections.size(); sectIndex++)
			printf("%s%zu - %s\n", global.tabs.c_str(), sectIndex, song->sections[sectIndex].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent List: ", global.tabs.c_str());
			for (size_t index = 0; index < sectionIndexes.size(); index++)
				printf("%s ", song->sections[sectionIndexes[index]].getName());
			putchar('\n');
		}
		switch (listValueInsert(sectionIndexes, "bgscynp", song->sections.size()))
		{
		case '?':

			break;
		case 'q':
			printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
			return false;
		case 'b':
		case 'g':
		case 's':
		case 'c':
		case 'y':
		case 'n':
		case 'p':
			if (sectionIndexes.size())
			{
				global.quit = true;
				break;
			}
		case '!':
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%s\n", global.tabs.c_str());
					printf("%sNo sections have been selected. Quit CH chart creation? [Y/N]\n", global.tabs.c_str());
					switch (menuChoices("yn"))
					{
					case 'q':
					case 'y':
						printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
						return false;
					case 'n':
						printf("%s\n", global.tabs.c_str());
						global.quit = true;
					}
				} while (!global.quit);
				global.quit = false;
			}
			else
				global.quit = true;
			break;
		}
	} while (!global.quit);
	global.quit = false;
	printf("%s", global.tabs.c_str());
	for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
		printf("%s ", song->sections[sectionIndexes[sect]].getName());
	putchar('\n');
#ifdef _DEBUG
	bool modchart = true;
#else
	bool modchart = false;
	{
		//"yes.txt" is essentially a run-time checked setting
		FILE* test;
		if (!fopen_s(&test, "yes.txt", "r"))
		{
			modchart = true;
			fclose(test);
		}
	}
#endif
	List<SyncTrack> sync;
	List<Event> events;
	//Used for modchart song->ini file that accompanies the .chart
	//Fills in the "song_length" tag
	double totalDuration = 0;
	if (!convertSong(sectionIndexes, sync, events, modchart, totalDuration))
		return false;
	banner(" All Notes Converted ");
	printf("%s\n", global.tabs.c_str());
	string filename = song->name.substr(0, song->name.length() - 4);
	bool written = false;
	if (modchart)
	{
		string filenameMod = filename;
		do
		{
			switch (fileOverwriteCheck(filenameMod + ".chart"))
			{
			case 'q':
				for (size_t i = 0; i < sync.size();)
				{
					sync[i].position -= 2 * TICKS_PER_BEAT;
					//Remove the two extra synctracks
					if (sync[i].position < 0)
						sync.erase(i);
					else
						i++;
				}
				for (size_t i = 0; i < events.size(); i++)
					events[i].position -= 2 * TICKS_PER_BEAT;
				global.quit = true;
				break;
			case 'n':
				filenameMod += "_T";
				printf("%s\n", global.tabs.c_str());
				break;
			case 'y':
				//Generate the ini file if it's a chart from the original games (stage 2 also including separate EN & JP charts)
				ChartFileExporter chart(filenameMod + ".ini");
				chart.writeIni(song->stage, totalDuration, song->shortname.find("HE") == string::npos);
				chart.open(filenameMod + ".chart");
				chart.write(sync, events, modchartNotes, true);
				chart.close();
				global.quit = true;
				written = true;
			}
			printf("%s\n", global.tabs.c_str());
		} while (!global.quit);
		global.quit = false;
	}
	filename += "_For_Reimporting";
	do
	{
		switch (fileOverwriteCheck(filename + ".chart"))
		{
		case 'q':
			global.quit = true;
			break;
		case 'n':
			printf("%s\n", global.tabs.c_str());
			filename += "_T";
			break;
		case 'y':
			ChartFileExporter chart(filename + ".chart");
			chart.write(sync, events, reimportNotes, false);
			chart.close();
			written = true;
			global.quit = true;
		}
	} while (!global.quit);
	global.quit = false;
	return written;
}

bool CH_Exporter::convertSong(List<size_t>& sectionIndexes, List<SyncTrack>& sync, List<Event>& events, bool modchart, double& totalDuration)
{
	const double TICKS_PER_BEAT = 480.0, GUARD_GAP = 8000.0;
	char orientationType = 0;
	char colorType[2] = { 0, 0 };
	size_t orientation = 3;
	double position = 0;
	size_t strumFret[2] = { 0, 0 };
	bool multiplayer = toupper(song->shortname[song->shortname.length() - 1]) == 'M' || (song->stage > 10 && song->imc[0]);
	if (modchart)
	{
		//Set starting position for gnerating the modchart
		sync.emplace_back(position, 2, 12632);
		position += TICKS_PER_BEAT;
		switch (song->stage)
		{
		case 0:
			break;
		case 1:
			sync.emplace_back(position, 2, 77538, false);
			break;
		case 2:
			if (song->shortname.find("ST02_HE") != string::npos)
				sync.emplace_back(position, 2, 73983, false);
			else
				sync.emplace_back(position, 2, 74070, false);
			break;
		case 3:
			sync.emplace_back(position, 2, 76473, false);
			break;
		case 4:
			sync.emplace_back(position, 2, 79798, false);
			break;
		case 5:
			sync.emplace_back(position, 2, 74718, false);
			break;
		case 6:
			sync.emplace_back(position, 2, 79658, false);
			break;
		case 7:
			sync.emplace_back(position, 2, 73913, false);
			break;
		case 8:
			sync.emplace_back(position, 2, 76523, false);
			break;
		case 9:
			sync.emplace_back(position, 2, 74219, false);
			break;
		case 10:
			sync.emplace_back(position, 2, 75500, false);
			break;
		case 11:
			sync.emplace_back(position, 2, 80000, false);
			break;
		case 12:
			sync.emplace_back(position, 2, 80000, false);
		}
		position += TICKS_PER_BEAT;
	}
	bool grdFound = !modchart, phrFound[2] = { false, false }, done = false;
	events.emplace_back(position, "GMFR EXPORT V2.0");
	for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
	{
		SongSection& section = song->sections[sectionIndexes[sect]];
		printf("%sConverting %s\n", global.tabs.c_str(), section.getName());
		switch (section.getPhase())
		{
		case SongSection::Phase::INTRO:
			events.emplace_back(position, "section INTRO - " + string(section.getName())); break;
		case SongSection::Phase::CHARGE:
			events.emplace_back(position, "section CHARGE - " + string(section.getName())); break;
		case SongSection::Phase::BATTLE:
			events.emplace_back(position, "section BATTLE - " + string(section.getName())); break;
		case SongSection::Phase::FINAL_AG:
			events.emplace_back(position, "section FINAL_AG - " + string(section.getName())); break;
		case SongSection::Phase::HARMONY:
			events.emplace_back(position, "section HARMONY - " + string(section.getName())); break;
		case SongSection::Phase::END:
			events.emplace_back(position, "section END - " + string(section.getName())); break;
		default:
			events.emplace_back(position, "section FINAL_I - " + string(section.getName()));
		}
		if (sync.size() == 0 || unsigned long(section.getTempo() * 1000) != sync.back().bpm)
		{
			if (song->stage == 12 && section.getPhase() == SongSection::Phase::INTRO)
			{
				sync.emplace_back(position, 3, unsigned long(section.getTempo() * 1000));
				sync.emplace_back(position + 3 * TICKS_PER_BEAT, 4, 0);
			}
			else
				sync.emplace_back(position, 4, unsigned long(section.getTempo() * 1000));
			if (section.getTempo() < 80.0f)
				sync.back().eighth = " 3";
		}
		if (section.getPhase() != SongSection::Phase::INTRO && !strstr(section.getName(), "BRK")) //If not INTRO phase or BRK section
		{
			if (!done || orientationType == 1)
			{
				for (size_t playerIndex = 0; playerIndex < section.getNumPlayers() && (!done || (orientationType == 1 && !global.quit)); playerIndex++)
				{
					if (!(playerIndex & 1) || multiplayer)
					{
						for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && (!done || (orientationType == 1 && !global.quit)); chartIndex++)
						{
							Chart& chart = section.getChart(playerIndex * (size_t)section.getNumCharts() + chartIndex);
							if (chart.getNumGuards() && !global.quit)
							{
								while (!grdFound)
								{
									printf("%sHow will guard phrases be handled? [Only effects the modchart export]\n", global.tabs.c_str());
									printf("%sB - Base Orientation Only\n", global.tabs.c_str());
									printf("%sG - Guitar Hero Conversion\n", global.tabs.c_str());
									printf("%sS - Determined per Section\n", global.tabs.c_str());
									printf("%sC - Determined per Chart\n", global.tabs.c_str());
									switch (menuChoices("bgsc"))
									{
									case 'b':
										orientation = 0;
									case 'g':
										printf("%s\n", global.tabs.c_str());
										grdFound = true;
										break;
									case 's':
										printf("%s\n", global.tabs.c_str());
										orientationType = 1;
										grdFound = true;
										break;
									case 'c':
										printf("%s\n", global.tabs.c_str());
										orientationType = 2;
										grdFound = true;
										break;
									case 'q':
										printf("%s\n", global.tabs.c_str());
										printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
										return false;
									}
								}
								if (orientationType == 1)
								{
									if (!getOrientation(section.getName()))
									{
										printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
										return false;
									}
									else
										global.quit = true;
								}
							}
							if (!(playerIndex & 1))
							{
								if (chart.getNumPhrases())
								{
									while (!phrFound[0])
									{
										printf("%sHow will strum phrases be handled for player 1?\n", global.tabs.c_str());
										printf("%sS - Same frets for the entire song\n", global.tabs.c_str());
										printf("%sN - Determined per Note\n", global.tabs.c_str());
										printf("%sP - Determined per Phrase Bar\n", global.tabs.c_str());
										//printf("%s", global.tabs.c_str(), "Note for C, N, & P: if a phrase bar has a pre-set color from a .chart import, that color will be used.\n";
										switch (menuChoices("snp"))
										{
										case 's':
											printf("%s\n", global.tabs.c_str());
											if (!getFrets(strumFret[0], section.getName(), 0))
											{
												printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
												return false;
											}
											phrFound[0] = true;
											break;
										case 'n':
											printf("%s\n", global.tabs.c_str());
											colorType[0] = 1;
											phrFound[0] = true;
											break;
										case 'p':
											printf("%s\n", global.tabs.c_str());
											colorType[0] = 2;
											phrFound[0] = true;
											break;
										case 'q':
											printf("%s\n", global.tabs.c_str());
											printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
											return false;
										}
									}
								}
							}
							else if (chart.getNumPhrases())
							{
								while (!phrFound[1])
								{
									printf("%sHow will strum phrases be handled for player 2?\n", global.tabs.c_str());
									printf("%sS - Same frets for the entire song\n", global.tabs.c_str());
									printf("%sN - Determined per Note\n", global.tabs.c_str());
									printf("%sP - Determined per Phrase Bar\n", global.tabs.c_str());
									//printf("%s", global.tabs.c_str(), "Note for C, N, & P: if a phrase bar has a pre-set color from a .chart import, that color will be used.\n";
									switch (menuChoices("snp"))
									{
									case 's':
										printf("%s\n", global.tabs.c_str());
										if (!getFrets(strumFret[1], section.getName(), 0))
										{
											printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
											return false;
										}
										phrFound[1] = true;
										break;
									case 'n':
										printf("%s\n", global.tabs.c_str());
										colorType[1] = 1;
										phrFound[1] = true;
										break;
									case 'p':
										printf("%s\n", global.tabs.c_str());
										colorType[1] = 2;
										phrFound[1] = true;
										break;
									case 'q':
										printf("%s\n", global.tabs.c_str());
										printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
										return false;
									}
								}
							}
							done = grdFound && phrFound[0] && phrFound[1];
						}
					}
				}
				global.quit = false;
			}
			const double TICKS_PER_SAMPLE = section.getTempo() * TICKS_PER_BEAT / SAMPLES_PER_MIN;
			//Marking where each the list the current section starts
			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); playerIndex++)
				{
					if (!(playerIndex & 1))
					{
						if (playerIndex == 2 && !song->imc[0])
							currentPlayer = 1;
						else
							currentPlayer = 0;
					}
					else if (multiplayer)
						currentPlayer = 1;
					else
						continue;
					Chart& chart = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
					if (modchart && orientationType == 2 && chart.getNumGuards() && !getOrientation(section.getName(), playerIndex, chartIndex))
					{
						printf("%sClone Hero chart creation cancelled.\n", global.tabs.c_str());
						return false;
					}
					size_t markIndex = reimportNotes[currentPlayer].allNotes.size();
					size_t grdIndex = convertGuard(chart, position, TICKS_PER_SAMPLE, currentPlayer, orientation, modchart);
					size_t phrIndex = 0;
					if (chart.getNumTracelines() > 1)
					{
						convertTrace(chart, position, TICKS_PER_SAMPLE, (long)round(TICKS_PER_SAMPLE * section.getDuration()), currentPlayer, modchart);
						try
						{
							phrIndex = convertPhrase(section, playerIndex, chartIndex, position, TICKS_PER_SAMPLE, currentPlayer, colorType[currentPlayer], strumFret[currentPlayer], modchart);
						}
						catch (const char* str)
						{
							printf("%s%s\n", global.tabs.c_str(), str);
							return false;
						}
					}
					//Firstly, determines whether a "start" marker should be placed
					if ((chartIndex != 0 || (playerIndex >= 2 && song->imc[0])) && (chart.getNumTracelines() > 1 || chart.getNumGuards()))
					{
						//AKA, if any notes or trace lines were added
						double pos;
						if (markIndex < reimportNotes[currentPlayer].allNotes.size())
							pos = (reimportNotes[currentPlayer].allNotes[markIndex]->position + reimportNotes[currentPlayer].allNotes[markIndex - 1]->position) / 2;
						else if (markIndex > 0)
							pos = reimportNotes[currentPlayer].allNotes[markIndex - 1]->position + 160;
						else
							pos = position + 160;
						reimportNotes[currentPlayer].addEvent(pos, "start");
					}
					//Sets star power phrases
					if (modchart)
					{
						switch (section.getPhase())
						{
						case SongSection::Phase::BATTLE:
							if (chart.getNumGuards())
								//Encapsulate all the guard marks in the subsection
								modchartNotes[currentPlayer].addStarPower(position + TICKS_PER_SAMPLE * (chart.getGuard(0).getPivotAlpha() + double(chart.getPivotTime())),
									20 + TICKS_PER_SAMPLE * ((double)chart.getGuard(chart.getNumGuards() - 1).getPivotAlpha() - chart.getGuard(0).getPivotAlpha()));
							break;
						case SongSection::Phase::CHARGE:
							if (chart.getNumPhrases())
								//Encapsulate all the phrase bars in the subsection
								modchartNotes[currentPlayer].addStarPower(position + TICKS_PER_SAMPLE * (chart.getPhrase(0).getPivotAlpha() + double(chart.getPivotTime())),
									TICKS_PER_SAMPLE * ((double)chart.getPhrase(chart.getNumPhrases() - 1).getEndAlpha() - chart.getPhrase(0).getPivotAlpha()));
						}
					}
				}
			}
		}
		position += TICKS_PER_BEAT * round(double(section.getDuration()) * section.getTempo() / SAMPLES_PER_MIN);
		if (modchart)
			totalDuration += (SAMPLES_PER_MIN / section.getTempo()) * round(double(section.getDuration()) * section.getTempo() / SAMPLES_PER_MIN);
	}
	return true;
}

size_t CH_Exporter::convertGuard(Chart& chart, const double position, const double TICKS_PER_SAMPLE, const size_t currentPlayer, const size_t orientation, const bool modchart)
{
	const static double GUARD_GAP = 8000.0;
	const double GUARD_OPEN_TICK_DISTANCE = GUARD_GAP * TICKS_PER_SAMPLE;
	size_t grdStarIndex = modchartNotes[currentPlayer].allNotes.size();
	for (size_t i = 0; i < chart.getNumGuards(); i++)
	{
		size_t fret, modfret;
		Guard& guard = chart.getGuard(i);
		switch (orientation)	//Determine button based on orientation
		{
		case 0:
			switch (guard.getButton())
			{
			case 0:		//Red
				modfret = 1;
				break;
			case 1:		//Blue
				modfret = 3;
				break;
			case 2:		//Orange
				modfret = 4;
				break;
			case 3:		//Green
				modfret = 0;
			}
			break;
		case 1:
			switch (guard.getButton())
			{
			case 0:		//Blue
				modfret = 3;
				break;
			case 1:		//Orange
				modfret = 4;
				break;
			case 2:		//Green
				modfret = 0;
				break;
			case 3:		//Red
				modfret = 1;
			}
			break;
		case 2:
			switch (guard.getButton())
			{
			case 0:		//Orange
				modfret = 4;
				break;
			case 1:		//Green
				modfret = 0;
				break;
			case 2:		//Red
				modfret = 1;
				break;
			case 3:		//Blue
				modfret = 3;
			}
			break;
		case 3:
			switch (guard.getButton())
			{
			case 0:		//Red
				modfret = 3;
				break;
			case 1:		//Green
				modfret = 0;
				break;
			case 2:		//Blue
				modfret = 1;
				break;
			case 3:		//Orange
				modfret = 4;
			}
		}
		switch (guard.getButton())
		{
		case 0:		//Red
			fret = 3;
			break;
		case 1:		//Green
			fret = 0;
			break;
		case 2:		//Blue
			fret = 1;
			break;
		case 3:		//Orange
			fret = 4;
		}
		double pos = position + TICKS_PER_SAMPLE * ((double)guard.getPivotAlpha() + chart.getPivotTime());
		if (modchart)
		{
			size_t val = modchartNotes[currentPlayer].addNote(pos, size_t(1) << modfret);
			modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
			if (i == 0)
				grdStarIndex = val;
			if (i + 1 != chart.getNumGuards())
			{
				long dif = chart.getGuard(i + 1).getPivotAlpha() - guard.getPivotAlpha();
				if (dif >= 480000)		//If dif is >= ten seconds
				{
					double openPos = pos + 240000 * TICKS_PER_SAMPLE;
					modchartNotes[currentPlayer].addNote(openPos, 32);
					modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
				else if (dif >= 240000) //If 5 seconds <= dif < ten seconds
				{
					double openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
					modchartNotes[currentPlayer].addNote(openPos, 32);
					modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
				else if (dif >= 2 * GUARD_GAP)
				{
					double openPos = pos + GUARD_OPEN_TICK_DISTANCE;
					modchartNotes[currentPlayer].addNote(openPos, 32);
					if (GUARD_OPEN_TICK_DISTANCE >= 162.5)
						modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
				else
				{
					double openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
					modchartNotes[currentPlayer].addNote(openPos, 32);
					if ((dif >> 1) * TICKS_PER_SAMPLE >= 162.5)
						modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
			}
			pos -= 2 * TICKS_PER_BEAT;
		}
		reimportNotes[currentPlayer].addNote(pos, size_t(1) << fret);
	}
	return grdStarIndex;
}

void CH_Exporter::convertTrace(Chart& chart, const double position, const double TICKS_PER_SAMPLE, const long sectionDuration, const size_t currentPlayer, const bool modchart)
{
	for (size_t i = 0; i < chart.getNumTracelines(); i++)
	{
		double pos = TICKS_PER_SAMPLE * (chart.getTraceline(i).getPivotAlpha() + double(chart.getPivotTime()));
		string name;
		if ((long)round(pos) < sectionDuration)
			name = "Trace";
		else
			name = "TraceP";
		if (modchart)
			pos += position - 2 * TICKS_PER_BEAT;
		else
			pos += position;
		if (i + 1 != chart.getNumTracelines())
		{
			if (chart.getTraceline(i).getAngle() == 0)
				reimportNotes[currentPlayer].addEvent(pos, name);
			else
				reimportNotes[currentPlayer].addEvent(pos, name + '_' + to_string(radiansToDegrees(chart.getTraceline(i).getAngle())));
			if (chart.getTraceline(i).getCurve())
				reimportNotes[currentPlayer].addEvent(pos, "Trace_curve");
		}
		else
			reimportNotes[currentPlayer].addEvent(pos, name + "_end");
	}
}

size_t CH_Exporter::convertPhrase(SongSection& section, const size_t playerIndex, const size_t chartIndex, const double position,
	const double TICKS_PER_SAMPLE,const size_t currentPlayer, const size_t colorType, size_t& strumFret, const bool modchart)
{
	Chart& chart = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
	size_t phrIndex = modchartNotes[currentPlayer].allNotes.size();
	size_t prevFret = strumFret;
	const size_t max = chart.getNumPhrases();
	for (size_t i = 0, note = 1, piece = 1; i < max; i++)
	{
		if (colorType)
		{
			strumFret = chart.getPhrase(i).getColor();
			if (strumFret == 0)
			{
				if (section.getPhase() == SongSection::Phase::BATTLE && note == 1 && piece == 1)
					global.quit = !getFrets(strumFret, section.getName(), 1, playerIndex + 1, chartIndex, note, piece);
				else if (piece > 1)
					global.quit = !getFrets(strumFret, section.getName(), 2, playerIndex + 1, chartIndex, note, piece);
				else
					global.quit = !getFrets(strumFret, section.getName(), 0, playerIndex + 1, chartIndex, note, piece);
				if (global.quit)
				{
					global.quit = false;
					throw "CH chart creation cancelled.";
				}
				else if (i + 1 == max)
					banner(" " + string(section.getName()) + "'s Phrase Bars Converted ");
			}
		}
		double pos = position + TICKS_PER_SAMPLE * (chart.getPhrase(i).getPivotAlpha() + double(chart.getPivotTime()));
		double endTick;
		if (colorType < 2 || strumFret == 256)
		{
			size_t maxIndex = max;
			while (i < maxIndex)
			{
				if (chart.getPhrase(i).getEnd() || i + 1 == maxIndex)
					break;
				else
					i++;
			}
		}
		if (strumFret >= 128)
			strumFret = prevFret;
		endTick = position + TICKS_PER_SAMPLE * (chart.getPhrase(i).getEndAlpha() + double(chart.getPivotTime()));
		size_t addedNotes = strumFret & 63, removedNotes = 0;
		if (piece > 1)
		{
			if (((prevFret & 31) != 31) == ((strumFret & 31) != 31))
			{
				for (size_t color = 0; color < 6; color++) //Used for setting extended sustains
				{
					size_t val = size_t(1) << color;
					if (prevFret & val && strumFret & val)
					{
						if (modchart)
						{
							CHNote& note = modchartNotes[currentPlayer].colors[color].back();
							note.setEndPoint(endTick);
							note.fret.writeSustain = note.fret.sustain >= 6200 * TICKS_PER_SAMPLE;
							reimportNotes[currentPlayer].colors[color].back().setEndPoint(endTick - 2 * TICKS_PER_BEAT);
						}
						else
							reimportNotes[currentPlayer].colors[color].back().setEndPoint(endTick);
						addedNotes &= ~val; //Removal
					}
					else if (prevFret & val)
						removedNotes |= val; //Addition
				}
			}
		}
		if (addedNotes)
		{
			double duration = endTick - pos;
			if (modchart)
			{
				bool hammeron = false;
				size_t ntIndex = modchartNotes[currentPlayer].addNote(pos, addedNotes, duration, duration >= 6200 * TICKS_PER_SAMPLE);
				size_t added = 0;
				for (size_t color = 0; added < 2 && color < 6; color++)
					if (addedNotes & (size_t(1) << color))
						added++;
				if (added == 1 && strumFret != prevFret)
				{
					for (size_t prev = ntIndex; prev > 0;)
					{
						if (modchartNotes[currentPlayer].allNotes[--prev]->type == CHNote::NoteType::NOTE)
						{
							hammeron = pos - modchartNotes[currentPlayer].allNotes[prev]->position < 162.5;
							break;
						}
					}
				}
				if (strumFret & 64)
				{
					if (!(addedNotes & 32))
						modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
					else if (!hammeron)
						modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				}
				else if ((piece == 1) == hammeron) //Ensures a strum on new notes and hammeron's for continuations
					modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				//Handles Sustain Gaps
				for (size_t color = 0; color < 6; color++)
				{
					size_t index = modchartNotes[currentPlayer].colors[color].size();
					if (index > 0 && (!(addedNotes & (size_t(1) << color)) || (strumFret & 31) == 31 || (prevFret & 31) == 31))
					{
						if ((size_t(1) << color) & addedNotes)
							--index;
						if (index > 0)
						{
							CHNote& prev = modchartNotes[currentPlayer].colors[color][--index];
							//Adds sustain gaps for non-extended sustain notes
							if (pos - (3400 * TICKS_PER_SAMPLE) < prev.position + prev.fret.sustain && prev.position + prev.fret.sustain <= pos)
								prev.fret.writeSustain = prev.setEndPoint(pos - (3400 * TICKS_PER_SAMPLE)) >= 6200 * TICKS_PER_SAMPLE;
						}
					}
				}
				pos -= 2 * TICKS_PER_BEAT;
				if (note == 1 && piece == 1)
					phrIndex = ntIndex;
			}
			reimportNotes[currentPlayer].addNote(pos, addedNotes, duration);
			if (strumFret & 64)
			{
				if (modchart)
					reimportNotes[currentPlayer].addModifier(pos - 2 * TICKS_PER_BEAT, CHNote::Modifier::FORCED);
				else
					reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
			}
			else if (piece > 1 && (removedNotes || strumFret == 31))
			{
				reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				if (!(addedNotes & 32))
					reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
			}
		}
		prevFret = strumFret & ~64;
		if (chart.getPhrase(i).getEnd())
		{
			note++;
			piece = 1;
		}
		else
			piece++;
	}
	return phrIndex;
}

//Selecting which controller setup to correspond to
size_t CH_Exporter::getOrientation(const char* sectionName, size_t player, size_t chart)
{
	size_t orientation;
	while (true)
	{
		printf("%sChoose orientation for %s", global.tabs.c_str(), sectionName);
		player ? printf(": Player %zu - Chart %zu\n", player + 1, chart) : putchar('\n');
		printf("%s   ||U|L|R|D||   T - Triangle\n", global.tabs.c_str());
		printf("%s===============  S - Square\n", global.tabs.c_str());
		printf("%s 1 ||T|S|O|X||   O - Circle\n", global.tabs.c_str());
		printf("%s 2 ||S|X|T|O||   X - Cross\n", global.tabs.c_str());
		printf("%s 3 ||O|T|X|S||\n", global.tabs.c_str());
		orientation = menuChoices("123", true);
		switch (orientation)
		{
		case 'q':
			printf("%s\n", global.tabs.c_str());
			return 'q';
		case '?':
			printf("%s\n", global.tabs.c_str());
		case '*':
			break;
		default:
			printf("%s\n", global.tabs.c_str());
			return orientation;
		}
	};
}

//Sets phrase bar/note color
//0 - No color (Five-note chord)
//1 (Bit 0) - Green
//2 (Bit 1) - Red
//4 (Bit 2) - Yellow
//8 (Bit 3) - Blue
//16 (Bit 4) - Orange
//32 (Bit 5) - Open Note
//64 (Bit 6) - Tap Variant
//128 (Bit 7) - Continue from previous Color || 256 - Extended continuation to note end
bool CH_Exporter::getFrets(size_t& strumFret, const char* sectionName, size_t promptType, size_t playerIndex, size_t chartIndex, size_t note, size_t piece, size_t prevFret)
{
	banner(" Clone Hero Export - Fret Selection ");
	string choices = "dgrybop";
	if (promptType == 1)
		choices += "0123456";
	else if (promptType == 2)
		choices += "ce";
	choices += 'm';
	do		//Ask for the fret color in each iteration
	{
		if (playerIndex)
		{
			printf("%sSelect the fret for %s: Player %zu - Chart %zu - Note %zu", global.tabs.c_str(), sectionName, playerIndex, chartIndex, note);
			if (piece > 1)
				printf(" - Piece %zu", piece);
			printf("\n");
		}
		else
			printf("%sSelect the fret to use for the entire song\n", global.tabs.c_str());
		if (!global.multi)
		{
			printf("%s==============================================================||\n", global.tabs.c_str());
			printf("%s            Default Color (D) || Open Note (P)                ||\n", global.tabs.c_str());
			printf("%s Green (G) || Red (R) || Yellow (Y) || Blue (B) || Orange (O) ||\n", global.tabs.c_str());
			if (promptType == 1)
			{
			printf("%s            Tap - Default (0) || Hopo - Open Note (6)         ||\n", global.tabs.c_str());
				printf("%s   Tap (1) || Tap (2) ||   Tap (3)  ||  Tap (4) ||    Tap (5) ||\n", global.tabs.c_str());
			}
			else if (promptType == 2)
			{
				printf("%s              Continue with the previous color (C)            ||\n", global.tabs.c_str());
				printf("%s        Extend previous color to the end of the note (E)      ||\n", global.tabs.c_str());
			}
			printf("%s         Manually toggle each fret you wish to use (M)        ||\n", global.tabs.c_str());
			printf("%s==============================================================||\n", global.tabs.c_str());
		}
		strumFret = menuChoices(choices, true);
		switch (strumFret)
		{
		case 'q':
			printf("%s\n", global.tabs.c_str());
			return false;
		case '?':
			printf("%s\n", global.tabs.c_str());
		case '*':
			break;
		default:
			switch (choices[strumFret])
			{
			case 'c':
				printf("%s\n", global.tabs.c_str());
				strumFret = 128;
				global.quit = true;
				break;
			case 'e':
				printf("%s\n", global.tabs.c_str());
				strumFret = 256;
				global.quit = true;
				break;
			case 'm':
			{
				strumFret = prevFret;
				size_t numColoredFrets = 0;
				global.adjustTabs(2);
				choices = "12345pf";
				if (piece == 1 && playerIndex)
					choices += 't';
				banner(" Clone Hero Export - Fret Selection - Toggle Mode ");
				do
				{
					if (playerIndex)
					{
						printf("%sColor Toggles for %s: Player %zu - Chart %zu - Note %zu", global.tabs.c_str(), sectionName, playerIndex, chartIndex, note);
						if (piece > 1)
							printf(" - Piece %zu", piece);
						printf("\n");
					}
					else
						printf("%sToggle the frets that you wish to use for the entire song\n", global.tabs.c_str());
					if (!global.multi)
					{
						printf("%s============================================================================================||\n", global.tabs.c_str());
						printf("%s           You can have a max of 4 different fret *colors* toggled ON at one time           ||\n", global.tabs.c_str());
						if (piece > 1 || playerIndex == 0)
						{
							if (playerIndex == 0)
								printf("%s                  Using 'P' will cancel out all other frets & vice versa                    ||\n", global.tabs.c_str());
							printf("%s============================================================================================||\n", global.tabs.c_str());
							printf("%s                                     Open Note (P) %s                                    ||\n", global.tabs.c_str(), strumFret & 32 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s            Using 'P' on this note will cancel out all other frets & vice versa             ||\n", global.tabs.c_str());
							printf("%s============================================================================================||\n", global.tabs.c_str());
							printf("%s                            Open Note (P) %s || Tap (T) %s                            ||\n", global.tabs.c_str(), strumFret & 32 ? "[ON] " : "[OFF]", strumFret & 64 ? "[ON] " : "[OFF]");
						}
						if (numColoredFrets < 4)
						{
							printf("%s Green (1) %s ||", global.tabs.c_str(), strumFret & 1 ? "[ON] " : "[OFF]");
							printf(" Red (2) %s ||", strumFret & 2 ? "[ON] " : "[OFF]");
							printf(" Yellow (3) %s ||", strumFret & 4 ? "[ON] " : "[OFF]");
							printf(" Blue (4) %s ||", strumFret & 8 ? "[ON] " : "[OFF]");
							printf(" Orange (5) %s ||\n", strumFret & 16 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s%s||", global.tabs.c_str(), strumFret & 1 ? " Green (1) [ON]  " : "                 ");
							printf("%s||", strumFret & 2 ? " Red (2) [ON]  " : "               ");
							printf("%s||", strumFret & 4 ? " Yellow (3) [ON]  " : "                  ");
							printf("%s||", strumFret & 8 ? " Blue (4) [ON]  " : "                ");
							printf("%s||\n", strumFret & 16 ? " Orange (5) [ON]  " : "                  ");
						}
						printf("%s                       Finished fret selection for this note/piece (F)                      ||\n", global.tabs.c_str());
						printf("%s============================================================================================||\n", global.tabs.c_str());
					}
					size_t choice = menuChoices(choices, true);
					switch (choice)
					{
					case 'q':
						printf("%s\n", global.tabs.c_str());
						return false;
					case '?':
						printf("%s\n", global.tabs.c_str());
					case '*':
						break;
					default:
						printf("%s\n", global.tabs.c_str());
						switch (choices[choice])
						{
						case 'f':
							if ((strumFret & 63) == 0)
								strumFret |= 31;
							printf("%s\n", global.tabs.c_str());
							global.quit = true;
							break;
						case 't':
							strumFret ^= 64;
							break;
						case 'p':
							if (!(strumFret & 32) && piece == 1 && numColoredFrets)
							{
								do
								{
									printf("%sAll colored frets will need to be untoggled. Continue toggling open note? [Y/N]\n", global.tabs.c_str());
									switch (menuChoices("yn"))
									{
									case 'q':
										return false;
									case 'y':
										strumFret &= 64;
										strumFret += 32;
										numColoredFrets = 0;
									case 'n':
										global.quit = true;
										printf("%s\n", global.tabs.c_str());
									}
								} while (!global.quit);
								global.quit = false;
							}
							else
							{
								size_t added = 0;
								bool toggle = true;
								for (size_t color = 0; color < 5; color++)
								{
									size_t val = (size_t(1) << color);
									if (strumFret & val && !(prevFret & val))
									{
										do
										{
											printf("%s\n", global.tabs.c_str());
											printf("%sAll colored frets not used for the previous note will need to be untoggled. Continue toggling open note? [Y/N]\n", global.tabs.c_str());
											switch (menuChoices("yn"))
											{
											case 'q':
												return false;
											case 'y':
												for (; color < 5; color++)
												{
													val = (size_t(1) << color);
													if (strumFret & val && !(prevFret & val))
													{
														strumFret ^= val;
														numColoredFrets--;
													}
												}
												strumFret |= 32;
											case 'n':
												toggle = false;
												global.quit = true;
											}
										} while (!global.quit);
										global.quit = false;
										break;
									}
								}
								if (toggle)
									strumFret ^= 32;
							}
							break;
						default:
						{
							size_t color = size_t(1) << choice;
							if (strumFret & color || numColoredFrets < 4)
							{
								if (piece == 1 && !(strumFret & color) && strumFret & 32)
								{
									do
									{
										printf("%s\n", global.tabs.c_str());
										printf("%sOpen note will need to be untoggled. Continue toggling this color? [Y/N]\n", global.tabs.c_str());
										switch (menuChoices("yn"))
										{
										case 'q':
											return false;
										case 'y':
											strumFret -= 32;
											strumFret += color;
											numColoredFrets++;
										case 'n':
											global.quit = true;
										}
									} while (!global.quit);
									global.quit = false;
								}
								else
								{
									strumFret ^= color;
									if (strumFret & color)
										numColoredFrets++;
									else
										numColoredFrets--;
								}
							}
							else
								printf("%sOne of the other four colors must be untoggled before you can toggle this one.\n", global.tabs.c_str());
						}
						}
					}
				} while (!global.quit);
				global.adjustTabs(1);
			}
			break;
			default:
				if (promptType == 1 && strumFret > 6)
				{
					if (strumFret == 7)
						strumFret = 95;
					else
						strumFret = 64 + (size_t(1) << (strumFret - 8));
				}
				else if (strumFret > 6)
					strumFret = size_t(1) << (strumFret + 1);
				else if (strumFret > 0)
					strumFret = size_t(1) << (strumFret - 1);
				else
					strumFret = 31;
				printf("%s\n", global.tabs.c_str());
				global.quit = true;
			}
		}
	} while (!global.quit);
	global.quit = false;
	return true;
}

void ChartFileExporter::write(List<SyncTrack>& sync, List<Event>& events, NoteTrack(&notes)[2], bool modchart)
{
	fprintf(chart, "[Song]\n{\n");
	fprintf(chart, "  Offset = 0\n");
	fprintf(chart, "  Resolution = 480\n");
	fprintf(chart, "  Player2 = bass\n");
	fprintf(chart, "  Difficulty = 0\n");
	fprintf(chart, "  PreviewStart = 0\n");
	fprintf(chart, "  PreviewEnd = 0\n");
	fprintf(chart, "  Genre = \"rock\"\n");
	fprintf(chart, "  MediaType = \"cd\"\n}\n");
	fprintf(chart, "[SyncTrack]\n{\n");
	if (modchart)
	{
		for (size_t i = 0; i < sync.size();)
		{
			sync[i].write(chart);
			sync[i].position -= 2 * TICKS_PER_BEAT;
			//Remove the two extra synctracks
			if (sync[i].position < 0)
				sync.erase(i);
			else
				i++;
		}
	}
	else
	{
		for (size_t i = 0; i < sync.size(); i++)
		{
			sync[i].eighth = "";
			sync[i].write(chart);
		}
	}
	fprintf(chart, "}\n[Events]\n{\n");
	if (modchart)
	{
		for (size_t i = 0; i < events.size(); i++)
		{
			events[i].write(chart);
			events[i].position -= 2 * TICKS_PER_BEAT;
		}
	}
	else
		for (size_t i = 0; i < events.size(); i++)
			events[i].write(chart);
	fprintf(chart, "}\n");
	for (size_t player = 0; player < 2; player++)
		notes[player].write(chart, player);
}

void ChartFileExporter::writeIni(const unsigned char stageNumber, const double totalDuration, const bool jap)
{
	fprintf(chart, "[song]\n");
	switch (stageNumber)
	{
	case 0:
		break;
	case 1:
		fprintf(chart, "artist = COIL, Tomohiro Harada, Kaleb James\n");
		fprintf(chart, "name = 1. Twisted Reality (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Rock\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Panpeus: His axe is an axe!\"<br>\"The Axe Gitaroo rips right through space-time, so watch out!\"<br>\"Panpeus may have a baby face, but not baby skills!\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 3\n");
		fprintf(chart, "preview_start_time = 29000\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 3\n");
		fprintf(chart, "playlist_track = 5\n");
		fprintf(chart, "video_start_time = 0\n");
		fprintf(chart, "lyrics = 1\n");
		break;
	case 2:
		fprintf(chart, "artist = Tomohiro Harada, YUAN, a - li\n");
		if (!jap)
			fprintf(chart, "name = 2E. Flyin' to Your Heart (EN) (MODCHART)\n");
		else
			fprintf(chart, "name = 2. Flyin' to Your Heart (JP) (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = J - Pop\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Flyin-O: A loyal soldier of the empire!\"<br>\"His synth gitaroo shoots 'Dance-Until-Death' rays!\"<br>\"His henchmen, the Little-Os, are nothing to be scoffed at either!\"");
		fprintf(chart, "<br><br>EPILEPSY WARNING DURING CHARGE PHASE<br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 3\n");
		fprintf(chart, "preview_start_time = 23460\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 4\n");
		fprintf(chart, "playlist_track = 7\n");
		fprintf(chart, "video_start_time = 0\n");
		fprintf(chart, "lyrics = 1\n");
		break;
	case 3:
		fprintf(chart, "artist = COIL, Tomohiro Harada, Keiichi Yano\n");
		fprintf(chart, "name = 3. Bee Jam Blues (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Funk\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Mojo King Bee: He rules the darkness with his trumpet Gitaroo!\"<br>\"No one has ever seen the man behind the shades...\"");
		fprintf(chart, "<br><br>EPILEPSY WARNING NEAR THE END<br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 4\n");
		fprintf(chart, "preview_start_time = 34260\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 5\n");
		fprintf(chart, "playlist_track = 9\n");
		fprintf(chart, "video_start_time = 0\n");
		fprintf(chart, "lyrics = 1\n");
		break;
	case 4:
		fprintf(chart, "artist = Tomohiro Harada, k - nzk\n");
		fprintf(chart, "name = 4. VOID (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Industrial\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Ben-K: As a space shark, he's a rough, tough, customer just looking for trouble!\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 4\n");
		fprintf(chart, "preview_start_time = 13650\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 6\n");
		fprintf(chart, "playlist_track = 11\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 5:
		fprintf(chart, "artist = COIL, Tomohiro Harada, feat.NAHKI\n");
		fprintf(chart, "name = 5. Nuff Respect (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Reggae\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Ben-K: He attacks by scratching the turntable Gitaroo on his chest - but his Karate moves prove he's a man of many talents!\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 3\n");
		fprintf(chart, "preview_start_time = 38480\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 7\n");
		fprintf(chart, "playlist_track = 13\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 6:
		fprintf(chart, "artist = COIL, Tomohiro Harada\n");
		fprintf(chart, "name = 6. The Legendary Theme (Acoustic) (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Acoustic\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Kirah: A shy and reserved girl with the heart of a warrior.\"<br>\"She excels at baking Gitaroo Apple Pies.\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 2\n");
		fprintf(chart, "preview_start_time = 26120\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 8\n");
		fprintf(chart, "playlist_track = 15\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 7:
		fprintf(chart, "artist = Tomohiro Harada, Steve Eto, Kazuki Abe\n");
		fprintf(chart, "name = 7. Born To Be Bone (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Flamenco\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Sanbone Trio: Carrot, Soda, & Pine are three mariachis who use their bones as Gitaroos!\"<br>\"Their bare-knuckled fighting style is truly praiseworthy!\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 5\n");
		fprintf(chart, "preview_start_time = 84330\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 9\n");
		fprintf(chart, "playlist_track = 17\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 8:
		fprintf(chart, "artist = Tomohiro Harada, Kozo Suganuma, Yoshifumi Yamaguchi\n");
		fprintf(chart, "name = 8. Tainted Lovers (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Metal\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Gregorio (Seigfried) Wilhelm III: That would be me - at your service.\"<br>\"The Great Cathedral itself is my Gitaroo, with which I share my fate.\"<br>\"I find inspiration for my art in tragedy and pain.\"");
		fprintf(chart, "<br><br>EPILEPSY WARNING<br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 5\n");
		fprintf(chart, "preview_start_time = 34730\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 10\n");
		fprintf(chart, "playlist_track = 19\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 9:
		fprintf(chart, "artist = Tomohiro Harada, Yoshifumi Yamaguchi, Yusuke Nishikawa\n");
		fprintf(chart, "name = 9. Overpass (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Rock\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Kirah: The most powerful Gravillian warrior who was trained from childhood to be a Gitaroo Master.\"<br>\"No one can defeat her - not even me!\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 2\n");
		fprintf(chart, "preview_start_time = 22750\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 11\n");
		fprintf(chart, "playlist_track = 21\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 10:
		fprintf(chart, "artist = COIL, Tomohiro Harada\n");
		fprintf(chart, "name = 10. Resurrection (MODCHART)\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Hard Rock\n");
		fprintf(chart, "year = 2001\n");
		fprintf(chart, "loading_phrase = \"Lord Zowie: He has revealed to us his true power!\"<br>\"His Armored Gitaroo, the apex of Gravillian technology, will now rev into life!\"");
		fprintf(chart, "<br><br>Modchart Full\n");
		fprintf(chart, "diff_guitar = 5\n");
		fprintf(chart, "preview_start_time = 23830\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 12\n");
		fprintf(chart, "playlist_track = 23\n");
		fprintf(chart, "video_start_time = 0\n");
		break;
	case 11:
		fprintf(chart, "artist = Tomozuin H, Andy MacKinlay, Satoshi Izumi\n");
		fprintf(chart, "name = 11. Metal Header (MODCHART) [MP-Only]\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Rock\n");
		fprintf(chart, "year = 2006\n");
		fprintf(chart, "loading_phrase =Modchart Full<br>Quickplay<br>Make sure to move the score meter off the middle of the screen\n");
		fprintf(chart, "diff_guitar = 4\n");
		fprintf(chart, "diff_rhythm = 4\n");
		fprintf(chart, "preview_start_time = 0\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 14\n");
		fprintf(chart, "playlist_track = 25\n");
		fprintf(chart, "video_start_time = 0\n");
		fprintf(chart, "lyrics = 1\n");
		break;
	case 12:
		fprintf(chart, "artist = Tomozuin H, Yordanka Farres, Kenjiro Imawara, Tetsuo Koizumi\n");
		fprintf(chart, "name = 12. Toda Pasion (MODCHART) [SP/Online MP]\n");
		fprintf(chart, "charter = Sonicfind (Assisted by CyclopsDragon)\n");
		fprintf(chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(chart, "genre = Latin Rock\n");
		fprintf(chart, "year = 2006\n");
		fprintf(chart, "loading_phrase =Modchart Full<br>Quickplay<br>Make sure to move the score meter off the middle of the screen\n");
		fprintf(chart, "diff_guitar = 4\n");
		fprintf(chart, "diff_rhythm = 4\n");
		fprintf(chart, "preview_start_time = 0\n");
		fprintf(chart, "icon = gitaroo\n");
		fprintf(chart, "album_track = 15\n");
		fprintf(chart, "playlist_track = 26\n");
		fprintf(chart, "video_start_time = 0\n");
		fprintf(chart, "lyrics = 1\n");
	}
	fprintf(chart, "modchart = 1\n");
	//Converting totalDur to milliseconds
	fprintf(chart, "song_length = %lu\n", (unsigned long)ceil(totalDuration / 48));
	fprintf(chart, "Property of Koei Co. Ltd. Gitaroo Man (C) KOEI 2001");
}