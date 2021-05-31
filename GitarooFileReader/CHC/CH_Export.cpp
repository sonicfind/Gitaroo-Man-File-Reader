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
#include "pch.h"
#include "Global_Functions.h"
#include "CH_Export.h"
using namespace std;

bool CH_Exporter::exportChart()
{
	std::vector<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to chart - w/ spaces inbetween.\n", g_global.tabs.c_str());
		for (size_t sectIndex = 0; sectIndex <m_song.m_sections.size(); sectIndex++)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), sectIndex, m_song.m_sections[sectIndex].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent LinkedList::List: ", g_global.tabs.c_str());
			for (size_t index : sectionIndexes)
				printf("%s ",m_song.m_sections[index].getName());
			putchar('\n');
		}
		switch (GlobalFunctions::insertIndexValues(sectionIndexes, "bgscynp",m_song.m_sections.size()))
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
		printf("%s ",m_song.m_sections[sectionIndexes[sect]].getName());
	putchar('\n');
#ifdef _DEBUG
	m_modchart = true;
#else
	{
		//"yes.txt" is essentially a run-time checked setting
		FILE* test;
		if (!fopen_s(&test, "yes.txt", "r"))
		{
			m_modchart = true;
			fclose(test);
		}
	}
#endif
	//Used for modchart song.ini file that accompanies the .chart
	//Fills in the "song_length" tag
	float totalDuration = 0;
	if (!convertSong(sectionIndexes))
		return false;
	printf("%s\n", g_global.tabs.c_str());
	printf("%sAll Notes Converted\n", g_global.tabs.c_str());
	string filename = m_song.m_filename.substr(0,m_song.m_filename.length() - 4);
	bool written = false;
	if (m_modchart)
	{
		string filenameMod = filename;
		do
		{
			switch (GlobalFunctions::fileOverwriteCheck(filenameMod + ".chart"))
			{
			case GlobalFunctions::ResultType::Quit:
				for (size_t i = 0; i < m_exporter.m_sync.size();)
				{
					m_exporter.m_sync[i].m_position -= 2 * s_TICKS_PER_BEAT;
					//Remove the two extra synctracks
					if (m_exporter.m_sync[i].m_position < 0)
						m_exporter.m_sync.erase(m_exporter.m_sync.begin() + i);
					else
						i++;
				}
				for (size_t i = 0; i < m_exporter.m_events.size(); i++)
					m_exporter.m_events[i].m_position -= 2 * s_TICKS_PER_BEAT;
				g_global.quit = true;
				break;
			case GlobalFunctions::ResultType::No:
				filenameMod += "_T";
				printf("%s\n", g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::Yes:
				//Generate the ini file if it's a chart from the original games (stage 2 also including separate EN & JP charts)
				m_exporter.open(filenameMod + ".ini");
				m_exporter.writeIni(m_song.m_stage, (unsigned long)ceilf(totalDuration * (.0625f / 3)),m_song.m_shortname.find("HE") == string::npos);
				m_exporter.open(filenameMod + ".chart");
				if (m_song.m_imc[0])
					m_exporter.write(true);
				else
					m_exporter.writeDuetModchart();
				m_exporter.close();
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
			m_exporter.open(filename + ".chart");
			m_exporter.write(false);
			m_exporter.close();
			written = true;
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return written;
}

bool CH_Exporter::convertSong(std::vector<size_t>& sectionIndexes)
{
	bool multiplayer = toupper(m_song.m_shortname.back()) == 'M' || (m_song.m_stage > 10 &&m_song.m_imc[0]);
	bool grdFound = true, phrFound[2] = { false, false }, done = false;
	if (m_modchart)
	{
		//Set starting position for generating the modchart
		m_exporter.m_sync.emplace_back(m_position, 2, 12632);
		m_position += s_TICKS_PER_BEAT;
		if (m_song.m_shortname.find("ST02_HE") != string::npos)
			m_exporter.m_sync.emplace_back(m_position, 2, 73983, false);
		else if (0 <m_song.m_stage && m_song.m_stage <= 12)
		{
			static const unsigned long bpmArray[] = { 77538, 74070, 76473, 79798, 74718, 79658, 73913, 76523, 74219, 75500, 80000, 80000 };
			m_exporter.m_sync.emplace_back(m_position, 2, bpmArray[m_song.m_stage - 1], false);
		}
		m_position += s_TICKS_PER_BEAT;
		grdFound = false;
	}

	m_exporter.m_events.reserve(sectionIndexes.size() + 1);
	m_exporter.m_events.resize(sectionIndexes.size() + 1);
	m_exporter.m_events[0] = { m_position, "GMFR EXPORT V2.0" };
	for (size_t sect = 0; sect < sectionIndexes.size(); ++sect)
	{
		SongSection& section = m_song.m_sections[sectionIndexes[sect]];
		printf("%sConverting %s\n", g_global.tabs.c_str(), section.getName());
		switch (section.getPhase())
		{
		case SongSection::Phase::INTRO:
			m_exporter.m_events[sect + 1] = { m_position, "section INTRO - " + string(section.getName()) }; break;
		case SongSection::Phase::CHARGE:
			m_exporter.m_events[sect + 1] = { m_position, "section CHARGE - " + string(section.getName()) }; break;
		case SongSection::Phase::BATTLE:
			m_exporter.m_events[sect + 1] = { m_position, "section BATTLE - " + string(section.getName()) }; break;
		case SongSection::Phase::FINAL_AG:
			m_exporter.m_events[sect + 1] = { m_position, "section FINAL_AG - " + string(section.getName()) }; break;
		case SongSection::Phase::HARMONY:
			m_exporter.m_events[sect + 1] = { m_position, "section HARMONY - " + string(section.getName()) }; break;
		case SongSection::Phase::END:
			m_exporter.m_events[sect + 1] = { m_position, "section END - " + string(section.getName()) }; break;
		default:
			m_exporter.m_events[sect + 1] = { m_position, "section FINAL_I - " + string(section.getName()) };
		}

		if (m_exporter.m_sync.size() == 0 || unsigned long(section.getTempo() * 1000) != m_exporter.m_sync.back().m_bpm)
		{
			if (m_song.m_stage == 12 && section.getPhase() == SongSection::Phase::INTRO)
			{
				m_exporter.m_sync.emplace_back(m_position, 3, unsigned long(section.getTempo() * 1000));
				m_exporter.m_sync.emplace_back(m_position + 3 * s_TICKS_PER_BEAT, 4, 0);
			}
			else
				m_exporter.m_sync.emplace_back(m_position, 4, unsigned long(section.getTempo() * 1000));
			if (section.getTempo() < 80.0f)
				m_exporter.m_sync.back().m_eighth = " 3";
		}

		if (section.getPhase() != SongSection::Phase::INTRO && !strstr(section.getName(), "BRK")) //If not INTRO phase or BRK section
		{
			if (!done)
			{
				for (size_t playerIndex = 0; playerIndex < section.getNumPlayers() && (!done && !g_global.quit);
					multiplayer ? ++playerIndex : playerIndex += 2)
				{
					const size_t endIndex = (playerIndex + 1) * section.getNumCharts();
					for (size_t index = playerIndex * section.getNumCharts();
						index < endIndex && (!done || (m_guardPromptType == 1 && !g_global.quit));
						++index)
					{
						const Chart& chart = section.m_charts[index++];
						if (chart.getNumGuards() && !g_global.quit)
						{
							while (!grdFound)
							{
								printf("%sHow will guard phrases be handled? [Only effects the modchart export]\n", g_global.tabs.c_str());
								printf("%sB - Base orientation only\n", g_global.tabs.c_str());
								printf("%sF - Choose full-song orientation\n", g_global.tabs.c_str());
								printf("%sS - Determined per section\n", g_global.tabs.c_str());
								printf("%sC - Determined per chart\n", g_global.tabs.c_str());
								switch (GlobalFunctions::menuChoices("bfsc"))
								{
								case GlobalFunctions::ResultType::Quit:
									printf("%s\n", g_global.tabs.c_str());
									printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
									return false;
								case GlobalFunctions::ResultType::Success:
									switch (g_global.answer.character)
									{
									case 'f':
										if (!getOrientation(section.getName()))
										{
											printf("%sClone Hero chart creation cancelled.\n", g_global.tabs.c_str());
											return false;
										}
										__fallthrough;
									case 'b':
										printf("%s\n", g_global.tabs.c_str());
										grdFound = true;
										break;
									case 's':
										printf("%s\n", g_global.tabs.c_str());
										m_guardPromptType = 1;
										grdFound = true;
										break;
									default:
										printf("%s\n", g_global.tabs.c_str());
										m_guardPromptType = 2;
										grdFound = true;
									}
								}
							}
							if (m_guardPromptType == 1)
							{
								if (!getOrientation(section.getName()))
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
								printf("%sN - Determined per note\n", g_global.tabs.c_str());
								printf("%sP - Determined per phrase bar\n", g_global.tabs.c_str());
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
										if (!getFrets(section.getName(), 0, subPlayer))
										{
											printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
											return false;
										}
										phrFound[subPlayer] = true;
										break;
									case 'n':
										printf("%s\n", g_global.tabs.c_str());
										m_phraseBarPromptType[subPlayer] = 1;
										phrFound[subPlayer] = true;
										break;
									default:
										printf("%s\n", g_global.tabs.c_str());
										m_phraseBarPromptType[subPlayer] = 2;
										phrFound[subPlayer] = true;
									}
								}
							}
						}
						if (m_guardPromptType != 1)
							done = grdFound && phrFound[0] && phrFound[1];
						else
							g_global.quit = grdFound && phrFound[0] && phrFound[1];
					}
				}
				g_global.quit = false;
			}
			const float TICKS_PER_SAMPLE = section.getTempo() * s_TICKS_PER_BEAT / s_SAMPLES_PER_MIN;
			//Marking where each the list the current section starts
			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); multiplayer ? ++playerIndex : playerIndex += 2)
				{
					Chart& chart = section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
					currentPlayer = playerIndex & 1 || (playerIndex == 2 && !m_song.m_imc[0]);
					size_t markIndex = m_exporter.m_reimportNotes[currentPlayer].m_allNotes.size();
					size_t grdIndex = 0;
					if (chart.getNumGuards())
					{
						if (m_modchart && m_guardPromptType == 2 && !getOrientation(section.getName(), playerIndex, chartIndex))
						{
							printf("%sClone Hero chart creation cancelled.\n", g_global.tabs.c_str());
							return false;
						}
						grdIndex = convertGuard(chart, TICKS_PER_SAMPLE, currentPlayer);
					}
					else
						grdIndex = m_exporter.m_modchartNotes[currentPlayer].m_allNotes.size();

					size_t phrIndex = 0;
					if (chart.getNumTracelines() > 1)
					{
						convertTrace(chart, TICKS_PER_SAMPLE, (long)roundf(TICKS_PER_SAMPLE * section.getDuration()), currentPlayer);
						try
						{
							phrIndex = convertPhrase(section, playerIndex, chartIndex, TICKS_PER_SAMPLE, currentPlayer);
						}
						catch (const char* str)
						{
							printf("%s%s\n", g_global.tabs.c_str(), str);
							return false;
						}
					}
					//Firstly, determines whether a "start" marker should be placed
					if ((chartIndex != 0 || (playerIndex >= 2 &&m_song.m_imc[0])) && (chart.getNumTracelines() > 1 || chart.getNumGuards()))
					{
						//AKA, if any notes or trace lines were added
						float pos;
						if (markIndex < m_exporter.m_reimportNotes[currentPlayer].m_allNotes.size())
							pos = 0.5f * (m_exporter.m_reimportNotes[currentPlayer].m_allNotes[markIndex]->m_position + m_exporter.m_reimportNotes[currentPlayer].m_allNotes[markIndex - 1]->m_position);
						else if (markIndex > 0)
							pos = m_exporter.m_reimportNotes[currentPlayer].m_allNotes[markIndex - 1]->m_position + 160;
						else
							pos = m_position + 160;
						m_exporter.m_reimportNotes[currentPlayer].addEvent(pos, "start");
					}
					//Sets star power phrases
					if (m_modchart)
					{
						switch (section.getPhase())
						{
						case SongSection::Phase::BATTLE:
							if (chart.getNumGuards())
								//Encapsulate all the guard marks in the subsection
								m_exporter.m_modchartNotes[currentPlayer].addStarPower(m_position + TICKS_PER_SAMPLE * (chart.m_guards[0].m_pivotAlpha + (float)chart.getPivotTime()),
									20 + TICKS_PER_SAMPLE * ((float)chart.m_guards[chart.getNumGuards() - 1].m_pivotAlpha - chart.m_guards[0].m_pivotAlpha));
							break;
						case SongSection::Phase::CHARGE:
							if (chart.getNumPhrases())
								//Encapsulate all the phrase bars in the subsection
								m_exporter.m_modchartNotes[currentPlayer].addStarPower(m_position + TICKS_PER_SAMPLE * (chart.m_phrases[0].m_pivotAlpha + (float)chart.getPivotTime()),
									TICKS_PER_SAMPLE * ((float)chart.m_phrases[chart.getNumPhrases() - 1].getEndAlpha() - chart.m_phrases[0].m_pivotAlpha));
						}
					}
				}
			}
		}
		m_position += s_TICKS_PER_BEAT * roundf(section.getDuration() * section.getTempo() / s_SAMPLES_PER_MIN);
		if (m_modchart)
			//							Samples per beat										Number of beats (rounded)
			m_samepleDuration += (s_SAMPLES_PER_MIN / section.getTempo()) * roundf(section.getDuration() * section.getTempo() / s_SAMPLES_PER_MIN);
	}
	return true;
}

