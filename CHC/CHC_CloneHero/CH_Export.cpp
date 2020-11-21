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
	LinkedList::List<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to chart - w/ spaces inbetween.\n", g_global.tabs.c_str());
		for (size_t sectIndex = 0; sectIndex < m_song->m_sections.size(); sectIndex++)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), sectIndex, m_song->m_sections[sectIndex].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent LinkedList::List: ", g_global.tabs.c_str());
			for (size_t index = 0; index < sectionIndexes.size(); index++)
				printf("%s ", m_song->m_sections[sectionIndexes[index]].getName());
			putchar('\n');
		}
		switch (GlobalFunctions::listValueInsert(sectionIndexes, "bgscynp", m_song->m_sections.size()))
		{
		case GlobalFunctions::ResultType::Help:

			break;
		case GlobalFunctions::ResultType::Quit:
			printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
			return false;
		case GlobalFunctions::ResultType::SpecialCase:
			if (sectionIndexes.size())
			{
				g_global.quit = true;
				break;
			}
		case GlobalFunctions::ResultType::Success:
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%s\n", g_global.tabs.c_str());
					printf("%sNo sections have been selected. Quit CH chart creation? [Y/N]\n", g_global.tabs.c_str());
					switch (GlobalFunctions::menuChoices("yn"))
					{
					case GlobalFunctions::ResultType::Quit:
					case GlobalFunctions::ResultType::Success:
						switch (g_global.answer.character)
						{
						case 'n':
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
							break;
						default:
							printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
							return false;
						}
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}
			else
				g_global.quit = true;
			break;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%s", g_global.tabs.c_str());
	for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
		printf("%s ", m_song->m_sections[sectionIndexes[sect]].getName());
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
	LinkedList::List<SyncTrack> sync;
	LinkedList::List<Event> events;
	//Used for modchart song.ini file that accompanies the .chart
	//Fills in the "song_length" tag
	double totalDuration = 0;
	if (!convertSong(sectionIndexes, sync, events, modchart, totalDuration))
		return false;
	GlobalFunctions::banner(" All Notes Converted ");
	printf("%s\n", g_global.tabs.c_str());
	string filename = m_song->m_name.substr(0, m_song->m_name.length() - 4);
	bool written = false;
	if (modchart)
	{
		string filenameMod = filename;
		do
		{
			switch (GlobalFunctions::fileOverwriteCheck(filenameMod + ".chart"))
			{
			case GlobalFunctions::ResultType::Quit:
				for (size_t i = 0; i < sync.size();)
				{
					sync[i].m_position -= 2 * TICKS_PER_BEAT;
					//Remove the two extra synctracks
					if (sync[i].m_position < 0)
						sync.erase(i);
					else
						i++;
				}
				for (size_t i = 0; i < events.size(); i++)
					events[i].m_position -= 2 * TICKS_PER_BEAT;
				g_global.quit = true;
				break;
			case GlobalFunctions::ResultType::No:
				filenameMod += "_T";
				printf("%s\n", g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::Yes:
				//Generate the ini file if it's a chart from the original games (stage 2 also including separate EN & JP charts)
				ChartFileExporter chart(filenameMod + ".ini");
				chart.writeIni(m_song->m_stage, totalDuration, m_song->m_shortname.find("HE") == string::npos);
				chart.open(filenameMod + ".chart");
				chart.write(sync, events, m_modchartNotes, true);
				chart.close();
				g_global.quit = true;
				written = true;
			}
			printf("%s\n", g_global.tabs.c_str());
		} while (!g_global.quit);
		g_global.quit = false;
	}
	filename += "_For_Reimporting";
	do
	{
		switch (GlobalFunctions::fileOverwriteCheck(filename + ".chart"))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::No:
			printf("%s\n", g_global.tabs.c_str());
			filename += "_T";
			break;
		case GlobalFunctions::ResultType::Yes:
			ChartFileExporter chart(filename + ".chart");
			chart.write(sync, events, m_reimportNotes, false);
			chart.close();
			written = true;
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return written;
}

bool CH_Exporter::convertSong(LinkedList::List<size_t>& sectionIndexes, LinkedList::List<SyncTrack>& sync, LinkedList::List<Event>& events, bool modchart, double& totalDuration)
{
	const double TICKS_PER_BEAT = 480.0, GUARD_GAP = 8000.0;
	unsigned orientationType = 0;
	unsigned colorType[2] = { 0, 0 };
	unsigned orientation = 3;
	double position = 0;
	unsigned long strumFret[2] = { 0, 0 };
	bool multiplayer = toupper(m_song->m_shortname[m_song->m_shortname.length() - 1]) == 'M' || (m_song->m_stage > 10 && m_song->m_imc[0]);
	if (modchart)
	{
		//Set starting position for generating the modchart
		sync.emplace_back(position, 2, 12632);
		position += TICKS_PER_BEAT;
		if (m_song->m_shortname.find("ST02_HE") != string::npos)
			sync.emplace_back(position, 2, 73983, false);
		else if (0 < m_song->m_stage && m_song->m_stage <= 12)
		{
			static const unsigned long bpmArray[] = { 77538, 74070, 76473, 79798, 74718, 79658, 73913, 76523, 74219, 75500, 80000, 80000 };
			sync.emplace_back(position, 2, bpmArray[m_song->m_stage - 1], false);
		}
		position += TICKS_PER_BEAT;
	}
	bool grdFound = !modchart, phrFound[2] = { false, false }, done = false;
	events.emplace_back(position, "GMFR EXPORT V2.0");
	for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
	{
		SongSection& section = m_song->m_sections[sectionIndexes[sect]];
		printf("%sConverting %s\n", g_global.tabs.c_str(), section.getName());
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
		if (sync.size() == 0 || unsigned long(section.getTempo() * 1000) != sync.back().m_bpm)
		{
			if (m_song->m_stage == 12 && section.getPhase() == SongSection::Phase::INTRO)
			{
				sync.emplace_back(position, 3, unsigned long(section.getTempo() * 1000));
				sync.emplace_back(position + 3 * TICKS_PER_BEAT, 4, 0);
			}
			else
				sync.emplace_back(position, 4, unsigned long(section.getTempo() * 1000));
			if (section.getTempo() < 80.0f)
				sync.back().m_eighth = " 3";
		}
		if (section.getPhase() != SongSection::Phase::INTRO && !strstr(section.getName(), "BRK")) //If not INTRO phase or BRK section
		{
			if (!done)
			{
				size_t playerIndex = 0;
				while (playerIndex < section.getNumPlayers() &&
					(!done && !g_global.quit))
				{
					size_t index = playerIndex * (size_t)section.getNumCharts();
					const size_t endIndex = (playerIndex + 1) * (size_t)section.getNumCharts();

					while (index < endIndex &&
						(!done || (orientationType == 1 && !g_global.quit)))
					{
						const Chart& chart = section.getChart(index++);
						if (chart.getNumGuards() && !g_global.quit)
						{
							while (!grdFound)
							{
								printf("%sHow will guard phrases be handled? [Only effects the modchart export]\n", g_global.tabs.c_str());
								printf("%sB - Base Orientation Only\n", g_global.tabs.c_str());
								printf("%sG - Guitar Hero Conversion\n", g_global.tabs.c_str());
								printf("%sS - Determined per Section\n", g_global.tabs.c_str());
								printf("%sC - Determined per Chart\n", g_global.tabs.c_str());
								switch (GlobalFunctions::menuChoices("bgsc"))
								{
								case GlobalFunctions::ResultType::Quit:
									printf("%s\n", g_global.tabs.c_str());
									printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
									return false;
								case GlobalFunctions::ResultType::Success:
									switch (g_global.answer.character)
									{
									case 'b':
										orientation = 0;
									case 'g':
										printf("%s\n", g_global.tabs.c_str());
										grdFound = true;
										break;
									case 's':
										printf("%s\n", g_global.tabs.c_str());
										orientationType = 1;
										grdFound = true;
										break;
									default:
										printf("%s\n", g_global.tabs.c_str());
										orientationType = 2;
										grdFound = true;
									}
								}
							}
							if (orientationType == 1)
							{
								if (!getOrientation(section.getName(), orientation))
								{
									printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
									return false;
								}
								else
									g_global.quit = true;
							}
						}
						if (chart.getNumPhrases())
						{
							const unsigned subPlayer = playerIndex & 1;
							while (!phrFound[subPlayer])
							{
								printf("%sHow will strum phrases be handled for player %u?\n", g_global.tabs.c_str(), subPlayer + 1);
								printf("%sS - Same frets for the entire song\n", g_global.tabs.c_str());
								printf("%sN - Determined per Note\n", g_global.tabs.c_str());
								printf("%sP - Determined per Phrase Bar\n", g_global.tabs.c_str());
								//printf("%s", g_global.tabs.c_str(), "Note for C, N, & P: if a phrase bar has a pre-set color from a .chart import, that color will be used.\n";
								switch (GlobalFunctions::menuChoices("snp"))
								{
								case GlobalFunctions::ResultType::Quit:
									printf("%s\n", g_global.tabs.c_str());
									printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
									return false;
								case GlobalFunctions::ResultType::Success:
									switch (g_global.answer.character)
									{
									case 's':
										printf("%s\n", g_global.tabs.c_str());
										if (!getFrets(strumFret[subPlayer], section.getName(), 0))
										{
											printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
											return false;
										}
										phrFound[subPlayer] = true;
										break;
									case 'n':
										printf("%s\n", g_global.tabs.c_str());
										colorType[subPlayer] = 1;
										phrFound[subPlayer] = true;
										break;
									default:
										printf("%s\n", g_global.tabs.c_str());
										colorType[subPlayer] = 2;
										phrFound[subPlayer] = true;
									}
								}
							}
						}
						if (orientationType != 1)
							done = grdFound && phrFound[0] && phrFound[1];
						else
							g_global.quit = grdFound && phrFound[0] && phrFound[1];
					}
					multiplayer ? ++playerIndex : playerIndex += 2;
				}
				g_global.quit = false;
			}
			const double TICKS_PER_SAMPLE = section.getTempo() * TICKS_PER_BEAT / s_SAMPLES_PER_MIN;
			//Marking where each the list the current section starts
			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); playerIndex++)
				{
					if (!(playerIndex & 1))
					{
						if (playerIndex == 2 && !m_song->m_imc[0])
							currentPlayer = 1;
						else
							currentPlayer = 0;
					}
					else if (multiplayer)
						currentPlayer = 1;
					else
						continue;
					Chart& chart = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
					if (modchart && orientationType == 2 && chart.getNumGuards() && !getOrientation(section.getName(), orientation, playerIndex, chartIndex))
					{
						printf("%sClone Hero chart creation cancelled.\n", g_global.tabs.c_str());
						return false;
					}
					size_t markIndex = m_reimportNotes[currentPlayer].m_allNotes.size();
					size_t grdIndex = convertGuard(chart, position, TICKS_PER_SAMPLE, currentPlayer, orientation, modchart);
					size_t phrIndex = 0;
					if (chart.getNumTracelines() > 1)
					{
						convertTrace(chart, position, TICKS_PER_SAMPLE, (long)round(TICKS_PER_SAMPLE * section.getDuration()), currentPlayer, modchart);
						try
						{
							phrIndex = convertPhrase(section, playerIndex, chartIndex, position, TICKS_PER_SAMPLE, currentPlayer,
																	colorType[currentPlayer], strumFret[currentPlayer], modchart);
						}
						catch (const char* str)
						{
							printf("%s%s\n", g_global.tabs.c_str(), str);
							return false;
						}
					}
					//Firstly, determines whether a "start" marker should be placed
					if ((chartIndex != 0 || (playerIndex >= 2 && m_song->m_imc[0])) && (chart.getNumTracelines() > 1 || chart.getNumGuards()))
					{
						//AKA, if any notes or trace lines were added
						double pos;
						if (markIndex < m_reimportNotes[currentPlayer].m_allNotes.size())
							pos = (m_reimportNotes[currentPlayer].m_allNotes[markIndex]->m_position + m_reimportNotes[currentPlayer].m_allNotes[markIndex - 1]->m_position) / 2;
						else if (markIndex > 0)
							pos = m_reimportNotes[currentPlayer].m_allNotes[markIndex - 1]->m_position + 160;
						else
							pos = position + 160;
						m_reimportNotes[currentPlayer].addEvent(pos, "start");
					}
					//Sets star power phrases
					if (modchart)
					{
						switch (section.getPhase())
						{
						case SongSection::Phase::BATTLE:
							if (chart.getNumGuards())
								//Encapsulate all the guard marks in the subsection
								m_modchartNotes[currentPlayer].addStarPower(position + TICKS_PER_SAMPLE * (chart.getGuard(0).getPivotAlpha() + double(chart.getPivotTime())),
									20 + TICKS_PER_SAMPLE * ((double)chart.getGuard(chart.getNumGuards() - 1).getPivotAlpha() - chart.getGuard(0).getPivotAlpha()));
							break;
						case SongSection::Phase::CHARGE:
							if (chart.getNumPhrases())
								//Encapsulate all the phrase bars in the subsection
								m_modchartNotes[currentPlayer].addStarPower(position + TICKS_PER_SAMPLE * (chart.getPhrase(0).getPivotAlpha() + double(chart.getPivotTime())),
									TICKS_PER_SAMPLE * ((double)chart.getPhrase(chart.getNumPhrases() - 1).getEndAlpha() - chart.getPhrase(0).getPivotAlpha()));
						}
					}
				}
			}
		}
		position += TICKS_PER_BEAT * round(double(section.getDuration()) * section.getTempo() / s_SAMPLES_PER_MIN);
		if (modchart)
			totalDuration += (s_SAMPLES_PER_MIN / section.getTempo()) * round(double(section.getDuration()) * section.getTempo() / s_SAMPLES_PER_MIN);
	}
	return true;
}

