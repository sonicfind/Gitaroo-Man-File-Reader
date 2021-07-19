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
#include "CH_Import.h"
#include <thread>

using namespace std;
using namespace GlobalFunctions;

bool CHC::importFromCloneHero(bool doSave)
{
	banner(" " + m_filename + " - .CHART Note Import/Replacement ");
	if (m_unorganized != 0)
	{
		do
		{
			printf("%s%s.CHC must be fully fixed & organized before importing notes from a .chart.\n", g_global.tabs.c_str(), m_filename.c_str());
			printf("%sFix & organize the file now? [Y/N]\n", g_global.tabs.c_str());
			const auto result = menuChoices("yn");
			if (result == ResultType::Success && g_global.answer.character == 'y')
			{
				applyChanges(true, false);
				banner(" " + m_filename + " - .CHART Note Import/Replacement ");
				g_global.quit = true;
			}
			else if (result != ResultType::Quit && result != ResultType::Failed)
			{
				printf("%sCH Chart Import on %s aborted due to not being fully organized.\n", g_global.tabs.c_str(), m_filename.c_str());
				return false;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}

	CloneHero_To_CHC importer;
	do
	{
		printf("%sProvide the name of the .CHART file you wish to use (Or 'Q' to exit): ", g_global.tabs.c_str());
		string chartName = "";
		switch (stringInsertion(chartName))
		{
		case ResultType::Quit:
			return false;
		case ResultType::Success:
			if (chartName.find(".CHART") == string::npos && chartName.find(".chart") == string::npos)
				chartName += ".CHART";
			if (importer.open(chartName.c_str()))
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

	if (CHC* song = importer.convertNotes(this))
	{
		banner(" " + m_filename + " - .CHART Note Import/Replacement Finalization ");
		const char* ext[2] = { "_IMPORTED", "_T" };
		bool found = song->m_filename.find("IMPORTED") != string::npos;
		do
		{
			printf("%sHow should the file be named?\n", g_global.tabs.c_str());
			printf("%sK - Keep filename as %s\n", g_global.tabs.c_str(), song->m_filename.c_str());
			printf("%sC - Change filename to %s%s\n", g_global.tabs.c_str(), song->m_filename.c_str(), ext[found]);
			switch (menuChoices("kc"))
			{
			case ResultType::Quit:
				delete song;
				return false;
			case ResultType::Failed:
				break;
			case ResultType::Success:
				if (g_global.answer.character == 'c')
					song->m_filename += ext[found];
				printf("%s\n", g_global.tabs.c_str());
				g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;

		ResultType result = ResultType::Success;
		do
		{
			// Either yes or no will still overwrite the CHC data held
			// in the FileMain<CHC> object
			if (!doSave)
			{
				printf("%sSave %s externally? [Y/N]\n", g_global.tabs.c_str(), song->m_filename.c_str());
				result = menuChoices("yn");
			}
			
			switch (result)
			{
			case ResultType::Quit:
				delete song;
				return false;
			case ResultType::Success:
				if (doSave || g_global.answer.character == 'y')
				{
					string filename = song->m_directory + song->m_filename;
					string ext = "";
					do
					{
						switch (fileOverwriteCheck(filename + ext + ".CHC"))
						{
						case ResultType::No:
							ext += "_T";
							break;
						case ResultType::Yes:
							song->create(filename + ext);
							song->m_filename += ext;
							g_global.quit = true;
							break;
						case ResultType::Quit:
							printf("%s\n", g_global.tabs.c_str());
							g_global.quit = true;
							song->m_saved = false;
						}
					} while (!g_global.quit);
				}
				else
					g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;

		m_saved = song->m_saved;
		m_filename = song->m_filename;
		m_optimized = song->m_optimized;
		m_sections = std::move(song->m_sections);
		delete song;
		return true;
	}
	else
	{
		printf("%s\n", g_global.tabs.c_str());
		printf("%sNote import aborted.\n", g_global.tabs.c_str());
		return false;
	}
}

Section::Section(string name, float pos_ticks)
	: CHObject(pos_ticks)
	, m_position_samples(0)
	, m_name(name)
{
	m_subs[0].push_back({ false });
	m_subs[1].push_back({ false });
}

Section::Tempo::Tempo(float pos_ticks, float sample_offset, unsigned long bpm)
	: CHObject(pos_ticks)
	, m_sample_offset_from_section(sample_offset)
	, m_bpm(bpm) {}

bool CloneHero_To_CHC::open(std::string filename)
{
	if (ChartFile::open(filename, false))
	{
		int chartVersion = 1;
		char ignore[400];
		// Skips the info section
		fscanf_s(m_chart, " %[^}]", ignore, 400);
		fscanf_s(m_chart, " %[^{]", ignore, 400);
		fseek(m_chart, 1, SEEK_CUR);

		std::list<SyncTrack> tempos;

		char test = 0;
		fscanf_s(m_chart, " %c", &test, 1);
		// Checking for the end of the synctrack section
		while (test != '}')
		{
			fseek(m_chart, -1, SEEK_CUR);
			//Creates new tempo object
			tempos.push_back({ m_chart });
			if (tempos.back().m_bpm == 0)
				tempos.pop_back();
			fscanf_s(m_chart, " %c", &test, 1);
		}

		fscanf_s(m_chart, " %[^{]", ignore, 400);
		fseek(m_chart, 1, SEEK_CUR);
		fscanf_s(m_chart, " %c", &test, 1);

		// Checking for the end of the event section
		while (test != '}')
		{
			fseek(m_chart, -1, SEEK_CUR);
			Event ev(m_chart);
			if (ev.m_name.find("GMFR EXPORT V2.0") != string::npos)
				chartVersion = 2;
			else if (ev.m_name.find("section") != string::npos)
				m_sections.push_back({ ev.m_name, ev.m_position_ticks });
			fscanf_s(m_chart, " %c", &test, 1);
		}

		{
			auto tempoIter = tempos.begin();
			for (size_t sectIndex = 0; sectIndex < m_sections.size(); ++sectIndex, --tempoIter)
			{
				Section& section = m_sections[sectIndex];
				if (sectIndex > 0)
				{
					Section::Tempo& prev = m_sections[sectIndex - 1].m_tempos.back();
					section.m_position_samples = m_sections[sectIndex - 1].m_position_samples + prev.m_sample_offset_from_section
										+ (1000 * s_SAMPLES_PER_MIN / s_TICKS_PER_BEAT) * (section.m_position_ticks - prev.m_position_ticks) / prev.m_bpm;
				}

				while (tempoIter != tempos.end() && (sectIndex + 1 == m_sections.size() || *tempoIter < m_sections[sectIndex + 1]))
				{
					float pos_samples = 0;
					float pos_ticks = tempoIter->m_position_ticks;
					if (section.m_tempos.size())
					{
						Section::Tempo& prev = section.m_tempos.back();
						pos_samples = prev.m_sample_offset_from_section
									+ (1000 * s_SAMPLES_PER_MIN / s_TICKS_PER_BEAT) * (tempoIter->m_position_ticks - prev.m_position_ticks) / prev.m_bpm;
					}
					else if (pos_ticks == 0)
						pos_ticks = section.m_position_ticks;


					section.m_tempos.push_back({ pos_ticks, pos_samples, tempoIter->m_bpm });
					++tempoIter;
				}
			}
		}

		bool convert = false;
		if (chartVersion == 1)
		{
			do
			{
				printf("%sGMFR import, while compatible, works best with a .chart file updated to the current Moonscraper->CHC charting style. Create a converted version of this file? [Y/N]\n", g_global.tabs.c_str());
				printf("%sEnter 'C' if this file already utilizes this style (this will add the proper global event marker that defines the GMFR import version).\n", g_global.tabs.c_str());
				switch (menuChoices("ync"))
				{
				case ResultType::Quit:
					return false;
				case ResultType::Success:
					switch (g_global.answer.character)
					{
					case 'c':
						chartVersion = 2;
						__fallthrough;
					case 'y':
						convert = true;
						__fallthrough;
					default:
						g_global.quit = true;
					}
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}

		// Note insertion

		while (!feof(m_chart))
		{
			fscanf_s(m_chart, " %[^{]", ignore, 399);
			fseek(m_chart, 1, SEEK_CUR);

			if (feof(m_chart))
				break;

			NoteTrack* track = nullptr;
			if (strstr(ignore, "[ExpertSingle]"))
				track = m_notes;
			else if (strstr(ignore, "[ExpertDoubleRhythm]"))
				track = &m_notes[1];

			if (track != nullptr)
			{
				fscanf_s(m_chart, " %c", &test, 1);
				while (test != '}' && !feof(m_chart))
				{
					fseek(m_chart, -1, SEEK_CUR);
					track->pushNote(CHNote(m_chart), chartVersion);
					fscanf_s(m_chart, " %c", &test, 1);
				}
			}
			else
				do
				{
					fscanf_s(m_chart, " %[^}]", ignore, 399);
					fscanf_s(m_chart, " %c", &test, 1);
				} while (test != '}');

			if (!feof(m_chart))
				fseek(m_chart, 1, SEEK_CUR);
		}
		close();

		if (convert)
		{
			std::list<Event> events;

			events.push_back({ 0.0f, "GMFR EXPORT V2.0" });
			for (size_t i = 0; i < m_sections.size(); ++i)
				events.push_back({ m_sections[i].m_position_ticks, m_sections[i].m_name });

			ChartFileExporter converter(tempos, events, m_notes);
			if (chartVersion == 1)
				converter.open(m_filename.substr(0, m_filename.length() - 6) + " v2.0 Convert.chart", true);
			else
				converter.open(m_filename, true);

			converter.write(false);
			converter.close();
		}

		// For example, if the section name is "BATTLE - something",
		// only use "something" / disregard "BATTLE - ".
		// Otherwise, just gets rid of "section"
		for (auto& sect : m_sections)
			sect.m_name = sect.m_name.substr(sect.m_name.find_last_of(' ') + 1);
		return true;
	}
	return false;
}

CHC* CloneHero_To_CHC::convertNotes(CHC* song)
{
	bool charted[2] = { false, false };
	for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
	{
		if (!m_notes[playerIndex].m_allNotes.size())
			continue;

		auto noteIterator = m_notes[playerIndex].m_allNotes.begin();
		for (size_t sectIndex = 0; sectIndex < m_sections.size(); ++sectIndex)
		{
			Section* section = &m_sections[sectIndex];
			Chart* chart = &section->m_subs[playerIndex].back();
			auto tempoIter = section->m_tempos.begin();
			float SAMPLES_PER_TICK = s_SPT_CONSTANT / (s_TICKS_PER_BEAT * tempoIter->m_bpm);
			enum class Status
			{
				newChart,
				StartWithGuard,
				StartWithPath,
				SwappedToPath,
				SwappedToGuard
			} status = Status::newChart;

			for (; noteIterator != m_notes[playerIndex].m_allNotes.end()
				&& (sectIndex + 1 == m_sections.size() || *noteIterator < m_sections[sectIndex + 1]);
				++noteIterator)
			{
				while (tempoIter + 1 != section->m_tempos.end()
					&& *noteIterator >= *(tempoIter + 1))
				{
					++tempoIter;
					SAMPLES_PER_TICK = (s_SPT_CONSTANT / s_TICKS_PER_BEAT) * (1.0f / tempoIter->m_bpm);
				}

				for (auto eventIterator = noteIterator->m_events.begin();
					eventIterator != noteIterator->m_events.end();)
				{
					if (eventIterator->find("start") != string::npos)
					{
						if (status == Status::StartWithPath)
						{
							chart->setPivotTime(chart->m_tracelines.front().m_pivotAlpha);
							chart->setEndTime(chart->m_tracelines.back().m_pivotAlpha + 1);
						}
						else if (status == Status::StartWithGuard)
						{
							chart->setPivotTime(chart->m_guards.back().m_pivotAlpha);
							chart->setEndTime(chart->m_guards.back().m_pivotAlpha + 20);
						}
						status = Status::newChart;

						section->m_subs[playerIndex].emplace_back(false);
						chart = &section->m_subs[playerIndex].back();

						noteIterator->m_events.erase(eventIterator);
						break;
					}
					else
						++eventIterator;
				}

				for (auto eventIterator = noteIterator->m_events.begin();
					eventIterator != noteIterator->m_events.end();)
				{
					// Signals a trace line related function or note
					if (eventIterator->find("Trace") != string::npos)
					{
						// Set previous Trace line's curve set to true
						if (eventIterator->find("curve") != string::npos)
							chart->m_tracelines.back().m_curve = true;
						else if (status != Status::SwappedToGuard)
						{
							const float position = (noteIterator->m_position_ticks - tempoIter->m_position_ticks) * SAMPLES_PER_TICK + tempoIter->m_sample_offset_from_section;
							switch (addTraceLine(position, *eventIterator, sectIndex, playerIndex))
							{
							case -1:
								printf("%sTrace line event at tick position %lu had extraneous data that could not be pulled.\n", g_global.tabs.c_str(),
									(unsigned long)noteIterator->m_position_ticks);
								printf("%sRemember: trace events *must* be formatted as \"Trace(P)\", \"Trace(P)_[float angle value]\", \"Trace(P)_end\", or \"Trace_curve\"\n", g_global.tabs.c_str());
								__fallthrough;
							case 1:
								if (status == Status::newChart)
									status = Status::StartWithPath;
								else if (status == Status::StartWithGuard)
								{
									status = Status::SwappedToPath;
									chart->setPivotTime(((long)roundf(position) + chart->m_guards.back().m_pivotAlpha) >> 1);
								}

								if (status == Status::SwappedToPath)
									chart->setEndTime((long)roundf(position) + 1);
							}
						}
						noteIterator->m_events.erase(eventIterator++);
					}
					else
						++eventIterator;
				}

				if (noteIterator->hasColors())
				{
					const float position = (noteIterator->m_position_ticks - tempoIter->m_position_ticks) * SAMPLES_PER_TICK + tempoIter->m_sample_offset_from_section;
					for (int index = 0; index < 6; ++index)
						if (noteIterator->m_colors[index].m_sustain > 0)
						{
							if (status == Status::StartWithPath || status == Status::SwappedToPath)
								addPhraseBar(chart, *noteIterator, position, SAMPLES_PER_TICK);
							goto CheckAnims;
						}

					const long roundedPosition = (long)roundf(position);
					if (status != Status::SwappedToPath
						&& addGuardMark(chart, *noteIterator, roundedPosition))
					{
						if (status == Status::newChart)
							status = Status::StartWithGuard;
						else if (status == Status::StartWithPath)
						{
							status = Status::SwappedToGuard;
							chart->setPivotTime((roundedPosition + chart->m_tracelines.back().m_pivotAlpha + 1) >> 1);
						}

						if (status == Status::SwappedToGuard)
							chart->setEndTime(roundedPosition + 20);
					}
				}

			CheckAnims:
				for (auto& ev : noteIterator->m_events)
					if (ev.find("Anim") != string::npos)
						chart->m_phrases.back().m_animation = ev[5] - 48;
			}
		}

		charted[playerIndex] = true;
	}

	if (charted[0] || charted[1])
	{
		CHC* temp = new CHC(*song);
		std::vector<std::thread> threads;
		for (size_t sectIndex = 0; sectIndex < m_sections.size(); ++sectIndex)
		{
			Section& section = m_sections[sectIndex];
			for (auto& songSection : temp->m_sections)
				if (string(songSection.getName()).compare(0, m_sections[sectIndex].m_name.length(), m_sections[sectIndex].m_name) == 0)
				{
					m_sections[sectIndex].m_insertions.push(&songSection);
					if (m_sections[sectIndex].m_insertions.size() == 0 && sectIndex + 1 < m_sections.size())
					{
						//Sets Section duration to an even and equal spacing based off marked tempo
						const float numBeats = roundf((m_sections[sectIndex + 1].m_position_ticks - section.m_position_ticks) / s_TICKS_PER_BEAT);
						songSection.setDuration((unsigned long)round(s_SAMPLES_PER_MIN * numBeats / songSection.getTempo()));
					}
				}

			if (m_sections[sectIndex].m_insertions.size())
			{
				if (song->isPS2Compatible())
					threads.push_back(std::thread(&Section::replaceNotes, std::ref(section), charted));
				else
					threads.push_back(std::thread(&Section::replaceNotes_Duet, std::ref(section), charted));
			}
		}

		for (std::thread& thr : threads)
			thr.join();

		for (SongSection& section : temp->m_sections)
			if (!section.getOrganized())
			{
				printf("%sReplaced %s\n", g_global.tabs.c_str(), section.getName());
				section.setOrganized(true);
			}

		temp->fixNotes();
		return temp;
	}
	return nullptr;
}

void Section::replaceNotes(const bool charted[2])
{
	SongSection* section = m_insertions.front();
	struct ChartRange
	{
		long start = 0;
		long end = 0;
		Chart* insert;
	};

	list<ChartRange> ranges[2];
	for (size_t chartIndex = 0; chartIndex < section->getNumCharts(); ++chartIndex)
		for (size_t playerIndex = 0; playerIndex < section->getNumPlayers(); ++playerIndex)
			if (charted[playerIndex & 1])
			{
				Chart* ins = &section->m_charts[playerIndex * section->getNumCharts() + chartIndex];
				if (ins->getNumGuards() && ins->getNumTracelines() > 1)
				{
					// Determining which comes first
					if (ins->m_guards.front().m_pivotAlpha <= ins->m_tracelines.front().m_pivotAlpha)
						ranges[playerIndex & 1].push_back({ ins->m_guards.front().m_pivotAlpha + ins->getPivotTime()
														, ins->m_tracelines.back().getEndAlpha() + ins->getPivotTime()
														, ins });
					else
						ranges[playerIndex & 1].push_back({ ins->m_tracelines.front().m_pivotAlpha + ins->getPivotTime()
														, ins->m_guards.back().m_pivotAlpha + ins->getPivotTime()
														, ins });
				}
				else if (ins->getNumGuards())
					ranges[playerIndex & 1].push_back({ ins->m_guards.front().m_pivotAlpha + ins->getPivotTime()
														, ins->m_guards.back().m_pivotAlpha + ins->getPivotTime()
														, ins });
				else if (ins->getNumTracelines() > 1)
					ranges[playerIndex & 1].push_back({ ins->m_tracelines.front().m_pivotAlpha + ins->getPivotTime()
														, ins->m_tracelines.back().getEndAlpha() + ins->getPivotTime()
														, ins });
			}

	bool swapped[2] = { (section->getSwapped() & 1) > 0, (section->getSwapped() & 1) > 0 };
	for (int playerIndex = 0; playerIndex < 2; ++playerIndex)
	{
		if (!charted[playerIndex])
			continue;

		for (Chart& chart : m_subs[playerIndex])
		{
			if (chart.m_tracelines.size() > 1 || chart.m_guards.size())
			{
				long start;
				long end;
				if (chart.getNumGuards() && chart.getNumTracelines() > 1)
				{
					// Determining which comes first
					if (chart.m_guards.front().m_pivotAlpha <= chart.m_tracelines.front().m_pivotAlpha)
					{
						start = chart.m_guards.front().m_pivotAlpha;
						end = chart.m_tracelines.back().getEndAlpha();
					}
					else
					{
						start = chart.m_tracelines.front().m_pivotAlpha;
						end = chart.m_guards.back().m_pivotAlpha;
					}
				}
				else if (chart.getNumGuards())
				{
					start = chart.m_guards.front().m_pivotAlpha;
					end = chart.m_guards.back().m_pivotAlpha;
				}
				else
				{
					start = chart.m_tracelines.front().m_pivotAlpha;
					end = chart.m_tracelines.back().getEndAlpha();
				}
				chart.finalizeNotes();

				for (auto rangeIter = ranges[playerIndex].begin(); rangeIter != ranges[playerIndex].end(); ++rangeIter)
				{
					if ((start <= rangeIter->start && rangeIter->end <= end)
						|| (rangeIter->start <= start && start < rangeIter->end)
						|| (rangeIter->start < end && end <= rangeIter->end))
					{
						chart.setJunk(rangeIter->insert->getJunk());
						*rangeIter->insert = std::move(chart);
						section->setOrganized(false);

						if (section->getPhase() == SongSection::Phase::BATTLE
							&& rangeIter->insert->m_guards.size() && rangeIter->insert->m_tracelines.size() > 1)
						{
							// Determining which comes first
							if (rangeIter->insert->m_guards.front().m_pivotAlpha < rangeIter->insert->m_tracelines.front().m_pivotAlpha)
							{
								if (playerIndex == 1)
									swapped[1] = true;
							}
							else if (rangeIter->insert->m_tracelines.front().m_pivotAlpha < rangeIter->insert->m_guards.front().m_pivotAlpha)
							{
								if (playerIndex == 0)
									swapped[0] = true;
							}
						}
						ranges->erase(rangeIter);
						break;
					}
				}
			}
		}
	}

	if (swapped[0] && swapped[1])
		section->setSwapped(1);

	m_insertions.pop();
	while (m_insertions.size())
	{
		*m_insertions.front() = *section;
		m_insertions.pop();
	}
}

void Section::replaceNotes_Duet(const bool charted[2])
{
	SongSection* section = m_insertions.front();
	struct ChartRange
	{
		long start = 0;
		long end = 0;
		Chart* insert;
	};

	list<ChartRange> ranges[2];
	for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
	{
		if (!charted[playerIndex])
			continue;

		for (size_t chartIndex = 0; chartIndex < section->getNumCharts(); chartIndex++)
		{
			Chart* ins = &section->m_charts[2 * playerIndex * section->getNumCharts() + chartIndex];
			if (ins->getNumGuards() && ins->getNumTracelines() > 1)
			{
				// Determining which comes first
				if (ins->m_guards.front().m_pivotAlpha <= ins->m_tracelines.front().m_pivotAlpha)
					ranges[playerIndex].push_back({ ins->m_guards.front().m_pivotAlpha + ins->getPivotTime()
													, ins->m_tracelines.back().getEndAlpha() + ins->getPivotTime()
													, ins });
				else
					ranges[playerIndex].push_back({ ins->m_tracelines.front().m_pivotAlpha + ins->getPivotTime()
													, ins->m_guards.back().m_pivotAlpha + ins->getPivotTime()
													, ins });
			}
			else if (ins->getNumGuards())
				ranges[playerIndex].push_back({ ins->m_guards.front().m_pivotAlpha + ins->getPivotTime()
													, ins->m_guards.back().m_pivotAlpha + ins->getPivotTime()
													, ins });
			else if (ins->getNumTracelines() > 1)
				ranges[playerIndex].push_back({ ins->m_tracelines.front().m_pivotAlpha + ins->getPivotTime()
													, ins->m_tracelines.back().getEndAlpha() + ins->getPivotTime()
													, ins });
		}

		for (Chart& chart : m_subs[playerIndex])
		{
			if (chart.m_tracelines.size() > 1 || chart.m_guards.size())
			{
				long start;
				long end;
				if (chart.getNumGuards() && chart.getNumTracelines() > 1)
				{
					// Determining which comes first
					if (chart.m_guards.front().m_pivotAlpha <= chart.m_tracelines.front().m_pivotAlpha)
					{
						start = chart.m_guards.front().m_pivotAlpha;
						end = chart.m_tracelines.back().getEndAlpha();
					}
					else
					{
						start = chart.m_tracelines.front().m_pivotAlpha;
						end = chart.m_guards.back().m_pivotAlpha;
					}
				}
				else if (chart.getNumGuards())
				{
					start = chart.m_guards.front().m_pivotAlpha;
					end = chart.m_guards.back().m_pivotAlpha;
				}
				else
				{
					start = chart.m_tracelines.front().m_pivotAlpha;
					end = chart.m_tracelines.back().getEndAlpha();
				}
				chart.finalizeNotes();

				for (auto rangeIter = ranges[playerIndex].begin(); rangeIter != ranges[playerIndex].end(); ++rangeIter)
				{
					if ((start <= rangeIter->start && rangeIter->end <= end)
						|| (rangeIter->start <= start && start < rangeIter->end)
						|| (rangeIter->start < end && end <= rangeIter->end))
					{
						chart.setJunk(rangeIter->insert->getJunk());
						*rangeIter->insert = std::move(chart);
						section->setOrganized(false);
						ranges[playerIndex].erase(rangeIter);
						break;
					}
				}
			}
		}
	}

	m_insertions.pop();
	while (m_insertions.size())
	{
		*m_insertions.front() = *section;
		m_insertions.pop();
	}
}

int CloneHero_To_CHC::addTraceLine(float pos, string name, const size_t sectIndex, const size_t playerIndex)
{
	Chart* currChart = &m_sections[sectIndex].m_subs[playerIndex].back();
	Chart* insert = nullptr;
	//If the trace line goes into the previous subsection
	if (sectIndex > 0
		&& ((name.length() > 5 && toupper(name[5]) == 'P')
			|| (pos == 0 && name.find("end") != string::npos)))
	{
		Section::Tempo& prevtempo = m_sections[sectIndex - 1].m_tempos.back();

		if (pos == 0 && name.find("end") != string::npos)
			--pos;
		else if (currChart->m_tracelines.size() > 0 && currChart->m_tracelines.front().m_pivotAlpha <= pos)
			pos = float(currChart->m_tracelines.front().m_pivotAlpha) - 1;

		pos += prevtempo.m_sample_offset_from_section + (s_SPT_CONSTANT / s_TICKS_PER_BEAT) * (m_sections[sectIndex].m_position_ticks - prevtempo.m_position_ticks) / prevtempo.m_bpm;
		insert = &m_sections[sectIndex - 1].m_subs[playerIndex].back();
	}
	else
		insert = currChart;

	// If the traceline is not an end piece AND has an angle tied to it
	if (name.find("end") == string::npos && name.find('_') != string::npos)
	{
		try
		{
			insert->emplaceTraceline((long)round(pos), 1, float(stof(name.substr(name.find('_') + 1)) * M_PI / 180));
			return 1;
		}
		catch (...)
		{
			insert->emplaceTraceline((long)round(pos));
			return -1;
		}
	}
	insert->emplaceTraceline((long)round(pos));
	return 0;
}

void CloneHero_To_CHC::addPhraseBar(Chart* currChart, const CHNote& note, const float position, const float SAMPLES_PER_TICK)
{
	unsigned color = 0;
	bool start = true;

	if (note.m_isForced)
	{
		if (currChart->m_phrases.size() > 0)
		{
			start = false;
			currChart->m_phrases.back().m_end = false;
			currChart->m_phrases.back().changeEndAlpha((long)roundf(position));
		}
		else
			color = 1 << 6;
	}

	std::vector<float> sustains;
	for (auto& fret : note.m_colors)
		if (fret.m_active)
		{
			auto iter = sustains.begin();
			while (iter != sustains.end() && fret.m_sustain > *iter)
				++iter;

			if (iter == sustains.end() || fret.m_sustain < *iter)
				sustains.insert(iter, fret.m_sustain);
		}

	float processed = 0;
	for (float sustain : sustains)
	{
		for (int index = 0; index < 6; ++index)
			if (note.m_colors[index].m_sustain >= sustain)
				color += 1UL << index;
		currChart->emplacePhrase( (long)roundf(position + processed * SAMPLES_PER_TICK)
								, (long)roundf((sustain - processed) * SAMPLES_PER_TICK)
								, start
								, false
								, 0
								, color);
		start = false;
		processed = sustain;
		color = 0;
	}

	currChart->m_phrases.back().m_end = true;
}

bool CloneHero_To_CHC::addGuardMark(Chart* currChart, const CHNote& note, const long position)
{
	static const int buttons[5] = { 1, 2, -1, 0, 3 };
	if (currChart->m_guards.size() == 0 || currChart->m_guards.back().m_pivotAlpha < position)
		for (int index = 0; index < 5; ++index)
			if (note.m_colors[index].m_active)
				try
				{
					currChart->emplaceGuard(position, buttons[index]);
					return true;
				}
				catch (...)
				{
					printf("%sGreen, Red, Blue, Orange are the only legal frets for guard marks\n", g_global.tabs.c_str());
				}
	return false;
}