size_t CH_Exporter::convertGuard(Chart& chart, const float TICKS_PER_SAMPLE, const size_t currentPlayer)
{
	// 1 = G; 2 = R; 4 = Y; 8 = B; 16 = O
	static const unsigned fretSets[5][4] = { { 4, 1, 2, 8 }, { 4, 1, 2, 16 }, { 8, 1, 2, 16 }, { 8, 1, 4, 16 }, { 8, 2, 4, 16 } };
	const static long GUARD_GAP = 8000;
	const float GUARD_OPEN_TICK_DISTANCE = GUARD_GAP * TICKS_PER_SAMPLE;
	size_t grdStarIndex = m_exporter.m_modchartNotes[currentPlayer].m_allNotes.size();

	struct open
	{
		float position = 0;
		bool forced = false;
	};

	const size_t arraySize = chart.getNumGuards() - 1;
	open* openNotes = m_modchart && arraySize ? new open[arraySize]() : nullptr;
	for (size_t i = 0, undersized = 0; i < chart.getNumGuards(); i++)
	{
		const Guard& guard = chart.m_guards[i];
		unsigned modfret = fretSets[m_guardOrientation][guard.m_button];
		unsigned fret = fretSets[2][guard.m_button];
		float pos = m_position + TICKS_PER_SAMPLE * ((float)guard.m_pivotAlpha + chart.getPivotTime());
		if (m_modchart)
		{
			if (i == 0)
				grdStarIndex = m_exporter.m_modchartNotes[currentPlayer].addNote(pos, modfret);
			else
				m_exporter.m_modchartNotes[currentPlayer].addNote(pos, modfret);
			m_exporter.m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
			if (openNotes && i < arraySize)
			{
				const long dif = chart.m_guards[i + 1].m_pivotAlpha - guard.m_pivotAlpha;
				if (dif < 5200)
				{
					if (undersized < 3)
					{
						float openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
						openNotes[i].position = openPos;
						openNotes[i].forced = (dif >> 1) * TICKS_PER_SAMPLE >= 162.5f;
						++undersized;
					}
					else
					{
						delete[arraySize] openNotes;
						openNotes = nullptr;
					}
				}
				else if (dif < 2 * GUARD_GAP)
				{
					float openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
					openNotes[i].position = openPos;
					openNotes[i].forced = (dif >> 1) * TICKS_PER_SAMPLE >= 162.5f;
					undersized = 0;
				}
				else if (dif < 240000) // five seconds
				{
					float openPos = pos + GUARD_OPEN_TICK_DISTANCE;
					openNotes[i].position = openPos;
					openNotes[i].forced = GUARD_OPEN_TICK_DISTANCE >= 162.5f;
					undersized = 0;
				}
				else if (dif < 480000) // ten seconds
				{
					float openPos = pos + (dif >> 1) * TICKS_PER_SAMPLE;
					openNotes[i].position = openPos;
					openNotes[i].forced = true;
					undersized = 0;
				}
				else
				{
					float openPos = pos + 240000 * TICKS_PER_SAMPLE;
					openNotes[i].position = openPos;
					openNotes[i].forced = true;
					undersized = 0;
				}
			}
			pos -= 2 * s_TICKS_PER_BEAT;
		}
		m_exporter.m_reimportNotes[currentPlayer].addNote(pos, fret);
	}

	if (m_modchart)
	{
		if (openNotes)
		{
			for (int index = 0; index < arraySize; ++index)
			{
				m_exporter.m_modchartNotes[currentPlayer].addNote(openNotes[index].position, 32);
				if (openNotes[index].forced)
					m_exporter.m_modchartNotes[currentPlayer].addModifier(openNotes[index].position, CHNote::Modifier::FORCED);
			}
			delete[arraySize] openNotes;
		}	
	}
	return grdStarIndex;
}

