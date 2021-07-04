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
#include "CHC.h"
#include <condition_variable>
using namespace std;

bool CHC::applyChanges(const bool fix, const bool swap, const bool save)
{
	try
	{
		if (fix)
		{
			fixNotes();
			organizeAll();
		}

		if (swap)
			playerSwapAll();

		if (save)
			create(m_directory + m_filename);
		return true;
	}
	catch (exception e)
	{
		printf("%sException Caught: %s\n", g_global.tabs.c_str(), e.what());
		return false;
	}
}

void CHC::edit(const bool multi)
{
	do
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - Editor ");
		string choices = "vagcd";
		if (!m_optimized)
		{
			choices += 'f';
			printf("%sF - Fix any problematic notes or trace lines\n", g_global.tabs.c_str());
		}
		if (m_unorganized)
		{
			choices += 'o';
			printf("%sO - Organize All Unorganized Sections\n", g_global.tabs.c_str());
		}
		if (strstr(m_imc, ".IMC"))
		{
			choices += 'i';
			printf("%sI - Swap IMC file (Current file: %s)\n", g_global.tabs.c_str(), m_imc + 34);
		}
		else
		{
			choices += 'p';
			printf("%sP - Convert for PS2-Version Compatibility\n", g_global.tabs.c_str());
		}
		printf("%sV - Volume & Pan Settings\n", g_global.tabs.c_str());
		printf("%sA - Win/Loss Animations\n", g_global.tabs.c_str());
		printf("%sG - Adjust Gameplay Speed: %g\n", g_global.tabs.c_str(), m_speed );
		printf("%sC - SongSections (%zu)\n", g_global.tabs.c_str(), m_sections.size());
		printf("%sD - Player Damage/Energy Factors\n", g_global.tabs.c_str());
		printf("%s? - Help info\n", g_global.tabs.c_str());
		if (multi)
			printf("%sQ - Close this file\n", g_global.tabs.c_str());
		else
			printf("%sQ - Exit Detail Editor\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices, true))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
			printf("%sS - Save File:\n", g_global.tabs.c_str());
			printf("%sPretty self-explanatory - save the file as an overwrite or as a new file.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			if (strstr(m_imc, ".IMC"))
			{
				printf("%sI - Swap IMC file (Current file: %s):\n", g_global.tabs.c_str(), m_imc + 34);
				printf("%sPS2 compatible CHC files contain a string that points to the location of the .IMC audio file used for the stage.\n", g_global.tabs.c_str());
				printf("%sUse this option to change which specific .IMC file the CHC will point to;\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}
			else
			{
				printf("%sP - Convert for PS2-Version Compatibility:\n", g_global.tabs.c_str());
				printf("%sUse this option to convert this (assumed) duet-made CHC into a PS2 compatible CHC. Doing so will open up a few other features\n", g_global.tabs.c_str());
				printf("%s- most prominently being TASing capability - at the cost of one of the player tracks: Player 1, Player 2, or the Enemy.\n", g_global.tabs.c_str());
				printf("%sThis will also reorganize all unorganized sections for optimal compatibility.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}
			printf("%sV - Volume & Pan Settings:\n", g_global.tabs.c_str());
			printf("%sAllows you to adjust the volume and pan of the 8 available audio channels.\n", g_global.tabs.c_str());
			printf("%sNote that you can change channel 1 & 2 and channel pan altogether only if the CHC in question is PS2 compatible.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sA - Win/Loss Animations:\n", g_global.tabs.c_str());
			printf("%sEvery stage in the game has a set of win and loss animations. These animations start at set points in a stage's frame data (located in the stage's SSQ file).\n", g_global.tabs.c_str());
			printf("%sThis option will allow you to change what frame any of the four animations (two wins, two losses) start on - although you'll most likely never use this.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sG - Adjust Gameplay Speed (Current speed: %g):\n", g_global.tabs.c_str(), m_speed);
			printf("%sThe speed of which trace lines, phrae bars, and guard marks approach the middle of the screen is determined by this value.\n", g_global.tabs.c_str());
			printf("%sThis option will allow you to adjust the value to whatever you want (negative even). Note: lower = faster; higher = slower.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sC - SongSections (%zu):\n", g_global.tabs.c_str(), m_sections.size());
			printf("%sIn the future, this option will be the gateway to digging into the nitty gritty of each section. It will allow you manually adjust a wide range\n", g_global.tabs.c_str());
			printf("%sof parameters, including a section's duration, conditions, and phase type while also allowing for manual adjustments on subsections and\n", g_global.tabs.c_str());
			printf("%stheir trace lines, phrase bars, and guard marks. Currently, however, it will give you a choice of two features:\n", g_global.tabs.c_str());
			printf("%s1. If there are any sections still unorganized, it will give you the option to reorganize every section that needs it.\n", g_global.tabs.c_str());
			printf("%s2. Change the order of which sections will proceed in gameplay using unconditional conditions. Useful for when you need to test only certain sections.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sF - Player Damage/Energy Factors:\n", g_global.tabs.c_str());
			printf("%sFor each player separately, each phase type has values pertaining to 8 different HP related factors.\n", g_global.tabs.c_str());
			printf("%sThis will bring up a series of menus so that you can edit any factors you like - although some factors have no effect in certain phase types.\n", g_global.tabs.c_str());
			printf("%s(No dealing or receiving damage in charge phases for example).\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			__fallthrough;
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			++g_global;
			switch (g_global.answer.character)
			{
			case 'v':
				audioSettings();
				break;
			case 'a':
				winLossSettings();
				break;
			case 'g':
				adjustSpeed();
				break;
			case 'c':
				sectionMassMenu();
				break;
			case 'd':
				adjustFactors();
				break;
			case 'f':
				fixNotes();
				break;
			case 'o':
				organizeAll();
				break;
			case 'i':
				swapIMC();
				break;
			case 'p':
				PSPToPS2();
			}
			--g_global;
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void CHC::organizeAll()
{
	if (!m_unorganized)
		return;

	GlobalFunctions::banner(" " + m_filename + ".CHC - Complete Reorganization ");
	std::condition_variable cv;
	std::mutex cv_m;
	bool* results = new bool[m_sections.size()]();
	auto organizeSection = [&](const size_t sectIndex)
	{
		auto& section = m_sections[sectIndex];
		if (!section.m_organized)
		{
			bool reorganized = section.reorganize(m_imc[0], m_stage);
			if (sectIndex > 0)
			{
				std::unique_lock<std::mutex> lk(cv_m);
				cv.wait(lk, [&] { return results[sectIndex - 1]; });
			}

			if (reorganized)
			{
				printf("%s%s organized - # of charts per player: %lu", g_global.tabs.c_str(), section.m_name, section.m_numCharts);
				if (m_imc[0])
				{
					size_t totals[2] = { 0, 0 };
					for (size_t pl = 0; pl < 4; ++pl)
						for (size_t ch = 0, index = pl * section.m_numCharts; ch < section.m_numCharts; ++index, ++ch)
							if (section.m_charts[index].m_guards.size() || section.m_charts[index].m_phrases.size() || section.m_charts[index].m_tracelines.size() > 1)
								++totals[pl & 1];
					printf(" (Pair total: %zu)\n", totals[0] >= totals[1] ? totals[0] : totals[1]);
				}
				else
					putchar('\n');
			}
			else
				printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", g_global.tabs.c_str(), section.m_name);
		}
		else if (sectIndex > 0)
		{
			std::unique_lock<std::mutex> lk(cv_m);
			cv.wait(lk, [&] { return results[sectIndex - 1]; });
		}
		results[sectIndex] = true;
		cv.notify_all();
	};

	std::vector<std::thread> threads;
	threads.reserve(m_sections.size());
	for (size_t sectIndex = 0; sectIndex < m_sections.size(); ++sectIndex)
		threads.push_back(std::thread(organizeSection, sectIndex));

	for (auto& thr : threads)
		thr.join();

	delete[m_sections.size()] results;
	printf("%sAll sections organized\n", g_global.tabs.c_str());
}

void CHC::fixNotes()
{
	GlobalFunctions::banner(" Fixing " + m_filename + ".CHC ");
	size_t tracelinesCurved = 0, tracelinesStraightened = 0, tracelinesDeleted = 0, phrasesDeleted = 0, phrasesShortened = 0, guardsDeleted = 0;
	m_optimized = true;
	try
	{
		std::condition_variable cv;
		std::mutex cv_m;
		bool* results = new bool[m_sections.size()]();
		auto fixSection = [&](const size_t sectIndex)
		{
			SongSection& section = m_sections[sectIndex];
			if (section.m_battlePhase == SongSection::Phase::INTRO
				|| (section.m_battlePhase == SongSection::Phase::BATTLE
						&& (strstr(section.m_name, "BRK") || strstr(section.m_name, "BREAK"))))
			{
				results[sectIndex] = true;
				cv.notify_all();
				return;
			}
			std::queue<string> strings;

			try
			{
				for (size_t chartIndex = 0; chartIndex < section.m_charts.size(); ++chartIndex)
				{
					Chart& chart = section.m_charts[chartIndex];
					size_t barsRemoved = 0, linesRemoved = 0;
					try
					{
						if (chart.m_tracelines.size() > 1)
						{
							for (size_t traceIndex = 0, phraseIndex = 0; traceIndex + 1 < chart.m_tracelines.size(); ++traceIndex)
							{
								unsigned long curve = false;
								while (phraseIndex < chart.m_phrases.size() && chart.m_phrases[phraseIndex].m_pivotAlpha < chart.m_tracelines[traceIndex].getEndAlpha())
								{
									if (chart.m_phrases[phraseIndex].m_start)
									{
										if (phraseIndex
											&& chart.m_phrases[phraseIndex].m_pivotAlpha - chart.m_phrases[phraseIndex - 1].getEndAlpha() < SongSection::s_SAMPLE_GAP)
										{
											chart.m_phrases[phraseIndex - 1].changeEndAlpha(chart.m_phrases[phraseIndex].m_pivotAlpha - SongSection::s_SAMPLE_GAP);
											strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(chartIndex) + ": ");
											strings.push("Phrase bar " + to_string(phraseIndex + barsRemoved - 1) + " shortened\n");
											++phrasesShortened;
										}

										while (!chart.m_phrases[phraseIndex].m_end
											&& chart.m_phrases[phraseIndex].m_pivotAlpha >= chart.m_tracelines[traceIndex].getEndAlpha() - 3000
											&& chart.m_phrases[phraseIndex].m_pivotAlpha > chart.m_tracelines[traceIndex].m_pivotAlpha + 800)
										{
											chart.m_phrases[phraseIndex].m_duration += chart.m_phrases[phraseIndex + 1].m_duration;
											chart.m_phrases[phraseIndex].m_end = chart.m_phrases[phraseIndex + 1].m_end;
											memcpy_s(chart.m_phrases[phraseIndex].m_junk, 12, chart.m_phrases[phraseIndex + 1].m_junk, 12);

											if (!chart.m_tracelines[traceIndex].changeEndAlpha(chart.m_phrases[phraseIndex].m_pivotAlpha))
											{
												strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(chartIndex) + ": ");
												if (chart.removeTraceline(traceIndex))
													strings.push("Trace line " + to_string(traceIndex + linesRemoved) + " removed\n");
												++linesRemoved;
											}
											else
												++traceIndex;
											chart.m_tracelines[traceIndex].changePivotAlpha(chart.m_phrases[phraseIndex].m_pivotAlpha);

											strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(chartIndex) + ": ");
											if (chart.removePhraseBar(phraseIndex + 1))
												strings.push("Phrase bar " + to_string(phraseIndex + barsRemoved + 1) + " removed\n");
											++barsRemoved;
										}

										if (m_imc[0] && chart.m_phrases[phraseIndex].m_pivotAlpha + 8000 >= chart.m_tracelines[traceIndex].getEndAlpha())
											curve = true;
									}
									++phraseIndex;
								}

								if (!curve
									&& chart.m_tracelines[traceIndex].m_curve
									&& traceIndex + 1 != chart.m_tracelines.size()
									&& chart.m_tracelines[traceIndex].m_angle != chart.m_tracelines[traceIndex + 1].m_angle)
									curve = true;

								if (curve != chart.m_tracelines[traceIndex].m_curve)
								{
									chart.m_tracelines[traceIndex].m_curve = curve;
									strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(chartIndex) + ": ");
									if (curve)
									{
										strings.push("Trace line " + to_string(traceIndex + linesRemoved) + " set to \"smooth\"\n");
										++tracelinesCurved;
									}
									else
									{
										strings.push("Trace line " + to_string(traceIndex + linesRemoved) + " set to \"rigid\"\n");
										++tracelinesStraightened;
									}
								}
							}
						}
					}
					catch (...)
					{
						strings.push(g_global.tabs + "There was an error when attempting to apply phrase bar & trace line fixes to section " + to_string(sectIndex)
							+ " (" + section.m_name + ") - Subsection " + to_string(chartIndex) + "\n");
						m_optimized = false;
					}
					tracelinesDeleted += linesRemoved;
					phrasesDeleted += barsRemoved;


					for (size_t guardIndex = 0; guardIndex + 1 < chart.m_guards.size();)
					{
						if (chart.m_guards[guardIndex].m_pivotAlpha + 1600 > chart.m_guards[guardIndex + 1].m_pivotAlpha)
						{
							strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(chartIndex) + ": ");
							if (chart.removeGuardMark(guardIndex + 1))
								strings.push("Guard Mark " + to_string(guardsDeleted) + " removed\n");
							++guardsDeleted;
						}
						else
							++guardIndex;
					}
				}
			}
			catch (...) {}

			if (section.m_battlePhase == SongSection::Phase::END || sectIndex + 1 == m_sections.size())
			{
				results[sectIndex] = true;
				cv.notify_all();
				return;
			}

			if (!m_imc[0] || section.m_battlePhase == SongSection::Phase::HARMONY)
			{
				for (size_t playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
				{
					for (size_t chartIndex = section.m_numCharts; chartIndex > 0;)
					{
						--chartIndex;
						Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
						if (chart.getNumPhrases())
						{
							for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
							{
								long* effect = &section.m_conditions[condIndex].m_trueEffect;
								for (size_t i = 0; i < 2; i++, effect++)
								{
									if (*effect >= 0)
									{
										long endTime = section.m_duration - (chart.m_phrases.back().getEndAlpha() + chart.m_pivotTime);
										Chart& chart2 = m_sections[*effect].m_charts[playerIndex * m_sections[*effect].m_numCharts];
										if (chart2.m_phrases.size())
										{
											long startTime = chart2.m_phrases.front().m_pivotAlpha + chart2.m_pivotTime;
											if (startTime + endTime < section.s_SAMPLE_GAP)
											{
												chart.m_phrases.back().changeEndAlpha((long)(section.m_duration - startTime - section.s_SAMPLE_GAP - chart.m_pivotTime));
												strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
												strings.push("Phrase bar " + to_string(chart.m_phrases.size() - 1) + " shortened\n");
												phrasesShortened++;
											}
										}
									}
								}
							}
							break;
						}
					}
				}
			}
			else
			{
				for (size_t p = 0; p < 2; p++)
				{
					for (size_t chartIndex = section.m_numCharts; chartIndex > 0;)
					{
						--chartIndex;
						for (size_t playerIndex = section.m_numPlayers + p; playerIndex > 1;)
						{
							playerIndex -= 2;
							Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
							if (chart.getNumPhrases())
							{
								for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
								{
									long* effect = &section.m_conditions[condIndex].m_trueEffect;
									for (size_t i = 0; i < 2; i++, effect++)
									{
										if (*effect >= 0)
										{
											long endTime = section.m_duration - (chart.m_phrases.back().getEndAlpha() + chart.m_pivotTime);
											Chart& chart2 = m_sections[*effect].m_charts[p * m_sections[*effect].m_numCharts];
											if (chart2.m_phrases.size())
											{
												long startTime = chart2.m_phrases.front().m_pivotAlpha + chart2.m_pivotTime;
												if (startTime + endTime < section.s_SAMPLE_GAP)
												{
													chart.m_phrases.back().changeEndAlpha(long(section.m_duration - startTime - section.s_SAMPLE_GAP - chart.m_pivotTime));
													strings.push(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
													strings.push("Phrase bar " + to_string(chart.m_phrases.size() - 1) + " shortened\n");
													phrasesShortened++;
												}
											}
										}
									}
								}
								goto Iteration;
							}
						}
					}
				Iteration:
					++p;
				}
			}

			if (sectIndex > 0)
			{
				std::unique_lock<std::mutex> lk(cv_m);
				cv.wait(lk, [&] { return results[sectIndex - 1]; });
			}

			while (strings.size())
			{
				printf("%s", strings.front().c_str());
				strings.pop();
			}
			results[sectIndex] = true;
			cv.notify_all();
		};

		std::vector<std::thread> threads;
		threads.reserve(m_sections.size());
		for (size_t sectIndex = 0; sectIndex < m_sections.size(); ++sectIndex)
			threads.push_back(std::thread(fixSection, sectIndex));

		for (auto& thr : threads)
			thr.join();
		delete[m_sections.size()] results;
	}
	catch (...)
	{
		printf("%sUnknown error occurred when applying note fixes. Thus, the chc will remain unoptimized for now.\n", g_global.tabs.c_str());
		m_optimized = false;
	}
	printf("%s\n", g_global.tabs.c_str());

	if (tracelinesCurved)
		printf("%s%zu \"rigid\"%s set to \"smooth\"\n", g_global.tabs.c_str(), tracelinesCurved, (tracelinesCurved > 1 ? " Trace lines" : " Trace line"));
	
	if (tracelinesStraightened)
		printf("%s%zu \"smooth\"%s set to \"rigid\"\n", g_global.tabs.c_str(), tracelinesStraightened, (tracelinesStraightened > 1 ? " Trace lines" : " Trace line"));
	
	if (tracelinesDeleted)
		printf("%s%zu%s deleted to accommodate deleted phrase bars\n", g_global.tabs.c_str(), tracelinesDeleted, (tracelinesDeleted > 1 ? " Trace lines" : " Trace line"));
	
	if (phrasesDeleted)
		printf("%s1 Phrase bar deleted for starting with a duration under 3000 samples\n", g_global.tabs.c_str());
	else if (phrasesDeleted)
		printf("%s%zu Phrase bars deleted for starting with durations under 3000 samples\n", g_global.tabs.c_str(), phrasesDeleted);
	
	if (phrasesShortened == 1)
		printf("%s1 Phrase Bar shortened for ending too close to a following Phrase Bar\n", g_global.tabs.c_str());
	else if (phrasesShortened)
		printf("%s%zu Phrase Bars shortened for ending too close to following Phrase Bars\n", g_global.tabs.c_str(), phrasesShortened);

	if (guardsDeleted == 1)
		printf("%s1 Guard Mark deleted for being within 1600 samples of a preceeding Guard Mark\n", g_global.tabs.c_str());
	else if (guardsDeleted)
		printf("%s%zu Guard Marks deleted for being within 1600 samples of preceeding Guard Marks\n", g_global.tabs.c_str(), guardsDeleted);
		
	if (tracelinesStraightened || tracelinesCurved || phrasesDeleted || phrasesShortened || guardsDeleted)
	{
		printf("%s\n%sChanges will be applied if you choose to save the file\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		m_saved = false;
	}
	else
		printf("%sNo changes made\n", g_global.tabs.c_str());
}

void CHC::PSPToPS2()
{
	GlobalFunctions::banner(" " + m_filename + ".CHC - Non-Duet Conversion ");
	bool player2 = false, enemy = false;
	do
	{
		printf("%sWhich players do you want to keep?\n", g_global.tabs.c_str());
		printf("%s1 - Player 1 & Enemy\n", g_global.tabs.c_str());
		printf("%s2 - Player 1 & Player 2\n", g_global.tabs.c_str());
		printf("%s3 - Player 2 & Enemy\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("123"))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Help:
			break;
		case GlobalFunctions::ResultType::Success:
			switch (g_global.answer.character)
			{
			case '3':
				player2 = true;
				__fallthrough;
			case '1':
				enemy = true;
			}
			m_filename += "_PS2_" + to_string(player2 + 1) + 'v' + (enemy ? 'E' : '2');
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;

	snprintf(m_imc, 44, "/PROJECTS/STDATA/STAGE%02d/SONGDATA/ST%02d.IMC", m_stage, m_stage);
	m_audio[2].pan = m_audio[3].pan = m_audio[6].volume = m_audio[7].volume = 0;
	m_audio[4].pan = m_audio[5].pan = 32767;
	bool* results = new bool[m_sections.size()]();
	std::condition_variable cv;
	std::mutex cv_m;
	auto convertToPS2 = [&](const size_t index)
	{
		SongSection& section = m_sections[index];
		bool doWait = !section.m_organized;
		bool reorganized = doWait ? section.reorganize(m_imc[0], m_stage) : false;

		// A B C D
		// to
		// A B C
		section.m_charts.erase(section.m_charts.begin() + 3ULL * section.m_numCharts, section.m_charts.begin() + 4ULL * section.m_numCharts);

		if (player2 || !enemy)
			// A B C
			// to
			// A C B
			std::swap_ranges(section.m_charts.begin() + 2ULL * section.m_numCharts,
							section.m_charts.begin() + 3ULL * section.m_numCharts,
							section.m_charts.begin() + section.m_numCharts);

		if (!player2)
			// A B C
			// to
			// A B
			section.m_charts.erase(section.m_charts.begin() + 2ULL * section.m_numCharts, section.m_charts.begin() + 3ULL * section.m_numCharts);
		else
			// A B C
			// to
			// B C
			section.m_charts.erase(section.m_charts.begin(), section.m_charts.begin() + section.m_numCharts);

		if (section.m_numCharts & 1)
		{
			section.m_charts.push_back(Chart());
			section.m_charts.insert(section.m_charts.begin() + section.m_numCharts, Chart());
			section.m_numCharts++;
		}

		{
			const size_t offsetA = section.m_charts.size() - 1;
			const size_t offSetB = section.m_numCharts - 1ULL;
			section.m_numCharts >>= 1;
			for (size_t c = 1; c < section.m_numCharts; c++)
			{
				auto beg = section.m_charts.rbegin();
				std::rotate(beg + (offsetA - c),
					beg + (offsetA - 2 * c),
					beg + (offsetA - 2 * c));

				beg = section.m_charts.rbegin();
				std::rotate(beg + (offSetB - c),
					beg + (offSetB - 2 * c),
					beg + (offSetB - 2 * c));
			}
		}
		
		// A B C D
		// to
		// A C B D
		std::swap_ranges(section.m_charts.begin() + 2ULL * section.m_numCharts,
						section.m_charts.begin() + 3ULL * section.m_numCharts,
						section.m_charts.begin() + section.m_numCharts);
		if (!enemy)
			section.m_swapped = (section.m_swapped >> 1) + 4;

		if (doWait)
		{
			if (index > 0)
			{
				std::unique_lock<std::mutex> lk(cv_m);
				cv.wait(lk, [&] {return results[index - 1]; });
			}

			if (reorganized)
				printf("%s%s organized - # of charts per player: %lu", g_global.tabs.c_str(), section.m_name, section.m_numCharts);
			else
				printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", g_global.tabs.c_str(), section.m_name);
		}
		results[index] = true;
		cv.notify_all();
	};

	std::vector<std::thread> threads;
	threads.reserve(m_sections.size());
	for (size_t sectIndex = 0; sectIndex < m_sections.size(); ++sectIndex)
		threads.push_back(std::thread(convertToPS2, sectIndex));

	for (auto& thr : threads)
		thr.join();

	for (size_t s = 0; s < 5; s++)
	{
		m_energyDamageFactors[0][s].attackInitial *= 2;
		m_energyDamageFactors[0][s].attackRelease *= 2;
		m_energyDamageFactors[0][s].attackMiss *= 2;
		m_energyDamageFactors[0][s].chargeInitial *= 2;
		m_energyDamageFactors[0][s].chargeRelease *= 2;
		m_energyDamageFactors[0][s].guardMiss *= 2;
	}
	m_saved = false;
	m_optimized = false;
}

void CHC::swapIMC()
{
	while (true)
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - IMC Swap ");
		printf("%sProvide the name (without the extension) of the .IMC file you wish to use (Or 'Q' to back out to Modify Menu)\n", g_global.tabs.c_str());
		printf("%sCurrent IMC File: %s\n", g_global.tabs.c_str(), m_imc);
		printf("%sInput: ", g_global.tabs.c_str());

		char newIMC[223];
		if (GlobalFunctions::charArrayInsertion(newIMC, 218) == GlobalFunctions::ResultType::Success)
		{
			do
			{
				printf("%sIs this correct?: /PROJECTS/STDATA/STAGE00/SONGDATA/%s [Y/N]\n", g_global.tabs.c_str(), newIMC);
				if (!strstr(newIMC, ".IMC"))
					printf("%s\".IMC\" will be added afterwards.\n", g_global.tabs.c_str());
				switch (GlobalFunctions::menuChoices("yn"))
				{
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'n')
					{
						g_global.quit = true;
						break;
					}
					else
					{
						string strIMC = newIMC;
						if (strIMC.find(".IMC") == string::npos)
							strIMC += ".IMC";
						std::copy(strIMC.begin(), strIMC.end(), m_imc + 34);
						m_imc[34 + strIMC.length()] = 0;
						m_saved = false;
					}
					__fallthrough;
				case GlobalFunctions::ResultType::Quit:
					return;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		else
			return;
	};
}

void CHC::audioSettings()
{
	while (true)
	{
		string choices;
		if (!m_imc[0])
		{
			GlobalFunctions::banner(" " + m_filename + ".CHC - Audio Channels ");
			printf("%s          ||        Volume        ||        Paning        ||\n", g_global.tabs.c_str());
			printf("%s Channels ||  Value  ||  Percent  ||   Left   |   Right   ||\n", g_global.tabs.c_str());
			printf("%s==========================================================||\n", g_global.tabs.c_str());
			for (size_t index = 0; index < 8; ++index)
			{
				printf("%s     %zu    ||", g_global.tabs.c_str(), index + 1);
				printf("%7lu  ||%7g%%   ||", m_audio[index].volume, m_audio[index].volume * 100.0f / 32767);
				switch (m_audio[index].pan)
				{
				case 0:     printf("   Left   |           ||\n"); break;
				case 16383: printf("        Center        ||\n"); break;
				case 32767: printf("          |   Right   ||\n"); break;
				default: printf("%8g%% |%8g%% ||\n", 100.0 - (double(m_audio[index].pan) * (100.0 / 32767)), double(m_audio[index].pan) * (100.0 / 32767));
				}
			}
			printf("%s==========================================================||\n", g_global.tabs.c_str());
			printf("%sChoose the channel you wish to edit [Type 'Q' to exit audio settings]\n", g_global.tabs.c_str());
			choices = "12345678";
		}
		else
		{
			GlobalFunctions::banner(" " + m_filename + ".CHC - Audio Channels [Duet Stages] ");
			printf("%s          ||        Volume        ||\n", g_global.tabs.c_str());
			printf("%s Channels ||  Value  ||  Percent  ||\n", g_global.tabs.c_str());
			printf("%s==================================||\n", g_global.tabs.c_str());
			printf("%s   1 & 2  ||        Unused        ||\n", g_global.tabs.c_str());
			for (size_t index = 2; index < 8; ++index)
			{
				printf("%s     %zu    ||", g_global.tabs.c_str(), index + 1);
				printf("%7lu  ||%7g%%   ||", m_audio[index].volume, m_audio[index].volume * 100.0f / 32767);
			}
			printf("%s==================================||\n", g_global.tabs.c_str());
			printf("%sChoose the channel you wish to edit [Type 'Q' to exit audio settings]\n", g_global.tabs.c_str());
			choices = "345678";
		}
		switch (GlobalFunctions::menuChoices(choices, true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Success:
		{
			size_t channel = g_global.answer.index;
			if (!m_imc[0])
				channel += 2;
			do
			{
				GlobalFunctions::banner(" " + m_filename + ".CHC - Audio Channel " + to_string(channel + 1));
				string choices = "vb";
				printf("%sV - Volume\n", g_global.tabs.c_str());
				if (!m_imc[0])
				{
					printf("%sP - Panning\n", g_global.tabs.c_str());
					choices += 'p';
				}
				printf("%sB - Choose Another Channel\n", g_global.tabs.c_str());
				printf("%sQ - Exit Audio Settings\n", g_global.tabs.c_str());
				switch (GlobalFunctions::menuChoices(choices))
				{
				case GlobalFunctions::ResultType::Quit:
					return;
				case GlobalFunctions::ResultType::Help:
					printf("%s\n", g_global.tabs.c_str());
					break;
				case GlobalFunctions::ResultType::Success:
					switch (g_global.answer.character)
					{
					case 'v':
						do
						{
							GlobalFunctions::banner(" " + m_filename + ".CHC - Audio Channel " + to_string(channel + 1) + " - Volume");
							printf("%sProvide value for Volume (0 - 32767[100%%]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n", g_global.tabs.c_str());
							printf("%sCurrent Volume: %lu (%g%%)\n", g_global.tabs.c_str(), m_audio[channel].volume, 100.0 * m_audio[channel].volume / 32767);
							printf("%sInput: ", g_global.tabs.c_str());
							unsigned long oldVol = m_audio[channel].volume;
							switch (GlobalFunctions::valueInsert(m_audio[channel].volume, false, 0UL, 32767UL, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (m_audio[channel].volume != oldVol)
									m_saved = false;
								break;
							case GlobalFunctions::ResultType::InvalidNegative:
							case GlobalFunctions::ResultType::MaxExceeded:
								printf("%sValue must be between 0 & 32767. Not adjusting.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
								GlobalFunctions::clearIn();
								break;
							case GlobalFunctions::ResultType::Failed:
								printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
								GlobalFunctions::clearIn();
							}
						} while (!g_global.quit);
						g_global.quit = false;
						break;
					case 'p':
						do
						{
							GlobalFunctions::banner(" " + m_filename + ".CHC - Audio Channel " + to_string(channel + 1) + " - Panning");
							printf("%sProvide value for Panning (Left[0] - Center[16383] - Right[32767]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n", g_global.tabs.c_str());
							printf("%sCurrent Panning: ", g_global.tabs.c_str());
							unsigned long oldPan = m_audio[channel].pan;
							switch (m_audio[channel].pan)
							{
							case 0: printf("Left (0)\n"); break;
							case 16383: printf("Center (16383)\n"); break;
							case 32767: printf("Right (32767)\n"); break;
							default: printf("%g%% Left | %g%% Right (%lu)\n", 100.0 - (double(m_audio[channel].pan) * (100.0 / 32767)), double(m_audio[channel].pan) * (100.0 / 32767), m_audio[channel].pan);
							}
							printf("%sInput: ", g_global.tabs.c_str());
							switch (GlobalFunctions::valueInsert(m_audio[channel].pan, false, 0UL, 32767UL, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (m_audio[channel].pan != oldPan)
									m_saved = false;
								break;
							case GlobalFunctions::ResultType::InvalidNegative:
							case GlobalFunctions::ResultType::MaxExceeded:
								printf("%sValue must be between 0 & 32767. Not adjusting.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
								GlobalFunctions::clearIn();
								break;
							case GlobalFunctions::ResultType::Failed:
								printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
								GlobalFunctions::clearIn();
							}
						} while (!g_global.quit);
						g_global.quit = false;
						break;
					case 'b':
						g_global.quit = true;
					}
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		}
	}
}

void CHC::winLossSettings()
{
	while (true)
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - Win/Loss Animations ");
		static const string ssqStrings[] = { "Win A", "Win B", "Lost Pre", "Lost Post" };
		printf("%s            ||       Frames       ||\n", g_global.tabs.c_str());
		printf("%s Animations ||  First  ||   Last  ||\n", g_global.tabs.c_str());
		printf("%s==================================||\n", g_global.tabs.c_str());
		printf("%s 1 - Win A  ||%7g  ||%7g  ||\n", g_global.tabs.c_str(), m_events[0].first, m_events[0].last);
		printf("%s 2 - Win B  ||%7g  ||%7g  ||\n", g_global.tabs.c_str(), m_events[1].first, m_events[1].last);
		printf("%s3 - Lost Pre||%7g  ||%7g  ||\n", g_global.tabs.c_str(), m_events[2].first, m_events[2].last);
		printf("%s4 - Lt. Loop||%7g  ||%7g  ||\n", g_global.tabs.c_str(), m_events[3].first, m_events[3].last);
		printf("%s\n\t\t      ||Select an SSQ event ('1'/'2'/'3'/'4') [Type 'Q' to exit win/loss anim. settings]\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("1234", true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Help:
			printf("%s\n", g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Success:
			do
			{
				GlobalFunctions::banner(" " + m_filename + ".CHC - SSQ Animation:" + ssqStrings[g_global.answer.index]);
				printf("%sF - Adjust first frame: %g\n", g_global.tabs.c_str(), m_events[g_global.answer.index].first );
				printf("%sL - Adjust last  frame: %g\n", g_global.tabs.c_str(), m_events[g_global.answer.index].last );
				printf("%sB - Choose different animation\n", g_global.tabs.c_str());
				printf("%sQ - Exit win/loss animation settings \n", g_global.tabs.c_str());
				switch (GlobalFunctions::menuChoices("flb"))
				{
				case GlobalFunctions::ResultType::Quit:
					return;
				case GlobalFunctions::ResultType::Help:
					printf("%s\n", g_global.tabs.c_str());
					break;
				case GlobalFunctions::ResultType::Success:
					switch (g_global.answer.character)
					{
					case 'b':
						g_global.quit = true;
						break;
					case 'f':
						do
						{
							GlobalFunctions::banner(" " + m_filename + ".CHC - SSQ Animation: " + ssqStrings[g_global.answer.index] + " - First Frame");
							printf("%sEnter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n", g_global.tabs.c_str());
							printf("%s(Can be a decimal)\n", g_global.tabs.c_str());
							float oldAnim = m_events[g_global.answer.index].first;
							switch (GlobalFunctions::valueInsert(m_events[g_global.answer.index].first, false, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (m_events[g_global.answer.index].first != oldAnim)
									m_saved = false;
								break;
							case GlobalFunctions::ResultType::InvalidNegative:
								printf("%sValue must be positive. Not adjusting.\n", g_global.tabs.c_str());
								GlobalFunctions::clearIn();
								break;
							case GlobalFunctions::ResultType::Failed:
								printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
								GlobalFunctions::clearIn();
							}
						} while (!g_global.quit);
						g_global.quit = false;
						break;
					case 'l':
						do
						{
							GlobalFunctions::banner(" " + m_filename + ".CHC - SSQ Animation: " + ssqStrings[g_global.answer.index] + " - Last Frame");
							printf("%sEnter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n", g_global.tabs.c_str());
							printf("%s(Can be a decimal)\n", g_global.tabs.c_str());
							float oldAnim = m_events[g_global.answer.index].last;
							switch (GlobalFunctions::valueInsert(m_events[g_global.answer.index].last, false, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (m_events[g_global.answer.index].last != oldAnim)
									m_saved = false;
								break;
							case GlobalFunctions::ResultType::InvalidNegative:
								printf("%sValue must be positive. Not adjusting.\n", g_global.tabs.c_str()); GlobalFunctions::clearIn();
								break;
							case GlobalFunctions::ResultType::Failed:
								printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
								GlobalFunctions::clearIn();
							}
						} while (!g_global.quit);
						g_global.quit = false;
					}
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
	}
}

void CHC::adjustSpeed()
{
	while (true)
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - Gameplay Speed Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit speed settings]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal... and/or negative with weird effects\n", g_global.tabs.c_str());
		printf("%sCurrent Value: %g\n", g_global.tabs.c_str(), m_speed );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldSpeed = m_speed;
		switch (GlobalFunctions::valueInsert(m_speed, true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (m_speed != oldSpeed)
				m_saved = false;
			break;
		case GlobalFunctions::ResultType::MaxExceeded:
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			GlobalFunctions::clearIn();
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void CHC::sectionMassMenu()
{
	do
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - Sections ");

		string choices = "psct";
		printf("%sP - Set Unconditional Play Order\n", g_global.tabs.c_str());
		if (m_sections.size() > 1)
		{
			printf("%sR - Rearrange Sections\n", g_global.tabs.c_str());
			choices += 'r';
		}
		printf("%sS - Swap Player 1 & Player 2 for All Sections\n", g_global.tabs.c_str());
		printf("%sC - Choose a Single Section to Edit\n", g_global.tabs.c_str());
		printf("%sT - Test Section Pathing\n", g_global.tabs.c_str());
		printf("%sQ - Back out to Modify Menu\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("prsct", true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Help:
			printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			__fallthrough;
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			++g_global;
			switch (g_global.answer.character)
			{
			case 'p': playOrder(); break;
			case 'r': rearrange(); break;
			case 's': playerSwapAll(); break;
			case 'c': sectionSelector(); break;
			case 't': testPathing(); break;
			}
			--g_global;
		}
	} while (!g_global.quit);
}

void CHC::playerSwapAll()
{
	GlobalFunctions::banner(" " + m_filename + ".CHC - Complete Player Swap");
	for (size_t sectIndex = 0; sectIndex < m_sections.size(); sectIndex++)
		m_sections[sectIndex].playerSwap(m_imc[0]);
	m_saved = false;
}

void CHC::playOrder()
{
	GlobalFunctions::banner(" " + m_filename + ".CHC - Play Order ");
	std::vector<size_t> sectionIndexes = GlobalFunctions::indexInsertionDialogue(m_sections
			, "Type the index for each section in the order you wish them to be played - w/ spaces in-between."
			, "Play Order Selection");

	if (sectionIndexes.size() == 0)
		return;

	printf("%s", g_global.tabs.c_str());
	for (size_t index = 0; index < sectionIndexes.size(); ++index)
	{
		if (index + 1 < sectionIndexes.size())
		{
			m_sections[sectionIndexes[index]].m_conditions.front().m_type = 0;
			m_sections[sectionIndexes[index]].m_conditions.front().m_trueEffect = (long)sectionIndexes[index + 1];
		}
		else if (!testPathing(sectionIndexes[index]))
		{
			m_sections[sectionIndexes[index]].m_conditions.front().m_type = 0;
			m_sections[sectionIndexes[index]].m_conditions.front().m_trueEffect = (long)m_sections.size();
		}

		printf("%s ", m_sections[sectionIndexes[index]].m_name);
		m_saved = false;
	}
	putchar('\n');
}

void CHC::rearrange()
{
	GlobalFunctions::banner(" " + m_filename + ".CHC - Section Rearrangement ");
	size_t startIndex = 0, numElements, position;
	do
	{
		printf("%sProvide the starting index for the range of sections you want to move.\n", g_global.tabs.c_str());
		for (size_t index = 0; index < m_sections.size(); ++index)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), index, m_sections[index].m_name );
		printf("%sInput: ", g_global.tabs.c_str());
		switch (GlobalFunctions::valueInsert(startIndex, false, size_t(0), m_sections.size() - 1))
		{
		case GlobalFunctions::ResultType::Success:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Quit:
			printf("%sSection rearrangement cancelled.\n", g_global.tabs.c_str());
			return;
		case GlobalFunctions::ResultType::InvalidNegative:
			printf("%sGiven value cannot be negative\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::MaxExceeded:
			printf("%sGiven value cannot be greater than %zu\n%s\n", g_global.tabs.c_str(), m_sections.size() - 1, g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			break;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	if (m_sections.size() - startIndex == 1)
		numElements = 1;
	else
	{
		do
		{
			printf("%sHow many elements do you want to move? [Max # of movable elements: %zu]\n", g_global.tabs.c_str(), m_sections.size() - startIndex);
			printf("%sInput: ", g_global.tabs.c_str());
			switch (GlobalFunctions::valueInsert(numElements, false, size_t(1), m_sections.size() - startIndex))
			{
			case GlobalFunctions::ResultType::Success:
				g_global.quit = true;
				break;
			case GlobalFunctions::ResultType::Quit:
				printf("%sSection rearrangement cancelled.\n", g_global.tabs.c_str());
				return;
			case GlobalFunctions::ResultType::InvalidNegative:
			case GlobalFunctions::ResultType::MinExceeded:
				printf("%sGiven value cannot be less than 1\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::MaxExceeded:
				printf("%sGiven value cannot be greater than %zu\n%s\n", g_global.tabs.c_str(), m_sections.size() - startIndex, g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::Failed:
				printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
				break;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}

	if (numElements < m_sections.size())
	{
		long alpha;
		if (m_sections.size() - numElements == 1)
		{
			if (!startIndex)
			{
				printf("%sMoving elements to the end of the list\n", g_global.tabs.c_str());
				position = m_sections.size();
				alpha = 1;
			}
			else
			{
				printf("%sMoving elements to the beginning of the list\n", g_global.tabs.c_str());
				position = 0;
				alpha = -1;
			}
		}
		else
		{
			do
			{
				printf("%sProvide the index that these elements will be moved to.\n", g_global.tabs.c_str());
				{
					for (size_t index = 0; index < startIndex; ++index)
						printf("%s%zu - %s\n", g_global.tabs.c_str(), index, m_sections[index].m_name);

					if (numElements > 1)
						printf("%sInvalid position range: %zu - %zu\n", g_global.tabs.c_str(), startIndex, startIndex + numElements);
					else
						printf("%sInvalid position: %zu\n", g_global.tabs.c_str(), startIndex);

					for (size_t index = startIndex + numElements; index <= m_sections.size(); ++index)
					{
						if (index < m_sections.size())
							printf("%s%zu - %s\n", g_global.tabs.c_str(), index, m_sections[index].m_name);
						else
							printf("%s%zu - End of the list\n", g_global.tabs.c_str(), m_sections.size());
					}
				}
				printf("%sInput: ", g_global.tabs.c_str());
				switch (GlobalFunctions::valueInsert(position, false, size_t(0), m_sections.size()))
				{
				case GlobalFunctions::ResultType::Success:
					if (position < startIndex)
					{
						alpha = long(position - startIndex);
						g_global.quit = true;
					}
					else if (position > startIndex + numElements)
					{
						alpha = long(position - (startIndex + numElements));
						g_global.quit = true;
					}
					else
						printf("%sCannot choose a value within the range of sections being moved\n", g_global.tabs.c_str());
					break;
				case GlobalFunctions::ResultType::Quit:
					printf("%sSection rearrangement cancelled.\n", g_global.tabs.c_str());
					return;
				case GlobalFunctions::ResultType::InvalidNegative:
					printf("%sGiven value cannot be negative\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
					break;
				case GlobalFunctions::ResultType::MaxExceeded:
					printf("%sGiven value cannot be greater than %zu\n%s\n", g_global.tabs.c_str(), m_sections.size(), g_global.tabs.c_str());
					break;
				case GlobalFunctions::ResultType::Failed:
					printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
					break;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		for (size_t sectIndex = 0; sectIndex < m_sections.size(); sectIndex++)
		{
			for (size_t condIndex = 0; condIndex < m_sections[sectIndex].m_conditions.size(); condIndex++)
			{
				long* effect = &m_sections[sectIndex].m_conditions[condIndex].m_trueEffect;
				for (size_t eff = 0; eff < 2; eff++, effect++)
				{
					if (*effect >= 0)
					{
						if (*effect < (long)startIndex)
						{
							if (*effect >= (long)position)
								*effect += (long)numElements;
						}
						else if (*effect < long(startIndex + numElements))
							*effect += alpha;
						else if (*effect < (long)position)
							*effect -= (long)numElements;
					}
				}
			}
		}

		std::swap_ranges(m_sections.begin() + startIndex, m_sections.begin() + startIndex + numElements, m_sections.begin() + position);
		for (size_t index = 0; index < m_sections.size(); ++index)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), index, m_sections[index].m_name );
	}
	else
		printf("%sReally? You didn't even do anything with that... *sigh*\n", g_global.tabs.c_str());
	printf("%s\n", g_global.tabs.c_str());
}

void CHC::adjustFactors()
{
	do
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - Damage/Energy Factors ", 1.53f);
		static const char* headers[] = { "||    Starting Energy   ||", "|| Initial-Press Energy ||", "|| Initial-Press Damage ||", "||   Guard Energy Gain  ||",
							 "||  Attack Miss Damage  ||", "||   Guard Miss Damage  ||", "|| Sustain Energy Coef. ||", "|| Sustain Damage Coef. ||" };
		for (size_t player = 0; player < 4; player += 2)
		{
			printf("%s        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||", g_global.tabs.c_str(), player + 1);
			printf("||        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n", player + 2);
			printf("%s%s||||%s||", g_global.tabs.c_str(), string(77, '=').c_str(), string(77, '=').c_str());
			for (size_t factor = 0; factor < 8; factor++)
			{
				printf("\n\t      ");
				for (size_t index = player; index < player + 2; ++index)
				{
					printf(headers[factor]);
					for (size_t phase = 0; phase < 5; phase++)
					{
						float* val = &m_energyDamageFactors[index][phase].initialEnergy + factor;
						printf("  %5g%% ||", (*val) * 100.0);
					}
				}
			}
			printf("\n%s%s||\n", g_global.tabs.c_str(), string(158, '=').c_str());
		}
		printf("%sSelect a player ('1'/'2'/'3'/'4') [Type 'Q' to exit factor settings]\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("1234", true))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Help:
			printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			__fallthrough;
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			const size_t player = g_global.answer.index;
			do
			{
				GlobalFunctions::banner(" " + m_filename + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " ");
				printf("%s        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n", g_global.tabs.c_str(), player + 1);
				printf("%s%s||", g_global.tabs.c_str(), string(77, '=').c_str());
				for (size_t factor = 0; factor < 8; factor++)
				{
					printf("\n\t      %s", headers[factor]);
					for (size_t phase = 0; phase < 5; phase++)
					{
						float* val = &m_energyDamageFactors[player][phase].initialEnergy + factor;
						printf("  %5g%% ||", (*val) * 100.0);
					}
				}
				printf("\n%s%s||\n", g_global.tabs.c_str(), string(77, '=').c_str());
				printf("%sSelect a phase ('I'/'C'/'B'/'H'/'E') [Type 'P' to choose a different player | 'Q' to exit factor settings]\n", g_global.tabs.c_str());
				const string phaseName[5] = { "Intro", "Charge", "Battle", "Harmony", "End" };
				switch (GlobalFunctions::menuChoices("icbhep", true))
				{
				case GlobalFunctions::ResultType::Quit:
					return;
				case GlobalFunctions::ResultType::Help:
					printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
					__fallthrough;
				case GlobalFunctions::ResultType::Failed:
					break;
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.index == 5)
					{
						g_global.quit = true;
						break;
					}
					else
					{
						//const size_t phase = g_global.answer.index;
						const string& phase = phaseName[g_global.answer.index];
						const unsigned nameLength = (unsigned)phaseName[g_global.answer.index].length();
						auto& factors = m_energyDamageFactors[player][g_global.answer.index];
						do
						{
							GlobalFunctions::banner(" " + m_filename + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " | " + phase + " ");
							printf("%s          Player %zu        || %*s ||\n", g_global.tabs.c_str(), player + 1, nameLength + 1, phase.c_str());
							printf("%s%s||\n", g_global.tabs.c_str(), string(31 + phase.length(), '=').c_str());
							printf("%s 1 - Starting Energy      || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.initialEnergy * 100.0);
							printf("%s 2 - Initial-Press Energy || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.chargeInitial * 100.0);
							printf("%s 3 - Initial-Press Damage || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.attackInitial * 100.0);
							printf("%s 4 - Guard Energy Gain    || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.guardEnergy * 100.0);
							printf("%s 5 - Attack Miss Damage   || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.attackMiss * 100.0);
							printf("%s 6 - Guard  Miss Damage   || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.guardMiss * 100.0);
							printf("%s 7 - Sustain Energy Coef. || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.chargeRelease * 100.0);
							printf("%s 8 - Sustain Damage Coef. || %*g%% ||\n", g_global.tabs.c_str(), nameLength, factors.attackRelease * 100.0);
							printf("%s%s||\n", g_global.tabs.c_str(), string(31ULL + nameLength, '=').c_str());
							printf("%sSelect a factor by number [Type 'B' to choose a different phase | 'Q' to exit factor settings]\n", g_global.tabs.c_str());
							switch (GlobalFunctions::menuChoices("12345678b", true))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::Help:
								printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
								__fallthrough;
							case GlobalFunctions::ResultType::Failed:
								break;
							case GlobalFunctions::ResultType::Success:
								if (g_global.answer.index == 8)
								{
									g_global.quit = true;
									break;
								}
								else
								{
									static const char* bannerEnds[] = { " | Starting Energy ", " | Initial-Press Energy ", " | Initial-Press Damage ", " | Guard Energy Gain ",
																		" | Attack Miss Damage ", " | Guard Miss Damage ", " | Sustain Energy Coef. ", " | Sustain Damage Coef. " };
									do
									{
										GlobalFunctions::banner(" " + m_filename + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) +
											" | " + phase + bannerEnds[g_global.answer.index]);
										float* val = &factors.initialEnergy + g_global.answer.index;

										printf("%s        Player %zu      || %s ||\n", g_global.tabs.c_str(), player + 1, phase.c_str());
										printf("%s%s||\n\t\t  ", g_global.tabs.c_str(), string(26 + phase.length(), '=').c_str());
										printf("%s %*g ||\n", headers[g_global.answer.index], (unsigned)phase.length(), *val);
										printf("%s%s||\n", g_global.tabs.c_str(), string(26 + phase.length(), '=').c_str());
										printf("%sProvide a new value for this factor [Type 'B' to choose a different factor | 'Q' to exit factor settings]\n", g_global.tabs.c_str());
										printf("%sCan be a decimal... and/or negative with weird effects\n", g_global.tabs.c_str());
										printf("%sInput: ", g_global.tabs.c_str());
										float oldFac = *val;
										switch (GlobalFunctions::valueInsert(*val, true, 0.0f, 0.0f, "b"))
										{
										case GlobalFunctions::ResultType::SpecialCase:
											g_global.quit = true;
											break;
										case GlobalFunctions::ResultType::Success:
											if (*val != oldFac)
												m_saved = false;
											break;
										case GlobalFunctions::ResultType::Quit:
											return;
										case GlobalFunctions::ResultType::Failed:
											printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
											GlobalFunctions::clearIn();
										}
									} while (!g_global.quit);
									g_global.quit = false;
								}
							}
						} while (!g_global.quit);
						g_global.quit = false;
					}
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

bool CHC::testPathing(const size_t index)
{
	GlobalFunctions::banner(" " + m_filename + ".CHC - Section Transversal ");
	const size_t size = m_sections.size();
	bool* isAccessible = new bool[size]();
	bool* canReachEnd = new bool[size]();
	
	for (size_t sectIndex = 0; sectIndex < size; ++sectIndex)
		sectionPathTest(sectIndex, isAccessible, canReachEnd);

	bool* hasBeenTested = new bool[size]();
	memcpy(hasBeenTested, isAccessible, size);

	for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
		if (!isAccessible[sectIndex])
			sectionPathTest(sectIndex, hasBeenTested, canReachEnd);

	printf("%s%s||\n", g_global.tabs.c_str(), string(40, '=').c_str());
	printf("%s Section || Accessible? || Can Reach End? ||\n", g_global.tabs.c_str());
	for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
	{
		printf("%s%8s ||      %c      ||        %c       ||\n"
			, g_global.tabs.c_str()
			, m_sections[sectIndex].m_name
			, isAccessible[sectIndex] ? 'Y' : 'N'
			, canReachEnd[sectIndex]  ? 'Y' : 'N');
	}
	printf("%s%s||\n", g_global.tabs.c_str(), string(40, '=').c_str());
	
	const bool res = canReachEnd[index];
	delete[size] isAccessible;
	delete[size] canReachEnd;
	return res;
}

bool CHC::sectionPathTest(const size_t index, bool* isAccessible, bool* canReachEnd)
{
	if (!canReachEnd[index] && !isAccessible[index])
	{
		isAccessible[index] = true;
		for (auto& cond : m_sections[index].m_conditions)
		{
			if (cond.m_trueEffect >= 0)
				if (cond.m_trueEffect >= (long)m_sections.size() || sectionPathTest(cond.m_trueEffect, isAccessible, canReachEnd))
					canReachEnd[index] = true;

			if (cond.m_type && cond.m_falseEffect >= 0)
				if (cond.m_falseEffect >= (long)m_sections.size() || sectionPathTest(cond.m_falseEffect, isAccessible, canReachEnd))
					canReachEnd[index] = true;
		}
	}
	return canReachEnd[index];
};

void CHC::sectionSelector()
{
	do
	{
		GlobalFunctions::banner(" " + m_filename + ".CHC - Section Selection");
		switch (GlobalFunctions::indexSelector(m_sections, "section"))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Success:
			++g_global;
			m_sections[g_global.answer.index].menu(m_imc[0], m_stage);
			--g_global;
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void SongSection::menu(const bool isPs2, const int stage)
{
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - Modify ");
		string choices = "nafptds"; //ENABLE CONDITION OPTION
		printf("%sN - Name\n", g_global.tabs.c_str());
		printf("%sA - Change the section of audio used: %s\n", g_global.tabs.c_str(), m_audio);
		printf("%sF - Adjust SSQ frame range\n", g_global.tabs.c_str());
		switch (m_battlePhase)
		{
		case SongSection::Phase::INTRO:
			printf("%sP - Phase: INTRO\n", g_global.tabs.c_str()); break;
		case SongSection::Phase::CHARGE:
			printf("%sP - Phase: CHARGE\n", g_global.tabs.c_str()); break;
		case SongSection::Phase::BATTLE:
			printf("%sP - Phase: BATTLE\n", g_global.tabs.c_str());  break;
		case SongSection::Phase::FINAL_AG:
			printf("%sP - Phase: FINAL_AG\n", g_global.tabs.c_str());  break;
		case SongSection::Phase::HARMONY:
			printf("%sP - Phase: HARMONY\n", g_global.tabs.c_str()); break;
		case SongSection::Phase::END:
			printf("%sP - Phase: \n", g_global.tabs.c_str());  break;
		default:
			printf("%sP - Phase: FINAL_I\n", g_global.tabs.c_str());
		}
		printf("%sT - Tempo: %g\n", g_global.tabs.c_str(), m_tempo);
		printf("%sD - Duration: %lu\n", g_global.tabs.c_str(), m_duration);
		//printf("%s", g_global.tabs.c_str(), "C - Modify Conditions (", numConditions << ")\n";
		if (!m_organized)
		{
			printf("%sO - Organize this section\n", g_global.tabs.c_str());
			choices += 'o';
		}
		printf("%sS - Swap players\n", g_global.tabs.c_str());
		printf("%sQ - Choose another section\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Help:
			printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Success:
			++g_global;
			switch (g_global.answer.character)
			{
			case 'n':
				changeName();
				break;
			case 'a':
				changeAudio();
				break;
			case 'f':
				changeFrames();
				break;
			case 'p':
				changeFrames();
				break;
			case 't':
				adjustTempo();
				break;
			case 'd':
				adjustDuration();
				break;
			case 'c':
				conditionMenu();
				break;
			case 'o':
				GlobalFunctions::banner(" " + string(m_name) + " - Organize ");
				if (reorganize(isPs2, stage))
				{
					printf("%s%s organized - # of charts per player: %lu", g_global.tabs.c_str(), m_name, m_numCharts);
					if (isPs2)
					{
						size_t totals[2] = { 0 };
						for (size_t pl = 0; pl < 4; ++pl)
							for (size_t ch = 0, index = pl * m_numCharts; ch < m_numCharts; ++index, ++ch)
								if (m_charts[index].m_guards.size() || m_charts[index].m_phrases.size() || m_charts[index].m_tracelines.size() <= 1)
									++totals[pl & 1];
						printf(" (Pair total: %zu)\n", totals[0] >= totals[1] ? totals[0] : totals[1]);
					}
					else
						putchar('\n');
				}
				break;
			case 's':
				playerSwap(isPs2);
				m_parent->m_saved = false;
			}
			--g_global;
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

bool SongSection::reorganize(const bool isPs2, const int stage)
{
	bool ret = true;
	if (m_battlePhase == SongSection::Phase::CHARGE || (m_battlePhase == SongSection::Phase::BATTLE && !strstr(m_name, "BRK") && !strstr(m_name, "BREAK")))
	{
		struct OrderedNote
		{
			long position = 0;
			Note* note = nullptr;
			bool operator<(const OrderedNote& note) { return position < note.position; }
		};
		std::list<OrderedNote> notes[4];
		std::vector<Chart> newCharts[4];
		for (size_t pl = 0; pl < 4; pl++)
		{
			size_t currentPlayer;
			if (isPs2) currentPlayer = pl & 1;
			else currentPlayer = pl;
			auto& player = notes[currentPlayer];
			for (size_t chIndex = 0; chIndex < m_numCharts; chIndex++)
			{
				size_t playerIndex, chartIndex;
				if (isPs2)
				{
					playerIndex = ((2 * ((pl >> 1) ^ (chIndex & 1))) + (pl & 1)) * m_numCharts;
					chartIndex = (m_numCharts >> 1) * (pl >> 1) + (((m_numCharts & 1) * (pl >> 1) + chIndex) >> 1);
				}
				else
				{
					playerIndex = pl * m_numCharts;
					chartIndex = chIndex;
				}
				Chart& ch = m_charts[(size_t)playerIndex + chartIndex];
				if (m_swapped >= 4
					|| ((!(pl & 1)) != (isPs2 && m_swapped & 1)))
				{
					for (auto& guard : ch.m_guards)
					{
						OrderedNote grd{ guard.m_pivotAlpha + ch.getPivotTime(), &guard };
						player.insert(std::lower_bound(player.begin(), player.end(), grd), grd);
					}

					if (ch.m_tracelines.size() > 1)
					{
						for (auto& phrase : ch.m_phrases)
						{
							OrderedNote phr{ phrase.m_pivotAlpha + ch.getPivotTime(), &phrase };
							player.insert(std::lower_bound(player.begin(), player.end(), phr), phr);
						}
					
						for (auto& trace : ch.m_tracelines)
						{
							OrderedNote trl{ trace.m_pivotAlpha + ch.getPivotTime(), &trace };
							player.insert(std::lower_bound(player.begin(), player.end(), trl), trl);
						}
					}
				}
				else
				{
					if (ch.m_tracelines.size() > 1)
					{
						for (auto& phrase : ch.m_phrases)
						{
							OrderedNote phr{ phrase.m_pivotAlpha + ch.getPivotTime(), &phrase };
							player.insert(std::lower_bound(player.begin(), player.end(), phr), phr);
						}

						for (auto& trace : ch.m_tracelines)
						{
							OrderedNote trl{ trace.m_pivotAlpha + ch.getPivotTime(), &trace };
							player.insert(std::lower_bound(player.begin(), player.end(), trl), trl);
						}
					}
					
					for (auto& guard : ch.m_guards)
					{
						OrderedNote grd{ guard.m_pivotAlpha + ch.getPivotTime(), &guard };
						player.insert(std::lower_bound(player.begin(), player.end(), grd), grd);
					}
				}
			}
		}
		const float SAMPLES_PER_BEAT = s_SAMPLES_PER_MIN / m_tempo;

		for (size_t player = 0; player < (isPs2 ? 2U : 4U); player++)
		{
			size_t currentPlayer = player;
			Chart* currentChart = nullptr;
			bool isPlayer = !(player & 1) || m_swapped >= 4;
			bool makeNewChart = true;
			for (auto ntIndex = notes[player].begin(); ntIndex != notes[player].end();)
			{
				if (makeNewChart)
				{
					if (ntIndex != notes[player].begin()
						&& isPs2
						&& stage != 0)
						currentPlayer ^= 2;

					newCharts[currentPlayer].push_back({ false });
					currentChart = &newCharts[currentPlayer].back();

					if (ntIndex->position >= 2 * SAMPLES_PER_BEAT)
						currentChart->setPivotTime((long)roundf(ntIndex->position - SAMPLES_PER_BEAT));
					else
						currentChart->setPivotTime(ntIndex->position >> 1);
					makeNewChart = false;
				}

				// Adjustment of the current note's pivot alpha to line up with this chart
				ntIndex->note->m_pivotAlpha = ntIndex->position - currentChart->getPivotTime();
				currentChart->add(ntIndex->note);

				auto prevIndex = ntIndex++;
				if (ntIndex != notes[player].end())
				{
					if (dynamic_cast<Path*>(prevIndex->note) != nullptr)
					{
						Path* pt = static_cast<Path*>(prevIndex->note);
						// If the next note is a guard mark
						if (dynamic_cast<Guard*>(ntIndex->note) != nullptr)
						{
							// If the note is a phrase bar or if the guard mark lies inside the note 
							if (dynamic_cast<Phrase*>(pt) != nullptr
								|| prevIndex->position + (long)pt->m_duration > ntIndex->position)
							{
								prevIndex = ntIndex++;
								// If there's another note after the guard mark
								if (ntIndex != notes[player].end())
								{
									// If said note is a Trace line and has a duration of 1, delete it so it can be replaced
									Traceline* tr2 = dynamic_cast<Traceline*>(ntIndex->note);
									if (tr2 != nullptr && tr2->m_duration == 1)
										notes[player].erase(ntIndex++);
								}

								// If it's the enemy's original charts
								// or if the next-next note, if it exists, is a guard mark
								if ((isPlayer == (isPs2 && m_swapped & 1))
									|| (ntIndex != notes[player].end() && dynamic_cast<Guard*>(ntIndex->note) != nullptr))
								{
									long endAlpha = ntIndex->position - SongSection::s_SAMPLE_GAP - 1 - currentChart->getPivotTime();
									float angle = 0;

									while (currentChart->m_tracelines.size() && !currentChart->m_tracelines.back().changeEndAlpha(endAlpha))
									{
										// Save angle value
										angle = currentChart->m_tracelines.back().m_angle;
										// Remove trace line
										if (currentChart->removeTraceline(currentChart->getNumTracelines() - 1))
											printf("%s%s: Trace line %zu removed\n", g_global.tabs.c_str(), m_name, currentChart->getNumTracelines());
									}

									if (currentChart->m_tracelines.size())
									{
										const auto& tr = currentChart->m_tracelines.back();
										size_t phraseIndex = currentChart->getNumPhrases();

										// If the phrase bar is past the trace line
										while (currentChart->m_phrases.size() && tr.getEndAlpha() <= currentChart->m_phrases.back().m_pivotAlpha)
										{
											currentChart->m_phrases.pop_back();
											printf("%s%s: Phrase bar %zu removed\n", g_global.tabs.c_str(), m_name, currentChart->getNumPhrases());
										}

										if (currentChart->getNumPhrases())
										{
											Phrase& phr = currentChart->m_phrases.back();
											phr.m_end = true;

											// Check for safety that the PB is in THIS particular trace line
											if (tr <= phr)
												phr.changeEndAlpha(endAlpha);
										}

										currentChart->emplaceTraceline(endAlpha, 1, angle);
									}
									else
										currentChart->m_phrases.clear();

									if (isPlayer != (isPs2 && m_swapped & 1))
									{
										currentChart->setEndTime(ntIndex->position - SongSection::s_SAMPLE_GAP);
										makeNewChart = true;
									}
									ntIndex = prevIndex;
								}
								else
									notes[player].erase(prevIndex);
							}
							else if (isPlayer != (isPs2 && m_swapped & 1))
							{
								currentChart->setEndTime(prevIndex->position + pt->m_duration);
								makeNewChart = true;
							}
						}
						// If the current note is a trace line
						else if (dynamic_cast<Traceline*>(pt) != nullptr)
						{
							Traceline* tr = static_cast<Traceline*>(prevIndex->note);
							// If the next note is a trace line
							if (dynamic_cast<Traceline*>(ntIndex->note) != nullptr)
							{
								// If the trace lines are disconnected
								if (prevIndex->position + (long)tr->m_duration != ntIndex->position)
								{
									currentChart->setEndTime(prevIndex->position + tr->m_duration);
									makeNewChart = true;
								}
							}
							// If the phrase bar is outside the trace line
							else if (prevIndex->position + (long)tr->m_duration <= ntIndex->position)
								notes[player].erase(ntIndex++);
						}
					}
					// Note is a Guard Mark
					else
					{
						bool adjustPivot = false;
						// If the next note is a trace line
						if (dynamic_cast<Traceline*>(ntIndex->note) != nullptr)
						{
							// ntIndex = next
							prevIndex = ntIndex++;
							// If the next-next note is a guard mark
							if (dynamic_cast<Guard*>(ntIndex->note) != nullptr)
							{
								// Either move the trace line backwards so that it starts after the guard mark
								// or remove the trace line altogether
								Traceline* tr = static_cast<Traceline*>(prevIndex->note);
								// ntIndex = next x 2
								auto next = ntIndex++;
								if (ntIndex != notes[player].end())
								{
									long newFirst = (next->position + ntIndex->position) >> 1;
									if (tr->changePivotAlpha(tr->m_pivotAlpha + newFirst - prevIndex->position))
									{
										prevIndex->position = newFirst;
										std::iter_swap(prevIndex, next);
									}
									else
										notes[player].erase(prevIndex++);
								}
								else
									notes[player].erase(prevIndex++);

								ntIndex = prevIndex--;
								// Now the next note is confirmed a guard mark, so the next dynamic_cast<Guard*>
								// will return true
							}
							// If it's the enemy's original charts
							else
							{
								// ntIndex = ntIndex
								ntIndex = prevIndex--;
								if (isPlayer == (isPs2 && m_swapped & 1))
								{
									currentChart->setEndTime((ntIndex->position + prevIndex->position) >> 1);
									makeNewChart = true;
								}
								else
								{
									// Move chartPivot in between these two notes
									// Calculate the value adjustment
									long pivotDifference = ((ntIndex->position + prevIndex->position) >> 1) - currentChart->getPivotTime();
									// Adjust the chartAlpha
									currentChart->adjustPivotTime(pivotDifference);
									// Adjust the pivot alphas of inserted notes
									for (size_t grdIndex = 0; grdIndex < currentChart->m_guards.size(); grdIndex++)
										currentChart->m_guards[grdIndex].adjustPivotAlpha(-pivotDifference);
								}
							}
						}
						// A guard mark followed by a phrase bar is an error
						else if (dynamic_cast<Phrase*>(ntIndex->note) != nullptr)
							notes[player].erase(ntIndex++);

						// If the next note is a guard mark
						// AND if there is enough distance between these guard marks in a duet or tutorial stage
						if (dynamic_cast<Guard*>(ntIndex->note) != nullptr
							&& (!isPs2 || stage == 0 || stage == 11 || stage == 12)
							&& ntIndex->position - prevIndex->position >= long(5.5 * SAMPLES_PER_BEAT))
						{
							currentChart->setEndTime(prevIndex->position + long(SAMPLES_PER_BEAT));
							makeNewChart = true;

							if (!currentChart->m_tracelines.size())
							{
								// Move chartPivot in between these two notes
								// Calculate the value adjustment
								long pivotDifference = ((ntIndex->position + prevIndex->position) >> 1) - currentChart->getPivotTime();
								// Adjust the chartAlpha
								currentChart->adjustPivotTime(pivotDifference);
								// Adjust the pivot alphas of inserted notes
								for (size_t grdIndex = 0; grdIndex < currentChart->m_guards.size(); grdIndex++)
									currentChart->m_guards[grdIndex].adjustPivotAlpha(-pivotDifference);
							}
						}
					}
				}
				else if (dynamic_cast<Path*>(prevIndex->note) != nullptr)
					currentChart->setEndTime(prevIndex->position + static_cast<Path*>(prevIndex->note)->m_duration);
				else
					currentChart->setEndTime(prevIndex->position);
			}
		}

		m_numCharts = 1;
		//Finds the proper new value for "m_numcharts"
		for (size_t pl = 0; pl < 4; pl++)
		{
			size_t perChart = newCharts[pl].size();
			for (size_t ch = 0; ch < newCharts[pl].size() && perChart > m_numCharts; ch++)
				if (!(newCharts[pl][ch].m_guards.size() || newCharts[pl][ch].m_phrases.size())
					&& newCharts[pl][ch].m_tracelines.size() <= 1)
					perChart--;

			if (perChart > m_numCharts)
				m_numCharts = (unsigned long)perChart;
		}

		m_charts.clear();
		for (unsigned pl = 0; pl < 4; pl++)
		{
			while (newCharts[pl].size() > m_numCharts)
				newCharts[pl].pop_back();
			while (newCharts[pl].size() < m_numCharts)
				newCharts[pl].push_back({});
			for (size_t i = 0; i < newCharts[pl].size(); i++)
				m_charts.push_back(newCharts[pl][i]);
		}
		m_parent->m_saved = false;
	}
	else
		ret = false;

	setOrganized(true);
	return ret;
}

void SongSection::playerSwap(const bool isPs2)
{
	GlobalFunctions::banner(" " + string(m_name) + " - Player Swap ");
	if (isPs2)
	{
		if (m_battlePhase != SongSection::Phase::HARMONY && m_battlePhase != SongSection::Phase::END)
		{
			std::swap_ranges(m_charts.begin(), m_charts.begin() + m_numCharts, m_charts.begin() + m_numCharts);
			std::swap_ranges(m_charts.begin() + 2ULL * m_numCharts, m_charts.begin() + 3ULL * m_numCharts, m_charts.begin() + 3ULL * m_numCharts);

			if (m_swapped < 2)
				printf("%s%s: %s\n", g_global.tabs.c_str(), m_name, !(m_swapped & 1) ? "P1/P3 -> P2/P4" : "P2/P4 -> P1/P3");
			else if (m_swapped < 4)
				printf("%s%s: %s\n", g_global.tabs.c_str(), m_name, !(m_swapped & 1) ? "P3/P1 -> P4/P2" : "P4/P2 -> P3/P1");
			else if (m_swapped < 6)
				printf("%s%s: %s (Duet->PS2 conversion)\n", g_global.tabs.c_str(), m_name, !(m_swapped & 1) ? "P1/P3 -> P2D/P4D" : "P2D/P4D -> P1/P3");
			else
				printf("%s%s: %s (Duet->PS2 conversion)\n", g_global.tabs.c_str(), m_name, !(m_swapped & 1) ? "P3/P1 -> P4D/P2D" : "P4D/P2D -> P3/P1");
			m_swapped ^= 1;
		}
		else
		{
			do
			{
				printf("%sHow do you want to swap %s?\n", g_global.tabs.c_str(), m_name);
				printf("%sA - Swap P1/P3 with P2/P4 respectively\n", g_global.tabs.c_str());
				printf("%sB - Swap P1/P2 with P3/P4 respectively\n", g_global.tabs.c_str());
				printf("%sCurrent Format: ", g_global.tabs.c_str());
				switch (m_swapped)
				{
				case 0: printf("P1/P2/P3/P4\n"); break;
				case 1: printf("P2/P1/P4/P3\n"); break;
				case 2: printf("P3/P4/P1/P2\n"); break;
				case 3: printf("P4/P3/P2/P1\n");
				}

				switch (GlobalFunctions::menuChoices("ab"))
				{
				case GlobalFunctions::ResultType::Quit:
					return;
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'a')
					{
						std::swap_ranges(m_charts.begin(), m_charts.begin() + m_numCharts, m_charts.begin() + m_numCharts);
						std::swap_ranges(m_charts.begin() + 2ULL * m_numCharts, m_charts.begin() + 3ULL * m_numCharts, m_charts.begin() + 3ULL * m_numCharts);

						if (!(m_swapped & 2))
							printf("%s%s: %s\n", g_global.tabs.c_str(), m_name, !(m_swapped & 1) ? "P1/P3 -> P2/P4" : "P2/P4 -> P1/P3");
						else
							printf("%s%s: %s\n", g_global.tabs.c_str(), m_name, !(m_swapped & 1) ? "P3/P1 -> P4/P2" : "P4/P2 -> P3/P1");

						m_swapped ^= 1;
					}
					else
					{
						std::swap_ranges(m_charts.begin(), m_charts.begin() + 2ULL * m_numCharts, m_charts.begin() + 2ULL * m_numCharts);

						if (!(m_swapped & 1))
							printf("%s%s: %s\n", g_global.tabs.c_str(), m_name, !(m_swapped & 2) ? "P1/P2 -> P3/P4" : "P3/P4 -> P1/P2");
						else
							printf("%s%s: %s\n", g_global.tabs.c_str(), m_name, !(m_swapped & 2) ? "P2/P1 -> P4/P3" : "P4/P3 -> P2/P1");
						m_swapped ^= 2;
					}
					g_global.quit = true;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
	}
	else
	{
		std::swap_ranges(m_charts.begin(), m_charts.begin() + m_numCharts, m_charts.begin() + 2ULL * m_numCharts);
		
		if (!m_swapped)
		{
			printf("%s%s: P1 -> P3\n", g_global.tabs.c_str(), m_name);
			m_swapped = 2;
		}
		else
		{
			printf("%s%s: P3 -> P1\n", g_global.tabs.c_str(), m_name);
			m_swapped = 0;
		}
	}
}

void SongSection::changeName()
{
	GlobalFunctions::banner(" " + string(m_name) + " - Name Change ");
	printf("%sProvide a new name for this section (16 character max) ('Q' to back out to the Section Menu)\n", g_global.tabs.c_str());
	printf("%sInput: ", g_global.tabs.c_str());
	char newName[17];
	if (GlobalFunctions::charArrayInsertion(newName, 16) == GlobalFunctions::ResultType::Success)
	{
		if (strcmp(m_name, newName))
		{
			memcpy_s(m_name, 17, newName, 16);
			m_parent->m_saved = false;
		}
	}
}

void SongSection::changeAudio()
{
	GlobalFunctions::banner(" " + string(m_name) + " - Audio Change ");
	printf("%sProvide the name for the section of audio you want to use (16 character max) ('Q' to back out to the Section Menu)\n", g_global.tabs.c_str());
	printf("%sInput: ", g_global.tabs.c_str());
	char newAudio[17];
	if (GlobalFunctions::charArrayInsertion(newAudio, 16) == GlobalFunctions::ResultType::Success)
	{
		if (strcmp(m_audio, newAudio))
		{
			memcpy_s(m_audio, 17, newAudio, 16);
			m_parent->m_saved = false;
		}
	}
}

void SongSection::changeFrames()
{
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - SSQ Starting Index ");
		printf("%sProvide a value for the starting SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal\n", g_global.tabs.c_str());
		printf("%sCurrent Value for Starting Index: %g\n", g_global.tabs.c_str(), m_frames.first );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldFirst = m_frames.first;
		switch (GlobalFunctions::valueInsert(m_frames.first, false, "u"))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (m_frames.first != oldFirst)
				m_parent->m_saved = false;
			__fallthrough;
		case GlobalFunctions::ResultType::SpecialCase:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::InvalidNegative:
			printf("%sProvided value *must* be a zero or greater.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			GlobalFunctions::clearIn();
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%s\n%sStarting index saved.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str(), g_global.tabs.c_str());
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - SSQ Ending Index ");
		printf("%sProvide a value for the Ending SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal\n", g_global.tabs.c_str());
		printf("%sCurrent Value for Ending Index: %g\n", g_global.tabs.c_str(), m_frames.last );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldLast = m_frames.last;
		switch (GlobalFunctions::valueInsert(m_frames.last, false, "u"))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (m_frames.last != oldLast)
				m_parent->m_saved = false;
			__fallthrough;
		case GlobalFunctions::ResultType::SpecialCase:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::InvalidNegative:
			printf("%sProvided value *must* be a zero or greater.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			GlobalFunctions::clearIn();
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%s\n%sEnding index saved.\n", g_global.tabs.c_str(), g_global.tabs.c_str());
}

void SongSection::switchPhase()
{
	string choices = "";
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - Phase Selection ");
		printf("%sSelect a phase type for this section\n", g_global.tabs.c_str());
		printf("%sCurrent Phase: ", g_global.tabs.c_str());
		switch (m_battlePhase)
		{
		case SongSection::Phase::INTRO: printf("INTRO\n"); break;
		case SongSection::Phase::CHARGE: printf("CHARGE\n"); break;
		case SongSection::Phase::BATTLE: printf("BATTLE\n");  break;
		case SongSection::Phase::FINAL_AG: printf("FINAL_AG\n");  break;
		case SongSection::Phase::HARMONY: printf("HARMONY\n"); break;
		case SongSection::Phase::END: printf("END\n");  break;
		default: printf("FINAL_I\n");
		}
		printf("%sI/0 - Intro\n", g_global.tabs.c_str());
		printf("%sC/1 - Charge\n", g_global.tabs.c_str());
		printf("%sB/2 - Battle\n", g_global.tabs.c_str());
		printf("%sH/4 - Harmony\n", g_global.tabs.c_str());
		printf("%sE/5 - End\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("i0c1b2h4e5", true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Help:
			printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Failed:
			g_global.quit = true;
			break;
		default:
			g_global.answer.index >>= 1;
			if (g_global.answer.index > 2)
				++g_global.answer.index;

			if (m_battlePhase != static_cast<SongSection::Phase>(g_global.answer.index))
			{
				m_battlePhase = static_cast<SongSection::Phase>(g_global.answer.index);
				m_parent->m_saved = false;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void SongSection::adjustTempo()
{
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - Tempo Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit tempo settings]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal... and/or negative with weird effects on PSP\n", g_global.tabs.c_str());
		printf("%sCurrent Value: %g\n", g_global.tabs.c_str(), m_tempo );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldTempo = m_tempo;
		switch (GlobalFunctions::valueInsert(m_tempo, true))
		{
		case GlobalFunctions::ResultType::Success:
			if (m_tempo != oldTempo)
				m_parent->m_saved = false;
			break;
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			GlobalFunctions::clearIn();
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void SongSection::adjustDuration()
{
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - Duration Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit duration settings]\n", g_global.tabs.c_str());
		printf("%sCurrent Value: %lu\n", g_global.tabs.c_str(), m_duration );
		printf("%sInput: ", g_global.tabs.c_str());
		unsigned long oldDuration = m_duration;
		switch (GlobalFunctions::valueInsert(m_duration, false))
		{
		case GlobalFunctions::ResultType::Success:
			if (m_duration != oldDuration)
				m_parent->m_saved = false;
			break;
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::InvalidNegative:
			printf("%sProvided value *must* be a zero or greater.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			GlobalFunctions::clearIn();
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

/*
void SongSection::conditionMenu()
{
	do
	{
		GlobalFunctions::banner(" " + string(m_name) + " - Condition Selection ", 1.5);
		printf(" i ||Condition Type || Argument ||      True Effect      ||      False Effect     ||\n");
		printf("==================================================================================||\n");
		for (size_t condIndex = 0; condIndex < m_conditions.size(); condIndex++)
		{
			printf("%s %zu || ", g_global.tabs.c_str(), condIndex);
			switch (m_conditions[condIndex].m_type)
			{
			case 0: printf("Unconditional ||   n/a    || "); break;
			case 1: printf("Left Side < # ||   %-4g   || ", m_conditions[condIndex].m_argument); break;
			case 2: printf("Right Side < #||   %-4g   || ", m_conditions[condIndex].m_argument); break;
			case 3: printf("Random < #    ||   %-4g   || ", m_conditions[condIndex].m_argument); break;
			case 4: printf("No Player 3?  ||   n/a    || "); break;
			case 5: printf("No Player 4?  ||   n/a    || "); break;
			case 6: printf("Left < Right? ||   n/a    || "); break;
			}
			if (m_conditions[condIndex].m_trueEffect >= 0)
				printf("Go to Section %7s || ", m_sections[m_conditions[condIndex].m_trueEffect].m_name);
			else
				printf("Go to Condition %5zu || ", condIndex - m_conditions[condIndex].m_trueEffect);
			if (m_conditions[condIndex].m_type > 0)
			{
				if (m_conditions[condIndex].m_falseEffect >= 0)
					printf("Go to Section %7s ||\n", m_sections[m_conditions[condIndex].m_falseEffect].m_name);
				else
					printf("Go to Condition %5zu ||\n", condIndex - m_conditions[condIndex].m_falseEffect);
			}
			else
				printf("         n/a         ||\n");
		}
		printf("==================================================================================||\n");
		printf("%sType the number for the condition that you wish to edit\n", g_global.tabs.c_str());
		size_t val;
		switch (GlobalFunctions::valueInsert(val, false, size_t(0), m_conditions.size() - 1))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::InvalidNegative:
			printf("%sGiven section value must be positive.\n", g_global.tabs.c_str());
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::MaxExceeded:
			printf("%sGiven section value cannot exceed %zu\n", g_global.tabs.c_str(), m_sections.size() - 1 );
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::Success:
			++g_global;
			do
			{
				GlobalFunctions::banner(" " + string(m_name) + " - Condition " + to_string(val) + ' ', 1.5);
				string choices = "ct";
				printf("%sc - Type: ", g_global.tabs.c_str());
				switch (m_conditions[val].m_type)
				{
				case 0: printf("Unconditional\n"); break;
				case 1: printf("Left Side < #\n"); break;
				case 2: printf("Right Side < #\n"); break;
				case 3: printf("Random < #\n"); break;
				case 4: printf("No Player 3?\n"); break;
				case 5: printf("No Player 4?\n"); break;
				case 6: printf("Left < Right?\n"); break;
				}
				if (m_conditions[val].m_type > 0 && m_conditions[val].m_type < 4)
				{
					printf("%sA - Argument: %g\n", g_global.tabs.c_str(), m_conditions[val].m_argument );
					choices += 'a';
				}
				printf("%sT - True Effect: ", g_global.tabs.c_str());
				if (m_conditions[val].m_trueEffect >= 0)
					printf("Go to Section %s\n", m_sections[m_conditions[val].m_trueEffect].m_name );
				else
					printf("Go to Condition %zu\n", val - m_conditions[val].m_trueEffect );
				if (m_conditions[val].m_type > 0)
				{
					printf("%sF - False Effect: ", g_global.tabs.c_str());
					if (m_conditions[val].m_falseEffect >= 0)
						printf("Go to Section %s\n", m_sections[m_conditions[val].m_falseEffect].m_name );
					else
						printf("Go to Condition %zu\n", val - m_conditions[val].m_falseEffect );
					choices += 'f';
				}
				if (m_conditions.size() > 1)
				{
					printf("%sD - Delete this condition\n", g_global.tabs.c_str());
					choices += 'd';
				}
				printf("%sQ - Choose another condition\n", g_global.tabs.c_str());
				switch (GlobalFunctions::menuChoices(choices))
				{
				case GlobalFunctions::ResultType::Quit:
					g_global.quit = true;
					break;
				case GlobalFunctions::ResultType::Help:
					printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
				case GlobalFunctions::ResultType::Failed:
					break;
				default:
					++g_global;
					switch (g_global.answer.character)
					{
					case 'c':
						changeName(m_sections[val]);
						break;
					case 'a':
						changeAudio(m_sections[val]);
						break;
					case 't':
						changeFrames(m_sections[val]);
						break;
					case 'f':
						changeFrames(m_sections[val]);
						break;
					case 'd':
						adjustTempo(m_sections[val]);
						break;
					}
					--g_global;
				}
			} while (!g_global.quit);
			g_global.quit = false;
			--g_global;
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void Editor_CHC::conditionDelete(SongSection& section, size_t index)
{
	auto oldcond = section.m_conditions;
	for (size_t condIndex = 0; condIndex < index; condIndex++)
	{
		if (section.m_conditions[condIndex].m_trueEffect < 0)
		{
			if (condIndex - section.m_conditions[condIndex].m_trueEffect == index)
			{
				printf("%sCondition %zu's true effect points to this condition.", g_global.tabs.c_str(), condIndex);
			}
			else if ((size_t)condIndex - section.m_conditions[condIndex].m_trueEffect > index)
				section.m_conditions[condIndex].m_trueEffect++;
		}
	}
}*/