size_t CH_Exporter::convertGuard(Chart& chart, const double position, const double TICKS_PER_SAMPLE, const size_t currentPlayer, const unsigned orientation, const bool modchart)
{
	static const unsigned fretSets[4][4] = { { 1, 3, 4, 0 }, { 3, 4, 0, 1 }, { 4, 0, 1, 3 }, { 3, 0, 1, 4 } };
	const static double GUARD_GAP = 8000.0;
	const double GUARD_OPEN_TICK_DISTANCE = GUARD_GAP * TICKS_PER_SAMPLE;
	size_t grdStarIndex = m_modchartNotes[currentPlayer].m_allNotes.size();
	for (size_t i = 0; i < chart.getNumGuards(); i++)
	{
		const Guard& guard = chart.getGuard(i);
		unsigned modfret = fretSets[orientation][guard.getButton()];
		unsigned fret = fretSets[3][guard.getButton()];
		double pos = position + TICKS_PER_SAMPLE * ((double)guard.getPivotAlpha() + chart.getPivotTime());
		if (modchart)
		{
			size_t val = m_modchartNotes[currentPlayer].addNote(pos, 1 << modfret);
			m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
			if (i == 0)
				grdStarIndex = val;
			if (i + 1 != chart.getNumGuards())
			{
				const long dif = chart.getGuard(i + 1).getPivotAlpha() - guard.getPivotAlpha();
				if (dif >= 480000)		//If dif is >= ten seconds
				{
					double openPos = pos + 240000 * TICKS_PER_SAMPLE;
					m_modchartNotes[currentPlayer].addNote(openPos, 32);
					m_modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
				else if (dif >= 240000) //If 5 seconds <= dif < ten seconds
				{
					double openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
					m_modchartNotes[currentPlayer].addNote(openPos, 32);
					m_modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
				else if (dif >= 2 * GUARD_GAP)
				{
					double openPos = pos + GUARD_OPEN_TICK_DISTANCE;
					m_modchartNotes[currentPlayer].addNote(openPos, 32);
					if (GUARD_OPEN_TICK_DISTANCE >= 162.5)
						m_modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
				else
				{
					double openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
					m_modchartNotes[currentPlayer].addNote(openPos, 32);
					if ((dif >> 1) * TICKS_PER_SAMPLE >= 162.5)
						m_modchartNotes[currentPlayer].addModifier(openPos, CHNote::Modifier::FORCED);
				}
			}
			pos -= 2 * TICKS_PER_BEAT;
		}
		m_reimportNotes[currentPlayer].addNote(pos, 1UL << fret);
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
				m_reimportNotes[currentPlayer].addEvent(pos, name);
			else
				m_reimportNotes[currentPlayer].addEvent(pos, name + '_' + to_string(GlobalFunctions::radiansToDegrees(chart.getTraceline(i).getAngle())));
			if (chart.getTraceline(i).getCurve())
				m_reimportNotes[currentPlayer].addEvent(pos, "Trace_curve");
		}
		else
			m_reimportNotes[currentPlayer].addEvent(pos, name + "_end");
	}
}

size_t CH_Exporter::convertPhrase(SongSection& section, const size_t playerIndex, const size_t chartIndex, const double position,
	const double TICKS_PER_SAMPLE,const size_t currentPlayer, const unsigned colorType, unsigned long& strumFret, const bool modchart)
{
	Chart& chart = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
	size_t phrIndex = m_modchartNotes[currentPlayer].m_allNotes.size();
	unsigned long prevFret = strumFret;
	const size_t max = chart.getNumPhrases();
	for (size_t i = 0, note = 1, piece = 1; i < max; i++)
	{
		if (colorType)
		{
			strumFret = chart.getPhrase(i).getColor();
			if (strumFret == 0)
			{
				if (section.getPhase() == SongSection::Phase::BATTLE && note == 1 && piece == 1)
					g_global.quit = !getFrets(strumFret, section.getName(), 1, playerIndex + 1, chartIndex, note, piece);
				else if (piece > 1)
					g_global.quit = !getFrets(strumFret, section.getName(), 2, playerIndex + 1, chartIndex, note, piece);
				else
					g_global.quit = !getFrets(strumFret, section.getName(), 0, playerIndex + 1, chartIndex, note, piece);
				if (g_global.quit)
				{
					g_global.quit = false;
					throw "CH chart creation cancelled.";
				}
				else if (i + 1 == max)
					GlobalFunctions::banner(" " + string(section.getName()) + "'s Phrase Bars Converted ");
			}
		}
		double pos = position + TICKS_PER_SAMPLE * (chart.getPhrase(i).getPivotAlpha() + double(chart.getPivotTime()));
		if (colorType < 2 || strumFret == 256)
		{
			size_t maxIndex = max;
			while (i < maxIndex && !chart.getPhrase(i).getEnd() && i + 1 != maxIndex)
				i++;
		}
		if (strumFret >= 128)
			strumFret = prevFret;
		double endTick = position + TICKS_PER_SAMPLE * (chart.getPhrase(i).getEndAlpha() + double(chart.getPivotTime()));
		unsigned long addedNotes = strumFret & 63, removedNotes = 0;
		if (piece > 1)
		{
			if (((prevFret & 31) != 31) == ((strumFret & 31) != 31))
			{
				for (unsigned long color = 0; color < 6; color++) //Used for setting extended sustains
				{
					unsigned long val = 1UL << color;
					if (prevFret & val && strumFret & val)
					{
						if (modchart)
						{
							CHNote& note = m_modchartNotes[currentPlayer].m_colors[color].back();
							note.setEndPoint(endTick);
							note.m_fret.m_writeSustain = note.m_fret.m_sustain >= 6200 * TICKS_PER_SAMPLE;
							m_reimportNotes[currentPlayer].m_colors[color].back().setEndPoint(endTick - 2 * TICKS_PER_BEAT);
						}
						else
							m_reimportNotes[currentPlayer].m_colors[color].back().setEndPoint(endTick);
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
				size_t ntIndex = m_modchartNotes[currentPlayer].addNote(pos, addedNotes, duration, duration >= 6200 * TICKS_PER_SAMPLE);
				size_t added = 0;
				for (size_t color = 0; added < 2 && color < 6; color++)
					if (addedNotes & (size_t(1) << color))
						added++;
				if (added == 1 && strumFret != prevFret)
				{
					for (size_t prev = ntIndex; prev > 0;)
					{
						if (m_modchartNotes[currentPlayer].m_allNotes[--prev]->m_type == CHNote::NoteType::NOTE)
						{
							hammeron = pos - m_modchartNotes[currentPlayer].m_allNotes[prev]->m_position < 162.5;
							break;
						}
					}
				}
				if (strumFret & 64)
				{
					if (!(addedNotes & 32))
						m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
					else if (!hammeron)
						m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				}
				else if ((piece == 1) == hammeron) //Ensures a strum on new notes and hammeron's for continuations
					m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				//Handles Sustain Gaps
				for (size_t color = 0; color < 6; color++)
				{
					size_t index = m_modchartNotes[currentPlayer].m_colors[color].size();
					if (index > 0 && (!(addedNotes & (size_t(1) << color)) || (strumFret & 31) == 31 || (prevFret & 31) == 31))
					{
						if ((size_t(1) << color) & addedNotes)
							--index;
						if (index > 0)
						{
							CHNote& prev = m_modchartNotes[currentPlayer].m_colors[color][--index];
							//Adds m_sustain gaps for non-extended m_sustain notes
							if (pos - (3400 * TICKS_PER_SAMPLE) < prev.m_position + prev.m_fret.m_sustain &&
								prev.m_position + prev.m_fret.m_sustain <= pos)
							{
								prev.m_fret.m_writeSustain = prev.setEndPoint(pos - (3400 * TICKS_PER_SAMPLE)) >= 6200 * TICKS_PER_SAMPLE;
							}
						}
					}
				}
				pos -= 2 * TICKS_PER_BEAT;
				if (note == 1 && piece == 1)
					phrIndex = ntIndex;
			}
			m_reimportNotes[currentPlayer].addNote(pos, addedNotes, duration);
			if (strumFret & 64)
			{
				if (modchart)
					m_reimportNotes[currentPlayer].addModifier(pos - 2 * TICKS_PER_BEAT, CHNote::Modifier::FORCED);
				else
					m_reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
			}
			else if (piece > 1 && (removedNotes || strumFret == 31))
			{
				m_reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				if (!(addedNotes & 32))
					m_reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
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
bool CH_Exporter::getOrientation(const char* sectionName, unsigned orientation, size_t player, size_t chart)
{
	while (true)
	{
		printf("%sChoose orientation for %s", g_global.tabs.c_str(), sectionName);
		player ? printf(": Player %zu - Chart %zu\n", player + 1, chart) : putchar('\n');
		printf("%s   ||U|L|R|D||   T - Triangle\n", g_global.tabs.c_str());
		printf("%s===============  S - Square\n", g_global.tabs.c_str());
		printf("%s 1 ||T|S|O|X||   O - Circle\n", g_global.tabs.c_str());
		printf("%s 2 ||S|X|T|O||   X - Cross\n", g_global.tabs.c_str());
		printf("%s 3 ||O|T|X|S||\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("123", true))
		{
		case GlobalFunctions::ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			return false;
		case GlobalFunctions::ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
		case GlobalFunctions::ResultType::Failed:
			break;
		case GlobalFunctions::ResultType::Success:
			printf("%s\n", g_global.tabs.c_str());
			orientation = (unsigned int) g_global.answer.index;
			return true;
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
bool CH_Exporter::getFrets(unsigned long& strumFret, const char* sectionName, unsigned promptType, size_t playerIndex,
													size_t chartIndex, size_t note, size_t piece, unsigned long prevFret)
{
	GlobalFunctions::banner(" Clone Hero Export - Fret Selection ");
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
			printf("%sSelect the fret for %s: Player %zu - Chart %zu - Note %zu", g_global.tabs.c_str(), sectionName, playerIndex, chartIndex, note);
			if (piece > 1)
				printf(" - Piece %zu", piece);
			printf("\n");
		}
		else
			printf("%sSelect the fret to use for the entire song\n", g_global.tabs.c_str());
		if (!g_global.multi)
		{
			printf("%s==============================================================||\n", g_global.tabs.c_str());
			printf("%s            Default Color (D) || Open Note (P)                ||\n", g_global.tabs.c_str());
			printf("%s Green (G) || Red (R) || Yellow (Y) || Blue (B) || Orange (O) ||\n", g_global.tabs.c_str());
			if (promptType == 1)
			{
			printf("%s            Tap - Default (0) || Hopo - Open Note (6)         ||\n", g_global.tabs.c_str());
				printf("%s   Tap (1) || Tap (2) ||   Tap (3)  ||  Tap (4) ||    Tap (5) ||\n", g_global.tabs.c_str());
			}
			else if (promptType == 2)
			{
				printf("%s              Continue with the previous color (C)            ||\n", g_global.tabs.c_str());
				printf("%s        Extend previous color to the end of the note (E)      ||\n", g_global.tabs.c_str());
			}
			printf("%s         Manually toggle each fret you wish to use (M)        ||\n", g_global.tabs.c_str());
			printf("%s==============================================================||\n", g_global.tabs.c_str());
		}
		switch (GlobalFunctions::menuChoices(choices, true))
		{
		case GlobalFunctions::ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			return false;
		case GlobalFunctions::ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			switch (choices[g_global.answer.index])
			{
			case 'c':
				printf("%s\n", g_global.tabs.c_str());
				strumFret = 128;
				g_global.quit = true;
				break;
			case 'e':
				printf("%s\n", g_global.tabs.c_str());
				strumFret = 256;
				g_global.quit = true;
				break;
			case 'm':
			{
				strumFret = prevFret;
				size_t numColoredFrets = 0;
				++g_global;
				choices = "12345pf";
				if (piece == 1 && playerIndex)
					choices += 't';
				GlobalFunctions::banner(" Clone Hero Export - Fret Selection - Toggle Mode ");
				do
				{
					if (playerIndex)
					{
						printf("%sColor Toggles for %s: Player %zu - Chart %zu - Note %zu", g_global.tabs.c_str(), sectionName, playerIndex, chartIndex, note);
						if (piece > 1)
							printf(" - Piece %zu", piece);
						printf("\n");
					}
					else
						printf("%sToggle the frets that you wish to use for the entire song\n", g_global.tabs.c_str());
					if (!g_global.multi)
					{
						printf("%s============================================================================================||\n", g_global.tabs.c_str());
						printf("%s           You can have a max of 4 different fret *colors* toggled ON at one time           ||\n", g_global.tabs.c_str());
						if (piece > 1 || playerIndex == 0)
						{
							if (playerIndex == 0)
								printf("%s                  Using 'P' will cancel out all other frets & vice versa                    ||\n", g_global.tabs.c_str());
							printf("%s============================================================================================||\n", g_global.tabs.c_str());
							printf("%s                                     Open Note (P) %s                                    ||\n", g_global.tabs.c_str(), strumFret & 32 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s            Using 'P' on this note will cancel out all other frets & vice versa             ||\n", g_global.tabs.c_str());
							printf("%s============================================================================================||\n", g_global.tabs.c_str());
							printf("%s                            Open Note (P) %s || Tap (T) %s                            ||\n", g_global.tabs.c_str(), strumFret & 32 ? "[ON] " : "[OFF]", strumFret & 64 ? "[ON] " : "[OFF]");
						}
						if (numColoredFrets < 4)
						{
							printf("%s Green (1) %s ||", g_global.tabs.c_str(), strumFret & 1 ? "[ON] " : "[OFF]");
							printf(" Red (2) %s ||", strumFret & 2 ? "[ON] " : "[OFF]");
							printf(" Yellow (3) %s ||", strumFret & 4 ? "[ON] " : "[OFF]");
							printf(" Blue (4) %s ||", strumFret & 8 ? "[ON] " : "[OFF]");
							printf(" Orange (5) %s ||\n", strumFret & 16 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s", g_global.tabs.c_str());
							printf("%s||", strumFret & 1 ? " Green (1) [ON]  " : "                 ");
							printf("%s||", strumFret & 2 ? " Red (2) [ON]  " : "               ");
							printf("%s||", strumFret & 4 ? " Yellow (3) [ON]  " : "                  ");
							printf("%s||", strumFret & 8 ? " Blue (4) [ON]  " : "                ");
							printf("%s||\n", strumFret & 16 ? " Orange (5) [ON]  " : "                  ");
						}
						printf("%s                        Finish fret selection for this note/piece (F)                       ||\n", g_global.tabs.c_str());
						printf("%s============================================================================================||\n", g_global.tabs.c_str());
					}
					switch (GlobalFunctions::menuChoices(choices, true))
					{
					case GlobalFunctions::ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						return false;
					case GlobalFunctions::ResultType::Help:
						printf("%s\n", g_global.tabs.c_str());
					case GlobalFunctions::ResultType::Failed:
						break;
					default:
						printf("%s\n", g_global.tabs.c_str());
						switch (choices[g_global.answer.index])
						{
						case 'f':
							if ((strumFret & 63) == 0)
								strumFret |= 31;
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
							break;
						case 't':
							strumFret ^= 64;
							break;
						case 'p':
							if (!(strumFret & 32) && piece == 1 && numColoredFrets)
							{
								do
								{
									printf("%sAll colored frets will need to be untoggled. Continue toggling open note? [Y/N]\n", g_global.tabs.c_str());
									switch (GlobalFunctions::menuChoices("yn"))
									{
									case GlobalFunctions::ResultType::Quit:
										return false;
									case GlobalFunctions::ResultType::Success:
										switch (g_global.answer.character)
										{
										case 'y':
											strumFret &= 64;
											strumFret += 32;
											numColoredFrets = 0;
										case 'n':
											g_global.quit = true;
											printf("%s\n", g_global.tabs.c_str());
										}
									}
								} while (!g_global.quit);
								g_global.quit = false;
							}
							else
							{
								unsigned long added = 0;
								bool toggle = true;
								for (unsigned color = 0; color < 5; color++)
								{
									unsigned long val = 1UL << color;
									if (strumFret & val && !(prevFret & val))
									{
										do
										{
											printf("%s\n", g_global.tabs.c_str());
											printf("%sAll colored frets not used for the previous note will need to be untoggled. Continue toggling open note? [Y/N]\n", g_global.tabs.c_str());
											switch (GlobalFunctions::menuChoices("yn"))
											{
											case GlobalFunctions::ResultType::Quit:
												return false;
											case GlobalFunctions::ResultType::Success:
												switch (g_global.answer.character)
												{
												case 'y':
													for (; color < 5; color++)
													{
														val = 1UL << color;
														if (strumFret & val && !(prevFret & val))
														{
															strumFret ^= val;
															numColoredFrets--;
														}
													}
													strumFret |= 32;
												case 'n':
													toggle = false;
													g_global.quit = true;
												}
											}
										} while (!g_global.quit);
										g_global.quit = false;
										break;
									}
								}
								if (toggle)
									strumFret ^= 32;
							}
							break;
						default:
						{
							unsigned long color = 1UL << g_global.answer.index;
							if (strumFret & color || numColoredFrets < 4)
							{
								if (piece == 1 && !(strumFret & color) && strumFret & 32)
								{
									do
									{
										printf("%s\n", g_global.tabs.c_str());
										printf("%sOpen note will need to be untoggled. Continue toggling this color? [Y/N]\n", g_global.tabs.c_str());
										switch (GlobalFunctions::menuChoices("yn"))
										{
										case GlobalFunctions::ResultType::Quit:
											return false;
										case GlobalFunctions::ResultType::Success:
											switch (g_global.answer.character)
											{
											case 'y':
												strumFret -= 32;
												strumFret += color;
												numColoredFrets++;
											case 'n':
												g_global.quit = true;
											}
										}
									} while (!g_global.quit);
									g_global.quit = false;
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
								printf("%sOne of the other four colors must be untoggled before you can toggle this one.\n", g_global.tabs.c_str());
						}
						}
					}
				} while (!g_global.quit);
				--g_global;
			}
			break;
			default:
				if (promptType == 1 && strumFret > 6)
				{
					if (strumFret == 7)
						strumFret = 95;
					else
						strumFret = 64 + (1UL << (strumFret - 8));
				}
				else if (strumFret > 6)
					strumFret = 1UL << (strumFret + 1);
				else if (strumFret > 0)
					strumFret = 1UL << (strumFret - 1);
				else
					strumFret = 31;
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return true;
}

void ChartFileExporter::write(LinkedList::List<SyncTrack>& sync, LinkedList::List<Event>& events, NoteTrack(&notes)[2], bool modchart)
{
	fprintf(m_chart, "[Song]\n{\n");
	fprintf(m_chart, "  Offset = 0\n");
	fprintf(m_chart, "  Resolution = 480\n");
	fprintf(m_chart, "  Player2 = bass\n");
	fprintf(m_chart, "  Difficulty = 0\n");
	fprintf(m_chart, "  PreviewStart = 0\n");
	fprintf(m_chart, "  PreviewEnd = 0\n");
	fprintf(m_chart, "  Genre = \"rock\"\n");
	fprintf(m_chart, "  MediaType = \"cd\"\n}\n");
	fprintf(m_chart, "[SyncTrack]\n{\n");
	if (modchart)
	{
		for (size_t i = 0; i < sync.size();)
		{
			sync[i].write(m_chart);
			sync[i].m_position -= 2 * TICKS_PER_BEAT;
			//Remove the two extra synctracks
			if (sync[i].m_position < 0)
				sync.erase(i);
			else
				i++;
		}
	}
	else
	{
		for (size_t i = 0; i < sync.size(); i++)
		{
			sync[i].m_eighth = "";
			sync[i].write(m_chart);
		}
	}
	fprintf(m_chart, "}\n[Events]\n{\n");
	if (modchart)
	{
		for (size_t i = 0; i < events.size(); i++)
		{
			events[i].write(m_chart);
			events[i].m_position -= 2 * TICKS_PER_BEAT;
		}
	}
	else
		for (Event& evt : events)
			evt.write(m_chart);
	fprintf(m_chart, "}\n");
	for (size_t player = 0; player < 2; player++)
		notes[player].write(m_chart, player);
}

void ChartFileExporter::writeIni(const unsigned char stageNumber, const double totalDuration, const bool jap)
{
	fprintf(m_chart, "[song]\n");
	switch (stageNumber)
	{
	case 0:
		break;
	case 1:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada, Kaleb James\n");
		fprintf(m_chart, "name = 1. Twisted Reality (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Rock\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Panpeus: His axe is an axe!\"<br>\"The Axe Gitaroo rips right through space-time, so watch out!\"<br>\"Panpeus may have a baby face, but not baby skills!\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 3\n");
		fprintf(m_chart, "preview_start_time = 29000\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 3\n");
		fprintf(m_chart, "playlist_track = 5\n");
		fprintf(m_chart, "video_start_time = 0\n");
		fprintf(m_chart, "lyrics = 1\n");
		break;
	case 2:
		fprintf(m_chart, "artist = Tomohiro Harada, YUAN, a - li\n");
		if (!jap)
			fprintf(m_chart, "name = 2E. Flyin' to Your Heart (EN) (MODCHART)\n");
		else
			fprintf(m_chart, "name = 2. Flyin' to Your Heart (JP) (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = J - Pop\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Flyin-O: A loyal soldier of the empire!\"<br>\"His synth gitaroo shoots 'Dance-Until-Death' rays!\"<br>\"His henchmen, the Little-Os, are nothing to be scoffed at either!\"");
		fprintf(m_chart, "<br><br>EPILEPSY WARNING DURING CHARGE PHASE<br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 3\n");
		fprintf(m_chart, "preview_start_time = 23460\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 4\n");
		fprintf(m_chart, "playlist_track = 7\n");
		fprintf(m_chart, "video_start_time = 0\n");
		fprintf(m_chart, "lyrics = 1\n");
		break;
	case 3:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada, Keiichi Yano\n");
		fprintf(m_chart, "name = 3. Bee Jam Blues (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Funk\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Mojo King Bee: He rules the darkness with his trumpet Gitaroo!\"<br>\"No one has ever seen the man behind the shades...\"");
		fprintf(m_chart, "<br><br>EPILEPSY WARNING NEAR THE END<br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 4\n");
		fprintf(m_chart, "preview_start_time = 34260\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 5\n");
		fprintf(m_chart, "playlist_track = 9\n");
		fprintf(m_chart, "video_start_time = 0\n");
		fprintf(m_chart, "lyrics = 1\n");
		break;
	case 4:
		fprintf(m_chart, "artist = Tomohiro Harada, k - nzk\n");
		fprintf(m_chart, "name = 4. VOID (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Industrial\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Ben-K: As a space shark, he's a rough, tough, customer just looking for trouble!\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 4\n");
		fprintf(m_chart, "preview_start_time = 13650\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 6\n");
		fprintf(m_chart, "playlist_track = 11\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 5:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada, feat.NAHKI\n");
		fprintf(m_chart, "name = 5. Nuff Respect (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Reggae\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Ben-K: He attacks by scratching the turntable Gitaroo on his chest - but his Karate moves prove he's a man of many talents!\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 3\n");
		fprintf(m_chart, "preview_start_time = 38480\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 7\n");
		fprintf(m_chart, "playlist_track = 13\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 6:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada\n");
		fprintf(m_chart, "name = 6. The Legendary Theme (Acoustic) (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Acoustic\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Kirah: A shy and reserved girl with the heart of a warrior.\"<br>\"She excels at baking Gitaroo Apple Pies.\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 2\n");
		fprintf(m_chart, "preview_start_time = 26120\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 8\n");
		fprintf(m_chart, "playlist_track = 15\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 7:
		fprintf(m_chart, "artist = Tomohiro Harada, Steve Eto, Kazuki Abe\n");
		fprintf(m_chart, "name = 7. Born To Be Bone (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Flamenco\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Sanbone Trio: Carrot, Soda, & Pine are three mariachis who use their bones as Gitaroos!\"<br>\"Their bare-knuckled fighting style is truly praiseworthy!\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 5\n");
		fprintf(m_chart, "preview_start_time = 84330\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 9\n");
		fprintf(m_chart, "playlist_track = 17\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 8:
		fprintf(m_chart, "artist = Tomohiro Harada, Kozo Suganuma, Yoshifumi Yamaguchi\n");
		fprintf(m_chart, "name = 8. Tainted Lovers (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Metal\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Gregorio (Seigfried) Wilhelm III: That would be me - at your service.\"<br>\"The Great Cathedral itself is my Gitaroo, with which I share my fate.\"<br>\"I find inspiration for my art in tragedy and pain.\"");
		fprintf(m_chart, "<br><br>EPILEPSY WARNING<br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 5\n");
		fprintf(m_chart, "preview_start_time = 34730\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 10\n");
		fprintf(m_chart, "playlist_track = 19\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 9:
		fprintf(m_chart, "artist = Tomohiro Harada, Yoshifumi Yamaguchi, Yusuke Nishikawa\n");
		fprintf(m_chart, "name = 9. Overpass (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Rock\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Kirah: The most powerful Gravillian warrior who was trained from childhood to be a Gitaroo Master.\"<br>\"No one can defeat her - not even me!\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 2\n");
		fprintf(m_chart, "preview_start_time = 22750\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 11\n");
		fprintf(m_chart, "playlist_track = 21\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 10:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada\n");
		fprintf(m_chart, "name = 10. Resurrection (MODCHART)\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Hard Rock\n");
		fprintf(m_chart, "year = 2001\n");
		fprintf(m_chart, "loading_phrase = \"Lord Zowie: He has revealed to us his true power!\"<br>\"His Armored Gitaroo, the apex of Gravillian technology, will now rev into life!\"");
		fprintf(m_chart, "<br><br>Modchart Full\n");
		fprintf(m_chart, "diff_guitar = 5\n");
		fprintf(m_chart, "preview_start_time = 23830\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 12\n");
		fprintf(m_chart, "playlist_track = 23\n");
		fprintf(m_chart, "video_start_time = 0\n");
		break;
	case 11:
		fprintf(m_chart, "artist = Tomozuin H, Andy MacKinlay, Satoshi Izumi\n");
		fprintf(m_chart, "name = 11. Metal Header (MODCHART) [MP-Only]\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Rock\n");
		fprintf(m_chart, "year = 2006\n");
		fprintf(m_chart, "loading_phrase =Modchart Full<br>Quickplay<br>Make sure to move the score meter off the middle of the screen\n");
		fprintf(m_chart, "diff_guitar = 4\n");
		fprintf(m_chart, "diff_rhythm = 4\n");
		fprintf(m_chart, "preview_start_time = 0\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 14\n");
		fprintf(m_chart, "playlist_track = 25\n");
		fprintf(m_chart, "video_start_time = 0\n");
		fprintf(m_chart, "lyrics = 1\n");
		break;
	case 12:
		fprintf(m_chart, "artist = Tomozuin H, Yordanka Farres, Kenjiro Imawara, Tetsuo Koizumi\n");
		fprintf(m_chart, "name = 12. Toda Pasion (MODCHART) [SP/Online MP]\n");
		fprintf(m_chart, "charter = Sonicfind\n");
		fprintf(m_chart, "album = Gitaroo Man: Warrior of Music [OST]\n");
		fprintf(m_chart, "genre = Latin Rock\n");
		fprintf(m_chart, "year = 2006\n");
		fprintf(m_chart, "loading_phrase =Modchart Full<br>Quickplay<br>Make sure to move the score meter off the middle of the screen\n");
		fprintf(m_chart, "diff_guitar = 4\n");
		fprintf(m_chart, "diff_rhythm = 4\n");
		fprintf(m_chart, "preview_start_time = 0\n");
		fprintf(m_chart, "icon = gitaroo\n");
		fprintf(m_chart, "album_track = 15\n");
		fprintf(m_chart, "playlist_track = 26\n");
		fprintf(m_chart, "video_start_time = 0\n");
		fprintf(m_chart, "lyrics = 1\n");
	}
	fprintf(m_chart, "modchart = 1\n");
	//Converting totalDur to milliseconds
	fprintf(m_chart, "song_length = %lu\n", (unsigned long)ceil(totalDuration / 48));
	fprintf(m_chart, "Property of Koei Co. Ltd. Gitaroo Man (C) KOEI 2001");
}
