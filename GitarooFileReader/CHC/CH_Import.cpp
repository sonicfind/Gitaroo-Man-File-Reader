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
#include "CH_Import.h"
#include "CHC/CHC_Editor.h"

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
		if (processor_count > 1)
		{
			int* results = new int[m_song.m_sections.size()]();
			bool* sectionUsage = new bool[m_sections.size()]();
			auto printResult = [&]()
			{
				for (size_t sectIndex = 0; sectIndex != m_song.m_sections.size(); ++sectIndex)
				{
					while (!results[sectIndex]);
					if (!m_song.m_sections[sectIndex].getOrganized())
					{
						++m_song.m_unorganized;
						printf("%sReplaced %s (%zu)\n", g_global.tabs.c_str(), m_song.m_sections[sectIndex].getName(), sectIndex);
					}
				}
			};

			auto replace = [&](size_t songSectIndex, size_t sectIndex)
			{
				while (sectionUsage[sectIndex]);
				sectionUsage[sectIndex] = true;
				replaceNotes(songSectIndex, sectIndex, charted, duet);
				sectionUsage[sectIndex] = false;
				results[songSectIndex] = true;
			};

			struct threadControl
			{
				size_t sectIndex;
				std::thread thisThread;
				threadControl(const size_t index) : sectIndex(index) {}
			};
			std::vector<threadControl> threads;
			threads.reserve(processor_count - 2);
			std::thread resultLoop(printResult);
			for (size_t songSectIndex = 0; songSectIndex < m_song.m_sections.size(); ++songSectIndex)
			{
				SongSection& songSection = m_song.m_sections[songSectIndex];
				if (!(songSection.getPhase() == SongSection::Phase::INTRO || strstr(songSection.getName(), "BRK") || strstr(songSection.getName(), "BREAK")))
				{
					size_t currSect = 0;
					for (;currSect < m_sections.size(); ++currSect)
					{
						if (string(songSection.getName()).substr(0, m_sections[currSect].m_name.length()).find(m_sections[currSect].m_name) != string::npos)
						{
							while (threads.size() == processor_count - 2)
							{
								for (size_t thr = 0; thr < threads.size();)
								{
									if (results[threads[thr].sectIndex])
									{
										threads[thr].thisThread.join();
										threads.erase(threads.begin() + thr);
									}
									else
										++thr;
								}
							}
							threads.emplace_back(songSectIndex);
							threads.back().thisThread = std::thread(replace, songSectIndex, currSect);
							break;
						}
					}

					if (currSect == m_sections.size())
						results[songSectIndex] = true;
				}
				else
					results[songSectIndex] = true;
			}

			for (threadControl& thr : threads)
				thr.thisThread.join();

			resultLoop.join();
			delete[m_song.m_sections.size()] results;
			delete[m_sections.size()] sectionUsage;
		}
		else
		{
			for (size_t songSectIndex = 0; songSectIndex < m_song.m_sections.size(); ++songSectIndex)
			{
				SongSection& songSection = m_song.m_sections[songSectIndex];
				if (!(songSection.getPhase() == SongSection::Phase::INTRO
					|| strstr(songSection.getName(), "BRK")
					|| strstr(songSection.getName(), "BREAK")))
				{
					for (size_t currSect = 0; currSect < m_sections.size(); ++currSect)
					{
						if (string(songSection.getName()).substr(0, m_sections[currSect].m_name.length()).find(m_sections[currSect].m_name) != string::npos)
						{
							replaceNotes(songSectIndex, currSect, charted, duet);
							break;
						}
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
				{
					CHC_Editor editor(m_song);
					editor.quickFix();
				}
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
		if (m_song.m_filename.find("IMPORTED") == string::npos)
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
						m_song.m_filename = m_song.m_filename.substr(0, m_song.m_filename.length() - 4) + "_IMPORTED.CHC";
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
						m_song.m_filename = m_song.m_filename.substr(0, m_song.m_filename.length() - 4) + "_T.CHC";
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
					string filename = m_song.m_filename.substr(0, m_song.m_filename.length() - 4);
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
							m_song.m_filename = filename + ext + ".CHC";
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

	std::vector<SyncTrack> tempos;

	char test = 0;
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
	fscanf_s(m_chart, " %c", &test, 1);

	SyncTrack* tempo = &tempos.front();
	//Checking for the end of the event section
	while (test != '}')
	{
		fseek(m_chart, -1, SEEK_CUR);
		Event ev(m_chart);
		if (ev.m_name.find("section") != string::npos)
		{

			//Add all tempos between the current section event and the previous section
			//to the previous section
			while (tempo != &tempos.back()
				&& (ev.m_position >= (tempo + 1)->m_position || tempo->m_bpm == 0))
			{
				++tempo;
				if (ev.m_position > tempo->m_position && tempo->m_bpm > 0)
				{
					Section::Tempo& prev = importer.m_sections.back().m_tempos.back();
					float pos_samples = prev.m_position_samples + (tempo->m_position - prev.m_position_ticks)
						* (s_SAMPLES_PER_MIN / (s_TICKS_PER_BEAT * prev.m_bpm / 1000));
					//Creates new tempo object
					importer.m_sections.back().m_tempos.emplace_back(tempo->m_bpm, tempo->m_position, pos_samples);
				}
			}

			float pos_samples = 0;
			//If this isn't the first section
			if (importer.m_sections.size())
			{
				Section::Tempo& prev = importer.m_sections.back().m_tempos.back();
				pos_samples = importer.m_sections.back().m_position_samples + prev.m_position_samples + (ev.m_position - prev.m_position_ticks)
					* (s_SAMPLES_PER_MIN / (s_TICKS_PER_BEAT * prev.m_bpm / 1000));
			}

			importer.m_sections.emplace_back(ev.m_name, ev.m_position, pos_samples, tempo->m_bpm);
			importer.m_sections.back().m_subs[0].emplace_back(false);
			importer.m_sections.back().m_subs[1].emplace_back(false);
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
			printf("%sEnter 'C' if this file already utilizes this style (this will add the proper global event marker that defines the GMFR import version).\n", g_global.tabs.c_str());
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

		NoteTrack* track = nullptr;
		NoteTrack* throwaway = nullptr;
		if (strstr(ignore, "[ExpertSingle]"))
			track = importer.m_notes;
		else if (strstr(ignore, "[ExpertDoubleRhythm]"))
			track = importer.m_notes + 1;
		else
			track = throwaway = new NoteTrack();

		fscanf_s(m_chart, " %c", &test, 1);
		while (test != '}' && !feof(m_chart))
		{
			fseek(m_chart, -1, SEEK_CUR);
			CHNote note(m_chart);
			switch (note.m_type)
			{
			case CHNote::NoteType::EVENT:
				track->addEvent(note.m_position, note.m_name);
				break;
			case CHNote::NoteType::NOTE:
				if (note.m_mod == CHNote::Modifier::NORMAL)
				{
					note.m_fret.m_lane = 1 << note.m_fret.m_lane;
					track->addNote(note);
				}
				else if (!chartVersion)
				{
					if (note.m_mod == CHNote::Modifier::TAP)
					{
						track->addModifier(note.m_position, CHNote::Modifier::FORCED);
						track->addModifier(note.m_position, CHNote::Modifier::TAP);
					}
				}
				else if (note.m_mod == CHNote::Modifier::FORCED)
				{
					track->addModifier(note.m_position, CHNote::Modifier::FORCED);
					track->addModifier(note.m_position, CHNote::Modifier::TAP);
				}
			}
			fscanf_s(m_chart, " %c", &test, 1);
		}

		if (throwaway)
			delete throwaway;

		if (!feof(m_chart))
			fseek(m_chart, 1, SEEK_CUR);
	}
	close();

	if (chartVersion < 2)
	{
		std::vector<Event> events;
		events.reserve(importer.m_sections.size() + 1);
		events.resize(importer.m_sections.size() + 1);

		events[0] = { 0.0f, "GMFR EXPORT V2.0" };
		for (size_t i = 0; i < importer.m_sections.size(); ++i)
			events[i + 1] = { importer.m_sections[i].m_position_ticks, importer.m_sections[i].m_name };

		ChartFileExporter converter(tempos, events, importer.m_notes);
		if (chartVersion == 0)
			converter.open(m_file + "v2.0 Convert");
		else
			converter.open(m_file);

		converter.write(false);
		converter.close();
	}

	// For example, if the section name is "BATTLE - something",
	// only use "something" / disregard "BATTLE - ".
	// Otherwise, just gets rid of "section"
	for (auto& sect : importer.m_sections)
		sect.m_name = sect.m_name.substr(sect.m_name.find_last_of(' ') + 1);
}

void CH_Importer::fillSections()
{
	for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
	{
		if (m_notes[playerIndex].m_allNotes.size())
		{
			for (size_t sectIndex = 0, noteIndex = 0; sectIndex < m_sections.size(); ++sectIndex)
			{
				Section* section = &m_sections[sectIndex];
				Section::Tempo* tempo = &section->m_tempos.front();
				Chart* chart = &section->m_subs[playerIndex].back();
				float SAMPLES_PER_TICK = s_SPT_CONSTANT / (s_TICKS_PER_BEAT * tempo->m_bpm);
				for (size_t tempoIndex = 0; noteIndex < m_notes[playerIndex].m_allNotes.size(); ++noteIndex)
				{
					CHNote* note = m_notes[playerIndex].m_allNotes[noteIndex];
					if (sectIndex + 1 < m_sections.size() && note->m_position >= m_sections[sectIndex + 1].m_position_ticks)
						break;

					while (tempoIndex + 1 < section->m_tempos.size() &&
						note->m_position >= section->m_tempos[tempoIndex + 1].m_position_ticks)
					{
						tempo = &section->m_tempos[++tempoIndex];
						SAMPLES_PER_TICK = s_SPT_CONSTANT / (s_TICKS_PER_BEAT * tempo->m_bpm);
					}

					switch (note->m_type)
					{
					case CHNote::NoteType::EVENT:
						// Signals the insertion of a new chart/subsection
						if (note->m_name.find("start") != string::npos)
						{
							section->m_subs[playerIndex].emplace_back(false);
							chart = &section->m_subs[playerIndex].back();
						}
						// Signals a trace line related function or note
						else if (note->m_name.find("Trace") != string::npos)
						{
							// Set previous Trace line's curve set to true
							if (note->m_name.find("curve") != string::npos)
								chart->m_tracelines.back().m_curve = true;
							else
							{
								try
								{
									addTraceLine(((note->m_position - tempo->m_position_ticks) * SAMPLES_PER_TICK) + tempo->m_position_samples, note->m_name,
										sectIndex, playerIndex);
								}
								catch (...)
								{
									printf("%sTrace line event at tick position %lu had extraneous data that could not be pulled.\n", g_global.tabs.c_str(),
										(unsigned long)note->m_position);
									printf("%sRemember: trace events *must* be formatted as \"Trace(P)\", \"Trace(P)_[float angle value]\", \"Trace(P)_end\", or \"Trace_curve\"\n", g_global.tabs.c_str());
								}
							}
						}
						else if (note->m_name.find("Anim") != string::npos)
							chart->m_phrases.back().m_animation = note->m_name[5] - 48;
						break;
					case CHNote::NoteType::NOTE:
						//If Phrase Bar
						if (note->m_fret.m_sustain)
						{
							addPhraseBar((long)roundf((note->m_position - tempo->m_position_ticks) * SAMPLES_PER_TICK + tempo->m_position_samples),
								(unsigned long)roundf(note->m_fret.m_sustain * SAMPLES_PER_TICK),
								1UL << note->m_fret.m_lane, chart, (long)round(SAMPLES_PER_TICK));
						}
						else
						{
							switch (note->m_mod)
							{
							case CHNote::Modifier::NORMAL: //Guard Mark
							{
								addGuardMark((long)roundf(((note->m_position - tempo->m_position_ticks) * SAMPLES_PER_TICK) + tempo->m_position_samples),
															(unsigned long)note->m_fret.m_lane, chart);
							}
							break;
							case CHNote::Modifier::FORCED:
								applyForced((long)roundf(((note->m_position - tempo->m_position_ticks) * SAMPLES_PER_TICK) + tempo->m_position_samples),
															sectIndex, playerIndex);
							}
						}
					}
				}
			}

			m_notes[playerIndex].clear();
		}
	}
}

void CH_Importer::replaceNotes(size_t songSectIndex, const size_t sectIndex, const bool(&charted)[2], const bool duet)
{
	SongSection& section = m_song.m_sections[songSectIndex];
	Section& section2 = m_sections[sectIndex];
	if (sectIndex + 1 < m_sections.size())
	{
		//Sets Section duration to an even and equal spacing based off marked tempo
		const float numBeats = roundf((m_sections[sectIndex + 1].m_position_ticks - section2.m_position_ticks) / s_TICKS_PER_BEAT);
		section.setDuration((unsigned long)round(s_SAMPLES_PER_MIN * numBeats / section.getTempo()));
	}

	//This differentiation is needed due to the difference in how
	//subsections are split in PS2 charts vs. Duet charts
	if (!duet)
	{
		long lastNotes[2] = { 0, 0 };
		bool swapped[2] = { (section.getSwapped() & 1) > 0, (section.getSwapped() & 1) > 0 };
		for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); ++chartIndex)
		{
			for (size_t playerIndex = 0; playerIndex < section.getNumPlayers(); ++playerIndex)
			{
				if (charted[playerIndex & 1])
				{
					size_t val = (chartIndex << 1) + (playerIndex >> 1);
					if (val < section2.m_subs[playerIndex & 1].size())
					{
						Chart* imp = &section2.m_subs[playerIndex & 1][val];
						if (imp->m_tracelines.size() > 1 || imp->m_guards.size())
						{
							section.setOrganized(false);
							Chart* ins = &section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
							long buf = ins->insertNotes(imp);

							if (lastNotes[playerIndex & 1] < buf)
								lastNotes[playerIndex & 1] = buf;

							if (section.getPhase() == SongSection::Phase::BATTLE)
							{
								if (ins->m_guards.size() && ins->m_tracelines.size() > 1)
								{
									//Determining which comes first
									if (ins->m_guards.front().m_pivotAlpha < ins->m_tracelines.front().m_pivotAlpha)
									{
										if ((playerIndex & 1))
											swapped[1] = true;
									}
									else if (ins->m_tracelines.front().m_pivotAlpha < ins->m_guards.front().m_pivotAlpha)
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
						Chart& skipped = section.m_charts[playerIndex * section.getNumCharts() + chartIndex];
						if ((skipped.m_guards.size() && skipped.m_guards.front().m_pivotAlpha < lastNotes[playerIndex & 1]) ||
							(skipped.m_tracelines.size() > 1 && skipped.m_tracelines.front().m_pivotAlpha < lastNotes[playerIndex & 1]))
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
		long lastNotes[2] = { 0, 0 };
		for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
		{
			if (charted[playerIndex])
			{
				std::vector<Chart>& sub = section2.m_subs[playerIndex];
				for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && chartIndex < sub.size(); chartIndex++)
				{
					Chart* imp = &sub[chartIndex];
					if (imp->m_tracelines.size() > 1 || imp->m_guards.size())
					{
						section.setOrganized(false);
						long buf = section.m_charts[2 * playerIndex * section.getNumCharts() + chartIndex].insertNotes(imp);

						if (lastNotes[playerIndex & 1] < buf)
							lastNotes[playerIndex & 1] = buf;
					}
					else
					{
						//Checks if the data in any unchanged subsections needs to be deleted as to not interfere with inserted m_notes
						Chart& skipped = section.m_charts[2 * playerIndex * section.getNumCharts() + chartIndex];
						if ((skipped.m_guards.size() && skipped.m_guards.front().m_pivotAlpha < lastNotes[playerIndex & 1]) ||
							(skipped.m_tracelines.size() > 1 && skipped.m_tracelines.front().m_pivotAlpha < lastNotes[playerIndex & 1]))
						{
							skipped.clear();
						}
					}
				}
			}
		}
	}
}

void CH_Importer::addTraceLine(float pos, string name, const size_t sectIndex, const size_t playerIndex)
{
	Chart* currChart = &m_sections[sectIndex].m_subs[playerIndex].back();
	Chart* insert = nullptr;
	//If the trace line goes into the previous subsection
	if ((name.find('P') != string::npos || name.find('p') != string::npos || (name.find("end") != string::npos && pos == 0)) && sectIndex > 0)
	{
		Section::Tempo& prevtempo = m_sections[sectIndex - 1].m_tempos.back();

		if (name.find("end") != string::npos && pos == 0)
			pos--;
		else if (currChart->m_tracelines.size() > 0)
		{
			if (currChart->m_tracelines.front().m_pivotAlpha <= pos)
				pos = float(currChart->m_tracelines.front().m_pivotAlpha) - 1;
		}

		pos += (m_sections[sectIndex].m_position_ticks - prevtempo.m_position_ticks) * (s_SPT_CONSTANT / (s_TICKS_PER_BEAT * prevtempo.m_bpm)) + prevtempo.m_position_samples;
		insert = &m_sections[sectIndex - 1].m_subs[playerIndex].back();
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
			if (previous->m_pivotAlpha <= pos - SAMPLES_PER_TICK_ROUNDED)
			{
				if (previous->getEndAlpha() >= pos + SAMPLES_PER_TICK_ROUNDED)
				{
					currChart->emplacePhrase(pos, unsigned long(previous->getEndAlpha() - pos), false, previous->m_end, 0, previous->getColor());
					previous->changeEndAlpha(pos);
					previous->m_end = false;
					index += 2;
				}
				else
				{
					currChart->emplacePhrase(pos, sus, true, true, 0, lane);
					break;
				}
			}
			else if (previous->m_pivotAlpha < pos + SAMPLES_PER_TICK_ROUNDED)
			{
				long dif = previous->m_pivotAlpha - pos;
				pos += dif;
				sus -= dif;
				for (; index < currChart->m_phrases.size(); index++)
				{
					previous = &currChart->m_phrases[index];
					if (previous->m_duration <= sus - SAMPLES_PER_TICK_ROUNDED)
					{
						previous->addColor(lane);
						pos += previous->m_duration;
						sus -= previous->m_duration;
						if (index + 1 == currChart->m_phrases.size())
						{
							previous->m_end = false;
							currChart->emplacePhrase(pos, sus, false, true, 0, lane);
							break;
						}
					}
					else
					{
						if (previous->m_duration >= sus + SAMPLES_PER_TICK_ROUNDED)
						{
							pos += sus;
							previous->m_end = false;
							if (index + 1 == currChart->m_phrases.size())
							{
								previous->m_end = false;
								currChart->emplacePhrase(pos, previous->m_duration - sus, false, true, 0, previous->getColor());
							}
							previous->changeEndAlpha(pos);
						}
						else
							previous->m_duration = sus;
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

void CH_Importer::addGuardMark(const long pos, const unsigned long fret, Chart* currChart)
{
	static const int buttons[5] = { 1, 2, -1, 0, 3 };
	if (currChart->m_guards.size() == 0 || currChart->m_guards.back().m_pivotAlpha < pos)
	{
		try
		{
			currChart->emplaceGuard(pos, buttons[fret]);
		}
		catch (...)
		{
			printf("%sGreen, Red, Blue, Orange are the only legal frets for guard marks\n", g_global.tabs.c_str());
		}
	}
		
}

void CH_Importer::applyForced(const long pos, const size_t sectIndex, const size_t playerIndex)
{
	Chart* currChart = &m_sections[sectIndex].m_subs[playerIndex].back();
	size_t index = currChart->m_phrases.size();
	while (index > 0)
	{
		Phrase& base = currChart->m_phrases[--index];
		if (base.m_pivotAlpha == pos)
		{
			if (index == 0)
				base.addColor(64);
			else if (base.m_start)
			{
				Phrase& previous = currChart->m_phrases[index - 1];
				base.m_start = false;
				previous.m_end = false;
				previous.changeEndAlpha(base.m_pivotAlpha);
			}
			else
			{
				bool remove = false;
				while (index > 0)
				{
					Phrase& previous = currChart->m_phrases[--index];
					for (unsigned long color = 0, val = 1; color < 6; ++color, val <<= 1)
					{
						if (!(base.getColor() & val) && (previous.getColor() & val))
						{
							if (remove)
							{
								printf("%s%s - Player %zu - Subsection %zu: ", g_global.tabs.c_str(), m_sections[sectIndex].m_name.c_str(),
																		playerIndex + 1, m_sections[sectIndex].m_subs[playerIndex].size() - 1);
								currChart->removePhraseBar(index + 1);
							}
							previous.m_end = false;
							previous.changeEndAlpha(base.m_pivotAlpha);
							g_global.quit = true;
							break;
						}
					}
					if (!(g_global.quit || remove || previous.m_start))
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
