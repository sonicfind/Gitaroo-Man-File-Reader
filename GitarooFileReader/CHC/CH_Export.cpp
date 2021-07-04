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
using namespace GlobalFunctions;

bool CHC::exportForCloneHero()
{
	banner(" " + m_filename + " - .CHART creation ");
	if (m_unorganized != 0)
	{
		do
		{
			printf("%s%s.CHC must be fully fixed & organized before it can exported to .chart.\n", g_global.tabs.c_str(), m_filename.c_str());
			printf("%sFix & organize the file now? [Y/N]\n", g_global.tabs.c_str());
			const auto result = menuChoices("yn");
			if (result == ResultType::Success && g_global.answer.character == 'y')
			{
				applyChanges(true, false);
				banner(" " + m_filename + " - .CHART creation ");
				g_global.quit = true;
			}
			else if (result != ResultType::Quit && result != ResultType::Failed)
			{
				printf("%sCH Chart Export on %s aborted due to not being fully organized.\n", g_global.tabs.c_str(), m_filename.c_str());
				return false;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}

	std::vector<size_t> sectionIndexes = indexInsertionDialogue(m_sections
			, "Type the index for each section that you wish to chart - in chronological order and w/ spaces in-between."
			, "CH Chart Creation"
			, "bgscnp");

	printf_tab("");
	for (auto sect : sectionIndexes)
		printf("%s ", m_sections[sect].getName());
	putchar('\n');

	CHC_To_CloneHero exporter;
	if (!exporter.convertSong(this, sectionIndexes))
		return false;
	printf("%s\n%sAll Notes Converted\n", g_global.tabs.c_str(), g_global.tabs.c_str());

	return exporter.writeChart(this);
}

CHC_To_CloneHero::CHC_To_CloneHero()
	: ChartFileExporter()
#ifdef _DEBUG
{}
#else
{
	FILE* test;
	// "yes.txt" is essentially a run-time checked setting
	// If an error occurs
	if (fopen_s(&test, "yes.txt", "r"))
	{
		m_modchart = false;
		m_grdFound = true;
	}
	else if (test)
		fclose(test);
}
#endif
	

bool CHC_To_CloneHero::writeChart(CHC* song)
{
	bool written = false;
	if (m_modchart)
	{
		string filenameMod = song->m_directory + song->m_filename;
		do
		{
			switch (fileOverwriteCheck(filenameMod + ".chart"))
			{
			case ResultType::Quit:
				for (auto iter = m_sync.begin(); iter != m_sync.end();)
				{
					iter->m_position_ticks -= 2 * s_TICKS_PER_BEAT;
					//Remove the two extra synctracks
					if (iter->m_position_ticks < 0)
						m_sync.erase(iter++);
					else
						++iter;
				}

				for (auto& ev : m_events)
					ev.m_position_ticks -= 2 * s_TICKS_PER_BEAT;

				g_global.quit = true;
				break;
			case ResultType::No:
				filenameMod += "_T";
				break;
			case ResultType::Yes:
				//Generate the ini file if it's a chart from the original games (stage 2 also including separate EN & JP charts)
				open(filenameMod + ".ini", true);
				writeIni(song->m_stage, (unsigned long)ceilf((.0625f / 3) * m_samepleDuration), song->m_filename.find("HE") == string::npos);
				open(filenameMod + ".chart", true);

				if (song->m_imc[0])
					write(true);
				else
					writeDuetModchart();

				close();
				g_global.quit = true;
				written = true;
			}
			printf("%s\n", g_global.tabs.c_str());
		} while (!g_global.quit);
		g_global.quit = false;
	}

	string filename = song->m_directory + song->m_filename + "_For_Reimporting";
	do
	{
		switch (fileOverwriteCheck(filename + ".chart"))
		{
		case ResultType::Quit:
			g_global.quit = true;
			break;
		case ResultType::No:
			filename += "_T";
			break;
		case ResultType::Yes:
			open(filename + ".chart", true);
			write(false);
			close();
			written = true;
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;

	return written;
}

bool CHC_To_CloneHero::convertSong(CHC* song, std::vector<size_t>& sectionIndexes)
{
	if (m_modchart)
	{
		// Set starting position for generating the modchart
		m_sync.emplace_back(m_position_ticks, 2, 12632);
		m_position_ticks += s_TICKS_PER_BEAT;

		if (song->m_filename.find("ST02_HE") != string::npos)
			m_sync.emplace_back(m_position_ticks, 0, 73983);
		else if (0 < song->m_stage && song->m_stage <= 12)
		{
			static const unsigned long bpmArray[] = { 77538, 74070, 76473, 79798, 74718, 79658, 73913, 76523, 74219, 75500, 80000, 80000 };
			m_sync.emplace_back(m_position_ticks, 0, bpmArray[song->m_stage - 1], false);
		}
		m_position_ticks += s_TICKS_PER_BEAT;
	}

	const bool multiplayer = toupper(song->m_filename.back()) == 'M' || (song->m_stage > 10 && song->m_imc[0]);
	bool doneChecking = false;
	m_events.push_back({ m_position_ticks, "GMFR EXPORT V2.0" });
	for (auto& sect : sectionIndexes)
	{
		SongSection& section = song->m_sections[sect];
		printf("%sConverting %s\n", g_global.tabs.c_str(), section.getName());
		switch (section.getPhase())
		{
		case SongSection::Phase::INTRO:
			m_events.push_back({ m_position_ticks, "section INTRO - " + string(section.getName()) }); break;
		case SongSection::Phase::CHARGE:
			m_events.push_back({ m_position_ticks, "section CHARGE - " + string(section.getName()) }); break;
		case SongSection::Phase::BATTLE:
			m_events.push_back({ m_position_ticks, "section BATTLE - " + string(section.getName()) }); break;
		case SongSection::Phase::FINAL_AG:
			m_events.push_back({ m_position_ticks, "section FINAL_AG - " + string(section.getName()) }); break;
		case SongSection::Phase::HARMONY:
			m_events.push_back({ m_position_ticks, "section HARMONY - " + string(section.getName()) }); break;
		case SongSection::Phase::END:
			m_events.push_back({ m_position_ticks, "section END - " + string(section.getName()) }); break;
		default:
			m_events.push_back({ m_position_ticks, "section FINAL_I - " + string(section.getName()) });
		}

		if (m_events.size() == 2 || unsigned long(section.getTempo() * 1000) != m_sync.back().m_bpm)
		{
			// Toda Pasion is off by one beat. Yay for rushed development
			if (song->m_stage == 12 && section.getPhase() == SongSection::Phase::INTRO)
			{
				m_sync.push_back({ m_position_ticks
								 , 3
								 , unsigned long(section.getTempo() * 1000) });

				m_sync.push_back({ m_position_ticks + 3 * s_TICKS_PER_BEAT
								 , 4 });
			}
			else
				m_sync.push_back({ m_position_ticks
								 , m_events.size() == 2 ? 4UL : 0UL
								 , unsigned long(section.getTempo() * 1000)
								 , section.getTempo() < 80.0f ? 3UL : 2UL });
		}

		if (section.getPhase() != SongSection::Phase::INTRO
			&& !strstr(section.getName(), "BRK")
			&& !strstr(section.getName(), "BREAK")) //If not INTRO phase or BRK section
		{
			if (!doneChecking)
			{
				try
				{
					doneChecking = checkNoteHandling(section, multiplayer);
				}
				catch (...)
				{
					printf("%sClone Hero chart creation cancelled.\n", g_global.tabs.c_str());
					return false;
				}
			}

			const float TICKS_PER_SAMPLE = section.getTempo() * s_TICKS_PER_BEAT / s_SAMPLES_PER_MIN;
			//Marking where each the list the current section starts
			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0; playerIndex < section.getNumPlayers(); multiplayer ? ++playerIndex : playerIndex += 2)
				{
					Chart& chart = section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
					size_t currentPlayer = playerIndex & 1 || (playerIndex == 2 && !song->m_imc[0]);
					auto markIter = m_reimportNotes[currentPlayer].m_allNotes.end();
					if (m_reimportNotes[currentPlayer].m_allNotes.size())
						--markIter;
					size_t grdIndex = 0;

					if (chart.getNumGuards())
					{
						if (m_modchart && m_guardPromptType == 2 && !getOrientation(section.getName(), playerIndex, chartIndex))
						{
							printf("%sClone Hero chart creation cancelled.\n", g_global.tabs.c_str());
							return false;
						}
						convertGuards(chart, TICKS_PER_SAMPLE, currentPlayer);
					}

					if (chart.getNumTracelines() > 1)
					{
						convertTrace(chart, TICKS_PER_SAMPLE, (long)roundf(TICKS_PER_SAMPLE * section.getDuration()), currentPlayer);
						try
						{
							if (m_phraseBarPromptType[currentPlayer])
								for (auto& phrase : chart.m_phrases)
									if (phrase.getColor() == 0)
									{
										banner(" Clone Hero Export - Fret Selections ");
										banner(" " + string(section.getName()) + ": Player " + to_string(playerIndex) + " - Chart " + to_string(chartIndex) + ' ');
										break;
									}

							convertPhrases(chart, TICKS_PER_SAMPLE, currentPlayer, section.getPhase());
						}
						catch (const char* str)
						{
							printf("%s%s\n", g_global.tabs.c_str(), str);
							printf("%sClone Hero chart creation cancelled.\n", g_global.tabs.c_str());
							return false;
						}
					}

					// Firstly, determines whether a "start" marker should be placed
					if ((chartIndex != 0
							// Second subsection for P1 of a non-duet song
							|| (playerIndex >= 2 && song->m_imc[0]))
						&& (chart.getNumTracelines() > 1 || chart.getNumGuards()))
					{
						markIter++;
						m_reimportNotes[currentPlayer].addEvent(markIter->m_position_ticks, "start");
					}
				}
			}
		}
		m_position_ticks += s_TICKS_PER_BEAT * roundf(section.getDuration() * section.getTempo() / s_SAMPLES_PER_MIN);

		if (m_modchart)
			//							Samples per beat										Number of beats (rounded)
			m_samepleDuration += (s_SAMPLES_PER_MIN / section.getTempo()) * roundf(section.getDuration() * section.getTempo() / s_SAMPLES_PER_MIN);
	}

	return true;
}

bool CHC_To_CloneHero::checkNoteHandling(SongSection& section, const bool multiplayer)
{
	for (size_t playerIndex = 0; playerIndex < section.getNumPlayers(); multiplayer ? ++playerIndex : playerIndex += 2)
	{
		for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); ++chartIndex)
		{
			const Chart& chart = section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
			if (chart.getNumGuards())
			{
				while (!m_grdFound)
				{
					printf("%sHow will guard phrases be handled? [Only effects the modchart export]\n", g_global.tabs.c_str());
					printf("%sB - Base orientation only\n", g_global.tabs.c_str());
					printf("%sF - Choose full-song orientation\n", g_global.tabs.c_str());
					printf("%sS - Determined per section\n", g_global.tabs.c_str());
					printf("%sC - Determined per chart\n", g_global.tabs.c_str());
					switch (menuChoices("bfsc"))
					{
					case ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						throw;
					case ResultType::Success:
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
							m_grdFound = true;
							break;
						case 's':
							printf("%s\n", g_global.tabs.c_str());
							m_guardPromptType = 1;
							m_grdFound = true;
							break;
						default:
							printf("%s\n", g_global.tabs.c_str());
							m_guardPromptType = 2;
							m_grdFound = true;
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
					else if (m_phraseFound[0] && m_phraseFound[1])
						return false;
				}
			}

			if (chart.getNumPhrases())
			{
				const unsigned subPlayer = playerIndex & 1;
				while (!m_phraseFound[subPlayer])
				{
					printf("%sHow will strum phrases be handled for player %u?\n", g_global.tabs.c_str(), subPlayer + 1);
					printf("%sS - Same frets for the entire song\n", g_global.tabs.c_str());
					printf("%sN - Determined per note\n", g_global.tabs.c_str());
					printf("%sP - Determined per phrase bar\n", g_global.tabs.c_str());
					//printf("%s", g_global.tabs.c_str(), "Note for C, N, & P: if a phrase bar has a pre-set color from a .chart import, that color will be used.\n";
					switch (menuChoices("snp"))
					{
					case ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
						return false;
					case ResultType::Success:
						switch (g_global.answer.character)
						{
						case 's':
							printf("%s\n", g_global.tabs.c_str());
							banner(" Clone Hero Export - Full Song Fret Selection ");
							banner(" Player " + to_string(playerIndex) + " ");
							if (!getFrets(3, subPlayer))
							{
								printf("%sCH chart creation cancelled.\n", g_global.tabs.c_str());
								return false;
							}
							m_phraseFound[subPlayer] = true;
							break;
						case 'n':
							printf("%s\n", g_global.tabs.c_str());
							m_phraseBarPromptType[subPlayer] = 1;
							m_phraseFound[subPlayer] = true;
							break;
						default:
							printf("%s\n", g_global.tabs.c_str());
							m_phraseBarPromptType[subPlayer] = 2;
							m_phraseFound[subPlayer] = true;
						}
					}
				}
			}

			if (m_grdFound && m_phraseFound[0] && m_phraseFound[1])
				return m_guardPromptType != 1;
		}
	}
	return false;
}

void CHC_To_CloneHero::convertGuards(Chart& chart, const float TICKS_PER_SAMPLE, const size_t currentPlayer)
{
	// 1  = G
	// 2  = R
	// 4  = Y
	// 8  = B
	// 16 = O
	static const unsigned fretSets[5][4] =
	{ { 4, 1, 2, 8 }
	, { 4, 1, 2, 16 }
	, { 8, 1, 2, 16 }
	, { 8, 1, 4, 16 }
	, { 8, 2, 4, 16 } };

	const static long GUARD_GAP = 8000;
	const float GUARD_OPEN_TICK_DISTANCE = GUARD_GAP * TICKS_PER_SAMPLE;
	std::pair<float, float> starPower;

	struct open
	{
		float position = 0;
		bool forced = false;
	};

	const size_t arraySize = chart.getNumGuards() - 1;
	open* openNotes = m_modchart && arraySize ? new open[arraySize]() : nullptr;
	for (size_t i = 0, undersized = 0; i < chart.getNumGuards(); i++)
	{
		float pos = m_position_ticks + TICKS_PER_SAMPLE * ((float)chart.m_guards[i].m_pivotAlpha + chart.getPivotTime());
		if (m_modchart)
		{
			if (i == 0)
				starPower.first = pos;
			else if (i + 1 == chart.getNumGuards())
				starPower.second = pos + 20;

			m_modchartNotes[currentPlayer].addNote(pos, fretSets[m_guardOrientation][chart.m_guards[i].m_button])->m_isTap = true;

			if (openNotes && i < arraySize)
			{
				const long dif = chart.m_guards[i + 1].m_pivotAlpha - chart.m_guards[i].m_pivotAlpha;
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
		m_reimportNotes[currentPlayer].addNote(pos, fretSets[2][chart.m_guards[i].m_button]);
	}

	if (m_modchart)
	{
		if (openNotes)
		{
			for (int index = 0; index < arraySize; ++index)
				m_modchartNotes[currentPlayer].addNote(openNotes[index].position, 32)->m_isForced = openNotes[index].forced;
			delete[arraySize] openNotes;
		}

		// Encapsulate all the guard marks in the subsection
		m_modchartNotes[currentPlayer].addStarPower(starPower.first, starPower.second);
	}
}

void CHC_To_CloneHero::convertTrace(Chart& chart, const float TICKS_PER_SAMPLE, const long sectionDuration, const size_t currentPlayer)
{
	for (size_t i = 0; i < chart.getNumTracelines(); i++)
	{
		float pos = TICKS_PER_SAMPLE * (chart.m_tracelines[i].m_pivotAlpha + (float)chart.getPivotTime());
		string name = (long)round(pos) < sectionDuration ? "Trace" : "TraceP";

		if (m_modchart)
			pos += m_position_ticks - 2 * s_TICKS_PER_BEAT;
		else
			pos += m_position_ticks;

		if (i + 1 != chart.getNumTracelines())
		{
			if (chart.m_tracelines[i].m_angle == 0)
				m_reimportNotes[currentPlayer].addEvent(pos, name);
			else
				m_reimportNotes[currentPlayer].addEvent(pos, name + '_' + to_string(radiansToDegrees(chart.m_tracelines[i].m_angle)));

			if (chart.m_tracelines[i].m_curve)
				m_reimportNotes[currentPlayer].addEvent(pos, "Trace_curve");
		}
		else
			m_reimportNotes[currentPlayer].addEvent(pos, name + "_end");
	}
}

void CHC_To_CloneHero::convertPhrases(Chart& chart, const float TICKS_PER_SAMPLE, const size_t currentPlayer, const SongSection::Phase phase)
{
	std::pair<float, float> starPower;
	unsigned long prevFret = m_strumFret[currentPlayer];
	unsigned long& fret = m_strumFret[currentPlayer];

	for (size_t i = 0, note = 1, piece = 1; i < chart.getNumPhrases(); i++)
	{
		if (m_phraseBarPromptType[currentPlayer])
		{
			fret = chart.m_phrases[i].getColor();
			if (fret == 0)
			{
				if (phase == SongSection::Phase::BATTLE && note == 1 && piece == 1)
					g_global.quit = !getFrets(1, currentPlayer, note, piece);
				else if (piece > 1)
					g_global.quit = !getFrets(2, currentPlayer, note, piece);
				else
					g_global.quit = !getFrets(0, currentPlayer, note, piece);

				if (g_global.quit)
				{
					g_global.quit = false;
					throw "CH chart creation cancelled.";
				}	
			}
		}

		float pos = m_position_ticks + TICKS_PER_SAMPLE * (chart.m_phrases[i].m_pivotAlpha + (float)chart.getPivotTime());
		if (m_phraseBarPromptType[currentPlayer] < 2 || fret == 256)
			while (i + 1 != chart.getNumPhrases() && !chart.m_phrases[i].m_end)
				++i;

		if (fret >= 128)
			fret = prevFret;

		float endTick = m_position_ticks + TICKS_PER_SAMPLE * (chart.m_phrases[i].getEndAlpha() + (float)chart.getPivotTime());
		unsigned long addedNotes = fret & 63, removedNotes = 0;
		if (piece > 1)
		{
			if (((prevFret & 31) != 31) == ((fret & 31) != 31))
			{
				// Used for setting extended sustains
				for (unsigned long color = 0, val = 1; color < 6; ++color, val <<= 1)
				{
					if ((prevFret & val) && (fret & val))
					{
						if (m_modchart)
						{
							for (auto colorIter = m_modchartNotes[currentPlayer].m_colors[color].rbegin();
								colorIter != m_modchartNotes[currentPlayer].m_colors[color].rend();
								++colorIter)
								if ((*colorIter)->m_sustain > 0)
								{
									(*colorIter)->setEndPoint(endTick, 6200 * TICKS_PER_SAMPLE);
									break;
								}
							
							for (auto colorIter = m_reimportNotes[currentPlayer].m_colors[color].rbegin();
								colorIter != m_reimportNotes[currentPlayer].m_colors[color].rend();
								++colorIter)
								if ((*colorIter)->m_sustain > 0)
								{
									(*colorIter)->setEndPoint(endTick - 2 * s_TICKS_PER_BEAT);
									break;
								}
						}
						else
							for (auto colorIter = m_reimportNotes[currentPlayer].m_colors[color].rbegin();
								colorIter != m_reimportNotes[currentPlayer].m_colors[color].rend();
								++colorIter)
								if ((*colorIter)->m_sustain > 0)
								{
									(*colorIter)->setEndPoint(endTick);
									break;
								}
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
				size_t added = 0;
				for (unsigned long color = 0, val = 1; added < 2 && color < 6; ++color, val <<= 1)
					if (addedNotes & val)
						++added;

				auto ntIterator = m_modchartNotes[currentPlayer].addNote(pos, addedNotes, duration, duration >= 9600 * TICKS_PER_SAMPLE);
				bool hammeron = false;
				if (added == 1 && fret != prevFret)
				{
					for (auto prev = std::make_reverse_iterator(ntIterator);
						prev != m_modchartNotes[currentPlayer].m_allNotes.rend();
						++prev)
					{
						if (prev->hasColors())
						{
							hammeron = pos - prev->m_position_ticks < 162.5f;
							break;
						}
					}
				}

				if (fret & 64)
				{
					// Open notes do not have a tapped form
					if (!(addedNotes & 32))
						ntIterator->m_isTap = true;
					else if (!hammeron)
						ntIterator->m_isForced = true;
				}
				// Ensures a strum on new notes and hammerons for continuations
				else if ((piece == 1) == hammeron) 
					ntIterator->m_isForced = true;

				// Handles Sustain Gaps
				for (unsigned long color = 0, val = 1; color < 6; ++color, val <<= 1)
				{
					auto index = m_modchartNotes[currentPlayer].m_colors[color].rbegin();
					if (index != m_modchartNotes[currentPlayer].m_colors[color].rend()
						&& (!(addedNotes & val)
							|| (fret & 31) == 31
							|| (prevFret & 31) == 31))
					{
						
						if (!(addedNotes & val)
							|| ++index != m_modchartNotes[currentPlayer].m_colors[color].rend())
						{
							//Adds sustain gaps for non-extended sustain notes
							if (pos - (3400 * TICKS_PER_SAMPLE) < (*index)->m_parent->m_position_ticks + (*index)->m_sustain
								&& (*index)->m_parent->m_position_ticks + (*index)->m_sustain <= pos)
								(*index)->setEndPoint(pos - (3400 * TICKS_PER_SAMPLE), 6200 * TICKS_PER_SAMPLE);
						}
					}
				}

				if (note == 1 && piece == 1)
					starPower.first = pos;
				else if (i + 1 == chart.getNumPhrases())
					starPower.second = endTick;
				pos -= 2 * s_TICKS_PER_BEAT;
			}
			
			auto ntIterator = m_reimportNotes[currentPlayer].addNote(pos, addedNotes, duration);
			if (fret & 64)
				ntIterator->m_isForced = true;
			else if (piece > 1 && (removedNotes || fret == 31))
			{
				ntIterator->m_isForced = true;
				if (!(addedNotes & 32))
					ntIterator->m_isTap = true;
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

	if (m_phraseBarPromptType[currentPlayer])
		banner(" Phrase Bars Converted ");

	if (m_modchart && phase == SongSection::Phase::CHARGE)
		// Encapsulate all the phrase bars in the subsection
		m_modchartNotes[currentPlayer].addStarPower(starPower.first, starPower.second);
}

//Selecting which controller setup to correspond to
bool CHC_To_CloneHero::getOrientation(const char* sectionName, const size_t player, const size_t chart)
{
	banner(" Clone Hero Export - Guard Orientation Selection ");
	while (true)
	{
		printf("%sChoose orientation for %s", g_global.tabs.c_str(), sectionName);
		player ? printf(": Player %zu - Chart %zu\n", player + 1, chart) : putchar('\n');
		printf("%s   ||G|R|Y|B|O||   T - Triangle\n", g_global.tabs.c_str());
		printf("%s================   S - Square\n", g_global.tabs.c_str());
		printf("%s 1 ||X|O|S|T| ||   O - Circle\n", g_global.tabs.c_str());
		printf("%s 2 ||X|O|S| |T||   X - Cross\n", g_global.tabs.c_str());
		printf("%s 3 ||X|O| |S|T||(Default)\n", g_global.tabs.c_str());
		printf("%s 4 ||X| |O|S|T||\n", g_global.tabs.c_str());
		printf("%s 5 || |X|O|S|T||\n", g_global.tabs.c_str());
		switch (menuChoices("12345", true))
		{
		case ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			return false;
		case ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
			__fallthrough;
		case ResultType::Failed:
			break;
		case ResultType::Success:
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
bool CHC_To_CloneHero::getFrets(unsigned promptType, const size_t currentplayer, size_t note, size_t piece)
{
	const unsigned long prevFret = m_strumFret[currentplayer];
	
	string choices = "dgrybop";
	if (promptType == 1)
		choices += "0123456";
	else if (promptType == 2)
		choices += "ce";
	choices += 'm';

	do		//Ask for the fret color in each iteration
	{
		if (promptType != 3)
		{
			printf("%sSelect the fret for Note %zu", g_global.tabs.c_str(), note);
			if (piece > 1)
				printf(" - Piece %zu", piece);
			printf("\n");
		}
		else
			printf("%sSelect the fret to use\n", g_global.tabs.c_str());

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
			printf("%s                 (Use this option for chords)                 ||\n", g_global.tabs.c_str());
			printf("%s==============================================================||\n", g_global.tabs.c_str());
		}
		switch (menuChoices(choices, true))
		{
		case ResultType::Quit:
			printf("%s\n", g_global.tabs.c_str());
			return false;
		case ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
			__fallthrough;
		case ResultType::Failed:
			break;
		default:
			switch (choices[g_global.answer.index])
			{
			case 'c':
				printf("%s\n", g_global.tabs.c_str());
				m_strumFret[currentplayer] = 128;
				g_global.quit = true;
				break;
			case 'e':
				printf("%s\n", g_global.tabs.c_str());
				m_strumFret[currentplayer] = 256;
				g_global.quit = true;
				break;
			case 'm':
			{
				m_strumFret[currentplayer] = prevFret;
				size_t numColoredFrets = 0;
				++g_global;
				choices = "12345pf";
				if (piece == 1 && currentplayer)
					choices += 't';
				banner(" Clone Hero Export - Fret Selection - Toggle Mode ");
				do
				{
					if (promptType != 3)
					{
						printf("%sColor Toggles for Note %zu", g_global.tabs.c_str(), note);
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
						if (piece > 1 || promptType != 3)
						{
							if (promptType != 3)
								printf("%s                  Using 'P' will cancel out all other frets & vice versa                    ||\n", g_global.tabs.c_str());
							printf("%s============================================================================================||\n", g_global.tabs.c_str());
							printf("%s                                     Open Note (P) %s                                    ||\n", g_global.tabs.c_str(), m_strumFret[currentplayer] & 32 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf("%s            Using 'P' on this note will cancel out all other frets & vice versa             ||\n", g_global.tabs.c_str());
							printf("%s============================================================================================||\n", g_global.tabs.c_str());
							printf("%s                            Open Note (P) %s || Tap (T) %s                            ||\n", g_global.tabs.c_str()
								, m_strumFret[currentplayer] & 32 ? "[ON] " : "[OFF]"
								, m_strumFret[currentplayer] & 64 ? "[ON] " : "[OFF]");
						}

						if (numColoredFrets < 4)
						{
							printf_tab("%s Green (1) %s || Red (2) %s || Yellow (3) %s || Blue (4) %s || Orange (5) %s ||\n"
								, m_strumFret[currentplayer] & 1  ? "[ON] " : "[OFF]"
								, m_strumFret[currentplayer] & 2  ? "[ON] " : "[OFF]"
								, m_strumFret[currentplayer] & 4  ? "[ON] " : "[OFF]"
								, m_strumFret[currentplayer] & 8  ? "[ON] " : "[OFF]"
								, m_strumFret[currentplayer] & 16 ? "[ON] " : "[OFF]");
						}
						else
						{
							printf_tab("%s||%s||%s||%s||%s||"
								, m_strumFret[currentplayer] & 1  ? " Green (1) [ON]  " : "                 "
								, m_strumFret[currentplayer] & 2  ? " Red (2) [ON]  " : "               "
								, m_strumFret[currentplayer] & 4  ? " Yellow (3) [ON]  " : "                  "
								, m_strumFret[currentplayer] & 8  ? " Blue (4) [ON]  " : "                "
								, m_strumFret[currentplayer] & 16 ? " Orange (5) [ON]  " : "                  ");
						}
						printf("%s                        Finish fret selection for this note/piece (F)                       ||\n", g_global.tabs.c_str());
						printf("%s============================================================================================||\n", g_global.tabs.c_str());
					}
					switch (menuChoices(choices, true))
					{
					case ResultType::Quit:
						printf("%s\n", g_global.tabs.c_str());
						return false;
					case ResultType::Help:
						printf("%s\n", g_global.tabs.c_str());
						__fallthrough;
					case ResultType::Failed:
						break;
					default:
						printf("%s\n", g_global.tabs.c_str());
						switch (choices[g_global.answer.index])
						{
						case 'f':
							if ((m_strumFret[currentplayer] & 63) == 0)
								m_strumFret[currentplayer] |= 31;
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
							break;
						case 't':
							m_strumFret[currentplayer] ^= 64;
							break;
						case 'p':
							if (!(m_strumFret[currentplayer] & 32) && piece == 1 && numColoredFrets)
							{
								do
								{
									printf("%sAll colored frets will need to be untoggled. Continue toggling open note? [Y/N]\n", g_global.tabs.c_str());
									switch (menuChoices("yn"))
									{
									case ResultType::Quit:
										return false;
									case ResultType::Success:
										switch (g_global.answer.character)
										{
										case 'y':
											m_strumFret[currentplayer] &= 64;
											m_strumFret[currentplayer] += 32;
											numColoredFrets = 0;
											__fallthrough;
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
									if (m_strumFret[currentplayer] & val && !(prevFret & val))
									{
										do
										{
											printf("%s\n", g_global.tabs.c_str());
											printf("%sAll colored frets not used for the previous note will need to be untoggled. Continue toggling open note? [Y/N]\n", g_global.tabs.c_str());
											switch (menuChoices("yn"))
											{
											case ResultType::Quit:
												return false;
											case ResultType::Success:
												switch (g_global.answer.character)
												{
												case 'y':
													for (; color < 5; color++)
													{
														val = 1UL << color;
														if (m_strumFret[currentplayer] & val && !(prevFret & val))
														{
															m_strumFret[currentplayer] ^= val;
															numColoredFrets--;
														}
													}
													m_strumFret[currentplayer] |= 32;
													__fallthrough;
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
									m_strumFret[currentplayer] ^= 32;
							}
							break;
						default:
						{
							unsigned long color = 1UL << g_global.answer.index;
							if (m_strumFret[currentplayer] & color || numColoredFrets < 4)
							{
								if (piece == 1 && !(m_strumFret[currentplayer] & color) && m_strumFret[currentplayer] & 32)
								{
									do
									{
										printf("%s\n", g_global.tabs.c_str());
										printf("%sOpen note will need to be untoggled. Continue toggling this color? [Y/N]\n", g_global.tabs.c_str());
										switch (menuChoices("yn"))
										{
										case ResultType::Quit:
											return false;
										case ResultType::Success:
											switch (g_global.answer.character)
											{
											case 'y':
												m_strumFret[currentplayer] -= 32;
												m_strumFret[currentplayer] += color;
												numColoredFrets++;
												__fallthrough;
											case 'n':
												g_global.quit = true;
											}
										}
									} while (!g_global.quit);
									g_global.quit = false;
								}
								else
								{
									m_strumFret[currentplayer] ^= color;
									if (m_strumFret[currentplayer] & color)
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
				if (promptType == 1 && g_global.answer.index > 6)
				{
					if (g_global.answer.index == 7)
						m_strumFret[currentplayer] = 95;
					else
						m_strumFret[currentplayer] = 64 + (1UL << (g_global.answer.index - 8));
				}
				else if (g_global.answer.index > 0)
					m_strumFret[currentplayer] = 1UL << (g_global.answer.index - 1);
				else
					m_strumFret[currentplayer] = 31;
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return true;
}

ChartFileExporter::ChartFileExporter(std::list<SyncTrack>& sync, std::list<Event>& events, NoteTrack(&notes)[2])
	: ChartFile()
	, m_sync(std::move(sync))
	, m_events(std::move(events))
{
	m_reimportNotes[0] = notes[0];
	m_reimportNotes[1] = notes[1];
}

void ChartFileExporter::writeHeader(const bool modchart)
{
	fprintf(m_chart,
		"[Song]\n"
		"{\n"
		"  Offset = 0\n"
		"  Resolution = 480\n"
		"  Player2 = bass\n"
		"  Difficulty = 0\n"
		"  PreviewStart = 0\n"
		"  PreviewEnd = 0\n"
		"  Genre = \"rock\"\n"
		"  MediaType = \"cd\"\n"
		"}\n"
		"[SyncTrack]\n"
		"{\n");

	if (modchart)
	{
		for (auto iter = m_sync.begin(); iter != m_sync.end();)
		{
			iter->write(m_chart);
			iter->m_position_ticks -= 2 * s_TICKS_PER_BEAT;
			//Remove the two extra synctracks
			if (iter->m_position_ticks < 0)
				m_sync.erase(iter++);
			else
				++iter;
		}
	}
	else
	{
		for (SyncTrack& syn : m_sync)
		{
			syn.m_timeSigDenomSelection = 2;
			syn.write(m_chart);
		}
	}

	fprintf(m_chart,
		"}\n"
		"[Events]\n"
		"{\n");
	if (modchart)
	{
		for (Event& evt : m_events)
		{
			evt.write(m_chart);
			evt.m_position_ticks -= 2 * s_TICKS_PER_BEAT;
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

void ChartFileExporter::writeIni(const int stageNumber, const unsigned long totalDuration, const bool jap)
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