void CH_Exporter::convertTrace(Chart& chart, const float TICKS_PER_SAMPLE, const long sectionDuration, const size_t currentPlayer)
{
	for (size_t i = 0; i < chart.getNumTracelines(); i++)
	{
		float pos = TICKS_PER_SAMPLE * (chart.m_tracelines[i].m_pivotAlpha + (float)chart.getPivotTime());
		string name;
		if ((long)round(pos) < sectionDuration)
			name = "Trace";
		else
			name = "TraceP";
		if (m_modchart)
			pos += m_position - 2 * s_TICKS_PER_BEAT;
		else
			pos += m_position;
		if (i + 1 != chart.getNumTracelines())
		{
			if (chart.m_tracelines[i].m_angle == 0)
				m_exporter.m_reimportNotes[currentPlayer].addEvent(pos, name);
			else
				m_exporter.m_reimportNotes[currentPlayer].addEvent(pos, name + '_' + to_string(GlobalFunctions::radiansToDegrees(chart.m_tracelines[i].m_angle)));
			if (chart.m_tracelines[i].m_curve)
				m_exporter.m_reimportNotes[currentPlayer].addEvent(pos, "Trace_curve");
		}
		else
			m_exporter.m_reimportNotes[currentPlayer].addEvent(pos, name + "_end");
	}
}

