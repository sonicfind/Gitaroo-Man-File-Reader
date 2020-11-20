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
#include "CH_Import.h"
using namespace std;

bool CH_Importer::importChart()
{
	bool duet = m_song.m_imc[0] == 0;
	ChartFileImporter chart;
	do
	{
		printf("%sProvide the name of the .CHART file you wish to use (Or 'Q' to exit): ", g_global.tabs.c_str());
		string chartName = "";
		switch (GlobalFunctions::stringInsertion(chartName))
		{
		case GlobalFunctions::ResultType::Quit:
			return false;
		case GlobalFunctions::ResultType::Success:
			if (chartName.find(".CHART") == string::npos && chartName.find(".chart") == string::npos)
				chartName += ".CHART";
			if (chart.open(chartName.c_str()))
				g_global.quit = true;
			else
			{
				size_t pos = chartName.find_last_of('\\');
				if (pos != string::npos)
					printf("%s\"%s\" is not a valid file of extension \".CHART\"\n", g_global.tabs.c_str(), chartName.substr(pos + 1).c_str());
				else
					printf("%s\"%s\" is not a valid file of extension \".CHART\"\n", g_global.tabs.c_str(), chartName.c_str());
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	chart.read(*this);
	bool charted[2] = { m_notes[0].m_allNotes.size() > 0, m_notes[1].m_allNotes.size() > 0 };
	if (charted[0] || charted[1])
	{
		fillSections();
		for (size_t sectIndex = 0; sectIndex < m_song.m_sections.size(); sectIndex++)
		{
			SongSection& section = m_song.m_sections[sectIndex];
			if (!(section.getPhase() == SongSection::Phase::INTRO || strstr(section.getName(), "BRK") || strstr(section.getName(), "BREAK")))
			{
				for (size_t sectIndex2 = 0; sectIndex2 < m_sections.size(); sectIndex2++)
				{
					if (string(section.getName()).substr(0, m_sections[sectIndex2].m_name.length()).find(m_sections[sectIndex2].m_name) != string::npos)
					{
						if (sectIndex2 + 1ULL != m_sections.size())
						{
							//Sets Section duration to an even and equal spacing based off marked tempo
							const long double numBeats = (long double)round((m_sections[sectIndex2 + 1ULL].m_position_ticks - m_sections[sectIndex2].m_position_ticks) / TICKS_PER_BEAT);
							section.setDuration((unsigned long)round(s_SAMPLES_PER_MIN * numBeats / section.getTempo()));
						}
						//This differentiation is needed due to the difference in how
						//subsections are split in PS2 charts vs. Duet charts
						long lastNotes[2] = { 0, 0 };
						if (!duet)
						{
							bool swapped[2];
							swapped[0] = swapped[1] = section.getSwapped() & 1;
							for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
							{
								for (size_t playerIndex = 0; playerIndex < section.getNumPlayers(); playerIndex++)
								{
									if (charted[playerIndex & 1])
									{
										size_t val = 2 * chartIndex + (playerIndex >> 1);
										if (val < m_sections[sectIndex2].m_subs[playerIndex & 1].size())
										{
											Chart& imp = m_sections[sectIndex2].m_subs[playerIndex & 1][val];
											if (imp.m_tracelines.size() > 1 || imp.m_guards.size())
											{
												section.setOrganized(false);
												Chart& ins = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
												long buf = ins.insertNotes(imp);
												if (lastNotes[playerIndex & 1] < buf)
													lastNotes[playerIndex & 1] = buf;
												if (section.getPhase() == SongSection::Phase::BATTLE)
												{
													if (ins.m_guards.size() && ins.m_tracelines.size() > 1)
													{
														//Determining which comes first
														if (ins.m_guards.front().getPivotAlpha() < ins.m_tracelines.front().getPivotAlpha())
														{
															if ((playerIndex & 1))
																swapped[1] = true;
														}
														else if (ins.m_tracelines.front().getPivotAlpha() < ins.m_guards.front().getPivotAlpha())
														{
															if (!(playerIndex & 1))
																swapped[0] = true;
														}
													}
												}
											}
										}
										else
										{
											//Checks if the data in any unchanged subsections needs to be deleted as to not interfere with inserted m_notes
											Chart& skipped = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
											if (skipped.m_guards.size() && skipped.m_guards.front().getPivotAlpha() < lastNotes[playerIndex & 1]
												|| skipped.m_tracelines.size() > 1 && skipped.m_tracelines.front().getPivotAlpha() < lastNotes[playerIndex & 1])
											{
												skipped.clear();
											}
										}
									}
								}
							}
							if (swapped[0] && swapped[1])
								section.setSwapped(1);
						}
						else
						{
							for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
							{
								if (charted[playerIndex & 1])
								{
									for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
									{
										if (chartIndex < m_sections[sectIndex2].m_subs[playerIndex].size())
										{
											Chart& imp = m_sections[sectIndex2].m_subs[playerIndex][chartIndex];
											if (imp.m_tracelines.size() > 1 || imp.m_guards.size())
											{
												section.setOrganized(false);
												long buf = section.getChart(2 * playerIndex * section.getNumCharts() + chartIndex).insertNotes(imp);
												if (lastNotes[playerIndex & 1] < buf)
													lastNotes[playerIndex & 1] = buf;
											}
											else
											{
												//Checks if the data in any unchanged subsections needs to be deleted as to not interfere with inserted m_notes
												Chart& skipped = section.getChart(2 * playerIndex * section.getNumCharts() + chartIndex);
												if (skipped.m_guards.size() && skipped.m_guards.front().getPivotAlpha() < lastNotes[playerIndex & 1]
													|| skipped.m_tracelines.size() > 1 && skipped.m_tracelines.front().getPivotAlpha() < lastNotes[playerIndex & 1])
												{
													skipped.clear();
												}
											}
										}
									}
								}
							}
						}
						if (!section.getOrganized())
							m_song.m_unorganized++;
						printf("%sReplaced %s\n", g_global.tabs.c_str(), section.getName());
						break;
					}
				}
			}
		}
		do
		{
			printf("%sFix & reorganize the newly imported notes for safety? [Y/N][or Q to cancel save]\n", g_global.tabs.c_str());
			printf("%sChoosing not to do so will leave all newly imported sections in an unorganized/possibly glitchy state.\n", g_global.tabs.c_str());
			switch (GlobalFunctions::menuChoices("yn"))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character == 'y')
					if (!GlobalFunctions::loadProc(g_dlls[s_DLL_INDEX].m_libraries[0].m_dll, "quickFix", m_song))
						printf("%sAn error was ecountered while attempting to alter %s\n", g_global.tabs.c_str(), m_song.m_shortname.c_str());
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
		if (m_song.m_name.find("IMPORTED") == string::npos)
		{
			do
			{
				printf("%sHow should the file be named?\n", g_global.tabs.c_str());
				printf("%sK - Keep filename as %s\n", g_global.tabs.c_str(), m_song.m_shortname.c_str());
				printf("%sC - Change filename to %s_IMPORTED\n", g_global.tabs.c_str(), m_song.m_shortname.c_str());
				switch (GlobalFunctions::menuChoices("kc"))
				{
				case GlobalFunctions::ResultType::Quit:
					return false;
				case GlobalFunctions::ResultType::Failed:
					break;
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'c')
					{
						m_song.m_name = m_song.m_name.substr(0, m_song.m_name.length() - 4) + "_IMPORTED.CHC";
						m_song.m_shortname += "_IMPORTED";
					}
					printf("%s\n", g_global.tabs.c_str());
					g_global.quit = true;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		else
		{
			do
			{
				printf("%sHow should the file be named?\n", g_global.tabs.c_str());
				printf("%sK - Keep filename as %s\n", g_global.tabs.c_str(), m_song.m_shortname.c_str());
				printf("%sC - Change filename to %s_T\n", g_global.tabs.c_str(), m_song.m_shortname.c_str());
				switch (GlobalFunctions::menuChoices("kc"))
				{
				case GlobalFunctions::ResultType::Quit:
					return false;
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'c')
					{
						m_song.m_name = m_song.m_name.substr(0, m_song.m_name.length() - 4) + "_T.CHC";
						m_song.m_shortname += "_T";
					}
					printf("%s\n", g_global.tabs.c_str());
					g_global.quit = true;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		do
		{
			//Either yes or no will still overwrite the old CHC data held
			//in the current CHC_Main object
			printf("%sSave %s externally? [Y/N]\n", g_global.tabs.c_str(), m_song.m_shortname.c_str());
			switch (GlobalFunctions::menuChoices("yn"))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character == 'y')
				{
					printf("%s\n", g_global.tabs.c_str());
					string filename = m_song.m_name.substr(0, m_song.m_name.length() - 4);
					string ext = "";
					do
					{
						switch (GlobalFunctions::fileOverwriteCheck(filename + ext + ".CHC"))
						{
						case GlobalFunctions::ResultType::No:
							printf("%s\n", g_global.tabs.c_str());
							ext += "_T";
							break;
						case GlobalFunctions::ResultType::Yes:
							m_song.create(filename + ext + ".CHC");
							m_song.m_name = filename + ext + ".CHC";
							m_song.m_shortname += ext;
							m_song.m_saved = 2;
							g_global.quit = true;
							break;
						case GlobalFunctions::ResultType::Quit:
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
						}
					} while (!g_global.quit);
				}
				else
					g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
		return true;
	}
	else
	{
		printf("%s\n", g_global.tabs.c_str());
		printf("%sNote import aborted.\n", g_global.tabs.c_str());
		return false;
	}
}

void ChartFileImporter::read(CH_Importer& importer)
{
	int chartVersion = 0;
	char ignore[400];
	//Skips the info section
	fscanf_s(m_chart, " %[^}]", ignore, 400);
	fscanf_s(m_chart, " %[^{]", ignore, 400);
	fseek(m_chart, 1, SEEK_CUR);
	char test = 0;
	LinkedList::List<SyncTrack> tempos;
	fscanf_s(m_chart, " %c", &test, 1);
	//Checking for the end of the synctrack section
	while (test != '}')
	{
		fseek(m_chart, -1, SEEK_CUR);
		//Creates new tempo object
		tempos.emplace_back(m_chart);
		fscanf_s(m_chart, " %c", &test, 1);
	}
	fscanf_s(m_chart, " %[^{]", ignore, 400);
	fseek(m_chart, 1, SEEK_CUR);
	size_t tempoIndex = 0;
	fscanf_s(m_chart, " %c", &test, 1);
	//Checking for the end of the event section
	while (test != '}')
	{
		fseek(m_chart, -1, SEEK_CUR);
		Event ev(m_chart);
		//For example, if the m_name is "BATTLE - something,"
		//only use "something."
		if (ev.m_name.find("section") != string::npos)
		{
			ev.m_name = ev.m_name.substr(ev.m_name.find_last_of(' ') + 1);
			//Add all m_tempos between the current section event and the previous section
			//to the previous section
			while (tempoIndex + 1ULL < tempos.size() && (ev.m_position >= tempos[tempoIndex + 1ULL].m_position || tempos[tempoIndex].m_bpm == 0))
			{
				if (ev.m_position > tempos[tempoIndex + 1ULL].m_position && tempos[tempoIndex + 1ULL].m_bpm > 0)
				{
					Section::Tempo& prev = importer.m_sections.back().m_tempos.back();
					double pos_samples = prev.m_position_samples + (tempos[tempoIndex + 1ULL].m_position - prev.m_position_ticks)
						* (s_SAMPLES_PER_MIN / (TICKS_PER_BEAT * prev.m_bpm / 1000));
					//Creates new tempo object
					importer.m_sections.back().m_tempos.emplace_back(tempos[tempoIndex + 1ULL].m_bpm, tempos[tempoIndex + 1ULL].m_position, pos_samples);
				}
				tempoIndex++;
			}
			double pos_samples = 0;
			//If this isn't the first section
			if (importer.m_sections.size())
			{
				Section::Tempo& prev = importer.m_sections.back().m_tempos.back();
				pos_samples = importer.m_sections.back().m_position_samples + prev.m_position_samples + (ev.m_position - prev.m_position_ticks)
					* (s_SAMPLES_PER_MIN / (TICKS_PER_BEAT * prev.m_bpm / 1000));
			}
			Section& section = importer.m_sections.emplace_back(ev.m_name, ev.m_position, pos_samples, tempos[tempoIndex].m_bpm);
			section.m_subs[0].emplace_front().clearTracelines();
			section.m_subs[1].emplace_front().clearTracelines();
		}
		else if (ev.m_name.find("GMFR EXPORT V2.0") != string::npos)
			chartVersion = 2;
		fscanf_s(m_chart, " %c", &test, 1);
	}
	if (!chartVersion)
	{
		do
		{
			printf("%sGMFR import, while compatible, works best with a .chart file updated to the current Moonscraper->CHC charting style. Create a converted version of this file? [Y/N]\n", g_global.tabs.c_str());
			printf("%sEnter 'C' if this file already utilizes this style (his will add the proper g_global event marker that defines the GMFR import version).\n", g_global.tabs.c_str());
			switch (GlobalFunctions::menuChoices("ync"))
			{
			case GlobalFunctions::ResultType::Quit:
				return;
			case GlobalFunctions::ResultType::Success:
				switch (g_global.answer.character)
				{
				case 'n':
					return;
				case 'c':
					chartVersion = 1;
					__fallthrough;
				default:
					g_global.quit = true;
				}
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}
	while (!feof(m_chart))
	{
		fscanf_s(m_chart, " %[^{]", ignore, 400);
		fseek(m_chart, 1, SEEK_CUR);
		if (feof(m_chart))
			break;
		fscanf_s(m_chart, " %c", &test, 1);
		size_t playerIndex;
		if (strstr(ignore, "[ExpertSingle]"))
			playerIndex = 0;
		else if (strstr(ignore, "[ExpertDoubleRhythm]"))
			playerIndex = 1;
		while (test != '}')
		{
			fseek(m_chart, -1, SEEK_CUR);
			CHNote note(m_chart);
			switch (note.m_type)
			{
			case CHNote::NoteType::EVENT:
				importer.m_notes[playerIndex].addEvent(note.m_position, note.m_name);
				break;
			case CHNote::NoteType::NOTE:
				if (note.m_mod == CHNote::Modifier::NORMAL)
				{
					note.m_fret.m_lane = 1 << note.m_fret.m_lane;
					importer.m_notes[playerIndex].addNote(note);
				}
				else if (!chartVersion)
				{
					if (note.m_mod == CHNote::Modifier::TAP)
					{
						importer.m_notes[playerIndex].addModifier(note.m_position, CHNote::Modifier::FORCED);
						importer.m_notes[playerIndex].addModifier(note.m_position, CHNote::Modifier::TAP);
					}
				}
				else if (note.m_mod == CHNote::Modifier::FORCED)
					importer.m_notes[playerIndex].addModifier(note.m_position, CHNote::Modifier::FORCED);
			}
			fscanf_s(m_chart, " %c", &test, 1);
		}
		if (!feof(m_chart))
			fseek(m_chart, 1, SEEK_CUR);
		else
			break;
	}
	close();
	if (chartVersion < 2)
	{
		LinkedList::List<Event> events(1, 0, "GMFR EXPORT V2.0");
		for (size_t sectIndex = 0; sectIndex < importer.m_sections.size(); sectIndex++)
			events.emplace_back(importer.m_sections[sectIndex].m_position_ticks, importer.m_sections[sectIndex].m_name);
		ChartFileExporter converter;
		if (chartVersion == 0)
			converter.open(m_file + "v2.0 Convert");
		else
			converter.open(m_file);
		converter.write(tempos, events, importer.m_notes, false);
		converter.close();
	}
}

void CH_Importer::fillSections()
{
	for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
	{
		const size_t listSize = m_notes[playerIndex].m_allNotes.size();
		if (listSize)
		{
			size_t sectIndex = 0;
			size_t tempoIndex = 0;
			long double SAMPLES_PER_TICK = s_SAMPLES_PER_MIN / (TICKS_PER_BEAT * m_sections[0].m_tempos[0].m_bpm / 1000);
			Section* currSection = &m_sections[0];
			Chart* currChart = &currSection->m_subs[0].back();
			for (size_t itemIndex = 0; itemIndex < listSize; itemIndex++)
			{
				CHNote* note = m_notes[playerIndex].m_allNotes[itemIndex];
				while (sectIndex + 1 < m_sections.size() && note->m_position >= m_sections[sectIndex + 1].m_position_ticks)
				{
					currSection = &m_sections[++sectIndex];
					currChart = &currSection->m_subs[playerIndex].back();
					tempoIndex = 0;
					SAMPLES_PER_TICK = s_SAMPLES_PER_MIN / (TICKS_PER_BEAT * currSection->m_tempos[tempoIndex].m_bpm / 1000);
				}
				while (tempoIndex + 1 < currSection->m_tempos.size() && note->m_position >= currSection->m_tempos[tempoIndex + 1].m_position_ticks)
					SAMPLES_PER_TICK = s_SAMPLES_PER_MIN / (TICKS_PER_BEAT * currSection->m_tempos[++tempoIndex].m_bpm / 1000);
				switch (note->m_type)
				{
				case CHNote::NoteType::EVENT:
					//Signals the insertion of a new chart/subsection
					if (note->m_name.find("start") != string::npos)
					{
						currChart = &currSection->m_subs[playerIndex].emplace_back();
						currChart->clearTracelines();
					}
					//Signals a trace line related function or note
					else if (note->m_name.find("Trace") != string::npos)
					{
						//Set previous Trace line's curve set to true
						if (note->m_name.find("curve") != string::npos)
							currChart->m_tracelines.back().setCurve(true);
						else
						{
							try
							{
								addTraceLine(((note->m_position - currSection->m_tempos[tempoIndex].m_position_ticks) * SAMPLES_PER_TICK) + currSection->m_tempos[tempoIndex].m_position_samples,
									note->m_name, sectIndex, playerIndex, currChart);
							}
							catch (...)
							{
								printf("%sTrace line event at tick position %lu had extraneous data that could not be pulled.\n", g_global.tabs.c_str(), (unsigned long)note->m_position);
								printf("%sRemember: trace events *must* be formatted as \"Trace(P)\", \"Trace(P)_[float angle value]\", \"Trace(P)_end\", or \"Trace_curve\"\n", g_global.tabs.c_str());
							}
						}
					}
					else if (note->m_name.find("Anim") != string::npos)
						currChart->m_phrases.back().setAnimation(stoi(note->m_name.substr(5)));
					break;
				case CHNote::NoteType::NOTE:
					//If Phrase Bar
					if (note->m_fret.m_sustain)
					{
						addPhraseBar((long)round((note->m_position - currSection->m_tempos[tempoIndex].m_position_ticks) * SAMPLES_PER_TICK + currSection->m_tempos[tempoIndex].m_position_samples),
										(unsigned long)round(note->m_fret.m_sustain * SAMPLES_PER_TICK), 1UL << note->m_fret.m_lane, currChart, (long)round(SAMPLES_PER_TICK));
					}
					else
					{
						switch (note->m_mod)
						{
						case CHNote::Modifier::NORMAL: //Guard Mark
						{
							addGuardMark((long)round(((note->m_position - m_sections[sectIndex].m_tempos[tempoIndex].m_position_ticks) * SAMPLES_PER_TICK) + m_sections[sectIndex].m_tempos[tempoIndex].m_position_samples),
											(unsigned long)note->m_fret.m_lane, currChart);
						}
						break;
						case CHNote::Modifier::FORCED:
							applyForced((long)round(((note->m_position - m_sections[sectIndex].m_tempos[tempoIndex].m_position_ticks) * SAMPLES_PER_TICK) + m_sections[sectIndex].m_tempos[tempoIndex].m_position_samples),
											currChart, currSection, playerIndex);
						}
					}
				}
			}
			m_notes[playerIndex].clear();
		}
	}
}

void CH_Importer::addTraceLine(double pos, string name, const size_t sectIndex, const size_t playerIndex, Chart* currChart)
{
	//If the trace line goes into the previous subsection
	Chart* insert;
	if ((name.find('P') != string::npos || name.find('p') != string::npos || (name.find("end") != string::npos && pos == 0))
		&& sectIndex > 0)
	{
		Section& prevSection = m_sections[sectIndex - 1];
		Section::Tempo& prevtempo = prevSection.m_tempos.back();
		if (name.find("end") != string::npos && pos == 0)
			pos--;
		else if (currChart->m_tracelines.size() > 0)
		{
			if (currChart->m_tracelines.front().getPivotAlpha() <= pos)
				pos = double(currChart->m_tracelines.front().getPivotAlpha()) - 1;
		}
		pos += (m_sections[sectIndex].m_position_ticks - prevtempo.m_position_ticks) * (s_SAMPLES_PER_MIN / (TICKS_PER_BEAT * prevtempo.m_bpm / 1000)) + prevtempo.m_position_samples;
		insert = &prevSection.m_subs[playerIndex].back();
	}
	else
		insert = currChart;
	//If the traceline is not an end piece AND has an angle tied to it
	if (name.find("end") == string::npos && name.find('_') != string::npos)
	{
		try
		{
			insert->emplaceTraceline((long)round(pos), 1, float(stof(name.substr(name.find('_') + 1)) * M_PI / 180));
		}
		catch (...)
		{
			insert->emplaceTraceline((long)round(pos));
			throw "No angle data found";
		}
	}
	else
		insert->emplaceTraceline((long)round(pos));
}

void CH_Importer::addPhraseBar(long pos, unsigned long sus, unsigned long lane, Chart* currChart, const long SAMPLES_PER_TICK_ROUNDED)
{
	if (currChart->m_phrases.size() != 0)
	{
		for (size_t index = currChart->m_phrases.size(); index > 0 && sus > 0;)
		{
			Phrase* previous = &currChart->m_phrases[--index];
			if (previous->getPivotAlpha() <= pos - SAMPLES_PER_TICK_ROUNDED)
			{
				if (previous->getEndAlpha() >= pos + SAMPLES_PER_TICK_ROUNDED)
				{
					currChart->emplacePhrase(pos, unsigned long(previous->getEndAlpha() - pos), false, previous->getEnd(), 0, previous->getColor());
					previous->changeEndAlpha(pos);
					previous->setEnd(false);
					index += 2;
				}
				else
				{
					currChart->emplacePhrase(pos, sus, true, true, 0, lane);
					break;
				}
			}
			else if (previous->getPivotAlpha() < pos + SAMPLES_PER_TICK_ROUNDED)
			{
				long dif = previous->getPivotAlpha() - pos;
				pos += dif;
				sus -= dif;
				for (; index < currChart->m_phrases.size(); index++)
				{
					previous = &currChart->m_phrases[index];
					if (previous->getDuration() <= sus - SAMPLES_PER_TICK_ROUNDED)
					{
						previous->addColor(lane);
						pos += previous->getDuration();
						sus -= previous->getDuration();
						if (index + 1 == currChart->m_phrases.size())
						{
							previous->setEnd(false);
							currChart->emplacePhrase(pos, sus, false, true, 0, lane);
							break;
						}
					}
					else
					{
						if (previous->getDuration() >= sus + SAMPLES_PER_TICK_ROUNDED)
						{
							pos += sus;
							previous->setEnd(false);
							if (index + 1 == currChart->m_phrases.size())
							{
								previous->setEnd(false);
								currChart->emplacePhrase(pos, previous->getDuration() - sus, false, true, 0, previous->getColor());
							}
							previous->changeEndAlpha(pos);
						}
						else
							previous->setDuration(sus);
						previous->addColor(lane);
						break;
					}
				}
				break;
			}
		}
	}
	else
		currChart->emplacePhrase(pos, sus, true, true, 0, lane);
}

void CH_Importer::addGuardMark(const long pos, const unsigned long button, Chart* currChart)
{
	if (currChart->m_guards.size() == 0 || currChart->m_guards.back().getPivotAlpha() < pos)
	{
		switch (button)
		{
		case 0:
			currChart->emplaceGuard(pos, 1);
			break;
		case 1:
			currChart->emplaceGuard(pos, 2);
			break;
		case 3:
			currChart->emplaceGuard(pos, 0);
			break;
		case 4:
			currChart->emplaceGuard(pos, 3);
		}
	}
}

void CH_Importer::applyForced(const long pos, Chart* currChart, const Section* currSection, const size_t playerIndex)
{
	size_t index = currChart->m_phrases.size();
	while (index > 0)
	{
		Phrase& base = currChart->m_phrases[--index];
		if (base.getPivotAlpha() == pos)
		{
			if (index == 0)
				base.addColor(64);
			else if (base.getStart())
			{
				Phrase& previous = currChart->m_phrases[index - 1];
				base.setStart(false);
				previous.setEnd(false);
				previous.changeEndAlpha(base.getPivotAlpha());
			}
			else
			{
				bool remove = false;
				while (index > 0)
				{
					Phrase& previous = currChart->m_phrases[--index];
					for (size_t color = 0; color < 6; color++)
					{
						unsigned long val = 1UL << color;
						if (!(base.getColor() & val) && (previous.getColor() & val))
						{
							if (remove)
							{
								printf("%s%s - Player %zu - Subsection %zu: ", g_global.tabs.c_str(), currSection->m_name.c_str(), playerIndex + 1, currSection->m_subs[playerIndex].size() - 1);
								currChart->remove(index + 1, 'p');
							}
							previous.setEnd(false);
							previous.changeEndAlpha(base.getPivotAlpha());
							g_global.quit = true;
							break;
						}
					}
					if (!(g_global.quit || remove || previous.getStart()))
						remove = true;
					else
						break;
				}
				g_global.quit = false;
			}
			break;
		}
	}
}
