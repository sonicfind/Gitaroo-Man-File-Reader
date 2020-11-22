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
#include "CHC_Main.h"
using namespace std;

//Applies the note fixing function & reorgarnizes the loaded CHC song given to it
bool quickFix(CHC& song)
{
	try
	{
		CHC_Editor editor(song);
		editor.fixNotes();
		editor.organizeAll();
		return true;
	}
	catch (exception e)
	{
		printf("%sQuickFix - Exception Caught: %s\n", g_global.tabs.c_str(), e.what());
		return false;
	}
}

void CHC_Editor::editSong(const bool multi)
{
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Editor ");
		//Holds addresses to the various functions that can be chosen in this menu prompt
		LinkedList::List<void(CHC_Editor::*)()> functions = { &CHC_Editor::audioSettings, &CHC_Editor::winLossSettings,
											   &CHC_Editor::adjustSpeed, &CHC_Editor::sectionMenu, &CHC_Editor::adjustFactors };
		string choices = "vagcd";
		if (!song->m_optimized)
		{
			choices += 'f'; functions.push_back(&CHC_Editor::fixNotes);
			printf("%sF - Fix any problematic notes or trace lines\n", g_global.tabs.c_str());
		}
		if (song->m_unorganized)
		{
			choices += 'o'; functions.push_back(&CHC_Editor::organizeAll);
			printf("%sO - Organize All Unorganized Sections\n", g_global.tabs.c_str());
		}
		if (strstr(song->m_imc, ".IMC"))
		{
			choices += 'i'; functions.push_back(&CHC_Editor::swapIMC);
			printf("%sI - Swap IMC file (Current file: %s)\n", g_global.tabs.c_str(), song->m_imc + 34);
		}
		else
		{
			choices += 'p'; functions.push_back(&CHC_Editor::PSPToPS2);
			printf("%sP - Convert for PS2-Version Compatibility\n", g_global.tabs.c_str());
		}
		printf("%sV - Volume & Pan Settings\n", g_global.tabs.c_str());
		printf("%sA - Win/Loss Animations\n", g_global.tabs.c_str());
		printf("%sG - Adjust Gameplay Speed: %g\n", g_global.tabs.c_str(), song->m_speed );
		printf("%sC - SongSections (%zu)\n", g_global.tabs.c_str(), song->m_sections.size());
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

			if (strstr(song->m_imc, ".IMC"))
			{
				printf("%sI - Swap IMC file (Current file: %s):\n", g_global.tabs.c_str(), song->m_imc + 34);
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

			printf("%sG - Adjust Gameplay Speed (Current speed: %g):\n", g_global.tabs.c_str(), song->m_speed);
			printf("%sThe speed of which trace lines, phrae bars, and guard marks approach the middle of the screen is determined by this value.\n", g_global.tabs.c_str());
			printf("%sThis option will allow you to adjust the value to whatever you want (negative even). Note: lower = faster; higher = slower.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sC - SongSections (%zu):\n", g_global.tabs.c_str(), song->m_sections.size());
			printf("%sIn the future, this option will be the gateway to digging into the nitty gritty of each section. It will allow you manually adjust a wide range\n", g_global.tabs.c_str());
			printf("%sof parameters, including a section's duration, conditions, and phase type while also allowing for manual adjustments on subsections and\n", g_global.tabs.c_str());
			printf("%stheir trace lines, phrase bars, and guard marks. Currently, however, it will give you a choice of two features:\n", g_global.tabs.c_str());
			printf("%s1. If there are any sections still unorganized, it will give you the option to reorganize every section that needs it.\n", g_global.tabs.c_str());
			printf("%s2. Change the order of which sections will proceed in gameplay using unconditional conditions. Useful for when you need to test only certain sections.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sF - Player Damage/Energy Factors:\n", g_global.tabs.c_str());
			printf("%sFor each player separately, each phase type has values pertaining to 8 different HP related factors.\n", g_global.tabs.c_str());
			printf("%sThis will bring up a series of menus so that you can edit any factors you like - although some factors have no effect in certain phase types.\n", g_global.tabs.c_str());
			printf("%s(No dealing or receiving damage in charge phases for example).\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			++g_global;
			(this->*functions[g_global.answer.index])();
			--g_global;
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void CHC_Editor::organizeAll()
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Complete Reorganization ");
	int* results = new int[song->m_sections.size()]();
	auto organizeSection = [&](const size_t sectIndex)
	{
		if (!song->m_sections[sectIndex].m_organized)
			results[sectIndex] = 2 + reorganize(song->m_sections[sectIndex]);
		else
			results[sectIndex] = 1;
	};
	auto printResult = [&]()
	{
		for (size_t sectIndex = 0; sectIndex < song->m_sections.size(); ++sectIndex)
		{
			while (!results[sectIndex]);
			SongSection& section = song->m_sections[sectIndex];
			switch (results[sectIndex])
			{
			case 2:
				printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", g_global.tabs.c_str(), section.m_name);
				break;
			case 3:
				printf("%s%s organized - # of charts per player: %lu", g_global.tabs.c_str(), section.m_name, section.m_numCharts);
				if (song->m_imc[0])
				{
					size_t totals[2] = { 0, 0 };
					for (size_t pl = 0; pl < 4; ++pl)
						for (size_t ch = 0, index = pl * section.m_numCharts; ch < section.m_numCharts; ++index, ++ch)
							if (section.m_charts[index].m_guards.size() || section.m_charts[index].m_phrases.size() || section.m_charts[index].m_tracelines.size() <= 1)
								++totals[pl & 1];
					printf(" (Pair total: %zu)\n", totals[0] >= totals[1] ? totals[0] : totals[1]);
				}
				else
					putchar('\n');
			}
		}
	};
	if (processor_count > 1)
	{
		struct threadControl
		{
			size_t sectIndex;
			std::thread thisThread;
			threadControl(const size_t index) : sectIndex(index) {}
		};
		LinkedList::List<threadControl> threads;
		std::thread resultLoop(printResult);
		for (size_t sectIndex = 0; song->m_unorganized && sectIndex < song->m_sections.size(); ++sectIndex)
		{
			while (threads.size() == processor_count - 2)
			{
				for (size_t thr = 0; thr < threads.size();)
				{
					if (results[threads[thr].sectIndex])
					{
						threads[thr].thisThread.join();
						threads.erase(thr);
					}
					else
						++thr;
				}
			}
			threads.emplace_back(sectIndex).thisThread = std::thread(organizeSection, sectIndex);
		}
		for (threadControl& thr : threads)
			thr.thisThread.join();

		resultLoop.join();
		delete[song->m_sections.size()] results;
	}
	else
	{
		for (size_t sectIndex = 0; sectIndex < song->m_sections.size(); ++sectIndex)
		{
			organizeSection(sectIndex);
			SongSection& section = song->m_sections[sectIndex];
			switch (results[sectIndex])
			{
			case 2:
				printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", g_global.tabs.c_str(), section.m_name);
				break;
			case 3:
				printf("%s%s organized - # of charts per player: %lu", g_global.tabs.c_str(), section.m_name, section.m_numCharts);
				if (song->m_imc[0])
				{
					size_t totals[2] = { 0, 0 };
					for (size_t pl = 0; pl < 4; ++pl)
					{
						size_t index = pl * section.m_numCharts;
						for (const size_t end = index + section.m_numCharts; index < end; ++index)
							if (section.m_charts[index].m_guards.size() || section.m_charts[index].m_phrases.size() || section.m_charts[index].m_tracelines.size() <= 1)
								++totals[pl & 1];
					}
					printf(" (Pair total: %zu)\n", totals[0] >= totals[1] ? totals[0] : totals[1]);
				}
				else
					putchar('\n');
			}
		}
	}
	printf("%sAll sections organized\n", g_global.tabs.c_str());
}

void CHC_Editor::fixNotes()
{
	GlobalFunctions::banner(" Fixing " + song->m_shortname + ".CHC ");
	size_t tracelinesCurved = 0, tracelinesStraightened = 0, tracelinesDeleted = 0, phrasesDeleted = 0, phrasesShortened = 0, guardsDeleted = 0;
	song->m_optimized = true;
	try
	{
		LinkedList::List<string>* strings = new LinkedList::List<string>[song->m_sections.size()];
		bool* results = new bool[song->m_sections.size()]();
		auto printResult = [&]()
		{
			for (size_t index = 0; index < song->m_sections.size(); ++index)
			{
				while (!results[index]);
				while (strings[index].size())
				{
					printf("%s", strings[index].front().c_str());
					strings[index].pop_front();
				}
			}
		};
		auto fixSection = [&](const size_t sectIndex)
		{
			SongSection& section = song->m_sections[sectIndex];
			if (section.m_battlePhase == SongSection::Phase::INTRO || (section.m_battlePhase == SongSection::Phase::BATTLE && (strstr(section.m_name, "BRK") || strstr(section.m_name, "BREAK"))))
			{
				results[sectIndex] = true;
				return;
			}
			for (size_t playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
			{
				for (size_t chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
				{
					Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
					size_t barsRemoved = 0, linesRemoved = 0;
					try
					{
						for (size_t traceIndex = 0, phraseIndex = 0; traceIndex < chart.m_tracelines.size() - 1; traceIndex++)
						{
							Traceline* trace = &chart.m_tracelines[traceIndex];
							bool adjust = trace->m_angle == chart.m_tracelines[traceIndex + 1].m_angle;
							for (; phraseIndex < chart.m_phrases.size(); phraseIndex++)
							{
								Phrase* phrase = &chart.m_phrases[phraseIndex];
								if (phrase->m_pivotAlpha >= trace->getEndAlpha())
									break;
								if (phrase->m_start)
								{
									if (phraseIndex && phrase->m_pivotAlpha - chart.m_phrases[phraseIndex - 1].getEndAlpha() < SongSection::s_SAMPLE_GAP)
									{
										chart.m_phrases[phraseIndex - 1].changeEndAlpha(phrase->m_pivotAlpha - SongSection::s_SAMPLE_GAP);
										strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
										strings[sectIndex].push_back("Phrase bar " + to_string(phraseIndex + barsRemoved - 1) + " shortened\n");
										++phrasesShortened;
									}
									while (!phrase->m_end && (phrase->m_pivotAlpha >= trace->getEndAlpha() - 3000)
										&& (phrase->m_pivotAlpha - trace->m_pivotAlpha > 800))
									{
										phrase->m_duration += chart.m_phrases[phraseIndex + 1].m_duration;
										phrase->m_end = chart.m_phrases[phraseIndex + 1].m_end;
										memcpy_s(phrase->m_junk, 12, chart.m_phrases[phraseIndex + 1].m_junk, 12);
										if (!trace->changeEndAlpha(phrase->m_pivotAlpha))
										{
											strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
											if (chart.remove(traceIndex, 't', linesRemoved))
												strings[sectIndex].push_back("Trace line " + to_string(traceIndex + linesRemoved) + " removed\n");
											++linesRemoved;
										}
										else
											++traceIndex;
										trace = &chart.m_tracelines[traceIndex];
										trace->changePivotAlpha(phrase->m_pivotAlpha);
										adjust = traceIndex + 1 < chart.m_tracelines.size() && trace->m_angle == chart.m_tracelines[traceIndex + 1].m_angle;
										strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
										if (chart.remove(phraseIndex + 1, 'p', barsRemoved))
											strings[sectIndex].push_back("Phrase bar " + to_string(phraseIndex + barsRemoved + 1) + " removed\n");
										++barsRemoved;
									}
									if (adjust && !trace->m_curve && song->m_imc[0])
									{
										if (phrase->m_pivotAlpha >= trace->getEndAlpha() - 8000)
										{

											trace->m_curve = true;
											strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
											strings[sectIndex].push_back("Trace line " + to_string(traceIndex + linesRemoved) + " set to \"smooth\"\n");
											++tracelinesCurved;
											adjust = false;
										}
									}
								}
							}
							if (adjust && trace->m_curve)
							{
								trace->m_curve = false;
								strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
								strings[sectIndex].push_back("Trace line " + to_string(traceIndex + linesRemoved) + " set to \"rigid\"\n");
								++tracelinesStraightened;
							}
						}
					}
					catch (...)
					{
						printf("%sThere was an error when attempting to apply phrase bar & trace line fixes to section #%zu (%s) - Subsection %zu\n", g_global.tabs.c_str(), sectIndex, section.m_name, playerIndex * section.m_numCharts + chartIndex);
						song->m_optimized = false;
					}
					tracelinesDeleted += linesRemoved;
					phrasesDeleted += barsRemoved;
					for (size_t guardIndex = 0; guardIndex < chart.m_guards.size(); guardIndex++)
					{
						while (guardIndex + 1 < chart.m_guards.size() && chart.m_guards[guardIndex].m_pivotAlpha + 1600 > chart.m_guards[guardIndex + 1].m_pivotAlpha)
						{
							strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
							if (chart.remove(guardIndex + 1, 'g', guardsDeleted))
								strings[sectIndex].push_back("Guard Mark " + to_string(guardsDeleted) + " removed\n");
							++guardsDeleted;
						}
					}
				}
			}
			if (section.m_battlePhase == SongSection::Phase::END || sectIndex + 1 == song->m_sections.size())
			{
				results[sectIndex] = true;
				return;
			}
			if (!song->m_imc[0] || section.m_battlePhase == SongSection::Phase::HARMONY)
			{
				for (size_t playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
				{
					for (size_t chartIndex = section.m_numCharts; chartIndex > 0;)
					{
						if (section.m_charts[playerIndex * section.m_numCharts + (--chartIndex)].getNumPhrases() > 0)
						{
							for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
							{
								long* effect = &section.m_conditions[condIndex].m_trueEffect;
								for (size_t i = 0; i < 2; i++, effect++)
								{
									if (*effect >= 0)
									{
										Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
										long endTime = section.m_duration - (chart.m_phrases[chart.m_phrases.size() - 1].getEndAlpha() + chart.m_pivotTime);
										Chart& chart2 = song->m_sections[*effect].m_charts[playerIndex * song->m_sections[*effect].m_numCharts];
										if (chart2.m_phrases.size())
										{
											long startTime = chart2.m_phrases[0].m_pivotAlpha + chart2.m_pivotTime;
											if (startTime + endTime < section.s_SAMPLE_GAP)
											{
												chart.m_phrases[chart.m_phrases.size() - 1].changeEndAlpha((long)(section.m_duration - startTime - section.s_SAMPLE_GAP - chart.m_pivotTime));
												strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
												strings[sectIndex].push_back("Phrase bar " + to_string(chart.m_phrases.size() - 1) + " shortened\n");
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
					for (size_t chartIndex = section.m_numCharts; !g_global.quit && chartIndex > 0;)
					{
						--chartIndex;
						for (size_t playerIndex = section.m_numPlayers + p; !g_global.quit && playerIndex > 1;)
						{
							playerIndex -= 2;
							if (section.m_charts[(playerIndex)*section.m_numCharts + chartIndex].m_phrases.size() != 0)
							{
								for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
								{
									long* effect = &section.m_conditions[condIndex].m_trueEffect;
									for (size_t i = 0; i < 2; i++, effect++)
									{
										if (*effect >= 0)
										{
											Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
											long endTime = section.m_duration - (chart.m_phrases[chart.m_phrases.size() - 1].getEndAlpha() + chart.m_pivotTime);
											Chart& chart2 = song->m_sections[*effect].m_charts[p * song->m_sections[*effect].m_numCharts];
											if (chart2.m_phrases.size())
											{
												long startTime = chart2.m_phrases[0].m_pivotAlpha + chart2.m_pivotTime;
												if (startTime + endTime < section.s_SAMPLE_GAP)
												{
													chart.m_phrases[chart.m_phrases.size() - 1].changeEndAlpha(long(section.m_duration - startTime - section.s_SAMPLE_GAP - chart.m_pivotTime));
													strings[sectIndex].push_back(g_global.tabs + section.m_name + " - Subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) + ": ");
													strings[sectIndex].push_back("Phrase bar " + to_string(chart.m_phrases.size() - 1) + " shortened\n");
													phrasesShortened++;
												}
											}
										}
									}
								}
								g_global.quit = true;
							}
						}
					}
					g_global.quit = false;
				}
			}
			results[sectIndex] = true;
		};
		if (processor_count > 1)
		{
			struct threadControl
			{
				size_t sectIndex;
				std::thread thisThread;
				threadControl(const size_t index) : sectIndex(index) {}
			};
			LinkedList::List<threadControl> threads;
			std::thread resultLoop(printResult);
			for (size_t i = 0; i < song->m_sections.size(); ++i)
			{
				while (threads.size() == processor_count - 2)
				{
					for (size_t thr = 0; thr < threads.size();)
					{
						if (results[threads[thr].sectIndex])
						{
							threads[thr].thisThread.join();
							threads.erase(thr);
						}
						else
							++thr;
					}
				}
				threads.emplace_back(i).thisThread = std::thread(fixSection, i);
			}
			for (threadControl& thr : threads)
				thr.thisThread.join();
			resultLoop.join();
		}
		else
		{
			for (size_t i = 0; i < song->m_sections.size(); ++i)
			{
				fixSection(i);
				for (const string& str : strings[i])
					printf("%s", str.c_str());
				strings[i].clear();
			}
		}
		delete[song->m_sections.size()] strings;
		delete[song->m_sections.size()] results;

	}
	catch (...)
	{
		printf("%sUnknown error occurred when applying note fixes. Thus, the chc will remain unoptimized for now.\n", g_global.tabs.c_str());
		song->m_optimized = false;
	}
	printf("%s\n", g_global.tabs.c_str());
	if (tracelinesCurved)
		printf("%s%zu%s\n", g_global.tabs.c_str(), tracelinesCurved, (tracelinesCurved > 1 ? " rigid Trace lines set to \"smooth\"" : " rigid Trace line set to \"smooth\""));
	if (tracelinesStraightened)
		printf("%s%zu%s\n", g_global.tabs.c_str(), tracelinesStraightened, (tracelinesStraightened > 1 ? " smooth Trace lines set to \"rigid\"" : " smooth Trace line set to \"rigid\""));
	if (tracelinesDeleted)
		printf("%s%zu%s deleted to accommodate deleted phrase bars\n", g_global.tabs.c_str(), tracelinesDeleted, (tracelinesDeleted > 1 ? " Trace lines" : " Trace line"));
	if (phrasesDeleted)
		printf("%s%zu%s deleted for starting with durations under 3000 samples\n", g_global.tabs.c_str(), phrasesDeleted, (phrasesDeleted > 1 ? " Phrase bars" : " Phrase bar"));
	if (phrasesShortened)
		printf("%s%zu%s Phrase Bars shortened for ending too close to following Phrase Bars\n", g_global.tabs.c_str(), phrasesShortened, (phrasesShortened > 1 ? " Phrase bars" : " Phrase bar"));
	if (guardsDeleted)
		printf("%s%zu%s deleted for being within 1600 samples of a preceeding Guard Mark\n", g_global.tabs.c_str(), guardsDeleted, (guardsDeleted > 1 ? " Guard marks" : " Guard mark"));
	if (tracelinesStraightened || tracelinesCurved || phrasesDeleted || phrasesShortened || guardsDeleted)
	{
		printf("%s\n%sChanges will be applied if you choose to save the file\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		song->m_saved = false;
	}
	else
		printf("%sNo changes made\n", g_global.tabs.c_str());
}

void CHC_Editor::PSPToPS2()
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Non-Duet Conversion ");
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
			song->m_filename = song->m_filename.substr(0, song->m_filename.length() - 4) + "_PS2_" + to_string(player2 + 1) + 'v' + (enemy ? 'E' : '2') + ".CHC";
			{
				size_t pos = song->m_filename.find_last_of("\\");
				if (pos != string::npos)
					song->m_shortname = song->m_filename.substr(pos + 1, song->m_filename.length() - pos - 5);
				else
					song->m_shortname = song->m_shortname.substr(0, song->m_shortname.length() - 4);
			}
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	snprintf(song->m_imc, 44, "/PROJECTS/STDATA/STAGE%02d/SONGDATA/ST%02d.IMC", song->m_stage, song->m_stage);
	song->m_audio[2].pan = song->m_audio[3].pan = song->m_audio[6].volume = song->m_audio[7].volume = 0;
	song->m_audio[4].pan = song->m_audio[5].pan = 32767;
	int* results = new int[song->m_sections.size()]();
	auto convertToPS2 = [&](const size_t index)
	{
		SongSection& section = song->m_sections[index];
		int val;
		if (!section.m_organized) //Organized as a duet-made section
			val = 2 + reorganize(section);
		else
			val = 1;
		section.m_charts.erase(3ULL * section.m_numCharts, section.m_numCharts);
		if (player2 || !enemy)
			section.m_charts.moveElements(2ULL * section.m_numCharts, section.m_numCharts, section.m_numCharts);
		if (!player2)
			section.m_charts.erase(2ULL * section.m_numCharts, section.m_numCharts);
		else
			section.m_charts.erase(0, section.m_numCharts);
		if (section.m_numCharts & 1)
		{
			section.m_charts.emplace_back();
			section.m_charts.emplace(section.m_numCharts, 1);
			section.m_numCharts++;
		}
		for (size_t c = 1; c < section.m_numCharts >> 1; c++)
		{
			section.m_charts.moveElements(c << 1, c);
			section.m_charts.moveElements((c << 1) + section.m_numCharts, c + section.m_numCharts);
		}
		section.m_numCharts >>= 1;
		section.m_charts.moveElements((size_t)section.m_numCharts << 1, section.m_numCharts, section.m_numCharts);
		if (!enemy)
			section.m_swapped = (section.m_swapped >> 1) + 4;
		results[index] = val;
	};
	auto printResult = [&]()
	{
		for (size_t index = 0; index < song->m_sections.size(); ++index)
		{
			while (!results[index]);
			switch (results[index])
			{
			case 2:
				printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", g_global.tabs.c_str(), song->m_sections[index].m_name);
				break;
			case 3:
				printf("%s%s organized - # of charts per player: %lu\n", g_global.tabs.c_str(), song->m_sections[index].m_name, song->m_sections[index].m_numCharts);
			}
		}
	};
	if (processor_count > 1)
	{
		struct threadControl
		{
			size_t sectIndex;
			std::thread thisThread;
			threadControl(const size_t index) : sectIndex(index) {}
		};
		LinkedList::List<threadControl> threads;
		std::thread resultLoop(printResult);
		for (size_t sectIndex = 0; sectIndex < song->m_sections.size(); ++sectIndex)
		{
			while (threads.size() == processor_count - 2)
			{
				for (size_t thr = 0; thr < threads.size();)
				{
					if (results[threads[thr].sectIndex])
					{
						threads[thr].thisThread.join();
						threads.erase(thr);
					}
					else
						++thr;
				}
			}
			threads.emplace_back(sectIndex).thisThread = std::thread(convertToPS2, sectIndex);
		}
		for (threadControl& thr : threads)
			thr.thisThread.join();
		resultLoop.join();
	}
	else
	{
		for (LinkedList::List<SongSection>::Iterator cur = song->m_sections.begin();
			cur != song->m_sections.end();
			++cur)
		{
			convertToPS2(cur.getIndex());
			switch (results[cur.getIndex()])
			{
			case 2:
				printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", g_global.tabs.c_str(), (*cur).m_name);
				break;
			case 3:
				printf("%s%s organized - # of charts per player: %lu\n", g_global.tabs.c_str(), (*cur).m_name, (*cur).m_numCharts);
			}
		}
	}
	delete[song->m_sections.size()] results;
	for (size_t s = 0; s < 5; s++)
	{
		song->m_energyDamageFactors[0][s].attackInitial *= 2;
		song->m_energyDamageFactors[0][s].attackRelease *= 2;
		song->m_energyDamageFactors[0][s].attackMiss *= 2;
		song->m_energyDamageFactors[0][s].chargeInitial *= 2;
		song->m_energyDamageFactors[0][s].chargeRelease *= 2;
		song->m_energyDamageFactors[0][s].guardMiss *= 2;
	}
	song->m_saved = false;
	song->m_optimized = false;
}

void CHC_Editor::swapIMC()
{
	while (true)
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - IMC Swap ");
		printf("%sProvide the name (without the extension) of the .IMC file you wish to use (Or 'Q' to back out to Modify Menu)\n", g_global.tabs.c_str());
		printf("%sCurrent IMC File: %s\n", g_global.tabs.c_str(), song->m_imc);
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
						std::copy(strIMC.begin(), strIMC.end(), song->m_imc + 34);
						song->m_imc[34 + strIMC.length()] = 0;
						song->m_saved = 0;
						__fallthrough;
					}
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

void CHC_Editor::audioSettings()
{
	while (true)
	{
		string choices;
		if (!song->m_imc[0])
		{
			GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Audio Channels ");
			printf("%s          ||        Volume        ||        Paning        ||\n", g_global.tabs.c_str());
			printf("%s Channels ||  Value  ||  Percent  ||   Left   |   Right   ||\n", g_global.tabs.c_str());
			printf("%s==========================================================||\n", g_global.tabs.c_str());
			for (size_t index = 0; index < 8; index++)
			{
				printf("%s     %zu    ||", g_global.tabs.c_str(), index + 1);
				printf("%7lu  ||%7g%%   ||", song->m_audio[index].volume, ((double)song->m_audio[index].volume) * 100 / 32767);
				switch (song->m_audio[index].pan)
				{
				case 0:     printf("   Left   |           ||\n"); break;
				case 16383: printf("        Center        ||\n"); break;
				case 32767: printf("          |   Right   ||\n"); break;
				default: printf("%8g%% |%8g%% ||\n", 100 - (((double)song->m_audio[index].pan) * 100 / 32767), ((double)song->m_audio[index].pan) * 100 / 32767);
				}
			}
			printf("%s==========================================================||\n", g_global.tabs.c_str());
			printf("%sChoose the channel you wish to edit [Type 'Q' to exit audio settings]\n", g_global.tabs.c_str());
			choices = "12345678";
		}
		else
		{
			GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Audio Channels [Duet Stages] ");
			printf("%s          ||        Volume        ||\n", g_global.tabs.c_str());
			printf("%s Channels ||  Value  ||  Percent  ||\n", g_global.tabs.c_str());
			printf("%s==================================||\n", g_global.tabs.c_str());
			printf("%s   1 & 2  ||        Unused        ||\n", g_global.tabs.c_str());
			for (size_t index = 2; index < 8; index++)
			{
				printf("%s     %zu    ||", g_global.tabs.c_str(), index + 1);
				printf("%7lu  ||%7g%%   ||", song->m_audio[index].volume, ((double)song->m_audio[index].volume) * 100 / 32767);
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
			if (!song->m_imc[0])
				channel += 2;
			do
			{
				GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Audio Channel " + to_string(channel + 1));
				string choices = "vb";
				printf("%sV - Volume\n", g_global.tabs.c_str());
				if (!song->m_imc[0])
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
							GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Audio Channel " + to_string(channel + 1) + " - Volume");
							printf("%sProvide value for Volume (0 - 32767[100%%]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n", g_global.tabs.c_str());
							printf("%sCurrent Volume: %lu (%g%%)\n", g_global.tabs.c_str(), song->m_audio[channel].volume, 100.0 * song->m_audio[channel].volume / 32767);
							printf("%sInput: ", g_global.tabs.c_str());
							unsigned long oldVol = song->m_audio[channel].volume;
							switch (GlobalFunctions::valueInsert(song->m_audio[channel].volume, false, 0UL, 32767UL, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (song->m_audio[channel].volume != oldVol)
									song->m_saved = 0;
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
							GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Audio Channel " + to_string(channel + 1) + " - Panning");
							printf("%sProvide value for Panning (Left[0] - Center[16383] - Right[32767]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n", g_global.tabs.c_str());
							printf("%sCurrent Panning: ", g_global.tabs.c_str());
							unsigned long oldPan = song->m_audio[channel].pan;
							switch (song->m_audio[channel].pan)
							{
							case 0: printf("Left (0)\n"); break;
							case 16383: printf("Center (16383)\n"); break;
							case 32767: printf("Right (32767)\n"); break;
							default: printf("%g%% Left | %g%% Right (%lu)\n", 100 - (song->m_audio[channel].pan * 100.0 / 32767), song->m_audio[channel].pan * 100.0 / 32767, song->m_audio[channel].pan);
							}
							printf("%sInput: ", g_global.tabs.c_str());
							switch (GlobalFunctions::valueInsert(song->m_audio[channel].pan, false, 0UL, 32767UL, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (song->m_audio[channel].pan != oldPan)
									song->m_saved = false;
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

void CHC_Editor::winLossSettings()
{
	while (true)
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Win/Loss Animations ");
		static const string ssqStrings[] = { "Win A", "Win B", "Lost Pre", "Lost Post" };
		printf("%s            ||       Frames       ||\n", g_global.tabs.c_str());
		printf("%s Animations ||  First  ||   Last  ||\n", g_global.tabs.c_str());
		printf("%s==================================||\n", g_global.tabs.c_str());
		printf("%s 1 - Win A  ||%7g  ||%7g  ||\n", g_global.tabs.c_str(), song->m_events[0].first, song->m_events[0].last);
		printf("%s 2 - Win B  ||%7g  ||%7g  ||\n", g_global.tabs.c_str(), song->m_events[1].first, song->m_events[1].last);
		printf("%s3 - Lost Pre||%7g  ||%7g  ||\n", g_global.tabs.c_str(), song->m_events[2].first, song->m_events[2].last);
		printf("%s4 - Lt. Loop||%7g  ||%7g  ||\n", g_global.tabs.c_str(), song->m_events[3].first, song->m_events[3].last);
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
				GlobalFunctions::banner(" " + song->m_shortname + ".CHC - SSQ Animation:" + ssqStrings[g_global.answer.index]);
				printf("%sF - Adjust first frame: %g\n", g_global.tabs.c_str(), song->m_events[g_global.answer.index].first );
				printf("%sL - Adjust last  frame: %g\n", g_global.tabs.c_str(), song->m_events[g_global.answer.index].last );
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
							GlobalFunctions::banner(" " + song->m_shortname + ".CHC - SSQ Animation: " + ssqStrings[g_global.answer.index] + " - First Frame");
							printf("%sEnter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n", g_global.tabs.c_str());
							printf("%s(Can be a decimal)\n", g_global.tabs.c_str());
							float oldAnim = song->m_events[g_global.answer.index].first;
							switch (GlobalFunctions::valueInsert(song->m_events[g_global.answer.index].first, false, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (song->m_events[g_global.answer.index].first != oldAnim)
									song->m_saved = false;
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
							GlobalFunctions::banner(" " + song->m_shortname + ".CHC - SSQ Animation: " + ssqStrings[g_global.answer.index] + " - Last Frame");
							printf("%sEnter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n", g_global.tabs.c_str());
							printf("%s(Can be a decimal)\n", g_global.tabs.c_str());
							float oldAnim = song->m_events[g_global.answer.index].last;
							switch (GlobalFunctions::valueInsert(song->m_events[g_global.answer.index].last, false, "b"))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::SpecialCase:
								g_global.quit = true;
								break;
							case GlobalFunctions::ResultType::Success:
								if (song->m_events[g_global.answer.index].last != oldAnim)
									song->m_saved = false;
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

void CHC_Editor::adjustSpeed()
{
	while (true)
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Gameplay Speed Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit speed settings]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal... and/or negative with weird effects\n", g_global.tabs.c_str());
		printf("%sCurrent Value: %g\n", g_global.tabs.c_str(), song->m_speed );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldSpeed = song->m_speed;
		switch (GlobalFunctions::valueInsert(song->m_speed, true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (song->m_speed != oldSpeed)
				song->m_saved = false;
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

void CHC_Editor::sectionMenu()
{
	typedef void(CHC_Editor::* functPointers)();
	functPointers results[5] = { &CHC_Editor::playOrder, &CHC_Editor::rearrange, &CHC_Editor::playerSwapAll, &CHC_Editor::sectionSubMenu, &CHC_Editor::fullPathTest };
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Sections ");
		printf("%sP - Set Unconditional Play Order\n", g_global.tabs.c_str());
		printf("%sR - Rearrange Sections\n", g_global.tabs.c_str());
		printf("%sS - Swap Player 1 & Player 2 for All Sections\n", g_global.tabs.c_str());
		printf("%sC - Choose a Single Section to Edit\n", g_global.tabs.c_str());
		printf("%sT - Test Section Accessibility\n", g_global.tabs.c_str());
		printf("%sQ - Back out to Modify Menu\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("prsct", true))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Help:
			printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			++g_global;
			(this->*results[g_global.answer.index])();
			--g_global;
		}
	} while (!g_global.quit);
}

void CHC_Editor::playerSwapAll()
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Complete Player Swap");
	for (size_t sectIndex = 0; sectIndex < song->m_sections.size(); sectIndex++)
		playerSwap(song->m_sections[sectIndex]);
	song->m_saved = false;
}

void CHC_Editor::playOrder()
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Play Order ");
	LinkedList::List<size_t> sectionIndexes;
	do
	{
		printf("%sType the index for each section in the order you wish them to be played - w/ spaces in-between.\n", g_global.tabs.c_str());
		for (size_t index = 0; index < song->m_sections.size(); index++)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), index, song->m_sections[index].m_name );
		switch (GlobalFunctions::listValueInsert(sectionIndexes, "yn", song->m_sections.size(), false))
		{
		case  GlobalFunctions::ResultType::Help:

			break;
		case  GlobalFunctions::ResultType::Quit:
			printf("%sPlay Order Selection cancelled.\n", g_global.tabs.c_str());
			return;
		case GlobalFunctions::ResultType::SpecialCase:
			if (g_global.answer.character == 'n')
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sOk... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n", g_global.tabs.c_str());
				printf("%s\n", g_global.tabs.c_str());
				break;
			}
			else if (!sectionIndexes.size())
			{
				printf("%sPlay Order Selection cancelled.\n", g_global.tabs.c_str());
				return;
			}
		case GlobalFunctions::ResultType::Success:
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%sNo sections have been selected. Quit Play Order Selection? [Y/N]\n", g_global.tabs.c_str());
					switch (GlobalFunctions::menuChoices("yn"))
					{
					case GlobalFunctions::ResultType::Quit:
					case GlobalFunctions::ResultType::Success:
						if (g_global.answer.character == 'y')
						{
							printf("%s\n", g_global.tabs.c_str());
							printf("%sPlay Order Selection cancelled.\n", g_global.tabs.c_str());
							return;
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
			{
				g_global.quit = true;
				break;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%s", g_global.tabs.c_str());
	for (size_t index = 0; index < sectionIndexes.size(); index++)
	{
		if (index + 1ULL < sectionIndexes.size())
		{
			song->m_saved = false;
			SongSection::Condition& cond = song->m_sections[sectionIndexes[index]].m_conditions.front();
			cond.m_type = 0;
			cond.m_trueEffect = (long)sectionIndexes[index + 1ULL];
		}
		else
		{
			if (!pathTest(sectionIndexes[index]))
			{
				SongSection::Condition& cond = song->m_sections[sectionIndexes[index]].m_conditions.front();
				cond.m_type = 0;
				cond.m_trueEffect = (long)song->m_sections.size();
			}
		}
		printf("%s ", song->m_sections[sectionIndexes[index]].m_name);
		song->m_saved = false;
	}
	putchar('\n');
}

void CHC_Editor::rearrange()
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Section Rearrangement ");
	size_t startIndex, numElements, position;
	do
	{
		printf("%sProvide the starting index for the range of sections you want to move.\n", g_global.tabs.c_str());
		for (size_t index = 0; index < song->m_sections.size(); index++)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), index, song->m_sections[index].m_name );
		printf("%sInput: ", g_global.tabs.c_str());
		switch (GlobalFunctions::valueInsert(startIndex, false, size_t(0), song->m_sections.size() - 1))
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
			printf("%sGiven value cannot be greater than %zu\n%s\n", g_global.tabs.c_str(), song->m_sections.size() - 1, g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			break;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	if (song->m_sections.size() - startIndex == 1)
		numElements = 1;
	else
	{
		do
		{
			printf("%sHow many elements do you want to move? [Max # of movable elements: %zu]\n", g_global.tabs.c_str(), song->m_sections.size() - startIndex);
			printf("%sInput: ", g_global.tabs.c_str());
			switch (GlobalFunctions::valueInsert(numElements, false, size_t(1), song->m_sections.size() - startIndex))
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
				printf("%sGiven value cannot be greater than %zu\n%s\n", g_global.tabs.c_str(), song->m_sections.size() - startIndex, g_global.tabs.c_str());
				break;
			case GlobalFunctions::ResultType::Failed:
				printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
				break;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}
	if (numElements < song->m_sections.size())
	{
		long alpha;
		if (song->m_sections.size() - numElements == 1)
		{
			if (!startIndex)
			{
				printf("%sMoving elements to the end of the list\n", g_global.tabs.c_str());
				position = song->m_sections.size();
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
					size_t index = 0;
					const size_t last = startIndex + numElements;
					for (; index < startIndex; index++)
						printf("%s%zu - %s\n", g_global.tabs.c_str(), index, song->m_sections[index].m_name);
					if (numElements > 1)
						printf("%sInvalid position range: %zu - %zu\n", g_global.tabs.c_str(), startIndex, startIndex + numElements);
					else
						printf("%sInvalid position: %zu\n", g_global.tabs.c_str(), startIndex);
					for (index = last + 1; index <= song->m_sections.size(); index++)
					{
						if (index < song->m_sections.size())
							printf("%s%zu - %s\n", g_global.tabs.c_str(), index, song->m_sections[index].m_name);
						else
							printf("%s%zu - End of the list\n", g_global.tabs.c_str(), song->m_sections.size());
					}
				}
				printf("%sInput: ", g_global.tabs.c_str());
				switch (GlobalFunctions::valueInsert(position, false, size_t(0), song->m_sections.size()))
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
					printf("%sGiven value cannot be greater than %zu\n%s\n", g_global.tabs.c_str(), song->m_sections.size(), g_global.tabs.c_str());
					break;
				case GlobalFunctions::ResultType::Failed:
					printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
					break;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		for (size_t sectIndex = 0; sectIndex < song->m_sections.size(); sectIndex++)
		{
			for (size_t condIndex = 0; condIndex < song->m_sections[sectIndex].m_conditions.size(); condIndex++)
			{
				long* effect = &song->m_sections[sectIndex].m_conditions[condIndex].m_trueEffect;
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
		song->m_sections.moveElements(startIndex, position, numElements);
		for (size_t index = 0; index < song->m_sections.size(); index++)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), index, song->m_sections[index].m_name );
	}
	else
		printf("%sReally? You didn't even do anything with that... *sigh*\n", g_global.tabs.c_str());
	printf("%s\n", g_global.tabs.c_str());
}

void CHC_Editor::adjustFactors()
{
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Damage/Energy Factors ", 1.53f);
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
				for (size_t index = player; index < player + 2; index++)
				{
					printf(headers[factor]);
					for (size_t phase = 0; phase < 5; phase++)
					{
						float* val = &song->m_energyDamageFactors[index][phase].initialEnergy + factor;
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
		case GlobalFunctions::ResultType::Failed:
			break;
		default:
			const size_t player = g_global.answer.index;
			do
			{
				GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " ");
				printf("%s        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n", g_global.tabs.c_str(), player + 1);
				printf("%s%s||", g_global.tabs.c_str(), string(77, '=').c_str());
				for (size_t factor = 0; factor < 8; factor++)
				{
					printf("\n\t      %s", headers[factor]);
					for (size_t phase = 0; phase < 5; phase++)
					{
						float* val = &song->m_energyDamageFactors[player][phase].initialEnergy + factor;
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
						const size_t phase = g_global.answer.index;
						do
						{
							GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " | " + phaseName[phase] + " ");
							printf("%s          Player %zu        || %*s ||\n", g_global.tabs.c_str(), player + 1, unsigned(phaseName[phase].length() + 1), phaseName[phase].c_str());
							printf("%s%s||\n", g_global.tabs.c_str(), string(31 + phaseName[phase].length(), '=').c_str());
							printf("%s 1 - Starting Energy      || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].initialEnergy * 100.0);
							printf("%s 2 - Initial-Press Energy || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].chargeInitial * 100.0);
							printf("%s 3 - Initial-Press Damage || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].attackInitial * 100.0);
							printf("%s 4 - Guard Energy Gain    || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].guardEnergy * 100.0);
							printf("%s 5 - Attack Miss Damage   || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].attackMiss * 100.0);
							printf("%s 6 - Guard  Miss Damage   || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].guardMiss * 100.0);
							printf("%s 7 - Sustain Energy Coef. || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].chargeRelease * 100.0);
							printf("%s 8 - Sustain Damage Coef. || %*g%% ||\n", g_global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->m_energyDamageFactors[player][phase].attackRelease * 100.0);
							printf("%s%s||\n", g_global.tabs.c_str(), string(31 + phaseName[phase].length(), '=').c_str());
							printf("%sSelect a factor by number [Type 'B' to choose a different phase | 'Q' to exit factor settings]\n", g_global.tabs.c_str());
							switch (GlobalFunctions::menuChoices("12345678b", true))
							{
							case GlobalFunctions::ResultType::Quit:
								return;
							case GlobalFunctions::ResultType::Help:
								printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
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
										GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) +
											" | " + phaseName[phase] + bannerEnds[g_global.answer.index]);
										float* val = &song->m_energyDamageFactors[player][phase].initialEnergy + g_global.answer.index;

										printf("%s        Player %zu      || %s ||\n", g_global.tabs.c_str(), player + 1, phaseName[phase].c_str());
										printf("%s%s||\n\t\t  ", g_global.tabs.c_str(), string(26 + phaseName[phase].length(), '=').c_str());
										printf("%s %*g ||\n", headers[g_global.answer.index], (unsigned)phaseName[phase].length(), *val);
										printf("%s%s||\n", g_global.tabs.c_str(), string(26 + phaseName[phase].length(), '=').c_str());
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
											if (*val != oldFac) song->m_saved = false;
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

void CHC_Editor::fullPathTest()
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Section Transversal ");
	pathTest(0, true);
}

bool CHC_Editor::pathTest(const size_t startIndex, const bool show)
{
	const size_t size = song->m_sections.size();
	if (startIndex < size)
	{
		bool** conditionTested = new bool* [size]();
		bool* results = new bool[size]();
		for (size_t sectIndex = size; sectIndex > startIndex;)
			testSection(--sectIndex, conditionTested, results, nullptr);
		if (show) //Only used for the full path test
		{
			for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
			{
				if (conditionTested[sectIndex] != nullptr)
				{
					for (size_t condIndex = 0; condIndex < song->m_sections[sectIndex].m_conditions.size(); sectIndex++)
						conditionTested[sectIndex][condIndex] = false;
				}
			}
			bool* reach = new bool[size]();
			testSection(startIndex, conditionTested, results, reach);
			printf("%s%s||\n", g_global.tabs.c_str(), string(40, '=').c_str());
			printf("%s Section || Accessible? || Can Reach End? ||\n", g_global.tabs.c_str());
			for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
			{
				printf("%s%8s ||      ", g_global.tabs.c_str(), song->m_sections[sectIndex].m_name);
				if (reach[sectIndex])
					printf("Y      ||        ");
				else
					printf("N      ||        ");
				if (results[sectIndex])
					printf("Y       ||\n");
				else
					printf("N       ||\n");
			}
			printf("%s%s||\n", g_global.tabs.c_str(), string(40, '=').c_str());
		}
		for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
			if (conditionTested[sectIndex] != nullptr)
				delete[song->m_sections[sectIndex].m_conditions.size()] conditionTested[sectIndex];
		bool res = results[startIndex];
		delete[size] results;
		delete[size] conditionTested;
		return res;
	}
	else
		return false;
}

char CHC_Editor::testSection(const size_t sectIndex, bool** conditionTested, bool* results, bool* reach)
{
	if (!results[sectIndex] || reach != nullptr)
	{
		if (reach != nullptr)
			reach[sectIndex] = true;
		if (conditionTested[sectIndex] == nullptr)
			conditionTested[sectIndex] = new bool[song->m_sections[sectIndex].m_conditions.size()]();
		for (size_t c = 0; c < song->m_sections[sectIndex].m_conditions.size(); c++)
		{
			if (!conditionTested[sectIndex][c])
			{
				traverseCondition(sectIndex, 0, conditionTested, results, reach);
				break;
			}
		}
	}
	return results[sectIndex];
}

void CHC_Editor::traverseCondition(const size_t sectIndex, const size_t condIndex,bool** conditionTested, bool* results, bool* reach)
{
	conditionTested[sectIndex][condIndex] = true;
	SongSection::Condition& cond = song->m_sections[sectIndex].m_conditions[condIndex];
	if (cond.m_trueEffect >= 0)
	{
		if (cond.m_trueEffect >= (long)song->m_sections.size())
			results[sectIndex] = true;
		else if (testSection(cond.m_trueEffect, conditionTested, results, reach))
			results[sectIndex] = true;
	}
	else if(!conditionTested[sectIndex][condIndex - cond.m_trueEffect])
			traverseCondition(sectIndex, condIndex - cond.m_trueEffect, conditionTested, results, reach);
	if (cond.m_type)
	{
		if (cond.m_falseEffect >= 0)
		{
			if (cond.m_falseEffect >= (long)song->m_sections.size())
				results[sectIndex] = true;
			else if(testSection(cond.m_falseEffect, conditionTested, results, reach))
				results[sectIndex] = true;
		}
		else if (!conditionTested[sectIndex][condIndex - cond.m_falseEffect])
			traverseCondition(sectIndex, condIndex - cond.m_falseEffect, conditionTested, results, reach);
	}
}

void CHC_Editor::sectionSubMenu()
{
	while (true)
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Section Selection");
		size_t val;
		switch (GlobalFunctions::ListIndexSelector(val, song->m_sections, "section"))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			++g_global;
			do
			{
				GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + song->m_sections[val].m_name + " - Modify ");
				SongSection& section = song->m_sections[val];
				string choices = "nafptdes"; //ENABLE CONDITION OPTION
				printf("%sN - Name\n", g_global.tabs.c_str());
				printf("%sA - Change the section of audio used: %s\n", g_global.tabs.c_str(), section.m_audio);
				printf("%sF - Adjust SSQ frame range\n", g_global.tabs.c_str());
				switch (section.m_battlePhase)
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
				printf("%sT - Tempo: %g\n", g_global.tabs.c_str(), section.m_tempo);
				printf("%sD - Duration: %lu\n", g_global.tabs.c_str(), section.m_duration);
				//printf("%s", g_global.tabs.c_str(), "C - Modify Conditions (", section.numConditions << ")\n";
				printf("%sE - Check if stage-end is reachable\n", g_global.tabs.c_str());
				if (!section.m_organized)
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
				case GlobalFunctions::ResultType::Success:
					++g_global;
					switch (g_global.answer.character)
					{
					case 'n':
						changeName(section);
						break;
					case 'a':
						changeAudio(section);
						break;
					case 'f':
						changeFrames(section);
						break;
					case 'p':
						changeFrames(section);
						break;
					case 't':
						adjustTempo(section);
						break;
					case 'd':
						adjustDuration(section);
						break;
					case 'c':
						conditionMenu(section);
						break;
					case 'e':
						GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Transversal ");
						if (pathTest(val))
							printf("%s%s can reach the end of the stage.\n", g_global.tabs.c_str(), section.m_name);
						else
							printf("%s%s is unable to reach the end of the stage.\n", g_global.tabs.c_str(), section.m_name);
						break;
					case 'o':
						GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Organize ");
						if (reorganize(section))
						{
							printf("%s%s organized - # of charts per player: %lu", g_global.tabs.c_str(), section.m_name, section.m_numCharts);
							if (song->m_imc[0])
							{
								size_t totals[2] = { 0 };
								for (size_t pl = 0; pl < 4; ++pl)
									for (size_t ch = 0, index = pl * section.m_numCharts; ch < section.m_numCharts; ++index, ++ch)
										if (section.m_charts[index].m_guards.size() || section.m_charts[index].m_phrases.size() || section.m_charts[index].m_tracelines.size() <= 1)
											++totals[pl & 1];
								printf(" (Pair total: %zu)\n", totals[0] >= totals[1] ? totals[0] : totals[1]);
							}
							else
								putchar('\n');
						}
						break;
					case 's':
						GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Player Swap ");
						playerSwap(section);
						song->m_saved = false;
					}
					--g_global;
				}
			} while (!g_global.quit);
			g_global.quit = false;
			--g_global;
		}
	}
}

bool CHC_Editor::reorganize(SongSection& section)
{
	bool ret = true;
	if (section.m_battlePhase == SongSection::Phase::CHARGE || (section.m_battlePhase == SongSection::Phase::BATTLE && !strstr(section.m_name, "BRK") && !strstr(section.m_name, "BREAK")))
	{
		typedef pair<long, Note*> sectNote;
		LinkedList::List<sectNote> notes[4];
		LinkedList::List<Chart> newCharts[4];
		for (size_t pl = 0; pl < 4; pl++)
		{
			size_t currentPlayer;
			if (song->m_imc[0]) currentPlayer = pl & 1;
			else currentPlayer = pl;
			LinkedList::List<sectNote>& player = notes[currentPlayer];
			for (size_t chIndex = 0; chIndex < section.m_numCharts; chIndex++)
			{
				size_t playerIndex, chartIndex;
				if (song->m_imc[0])
				{
					playerIndex = ((2 * ((pl >> 1) ^ (chIndex & 1))) + (pl & 1)) * section.m_numCharts;
					chartIndex = (section.m_numCharts >> 1) * (pl >> 1) + (((section.m_numCharts & 1) * (pl >> 1) + chIndex) >> 1);
				}
				else
				{
					playerIndex = pl * section.m_numCharts;
					chartIndex = chIndex;
				}
				Chart& ch = section.m_charts[(size_t)playerIndex + chartIndex];
				size_t index = 0;
				if (section.m_swapped >= 4 || ((!(pl & 1)) != (song->m_imc[0] && section.m_swapped & 1)))
				{
					for (size_t i = 0; i < ch.getNumGuards(); i++)
					{
						while (index < player.size())
						{
							if (ch.m_guards[i].m_pivotAlpha + ch.m_pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.m_guards[i].m_pivotAlpha + ch.m_pivotTime, &ch.m_guards[i]));
						index++;
					}
					index = 0;
					for (size_t i = 0; i < ch.m_phrases.size(); i++)
					{
						while (index < player.size())
						{
							if (ch.m_phrases[i].m_pivotAlpha + ch.m_pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.m_phrases[i].m_pivotAlpha + ch.m_pivotTime, &ch.m_phrases[i]));
						index++;
					}
					index = 0;
					if (ch.m_tracelines.size() > 1)
					{
						for (size_t i = 0; i < ch.m_tracelines.size(); i++)
						{
							while (index < player.size())
							{
								if (ch.m_tracelines[i].m_pivotAlpha + ch.m_pivotTime <= player[index].first)
									break;
								else
									index++;
							}
							player.insert(index, sectNote(ch.m_tracelines[i].m_pivotAlpha + ch.m_pivotTime, &ch.m_tracelines[i]));
							index++;
						}
					}
				}
				else
				{
					for (size_t i = 0; i < ch.m_phrases.size(); i++)
					{
						while (index < player.size())
						{
							if (ch.m_phrases[i].m_pivotAlpha + ch.m_pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.m_phrases[i].m_pivotAlpha + ch.m_pivotTime, &ch.m_phrases[i]));
						index++;
					}
					index = 0;
					if (ch.m_tracelines.size() > 1)
					{
						for (size_t i = 0; i < ch.m_tracelines.size(); i++)
						{
							while (index < player.size())
							{
								if (ch.m_tracelines[i].m_pivotAlpha + ch.m_pivotTime <= player[index].first)
									break;
								else
									index++;
							}
							player.insert(index, sectNote(ch.m_tracelines[i].m_pivotAlpha + ch.m_pivotTime, &ch.m_tracelines[i]));
							index++;
						}
					}
					index = 0;
					for (size_t i = 0; i < ch.getNumGuards(); i++)
					{
						while (index < player.size())
						{
							if (ch.m_guards[i].m_pivotAlpha + ch.m_pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.m_guards[i].m_pivotAlpha + ch.m_pivotTime, &ch.m_guards[i]));
						index++;
					}
				}
			}
		}
		const long double SAMPLES_PER_BEAT = 2880000.0L / section.m_tempo;
		auto adjustPhrases = [&](Chart* currentChart, const size_t traceIndex, const long& endAlpha, float& angle)
		{
			//Save angle value
			angle = currentChart->m_tracelines[traceIndex].m_angle;
			//Remove trace line
			printf("%s%s: ", g_global.tabs.c_str(), section.m_name);
			if (currentChart->remove(traceIndex, 't'))
				printf("Trace line %zu removed\n", traceIndex);
			if (currentChart->m_tracelines.size())
			{
				//Same idea for phrase bars
				for (size_t phraseIndex = currentChart->m_phrases.size(); phraseIndex > 0;)
				{
					//If the phrase bar is past the trace line
					if (currentChart->m_tracelines[traceIndex].getEndAlpha() <= currentChart->m_phrases[--phraseIndex].m_pivotAlpha)
					{
						//Delete the phrase bar
						if (!currentChart->m_phrases[phraseIndex].m_start)
							currentChart->m_phrases[phraseIndex - 1ULL].m_end = true;
						printf("%s%s: ", g_global.tabs.c_str(), section.m_name);
						if (currentChart->remove(phraseIndex, 'p'))
							printf("Phrase bar %zu removed\n", phraseIndex);
					}
					else
					{
						//If inside the trace line
						if (currentChart->m_tracelines[traceIndex] <= currentChart->m_phrases[phraseIndex])
							currentChart->m_phrases[phraseIndex].changeEndAlpha(endAlpha);
						break;
					}
				}
			}
			else
			{
				for (size_t phraseIndex = currentChart->m_phrases.size(); phraseIndex > 0;)
				{
					printf("%s%s: ", g_global.tabs.c_str(), section.m_name);
					if (currentChart->remove(--phraseIndex, 'p'))
						printf("Phrase bar %zu removed\n", phraseIndex);
				}
			}
		};
		for (size_t pl = 0; pl < (song->m_imc[0] ? 2U : 4U); pl++)
		{
			size_t currentPlayer = pl;
			newCharts[currentPlayer].emplace_back();
			Chart* currentChart = &newCharts[currentPlayer][0];
			bool isPlayer = !(pl & 1) || section.m_swapped >= 4;
			if (notes[pl].size())
			{
				currentChart->clearTracelines();
				for (size_t ntIndex = 0, startingIndex = 0; ntIndex < notes[pl].size(); ntIndex++)
				{
					//If not empty, set pivotAlpha to a value based off the current index
					if (ntIndex == startingIndex)
					{
						if (notes[pl][startingIndex].first >= 2 * SAMPLES_PER_BEAT)
							currentChart->setPivotTime((long)round(notes[pl][startingIndex].first - SAMPLES_PER_BEAT));
						else
							currentChart->setPivotTime(notes[pl][startingIndex].first >> 1);
						notes[pl][startingIndex].second->setPivotAlpha(notes[pl][startingIndex].first - currentChart->m_pivotTime);
					}
					else
						//safety adjustment of the current note's pivot alpha to line up with this chart
						notes[pl][ntIndex].second->setPivotAlpha(notes[pl][ntIndex].first - currentChart->m_pivotTime);
					currentChart->add(notes[pl][ntIndex].second);
					if (ntIndex + 1ULL != notes[pl].size())
					{
						bool makeNewChart = false;
						if (dynamic_cast<Traceline*>(notes[pl][ntIndex].second) != nullptr)	//If the current note is a trace line
						{
							Traceline* tr = static_cast<Traceline*>(notes[pl][ntIndex].second);
							if (dynamic_cast<Traceline*>(notes[pl][ntIndex + 1ULL].second) != nullptr)	//If the next note is a trace line
							{
								//If the trace lines are disconnected
								if (notes[pl][ntIndex].first + (long)tr->m_duration != notes[pl][ntIndex + 1ULL].first)
								{
									currentChart->setEndTime(notes[pl][ntIndex].first + tr->m_duration);
									makeNewChart = true;
								}
							}
							else if (dynamic_cast<Guard*>(notes[pl][ntIndex + 1ULL].second) != nullptr)	//If the next note is a guard mark
							{
								if (notes[pl][ntIndex].first + (long)tr->m_duration <= notes[pl][ntIndex + 1ULL].first)	//Outside the trace line
								{
									if (isPlayer != (song->m_imc[0] && section.m_swapped & 1))	//It's not the enemy's original charts
									{
										//Mark the end of the current chart
										currentChart->setEndTime(notes[pl][ntIndex].first + tr->m_duration);
										makeNewChart = true;
									}
								}
								else	//Inside the trace line
								{
									//If there's another note after the guard mark
									if (ntIndex + 2ULL != notes[pl].size())
									{
										//If said note is a Trace line and has a duration of 1, delete it so it can be replaced
										if (dynamic_cast<Traceline*>(notes[pl][ntIndex + 2ULL].second) != nullptr)
											if (static_cast<Traceline*>(notes[pl][ntIndex + 2ULL].second)->m_duration == 1)
												notes[pl].erase(ntIndex + 2ULL);
									}
									//If it's the enemy's original charts or the next-next note, if it exists, is a guard mark
									if ((isPlayer == (song->m_imc[0] && section.m_swapped & 1)) || (ntIndex + 2ULL != notes[pl].size() && dynamic_cast<Path*>(notes[pl][ntIndex + 2ULL].second) == nullptr))
									{
										long endAlpha = notes[pl][ntIndex + 1ULL].first - SongSection::s_SAMPLE_GAP - 1 - currentChart->m_pivotTime;
										float angle = 0;
										for (size_t t = currentChart->m_tracelines.size(); t > 0;)
										{
											//If the new end point is less than or equal to the last trace line's pivot
											if (!currentChart->m_tracelines[--t].changeEndAlpha(endAlpha))
												adjustPhrases(currentChart, t, endAlpha, angle);
											else
												break;
										}
										if (currentChart->m_tracelines.size())
										{
											Traceline* tr2 = new Traceline(endAlpha, 1, angle);
											currentChart->add(tr2);
											delete tr2;
										}
										if (isPlayer != (song->m_imc[0] && section.m_swapped & 1)) //It's not the enemy's original charts
										{
											//Mark the end of the current chart
											currentChart->setEndTime(notes[pl][ntIndex + 1ULL].first - SongSection::s_SAMPLE_GAP);
											makeNewChart = true;
										}
									}
									else
										notes[pl].erase(ntIndex + 1ULL);
								}
							}
							else if (notes[pl][ntIndex].first + (long)tr->m_duration <= notes[pl][ntIndex + 1ULL].first)	//If the phrase bar is outside the trace line
								notes[pl].erase(ntIndex + 1ULL);
						}
						else if (dynamic_cast<Guard*>(notes[pl][ntIndex].second) != nullptr)	//If the current note is a guard mark
						{
							if (dynamic_cast<Guard*>(notes[pl][ntIndex + 1ULL].second) != nullptr)	//If the next note is a guard mark
							{
								//If there is enough distance between these guard marks in a duet or tutorial stage
								if ((!song->m_imc[0] || song->m_stage == 0 || song->m_stage == 11 || song->m_stage == 12) && notes[pl][ntIndex + 1ULL].first - notes[pl][ntIndex].first >= long(5.5 * SAMPLES_PER_BEAT))
								{
									if (!currentChart->m_tracelines.size())
									{
										//Move chartPivot in between these two notes
										//Calculate the value adjustment
										long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->m_pivotTime;
										//Adjust the chartAlpha
										currentChart->adjustPivotTime(pivotDifference);
										//Adjust the pivot alphas of inserted notes
										for (size_t grdIndex = 0; grdIndex < currentChart->m_guards.size(); grdIndex++)
											currentChart->m_guards[grdIndex].adjustPivotAlpha(-pivotDifference);
									}
									currentChart->setEndTime(notes[pl][ntIndex].first + long(SAMPLES_PER_BEAT));
									makeNewChart = true;
								}
							}
							else if (dynamic_cast<Traceline*>(notes[pl][ntIndex + 1ULL].second) != nullptr)	//If the next note is a trace line
							{
								//If the next-next note is a guard mark
								if (dynamic_cast<Guard*>(notes[pl][ntIndex + 2ULL].second) != nullptr)
								{
									Traceline* tr = static_cast<Traceline*>(notes[pl][ntIndex + 1ULL].second);
									if (ntIndex + 3ULL != notes[pl].size())
									{
										long newFirst = (notes[pl][ntIndex + 3ULL].first + notes[pl][ntIndex + 2ULL].first) >> 1;
										if (tr->changePivotAlpha(tr->m_pivotAlpha + newFirst - notes[pl][ntIndex + 1ULL].first))
										{
											notes[pl][ntIndex + 1ULL].first = newFirst;
											notes[pl].moveElements(ntIndex + 1ULL, ntIndex + 3ULL);
										}
										else
											notes[pl].erase(ntIndex + 1ULL);
									}
									else
										notes[pl].erase(ntIndex + 1ULL);
									//With the next note now being a confirmed guard mark...
									//If there is enough distance between these guard marks in the tutorial or a duet stage
									if ((!song->m_imc[0] || song->m_stage == 0 || song->m_stage == 11 || song->m_stage == 12) && notes[pl][ntIndex + 1ULL].first - notes[pl][ntIndex].first >= long(5.5 * SAMPLES_PER_BEAT))
									{
										if (!currentChart->m_tracelines.size())
										{
											//Move chartPivot in between these two notes
											//Calculate the value adjustment
											long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->m_pivotTime;
											//Adjust the chartAlpha
											currentChart->adjustPivotTime(pivotDifference);
											//Adjust the pivot alphas of inserted notes
											for (size_t grdIndex = 0; grdIndex < currentChart->m_guards.size(); grdIndex++)
												currentChart->m_guards[grdIndex].adjustPivotAlpha(-pivotDifference);
										}
										currentChart->setEndTime(notes[pl][ntIndex].first + long(SAMPLES_PER_BEAT));
										makeNewChart = true;
									}
								}
								else
								{
									if (isPlayer != (song->m_imc[0] && section.m_swapped & 1))	//If it's not the enemy's original charts
									{
										//Move chartPivot in between these two notes
										//Calculate the value adjustment
										long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->m_pivotTime;
										//Adjust the chartAlpha
										currentChart->adjustPivotTime(pivotDifference);
										//Adjust the pivot alphas of inserted notes
										for (size_t grdIndex = 0; grdIndex < currentChart->m_guards.size(); grdIndex++)
											currentChart->m_guards[grdIndex].adjustPivotAlpha(-pivotDifference);
									}
									else
									{
										currentChart->setEndTime((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1);
										makeNewChart = true;
									}
								}
							}
							else if (dynamic_cast<Phrase*>(notes[pl][ntIndex + 1ULL].second) != nullptr)
							{
								//A guard mark followed by a phrase bar is an error
								notes[pl].erase(ntIndex + 1ULL);
							}
						}
						else
						{
							if (dynamic_cast<Guard*>(notes[pl][ntIndex + 1ULL].second) != nullptr)
							{
								if (ntIndex + 2ULL != notes[pl].size())
								{
									//If said note is a Trace line and has a duration of 1, delete it so it can be replaced
									if (dynamic_cast<Traceline*>(notes[pl][ntIndex + 2ULL].second) != nullptr)
										if (static_cast<Traceline*>(notes[pl][ntIndex + 2ULL].second)->m_duration == 1)
											notes[pl].erase(ntIndex + 2ULL);
								}
								//If it's the enemy's original charts or the next-next note, if it exists, is a guard mark
								if ((isPlayer == (song->m_imc[0] && section.m_swapped & 1)) || (ntIndex + 2ULL != notes[pl].size() && dynamic_cast<Path*>(notes[pl][ntIndex + 2ULL].second) == nullptr))
								{
									long endAlpha = notes[pl][ntIndex + 1ULL].first - SongSection::s_SAMPLE_GAP - 1 - currentChart->m_pivotTime;
									float angle = 0;
									for (size_t t = currentChart->m_tracelines.size(); t > 0;)
									{
										//If the new end point is less than or equal to the last trace line's pivot
										if (!currentChart->m_tracelines[--t].changeEndAlpha(endAlpha))
											adjustPhrases(currentChart, t, endAlpha, angle);
										else
										{
											for (size_t phraseIndex = currentChart->m_phrases.size(); phraseIndex > 0;)
											{
												//If the phrase bar is past the trace line
												if (currentChart->m_tracelines[t].getEndAlpha() <= currentChart->m_phrases[--phraseIndex].m_pivotAlpha)
												{
													//Delete the phrase bar
													if (!currentChart->m_phrases[phraseIndex].m_start)
														currentChart->m_phrases[phraseIndex - 1ULL].m_end = true;
													printf("%s%s: ", g_global.tabs.c_str(), section.m_name);
													if (currentChart->remove(phraseIndex, 'p'))
														printf("Phrase bar %zu removed\n", phraseIndex);
												}
												else
												{
													//If inside the trace line
													if (currentChart->m_tracelines[t] <= currentChart->m_phrases[phraseIndex])
														currentChart->m_phrases[phraseIndex].changeEndAlpha(endAlpha);
													break;
												}
											}
											break;
										}
									}
									if (currentChart->m_tracelines.size())
									{
										Traceline* tr = new Traceline(endAlpha, 1, angle);
										currentChart->add(tr);
										delete tr;
									}
									if (isPlayer != (song->m_imc[0] && section.m_swapped & 1)) //It's not the enemy's original charts
									{
										//Mark the end of the current chart
										currentChart->setEndTime(notes[pl][ntIndex + 1ULL].first - SongSection::s_SAMPLE_GAP);
										makeNewChart = true;
									}
								}
								else
									notes[pl].erase(ntIndex + 1ULL);
							}
						}
						if (makeNewChart)
						{
							if (song->m_imc[0] && song->m_stage != 0)
							{
								if (currentPlayer > 1)
									currentPlayer -= 2;
								else
									currentPlayer += 2;
							}
							startingIndex = ntIndex + 1;
							newCharts[currentPlayer].emplace_back();
							currentChart = &newCharts[currentPlayer].back();
							currentChart->clearTracelines();
						}
					}
				}
				if (dynamic_cast<Path*>(notes[pl].back().second) != nullptr)
					currentChart->setEndTime(notes[pl].back().first + static_cast<Path*>(notes[pl].back().second)->m_duration);
				else
					currentChart->setEndTime(notes[pl].back().first);
			}
		}
		section.m_numCharts = 1;
		//Finds the proper new value for "section.numcharts"
		for (size_t pl = 0; pl < 4; pl++)
		{
			size_t perChart = newCharts[pl].size();
			for (size_t ch = 0; ch < newCharts[pl].size() && perChart > section.m_numCharts; ch++)
				if (!(newCharts[pl][ch].m_guards.size() || newCharts[pl][ch].m_phrases.size()) && newCharts[pl][ch].m_tracelines.size() <= 1)
					perChart--;
			if (perChart > section.m_numCharts)
				section.m_numCharts = (unsigned long)perChart;
		}

		section.m_charts.clear();
		section.m_size = unsigned long(64 + 16 * section.m_conditions.size());
		for (unsigned pl = 0; pl < 4; pl++)
		{
			while (newCharts[pl].size() > section.m_numCharts)
				newCharts[pl].pop_back();
			while (newCharts[pl].size() < section.m_numCharts)
				newCharts[pl].emplace_back();
			for (size_t i = 0; i < newCharts[pl].size(); i++)
			{
				section.m_size += newCharts[pl][i].m_size;
				section.m_charts.push_back(newCharts[pl][i]);
			}
		}
		song->m_saved = false;
	}
	else
		ret = false;

	section.m_organized = true;
	song->m_unorganized--;
	return ret;
}

void CHC_Editor::playerSwap(SongSection& section)
{
	if (song->m_imc[0])
	{
		if (section.m_battlePhase != SongSection::Phase::HARMONY && section.m_battlePhase != SongSection::Phase::END)
		{
			for (long playerIndex = section.m_numPlayers - 1; playerIndex > 0; playerIndex -= 2)
				section.m_charts.moveElements((size_t)playerIndex * section.m_numCharts, (playerIndex - 1ULL) * section.m_numCharts, section.m_numCharts);
			if (!(section.m_swapped & 1))
			{
				if (section.m_swapped == 0)
					printf("%s%s: P1/P3 -> P2/P4\n", g_global.tabs.c_str(), section.m_name);
				else if (section.m_swapped == 2)
					printf("%s%s: P3/P1 -> P4/P2\n", g_global.tabs.c_str(), section.m_name);
				else if (section.m_swapped == 4)
					printf("%s%s: P1/P3 -> P2D/P4D (Duet->PS2 conversion)\n", g_global.tabs.c_str(), section.m_name);
				else
					printf("%s%s: P3/P1 -> P4D/P2D (Duet->PS2 conversion)\n", g_global.tabs.c_str(), section.m_name);
				section.m_swapped++;
			}
			else
			{
				if (section.m_swapped == 1)
					printf("%s%s: P2/P4 -> P1/P3\n", g_global.tabs.c_str(), section.m_name);
				else if (section.m_swapped == 3)
					printf("%s%s: P4/P2 -> P3/P1\n", g_global.tabs.c_str(), section.m_name);
				else if (section.m_swapped == 5)
					printf("%s%s: P2D/P4D -> P1/P3 (Duet->PS2 conversion)\n", g_global.tabs.c_str(), section.m_name);
				else
					printf("%s%s: P4D/P2D -> P3/P1 (Duet->PS2 conversion)\n", g_global.tabs.c_str(), section.m_name);
				section.m_swapped--;
			}
		}
		else
		{
			do
			{
				printf("%sHow do you want to swap %s?\n", g_global.tabs.c_str(), section.m_name);
				printf("%sA - Swap P1/P3 with P2/P4 respectively\n", g_global.tabs.c_str());
				printf("%sB - Swap P1/P2 with P3/P4 respectively\n", g_global.tabs.c_str());
				printf("%sCurrent Format: ", g_global.tabs.c_str());
				if (!section.m_swapped)
					printf("P1/P2/P3/P4\n");
				else if (section.m_swapped == 1)
					printf("P2/P1/P4/P3\n");
				else if (section.m_swapped == 2)
					printf("P3/P4/P1/P2\n");
				else if (section.m_swapped == 3)
					printf("P4/P3/P2/P1\n");
				switch (GlobalFunctions::menuChoices("ab"))
				{
				case GlobalFunctions::ResultType::Quit:
					return;
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'a')
					{
						for (long playerIndex = section.m_numPlayers - 1; playerIndex > 0; playerIndex -= 2)
							section.m_charts.moveElements((size_t)playerIndex * section.m_numCharts, (playerIndex - 1ULL) * section.m_numCharts, section.m_numCharts);
						if (!(section.m_swapped & 1))
						{
							if ((section.m_swapped & 2) == 0)
								printf("%s%s: P1/P3 -> P2/P4\n", g_global.tabs.c_str(), section.m_name);
							else
								printf("%s%s: P3/P1 -> P4/P2\n", g_global.tabs.c_str(), section.m_name);
							section.m_swapped++;
						}
						else
						{
							if ((section.m_swapped & 2) == 0)
								printf("%s%s: P2/P4 -> P1/P3\n", g_global.tabs.c_str(), section.m_name);
							else
								printf("%s%s: P4/P2 -> P3/P1\n", g_global.tabs.c_str(), section.m_name);
							section.m_swapped--;
						}
					}
					else
					{
						section.m_charts.moveElements(2ULL * section.m_numCharts, 0, 2ULL * section.m_numCharts);
						if ((section.m_swapped & 2) == 0)
						{
							if (!(section.m_swapped & 1))
								printf("%s%s: P1/P2 -> P3/P4\n", g_global.tabs.c_str(), section.m_name);
							else
								printf("%s%s: P2/P1 -> P4/P3\n", g_global.tabs.c_str(), section.m_name);
							section.m_swapped += 2;
						}
						else
						{
							if (!(section.m_swapped & 1))
								printf("%s%s: P3/P4 -> P1/P2\n", g_global.tabs.c_str(), section.m_name);
							else
								printf("%s%s: P4/P3 -> P2/P1\n", g_global.tabs.c_str(), section.m_name);
							section.m_swapped -= 2;
						}
					}
					g_global.quit = true;
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
	}
	else
	{
		section.m_charts.moveElements(0, 2ULL * section.m_numCharts, section.m_numCharts);
		section.m_charts.moveElements(2ULL * section.m_numCharts, 0, section.m_numCharts);
		if (!section.m_swapped)
		{
			printf("%s%s: P1 -> P3\n", g_global.tabs.c_str(), section.m_name);
			section.m_swapped = 2;
		}
		else
		{
			printf("%s%s: P3 -> P1\n", g_global.tabs.c_str(), section.m_name);
			section.m_swapped = 0;
		}

	}
}

void CHC_Editor::changeName(SongSection& section)
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Name Change ");
	printf("%sProvide a new name for this section (16 character max) ('Q' to back out to the Section Menu)\n", g_global.tabs.c_str());
	printf("%sInput: ", g_global.tabs.c_str());
	char newName[17];
	if (GlobalFunctions::charArrayInsertion(newName, 16) == GlobalFunctions::ResultType::Success)
	{
		if (strcmp(section.m_name, newName))
		{
			memcpy_s(section.m_name, 17, newName, 16);
			song->m_saved = false;
		}
	}
}

void CHC_Editor::changeAudio(SongSection& section)
{
	GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Audio Change ");
	printf("%sProvide the name for the section of audio you want to use (16 character max) ('Q' to back out to the Section Menu)\n", g_global.tabs.c_str());
	printf("%sInput: ", g_global.tabs.c_str());
	char newAudio[17];
	if (GlobalFunctions::charArrayInsertion(newAudio, 16) == GlobalFunctions::ResultType::Success)
	{
		if (strcmp(section.m_audio, newAudio))
		{
			memcpy_s(section.m_audio, 17, newAudio, 16);
			song->m_saved = false;
		}
	}
}

void CHC_Editor::changeFrames(SongSection& section)
{
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - SSQ Starting Index ");
		printf("%sProvide a value for the starting SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal\n", g_global.tabs.c_str());
		printf("%sCurrent Value for Starting Index: %g\n", g_global.tabs.c_str(), section.m_frames.first );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldFirst = section.m_frames.first;
		switch (GlobalFunctions::valueInsert(section.m_frames.first, false, "u"))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (section.m_frames.first != oldFirst)
				song->m_saved = false;
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
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - SSQ Ending Index ");
		printf("%sProvide a value for the Ending SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal\n", g_global.tabs.c_str());
		printf("%sCurrent Value for Ending Index: %g\n", g_global.tabs.c_str(), section.m_frames.last );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldLast = section.m_frames.last;
		switch (GlobalFunctions::valueInsert(section.m_frames.last, false, "u"))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (section.m_frames.last != oldLast)
				song->m_saved = false;
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

void CHC_Editor::switchPhase(SongSection& section)
{
	string choices = "";
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Phase Selection ");
		printf("%sSelect a phase type for this section\n", g_global.tabs.c_str());
		printf("%sCurrent Phase: ", g_global.tabs.c_str());
		switch (section.m_battlePhase)
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
				g_global.answer.index++;
			if (section.m_battlePhase != static_cast<SongSection::Phase>(g_global.answer.index))
			{
				section.m_battlePhase = static_cast<SongSection::Phase>(g_global.answer.index);
				song->m_saved = false;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void CHC_Editor::adjustTempo(SongSection& section)
{
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Tempo Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit tempo settings]\n", g_global.tabs.c_str());
		printf("%sCan be a decimal... and/or negative with weird effects on PSP\n", g_global.tabs.c_str());
		printf("%sCurrent Value: %g\n", g_global.tabs.c_str(), section.m_tempo );
		printf("%sInput: ", g_global.tabs.c_str());
		float oldTempo = section.m_tempo;
		switch (GlobalFunctions::valueInsert(section.m_tempo, true))
		{
		case GlobalFunctions::ResultType::Success:
			if (section.m_tempo != oldTempo)
				song->m_saved = false;
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

void CHC_Editor::adjustDuration(SongSection& section)
{
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - " + section.m_name + " - Duration Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit duration settings]\n", g_global.tabs.c_str());
		printf("%sCurrent Value: %lu\n", g_global.tabs.c_str(), section.m_duration );
		printf("%sInput: ", g_global.tabs.c_str());
		unsigned long oldDuration = section.m_duration;
		switch (GlobalFunctions::valueInsert(section.m_duration, false))
		{
		case GlobalFunctions::ResultType::Success:
			if (section.m_duration != oldDuration)
				song->m_saved = false;
			break;
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
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

void CHC_Editor::conditionMenu(SongSection& section)
{
	do
	{
		GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Section " + section.m_name + " - Condition Selection ", 1.5);
		printf(" i ||Condition Type || Argument ||      True Effect      ||      False Effect     ||\n");
		printf("==================================================================================||\n");
		for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
		{
			printf("%s %zu || ", g_global.tabs.c_str(), condIndex);
			switch (section.m_conditions[condIndex].m_type)
			{
			case 0: printf("Unconditional ||   n/a    || "); break;
			case 1: printf("Left Side < # ||   %-4g   || ", section.m_conditions[condIndex].m_argument); break;
			case 2: printf("Right Side < #||   %-4g   || ", section.m_conditions[condIndex].m_argument); break;
			case 3: printf("Random < #    ||   %-4g   || ", section.m_conditions[condIndex].m_argument); break;
			case 4: printf("No Player 3?  ||   n/a    || "); break;
			case 5: printf("No Player 4?  ||   n/a    || "); break;
			case 6: printf("Left < Right? ||   n/a    || "); break;
			}
			if (section.m_conditions[condIndex].m_trueEffect >= 0)
				printf("Go to Section %7s || ", song->m_sections[section.m_conditions[condIndex].m_trueEffect].m_name);
			else
				printf("Go to Condition %5zu || ", condIndex - section.m_conditions[condIndex].m_trueEffect);
			if (section.m_conditions[condIndex].m_type > 0)
			{
				if (section.m_conditions[condIndex].m_falseEffect >= 0)
					printf("Go to Section %7s ||\n", song->m_sections[section.m_conditions[condIndex].m_falseEffect].m_name);
				else
					printf("Go to Condition %5zu ||\n", condIndex - section.m_conditions[condIndex].m_falseEffect);
			}
			else
				printf("         n/a         ||\n");
		}
		printf("==================================================================================||\n");
		printf("%sType the number for the condition that you wish to edit\n", g_global.tabs.c_str());
		size_t val;
		switch (GlobalFunctions::valueInsert(val, false, size_t(0), section.m_conditions.size() - 1))
		{
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::InvalidNegative:
			printf("%sGiven section value must be positive.\n", g_global.tabs.c_str());
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::MaxExceeded:
			printf("%sGiven section value cannot exceed %zu\n", g_global.tabs.c_str(), song->m_sections.size() - 1 );
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
				GlobalFunctions::banner(" " + song->m_shortname + ".CHC - Section " + section.m_name + " - Condition " + to_string(val) + ' ', 1.5);
				string choices = "ct";
				printf("%sc - Type: ", g_global.tabs.c_str());
				switch (section.m_conditions[val].m_type)
				{
				case 0: printf("Unconditional\n"); break;
				case 1: printf("Left Side < #\n"); break;
				case 2: printf("Right Side < #\n"); break;
				case 3: printf("Random < #\n"); break;
				case 4: printf("No Player 3?\n"); break;
				case 5: printf("No Player 4?\n"); break;
				case 6: printf("Left < Right?\n"); break;
				}
				if (section.m_conditions[val].m_type > 0 && section.m_conditions[val].m_type < 4)
				{
					printf("%sA - Argument: %g\n", g_global.tabs.c_str(), section.m_conditions[val].m_argument );
					choices += 'a';
				}
				printf("%sT - True Effect: ", g_global.tabs.c_str());
				if (section.m_conditions[val].m_trueEffect >= 0)
					printf("Go to Section %s\n", song->m_sections[section.m_conditions[val].m_trueEffect].m_name );
				else
					printf("Go to Condition %zu\n", val - section.m_conditions[val].m_trueEffect );
				if (section.m_conditions[val].m_type > 0)
				{
					printf("%sF - False Effect: ", g_global.tabs.c_str());
					if (section.m_conditions[val].m_falseEffect >= 0)
						printf("Go to Section %s\n", song->m_sections[section.m_conditions[val].m_falseEffect].m_name );
					else
						printf("Go to Condition %zu\n", val - section.m_conditions[val].m_falseEffect );
					choices += 'f';
				}
				if (section.m_conditions.size() > 1)
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
						changeName(song->m_sections[val]);
						break;
					case 'a':
						changeAudio(song->m_sections[val]);
						break;
					case 't':
						changeFrames(song->m_sections[val]);
						break;
					case 'f':
						changeFrames(song->m_sections[val]);
						break;
					case 'd':
						adjustTempo(song->m_sections[val]);
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

void CHC_Editor::conditionDelete(SongSection& section, size_t index)
{
	LinkedList::List<SongSection::Condition> oldcond = section.m_conditions;
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
}