size_t CH_Exporter::convertPhrase(SongSection& section, const size_t playerIndex, const size_t chartIndex, const float TICKS_PER_SAMPLE, const size_t currentPlayer)
{
	Chart& chart = section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
	size_t phrIndex = m_exporter.m_modchartNotes[currentPlayer].m_allNotes.size();
	unsigned long prevFret = m_strumFret[currentPlayer];
	unsigned long& fret = m_strumFret[currentPlayer];
	const size_t max = chart.getNumPhrases();
	for (size_t i = 0, note = 1, piece = 1; i < max; i++)
	{
		if (m_phraseBarPromptType[currentPlayer])
		{
			fret = chart.m_phrases[i].getColor();
			if (fret == 0)
			{
				if (section.getPhase() == SongSection::Phase::BATTLE && note == 1 && piece == 1)
					g_global.quit = !getFrets(section.getName(), 1, playerIndex + 1, chartIndex, note, piece);
				else if (piece > 1)
					g_global.quit = !getFrets(section.getName(), 2, playerIndex + 1, chartIndex, note, piece);
				else
					g_global.quit = !getFrets(section.getName(), 0, playerIndex + 1, chartIndex, note, piece);
				if (g_global.quit)
				{
					g_global.quit = false;
					throw "CH chart creation cancelled.";
				}
				else if (i + 1 == max)
					GlobalFunctions::banner(" " + string(section.getName()) + "'s Phrase Bars Converted ");
			}
		}
		float pos = m_position + TICKS_PER_SAMPLE * (chart.m_phrases[i].m_pivotAlpha + (float)chart.getPivotTime());
		if (m_phraseBarPromptType[currentPlayer] < 2 || fret == 256)
		{
			size_t maxIndex = max;
			while (i < maxIndex && !chart.m_phrases[i].m_end && i + 1 != maxIndex)
				i++;
		}
		if (fret >= 128)
			fret = prevFret;
		float endTick = m_position + TICKS_PER_SAMPLE * (chart.m_phrases[i].getEndAlpha() + (float)chart.getPivotTime());
		unsigned long addedNotes = fret & 63, removedNotes = 0;
		if (piece > 1)
		{
			if (((prevFret & 31) != 31) == ((fret & 31) != 31))
			{
				for (unsigned long color = 0, val = 1; color < 6; ++color, val <<= 1) //Used for setting extended sustains
				{
					if ((prevFret & val) && (fret & val))
					{
						if (m_modchart)
						{
							CHNote* note = m_exporter.m_modchartNotes[currentPlayer].m_colors[color].back();
							note->setEndPoint(endTick);
							note->m_fret.m_writeSustain = note->m_fret.m_sustain >= 6200 * TICKS_PER_SAMPLE;
							m_exporter.m_reimportNotes[currentPlayer].m_colors[color].back()->setEndPoint(endTick - 2 * s_TICKS_PER_BEAT);
						}
						else
							m_exporter.m_reimportNotes[currentPlayer].m_colors[color].back()->setEndPoint(endTick);
						addedNotes &= ~val; //Removal
					}
					else if (prevFret & val)
						removedNotes |= val; //Addition
				}
			}
		}
		if (addedNotes)
		{
			float duration = endTick - pos;
			if (m_modchart)
			{
				bool hammeron = false;
				size_t ntIndex = m_exporter.m_modchartNotes[currentPlayer].addNote(pos, addedNotes, duration, duration >= 9600 * TICKS_PER_SAMPLE);
				size_t added = 0;
				for (unsigned long color = 0, val = 1; added < 2 && color < 6; ++color, val <<= 1)
					if (addedNotes & val)
						added++;
				if (added == 1 && fret != prevFret)
				{
					for (size_t prev = ntIndex; prev > 0;)
					{
						if (m_exporter.m_modchartNotes[currentPlayer].m_allNotes[--prev]->m_type == CHNote::NoteType::NOTE)
						{
							hammeron = pos - m_exporter.m_modchartNotes[currentPlayer].m_allNotes[prev]->m_position < 162.5f;
							break;
						}
					}
				}
				if (fret & 64)
				{
					if (!(addedNotes & 32))
						m_exporter.m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
					else if (!hammeron)
						m_exporter.m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				}
				else if ((piece == 1) == hammeron) //Ensures a strum on new notes and hammeron's for continuations
					m_exporter.m_modchartNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				//Handles Sustain Gaps
				for (unsigned long color = 0, val = 1; color < 6; ++color, val <<= 1)
				{
					size_t index = m_exporter.m_modchartNotes[currentPlayer].m_colors[color].size();
					if (index && (!(addedNotes & val) || (fret & 31) == 31 || (prevFret & 31) == 31))
					{
						
						if (val & addedNotes)
							--index;
						if (index)
						{
							CHNote* prev = m_exporter.m_modchartNotes[currentPlayer].m_colors[color][--index];
							//Adds sustain gaps for non-extended sustain notes
							if (pos - (3400 * TICKS_PER_SAMPLE) < prev->m_position + prev->m_fret.m_sustain &&
								prev->m_position + prev->m_fret.m_sustain <= pos)
							{
								prev->m_fret.m_writeSustain = prev->setEndPoint(pos - (3400 * TICKS_PER_SAMPLE)) >= 6200 * TICKS_PER_SAMPLE;
							}
						}
					}
				}
				pos -= 2 * s_TICKS_PER_BEAT;
				if (note == 1 && piece == 1)
					phrIndex = ntIndex;
			}
			m_exporter.m_reimportNotes[currentPlayer].addNote(pos, addedNotes, duration);
			if (fret & 64)
			{
				if (m_modchart)
					m_exporter.m_reimportNotes[currentPlayer].addModifier(pos - 2 * s_TICKS_PER_BEAT, CHNote::Modifier::FORCED);
				else
					m_exporter.m_reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
			}
			else if (piece > 1 && (removedNotes || fret == 31))
			{
				m_exporter.m_reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::FORCED);
				if (!(addedNotes & 32))
					m_exporter.m_reimportNotes[currentPlayer].addModifier(pos, CHNote::Modifier::TAP);
			}
		}
		prevFret = fret & ~64;
		if (chart.m_phrases[i].m_end)
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
bool CH_Exporter::getOrientation(const char* sectionName, const size_t player, const size_t chart)
{
	while (true)
	{
		printf("%sChoose orientation for %s", g_global.tabs.c_str(), sectionName);
		player ? printf(": Player %zu - Chart %zu\n", player + 1, chart) : putchar('\n');
		printf("%s   ||G|R|Y|B|O||   T - Triangle\n", g_global.tabs.c_str());
		printf("%s===============    S - Square\n", g_global.tabs.c_str());
		printf("%s 1 ||X|O|S|T| ||   O - Circle\n", g_global.tabs.c_str());
		printf("%s 2 ||X|O|S| |T||   X - Cross\n", g_global.tabs.c_str());
		printf("%s 3 ||X|O| |S|T||(Default)\n", g_global.tabs.c_str());
		printf("%s 4 ||X| |O|S|T||\n", g_global.tabs.c_str());
		printf("%s 5 || |X|O|S|T||\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("12345", true))
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
			m_guardOrientation = (unsigned int) g_global.answer.index;
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
bool CH_Exporter::getFrets(const char* sectionName, unsigned promptType, size_t playerIndex,
													size_t chartIndex, size_t note, size_t piece)
{
	const unsigned long prevFret = m_strumFret[playerIndex];
	unsigned long& nextFret = m_strumFret[playerIndex];
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
				nextFret = 128;
				g_global.quit = true;
				break;
			case 'e':
				printf("%s\n", g_global.tabs.c_str());
				nextFret = 256;
				g_global.quit = true;
				break;
			case 'm':
			{
				nextFret = prevFret;
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
							printf("%s                                     Open Note (P) %s                                    ||\n", g_global.tabs.c_str(), nextFret & 32 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s            Using 'P' on this note will cancel out all other frets & vice versa             ||\n", g_global.tabs.c_str());
							printf("%s============================================================================================||\n", g_global.tabs.c_str());
							printf("%s                            Open Note (P) %s || Tap (T) %s                            ||\n", g_global.tabs.c_str(), nextFret & 32 ? "[ON] " : "[OFF]", nextFret & 64 ? "[ON] " : "[OFF]");
						}
						if (numColoredFrets < 4)
						{
							printf("%s Green (1) %s ||", g_global.tabs.c_str(), nextFret & 1 ? "[ON] " : "[OFF]");
							printf(" Red (2) %s ||", nextFret & 2 ? "[ON] " : "[OFF]");
							printf(" Yellow (3) %s ||", nextFret & 4 ? "[ON] " : "[OFF]");
							printf(" Blue (4) %s ||", nextFret & 8 ? "[ON] " : "[OFF]");
							printf(" Orange (5) %s ||\n", nextFret & 16 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s", g_global.tabs.c_str());
							printf("%s||", nextFret & 1 ? " Green (1) [ON]  " : "                 ");
							printf("%s||", nextFret & 2 ? " Red (2) [ON]  " : "               ");
							printf("%s||", nextFret & 4 ? " Yellow (3) [ON]  " : "                  ");
							printf("%s||", nextFret & 8 ? " Blue (4) [ON]  " : "                ");
							printf("%s||\n", nextFret & 16 ? " Orange (5) [ON]  " : "                  ");
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
							if ((nextFret & 63) == 0)
								nextFret |= 31;
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
							break;
						case 't':
							nextFret ^= 64;
							break;
						case 'p':
							if (!(nextFret & 32) && piece == 1 && numColoredFrets)
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
											nextFret &= 64;
											nextFret += 32;
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
									if (nextFret & val && !(prevFret & val))
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
														if (nextFret & val && !(prevFret & val))
														{
															nextFret ^= val;
															numColoredFrets--;
														}
													}
													nextFret |= 32;
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
									nextFret ^= 32;
							}
							break;
						default:
						{
							unsigned long color = 1UL << g_global.answer.index;
							if (nextFret & color || numColoredFrets < 4)
							{
								if (piece == 1 && !(nextFret & color) && nextFret & 32)
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
												nextFret -= 32;
												nextFret += color;
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
									nextFret ^= color;
									if (nextFret & color)
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
				if (promptType == 1 && nextFret > 6)
				{
					if (nextFret == 7)
						nextFret = 95;
					else
						nextFret = 64 + (1UL << (nextFret - 8));
				}
				else if (nextFret > 6)
					nextFret = 1UL << (nextFret + 1);
				else if (nextFret > 0)
					nextFret = 1UL << (nextFret - 1);
				else
					nextFret = 31;
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return true;
}

ChartFileExporter::ChartFileExporter(std::vector<SyncTrack>& sync, std::vector<Event>& events, NoteTrack notes[2]) : ChartFile()
{
	m_sync = sync;
	m_events = events;
	std::copy(notes, notes + 2, m_reimportNotes);
}

void ChartFileExporter::writeHeader(const bool modchart)
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
		for (size_t i = 0; i < m_sync.size();)
		{
			m_sync[i].write(m_chart);
			m_sync[i].m_position -= 2 * s_TICKS_PER_BEAT;
			//Remove the two extra synctracks
			if (m_sync[i].m_position < 0)
				m_sync.erase(m_sync.begin() + i);
			else
				++i;
		}
	}
	else
	{
		for (SyncTrack& syn : m_sync)
		{
			syn.m_eighth = "";
			syn.write(m_chart);
		}
	}
	fprintf(m_chart, "}\n[Events]\n{\n");
	if (modchart)
	{
		for (Event& evt : m_events)
		{
			evt.write(m_chart);
			evt.m_position -= 2 * s_TICKS_PER_BEAT;
		}
	}
	else
	{
		for (Event& evt : m_events)
			evt.write(m_chart);
	}
}

void ChartFileExporter::write(bool modchart)
{
	writeHeader(modchart);
	fprintf(m_chart, "}\n");
	for (size_t player = 0; player < 2; ++player)
	{
		if (modchart)
			m_modchartNotes[player].write(m_chart, player);
		else
			m_reimportNotes[player].write(m_chart, player);
	}
}

void ChartFileExporter::writeDuetModchart()
{
	writeHeader(true);
	fprintf(m_chart, "}\n");
	for (size_t player = 0; player < 2; ++player)
		m_reimportNotes[player].writeDuet(m_chart, player);
}

void ChartFileExporter::writeIni(const unsigned char stageNumber, const unsigned long totalDuration, const bool jap)
{
	fprintf(m_chart, "[song]\n");
	switch (stageNumber)
	{
	case 0:
		fprintf(m_chart, "artist = Tomohiro Harada\n"
		"name = 0. Boogie For An Afternoon <p style=\"color:#FF0000\";>(MODCHART)</p> [Tutorial]\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = You tell me\n"
		"year = 2001\n"
		"loading_phrase = \"Puma: This infernal dog is Gitaroo Man's partner.\"<br>\"I find his cutesy facade quite nauseating!\"<br>\"He aids Gitaroo Man in destroying *us* Gravillians!\""
		"<br><br>Lights Out\n"
		"diff_guitar = 1\n"
		"preview_start_time = 336070\n"
		"icon = gitaroo\n"
		"album_track = 2\n"
		"playlist_track = 1\n"
		"video_start_time = 0\n"
		"lyrics = 1\n");
		break;
	case 1:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada, Kaleb James\n"
		"name = 1. Twisted Reality <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Rock\n"
		"year = 2001\n"
		"loading_phrase = \"Panpeus: His axe is an axe!\"<br>\"The Axe Gitaroo rips right through space-time, so watch out!\"<br>\"Panpeus may have a baby face, but not baby skills!\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 3\n"
		"preview_start_time = 29000\n"
		"icon = gitaroo\n"
		"album_track = 3\n"
		"playlist_track = 2\n"
		"video_start_time = 0\n"
		"lyrics = 1\n");
		break;
	case 2:
		fprintf(m_chart, "artist = Tomohiro Harada, YUAN, a - li\n");
		if (!jap)
			fprintf(m_chart, "name = 2E. Flyin' to Your Heart (EN) <p style=\"color:#FF0000\";>(MODCHART)</p>\n");
		else
			fprintf(m_chart, "name = 2. Flyin' to Your Heart (JP) <p style=\"color:#FF0000\";>(MODCHART)</p>\n");
		fprintf(m_chart, "charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = J - Pop\n"
		"year = 2001\n"
		"loading_phrase = \"Flyin-O: A loyal soldier of the empire!\"<br>\"His synth gitaroo shoots 'Dance-Until-Death' rays!\"<br>\"His henchmen, the Little-Os, are nothing to be scoffed at either!\""
		"<br><br>EPILEPSY WARNING DURING CHARGE PHASE<br>Modchart Full\n"
		"diff_guitar = 3\n"
		"preview_start_time = 23460\n"
		"icon = gitaroo\n"
		"album_track = 4\n"
		"playlist_track = 3\n"
		"video_start_time = 0\n"
		"lyrics = 1\n");
		break;
	case 3:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada, Keiichi Yano\n"
		"name = 3. Bee Jam Blues <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Funk\n"
		"year = 2001\n"
		"loading_phrase = \"Mojo King Bee: He rules the darkness with his trumpet Gitaroo!\"<br>\"No one has ever seen the man behind the shades...\""
		"<br><br>EPILEPSY WARNING NEAR THE END<br>Modchart Full\n"
		"diff_guitar = 4\n"
		"preview_start_time = 34260\n"
		"icon = gitaroo\n"
		"album_track = 5\n"
		"playlist_track = 4\n"
		"video_start_time = 0\n"
		"lyrics = 1\n");
		break;
	case 4:
		fprintf(m_chart, "artist = Tomohiro Harada, k - nzk\n"
		"name = 4. VOID <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Industrial\n"
		"year = 2001\n"
		"loading_phrase = \"Ben-K: As a space shark, he's a rough, tough, customer just looking for trouble!\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 4\n"
		"preview_start_time = 13650\n"
		"icon = gitaroo\n"
		"album_track = 6\n"
		"playlist_track = 5\n"
		"video_start_time = 0\n");
		break;
	case 5:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada, feat.NAHKI\n"
		"name = 5. Nuff Respect <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Reggae\n"
		"year = 2001\n"
		"loading_phrase = \"Ben-K: He attacks by scratching the turntable Gitaroo on his chest - but his Karate moves prove he's a man of many talents!\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 3\n"
		"preview_start_time = 38480\n"
		"icon = gitaroo\n"
		"album_track = 7\n"
		"playlist_track = 6\n"
		"video_start_time = 0\n");
		break;
	case 6:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada\n"
		"name = 6. The Legendary Theme (Acoustic) <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Acoustic\n"
		"year = 2001\n"
		"loading_phrase = \"Kirah: A shy and reserved girl with the heart of a warrior.\"<br>\"She excels at baking Gitaroo Apple Pies.\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 2\n"
		"preview_start_time = 26120\n"
		"icon = gitaroo\n"
		"album_track = 8\n"
		"playlist_track = 7\n"
		"video_start_time = 0\n");
		break;
	case 7:
		fprintf(m_chart, "artist = Tomohiro Harada, Steve Eto, Kazuki Abe\n"
		"name = 7. Born To Be Bone <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Flamenco\n"
		"year = 2001\n"
		"loading_phrase = \"Sanbone Trio: Carrot, Soda, & Pine are three mariachis who use their bones as Gitaroos!\"<br>\"Their bare-knuckled fighting style is truly praiseworthy!\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 5\n"
		"preview_start_time = 84330\n"
		"icon = gitaroo\n"
		"album_track = 9\n"
		"playlist_track = 8\n"
		"video_start_time = 0\n");
		break;
	case 8:
		fprintf(m_chart, "artist = Tomohiro Harada, Kozo Suganuma, Yoshifumi Yamaguchi\n"
		"name = 8. Tainted Lovers <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Metal\n"
		"year = 2001\n"
		"loading_phrase = \"Gregorio (Seigfried) Wilhelm III: That would be me - at your service.\"<br>\"The Great Cathedral itself is my Gitaroo, with which I share my fate.\"<br>\"I find inspiration for my art in tragedy and pain.\""
		"<br><br>EPILEPSY WARNING<br>Modchart Full\n"
		"diff_guitar = 5\n"
		"preview_start_time = 34730\n"
		"icon = gitaroo\n"
		"album_track = 10\n"
		"playlist_track = 9\n"
		"video_start_time = 0\n");
		break;
	case 9:
		fprintf(m_chart, "artist = Tomohiro Harada, Yoshifumi Yamaguchi, Yusuke Nishikawa\n"
		"name = 9. Overpass <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Rock\n"
		"year = 2001\n"
		"loading_phrase = \"Kirah: The most powerful Gravillian warrior who was trained from childhood to be a Gitaroo Master.\"<br>\"No one can defeat her - not even me!\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 2\n"
		"preview_start_time = 22750\n"
		"icon = gitaroo\n"
		"album_track = 11\n"
		"playlist_track = 10\n"
		"video_start_time = 0\n");
		break;
	case 10:
		fprintf(m_chart, "artist = COIL, Tomohiro Harada\n"
		"name = 10. Resurrection <p style=\"color:#FF0000\";>(MODCHART)</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Hard Rock\n"
		"year = 2001\n"
		"loading_phrase = \"Lord Zowie: He has revealed to us his true power!\"<br>\"His Armored Gitaroo, the apex of Gravillian technology, will now rev into life!\""
		"<br><br>Modchart Full\n"
		"diff_guitar = 5\n"
		"preview_start_time = 23830\n"
		"icon = gitaroo\n"
		"album_track = 13\n"
		"playlist_track = 11\n"
		"video_start_time = 0\n");
		break;
	case 11:
		fprintf(m_chart, "artist = Tomozuin H, Andy MacKinlay, Satoshi Izumi\n"
		"name = 11. Metal Header <p style=\"color:#FF0000\";>(MODCHART)</p> <p style=\"color:#FF0000\";>[Online MP]</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Rock\n"
		"year = 2006\n"
		"loading_phrase =The first of two duet stage from the PSP re-release of Gitaroo Man. This is only works properly in singleplayer or Co-op ONLINE multipayer. "
		"Unless both players are on the same instrument, local multiplayer is incompatible.<br><br>Modchart Full<br>Quickplay<br>Make sure to move the score meter off the middle of the screen\n"
		"diff_guitar = 4\n"
		"diff_rhythm = 4\n"
		"preview_start_time = 0\n"
		"icon = gitaroo\n"
		"album_track = 14\n"
		"playlist_track = 12\n"
		"video_start_time = 0\n"
		"lyrics = 1\n");
		break;
	case 12:
		fprintf(m_chart, "artist = Tomozuin H, Yordanka Farres, Kenjiro Imawara, Tetsuo Koizumi\n"
		"name = 12. Toda Pasion <p style=\"color:#FF0000\";>(MODCHART)</p> <p style=\"color:#FF0000\";>[Online MP]</p>\n"
		"charter = Sonicfind\n"
		"album = Gitaroo Man: Warrior of Music [OST]\n"
		"genre = Latin Rock\n"
		"year = 2006\n"
		"loading_phrase =The second of two duet stage from the PSP re-release of Gitaroo Man. This is only works properly in singleplayer or Co-op ONLINE multipayer. "
		"Unless both players are on the same instrument, local multiplayer is incompatible.<br><br>Modchart Full<br>Quickplay<br>Make sure to move the score meter off the middle of the screen\n"
		"diff_guitar = 4\n"
		"diff_rhythm = 4\n"
		"preview_start_time = 0\n"
		"icon = gitaroo\n"
		"album_track = 15\n"
		"playlist_track = 13\n"
		"video_start_time = 0\n"
		"lyrics = 1\n");
	}
	fprintf(m_chart, "modchart = 1\n");
	//Converting totalDur to milliseconds
	fprintf(m_chart, "song_length = %lu\n", totalDuration);
	fprintf(m_chart, "Property of Koei Co. Ltd. Gitaroo Man (C) KOEI 2001");
}
