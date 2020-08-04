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
		printf("%sQuickFix - Exception Caught: %s\n", global.tabs.c_str(), e.what());
		return false;
	}
}

void CHC_Editor::editSong(const bool multi)
{
	do
	{
		banner(" " + song->shortname + ".CHC - Editor ");
		//Holds addresses to the various functions that can be chosen in this menu prompt
		List<void(CHC_Editor::*)()> functions = { &CHC_Editor::audioSettings, &CHC_Editor::winLossSettings,
											   &CHC_Editor::adjustSpeed, &CHC_Editor::sectionMenu, &CHC_Editor::adjustFactors };
		string choices = "vagcd";
		if (!song->optimized)
		{
			choices += 'f'; functions.push_back(&CHC_Editor::fixNotes);
			printf("%sF - Fix any problematic notes or trace lines\n", global.tabs.c_str());
		}
		if (song->unorganized)
		{
			choices += 'o'; functions.push_back(&CHC_Editor::organizeAll);
			printf("%sO - Organize All Unorganized Sections\n", global.tabs.c_str());
		}
		if (strstr(song->imc, ".IMC"))
		{
			choices += 'i'; functions.push_back(&CHC_Editor::swapIMC);
			printf("%sI - Swap IMC file (Current file: %s)\n", global.tabs.c_str(), song->imc + 34);
		}
		else
		{
			choices += 'p'; functions.push_back(&CHC_Editor::PSPToPS2);
			printf("%sP - Convert for PS2-Version Compatibility\n", global.tabs.c_str());
		}
		printf("%sV - Volume & Pan Settings\n", global.tabs.c_str());
		printf("%sA - Win/Loss Animations\n", global.tabs.c_str());
		printf("%sG - Adjust Gameplay Speed: %g\n", global.tabs.c_str(), song->speed );
		printf("%sC - SongSections (%zu)\n", global.tabs.c_str(), song->sections.size());
		printf("%sD - Player Damage/Energy Factors\n", global.tabs.c_str());
		printf("%s? - Help info\n", global.tabs.c_str());
		if (multi)
			printf("%sQ - Close this file\n", global.tabs.c_str());
		else
			printf("%sQ - Exit Detail Editor\n", global.tabs.c_str());
		size_t result = menuChoices(choices, true);
		switch (result)
		{
		case 'q':
			global.quit = true;
			break;
		case '?':
			printf("%s\n", global.tabs.c_str());
			printf("%sS - Save File:\n", global.tabs.c_str());
			printf("%sPretty self-explanatory - save the file as an overwrite or as a new file.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			if (strstr(song->imc, ".IMC"))
			{
				printf("%sI - Swap IMC file (Current file: %s):\n", global.tabs.c_str(), song->imc + 34);
				printf("%sPS2 compatible CHC files contain a string that points to the location of the .IMC audio file used for the stage.\n", global.tabs.c_str());
				printf("%sUse this option to change which specific .IMC file the CHC will point to;\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}
			else
			{
				printf("%sP - Convert for PS2-Version Compatibility:\n", global.tabs.c_str());
				printf("%sUse this option to convert this (assumed) duet-made CHC into a PS2 compatible CHC. Doing so will open up a few other features\n", global.tabs.c_str());
				printf("%s- most prominently being TASing capability - at the cost of one of the player tracks: Player 1, Player 2, or the Enemy.\n", global.tabs.c_str());
				printf("%sThis will also reorganize all unorganized sections for optimal compatibility.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}
			printf("%sV - Volume & Pan Settings:\n", global.tabs.c_str());
			printf("%sAllows you to adjust the volume and pan of the 8 available audio channels.\n", global.tabs.c_str());
			printf("%sNote that you can change channel 1 & 2 and channel pan altogether only if the CHC in question is PS2 compatible.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sA - Win/Loss Animations:\n", global.tabs.c_str());
			printf("%sEvery stage in the game has a set of win and loss animations. These animations start at set points in a stage's frame data (located in the stage's SSQ file).\n", global.tabs.c_str());
			printf("%sThis option will allow you to change what frame any of the four animations (two wins, two losses) start on - although you'll most likely never use this.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sG - Adjust Gameplay Speed (Current speed: %g):\n", global.tabs.c_str(), song->speed);
			printf("%sThe speed of which trace lines, phrae bars, and guard marks approach the middle of the screen is determined by this value.\n", global.tabs.c_str());
			printf("%sThis option will allow you to adjust the value to whatever you want (negative even). Note: lower = faster; higher = slower.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sC - SongSections (%zu):\n", global.tabs.c_str(), song->sections.size());
			printf("%sIn the future, this option will be the gateway to digging into the nitty gritty of each section. It will allow you manually adjust a wide range\n", global.tabs.c_str());
			printf("%sof parameters, including a section's duration, conditions, and phase type while also allowing for manual adjustments on subsections and\n", global.tabs.c_str());
			printf("%stheir trace lines, phrase bars, and guard marks. Currently, however, it will give you a choice of two features:\n", global.tabs.c_str());
			printf("%s1. If there are any sections still unorganized, it will give you the option to reorganize every section that needs it.\n", global.tabs.c_str());
			printf("%s2. Change the order of which sections will proceed in gameplay using unconditional conditions. Useful for when you need to test only certain sections.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sF - Player Damage/Energy Factors:\n", global.tabs.c_str());
			printf("%sFor each player separately, each phase type has values pertaining to 8 different HP related factors.\n", global.tabs.c_str());
			printf("%sThis will bring up a series of menus so that you can edit any factors you like - although some factors have no effect in certain phase types.\n", global.tabs.c_str());
			printf("%s(No dealing or receiving damage in charge phases for example).\n%s\n", global.tabs.c_str(), global.tabs.c_str());
		case '*':
			break;
		default:
			global.adjustTabs(2);
			(this->*functions[result])();
			global.adjustTabs(1);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::organizeAll()
{
	banner(" " + song->shortname + ".CHC - Complete Reorganization ");
	for (size_t sectIndex = 0; song->unorganized && sectIndex < song->sections.size(); sectIndex++)
	{
		if (!song->sections[sectIndex].organized)
			reorganize(song->sections[sectIndex]);
	}
	printf("%sAll sections organized\n", global.tabs.c_str());
}

void CHC_Editor::fixNotes()
{
	banner(" Fixing " + song->shortname + ".CHC ");
	size_t tracelinesCurved = 0, tracelinesStraightened = 0, tracelinesDeleted = 0, phrasesDeleted = 0, phrasesShortened = 0, guardsDeleted = 0;
	song->optimized = true;
	try
	{
		for (size_t sectIndex = 0; sectIndex < song->sections.size(); sectIndex++) //SongSections
		{
			SongSection& section = song->sections[sectIndex];
			if (section.battlePhase == SongSection::Phase::INTRO || (section.battlePhase == SongSection::Phase::BATTLE && !strstr(section.name, "BRK") && !strstr(section.name, "BREAK")))
				continue;
			for (size_t playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
			{
				for (size_t chartIndex = 0; chartIndex < section.numCharts; chartIndex++)
				{
					Chart& chart = section.charts[playerIndex * section.numCharts + chartIndex];
					size_t barsRemoved = 0, linesRemoved = 0;
					try
					{
						for (size_t traceIndex = 0, phraseIndex = 0; traceIndex < chart.tracelines.size() - 1; traceIndex++)
						{
							Traceline* trace = &chart.tracelines[traceIndex];
							bool adjust = trace->angle == chart.tracelines[traceIndex + 1].angle;
							for (; phraseIndex < chart.phrases.size(); phraseIndex++)
							{
								Phrase& phrase = chart.phrases[phraseIndex];
								if (phrase.pivotAlpha >= trace->getEndAlpha())
									break;
								if (phrase.start)
								{
									if (phraseIndex && phrase.pivotAlpha - chart.phrases[phraseIndex - 1].getEndAlpha() < SongSection::SAMPLE_GAP)
									{
										chart.phrases[phraseIndex - 1].changeEndAlpha((long)round(phrase.pivotAlpha - SongSection::SAMPLE_GAP));
										printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
										printf("Phrase bar %zu shortened\n", phraseIndex + barsRemoved - 1);
										++phrasesShortened;
									}
									while (!phrase.end && (phrase.pivotAlpha >= trace->getEndAlpha() - 3000)
										&& (phrase.pivotAlpha - trace->pivotAlpha > 800))
									{
										phrase.duration += chart.phrases[phraseIndex + 1].duration;
										phrase.end = chart.phrases[phraseIndex + 1].end;
										memcpy_s(phrase.junk, 12, chart.phrases[phraseIndex + 1].junk, 12);
										if (!trace->changeEndAlpha(phrase.pivotAlpha))
										{
											printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
											chart.remove(traceIndex, 't', linesRemoved);
											++linesRemoved;
										}
										else
											++traceIndex;
										trace = &chart.tracelines[traceIndex];
										trace->changePivotAlpha(phrase.pivotAlpha);
										adjust = traceIndex + 1 < chart.tracelines.size() && trace->angle == chart.tracelines[traceIndex + 1].angle;
										printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
										chart.remove(phraseIndex + 1, 'p', barsRemoved);
										++barsRemoved;
									}
									if (adjust && !trace->curve && song->imc[0])
									{
										if (phrase.pivotAlpha >= trace->getEndAlpha() - 8000)
										{

											trace->curve = true;
											printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
											printf("Trace line %zu set to \"smooth\"\n", traceIndex + linesRemoved);
											++tracelinesCurved;
											adjust = false;
										}
									}
								}
							}
							if (adjust && trace->curve)
							{
								trace->curve = false;
								printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
								printf("Trace line %zu set to \"rigid\"\n", traceIndex);
								++tracelinesStraightened;
							}
						}
					}
					catch (...)
					{
						printf("%sThere was an error when attempting to apply phrase bar & trace line fixes to section #%zu (%s) - Subsection %zu\n", global.tabs.c_str(), sectIndex, section.name, playerIndex * section.numCharts + chartIndex);
						song->optimized = false;
					}
					tracelinesDeleted += linesRemoved;
					phrasesDeleted += barsRemoved;
					for (size_t guardIndex = 0; guardIndex < chart.guards.size(); guardIndex++)
					{
						while (guardIndex + 1 < chart.guards.size() && chart.guards[guardIndex].pivotAlpha + 1600 > chart.guards[guardIndex + 1].pivotAlpha)
						{
							printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
							chart.remove(guardIndex + 1, 'g', guardsDeleted);
							++guardsDeleted;
						}
					}
				}
			}
			if (section.battlePhase == SongSection::Phase::END || sectIndex + 1 == song->sections.size())
				continue;
			if (!song->imc[0] || section.battlePhase == SongSection::Phase::HARMONY)
			{
				for (size_t playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
				{
					for (size_t chartIndex = section.numCharts; chartIndex > 0;)
					{
						if (section.charts[playerIndex * section.numCharts + (--chartIndex)].getNumPhrases() > 0)
						{
							for (size_t condIndex = 0; condIndex < section.conditions.size(); condIndex++)
							{
								long* effect = &section.conditions[condIndex].trueEffect;
								for (size_t i = 0; i < 2; i++, effect++)
								{
									if (*effect >= 0)
									{
										Chart& chart = section.charts[playerIndex * section.numCharts + chartIndex];
										long endTime = section.duration - (chart.phrases[chart.phrases.size() - 1].getEndAlpha() + chart.pivotTime);
										Chart& chart2 = song->sections[*effect].charts[playerIndex * song->sections[*effect].numCharts];
										if (chart2.phrases.size())
										{
											long startTime = chart2.phrases[0].pivotAlpha + chart2.pivotTime;
											if (startTime + endTime < section.SAMPLE_GAP)
											{
												chart.phrases[chart.phrases.size() - 1].changeEndAlpha((long)(section.duration - startTime - section.SAMPLE_GAP - chart.pivotTime));
												printf("%s%s - Subsection %zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
												printf("Phrase bar %zu shortened\n", chart.phrases.size() - 1);
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
					for (size_t chartIndex = section.numCharts; !global.quit && chartIndex > 0;)
					{
						--chartIndex;
						for (size_t playerIndex = section.numPlayers + p; !global.quit && playerIndex > 1;)
						{
							playerIndex -= 2;
							if (section.charts[(playerIndex) * section.numCharts + chartIndex].phrases.size() != 0)
							{
								for (size_t condIndex = 0; condIndex < section.conditions.size(); condIndex++)
								{
									long* effect = &section.conditions[condIndex].trueEffect;
									for (size_t i = 0; i < 2; i++, effect++)
									{
										if (*effect >= 0)
										{
											Chart& chart = section.charts[playerIndex * section.numCharts + chartIndex];
											long endTime = section.duration - (chart.phrases[chart.phrases.size() - 1].getEndAlpha() + chart.pivotTime);
											Chart& chart2 = song->sections[*effect].charts[p * song->sections[*effect].numCharts];
											if (chart2.phrases.size())
											{
												long startTime = chart2.phrases[0].pivotAlpha + chart2.pivotTime;
												if (startTime + endTime < section.SAMPLE_GAP)
												{
													chart.phrases[chart.phrases.size() - 1].changeEndAlpha(long(section.duration - startTime - section.SAMPLE_GAP - chart.pivotTime));
													printf("%s%s - Subsection #%zu: ", global.tabs.c_str(), section.name, playerIndex * section.numCharts + chartIndex);
													printf("Phrase bar %zu shortened\n", chart.phrases.size() - 1);
													phrasesShortened++;
												}
											}
										}
									}
								}
								global.quit = true;
							}
						}
					}
					global.quit = false;
				}
			}
		}
	}
	catch (...)
	{
		printf("%sUnknown error occurred when applying note fixes. Thus, the chc will remain unoptimized for now.\n", global.tabs.c_str());
		song->optimized = false;
	}
	printf("%s\n", global.tabs.c_str());
	if (tracelinesCurved)
		printf("%s%zu%s\n", global.tabs.c_str(), tracelinesCurved, (tracelinesCurved > 1 ? " rigid Trace lines set to \"smooth\"" : " rigid Trace line set to \"smooth\""));
	if (tracelinesStraightened)
		printf("%s%zu%s\n", global.tabs.c_str(), tracelinesStraightened, (tracelinesStraightened > 1 ? " smooth Trace lines set to \"rigid\"" : " smooth Trace line set to \"rigid\""));
	if (tracelinesDeleted)
		printf("%s%zu%s deleted to accommodate deleted phrase bars\n", global.tabs.c_str(), tracelinesDeleted, (tracelinesDeleted > 1 ? " Trace lines" : " Trace line"));
	if (phrasesDeleted)
		printf("%s%zu%s deleted for starting with durations under 3000 samples\n", global.tabs.c_str(), phrasesDeleted, (phrasesDeleted > 1 ? " Phrase bars" : " Phrase bar"));
	if (phrasesShortened)
		printf("%s%zu%s Phrase Bars shortened for ending too close to following Phrase Bars\n", global.tabs.c_str(), phrasesShortened, (phrasesShortened > 1 ? " Phrase bars" : " Phrase bar"));
	if (guardsDeleted)
		printf("%s%zu%s deleted for being within 1600 samples of a preceeding Guard Mark\n", global.tabs.c_str(), guardsDeleted, (guardsDeleted > 1 ? " Guard marks" : " Guard mark"));
	if (tracelinesStraightened || tracelinesCurved || phrasesDeleted || phrasesShortened || guardsDeleted)
	{
		printf("%s\n%sChanges will be applied if you choose to save the file\n", global.tabs.c_str(), global.tabs.c_str());
		song->saved = false;
	}
	else
		printf("%sNo changes made\n", global.tabs.c_str());
}

void CHC_Editor::PSPToPS2()
{
	banner(" " + song->shortname + ".CHC - Non-Duet Conversion ");
	bool player2 = false, enemy = false;
	do
	{
		printf("%sWhich players do you want to keep?\n", global.tabs.c_str());
		printf("%s1 - Player 1 & Enemy\n", global.tabs.c_str());
		printf("%s2 - Player 1 & Player 2\n", global.tabs.c_str());
		printf("%s3 - Player 2 & Enemy\n", global.tabs.c_str());
		size_t value = menuChoices("123");
		switch (value)
		{
		case 'q':
			return;
		case '?':
		case '*':
			break;
		default:
			player2 = value == '3';
			enemy = value == '1' || value == '3';
			song->name = song->name.substr(0, song->name.length() - 4) + "_PS2_" + to_string(player2 + 1) + 'v' + (enemy ? 'E' : '2') + ".CHC";
			{
				size_t pos = song->name.find_last_of("\\");
				if (pos != string::npos)
					song->shortname = song->name.substr(pos + 1, song->name.length() - pos - 5);
				else
					song->shortname = song->shortname.substr(0, song->shortname.length() - 4);
			}
			global.quit = true;
		}
	} while (!global.quit);
	global.quit = false;
	snprintf(song->imc, 44, "/PROJECTS/STDATA/STAGE%02d/SONGDATA/ST%02d.IMC", song->stage, song->stage);
	song->audio[2].pan = song->audio[3].pan = song->audio[6].volume = song->audio[7].volume = 0;
	song->audio[4].pan = song->audio[5].pan = 32767;
	for (size_t i = 0; i < song->sections.size(); i++)
	{
		SongSection& section = song->sections[i];
		if (!section.organized) //Organized as a duet-made section
			reorganize(section);
		section.charts.erase(3ULL * section.numCharts, section.numCharts);
		if (player2 || !enemy)
			section.charts.moveElements(2ULL * section.numCharts, section.numCharts, section.numCharts);
		if (!player2)
			section.charts.erase(2ULL * section.numCharts, section.numCharts);
		else
			section.charts.erase(0, section.numCharts);
		if (section.numCharts & 1)
		{
			section.charts.emplace_back();
			section.charts.emplace(section.numCharts, 1);
			section.numCharts++;
		}
		for (size_t c = 1; c < section.numCharts >> 1; c++)
		{
			section.charts.moveElements(c << 1, c);
			section.charts.moveElements((c << 1) + section.numCharts, c + section.numCharts);
		}
		section.numCharts >>= 1;
		section.charts.moveElements((size_t)section.numCharts << 1, section.numCharts, section.numCharts);
		if (!enemy)
			section.swapped = (section.swapped >> 1) + 4;
	}
	for (size_t s = 0; s < 5; s++)
	{
		song->energyDamageFactors[0][s].attackInitial *= 2;
		song->energyDamageFactors[0][s].attackRelease *= 2;
		song->energyDamageFactors[0][s].attackMiss *= 2;
		song->energyDamageFactors[0][s].chargeInitial *= 2;
		song->energyDamageFactors[0][s].chargeRelease *= 2;
		song->energyDamageFactors[0][s].guardMiss *= 2;
	}
	song->saved = false;
	song->optimized = false;
}

void CHC_Editor::swapIMC()
{

	do
	{
		banner(" " + song->shortname + ".CHC - IMC Swap ");
		printf("%sProvide the name (just the name) of the .IMC file you wish to use (Or 'Q' to back out to Modify Menu)\n", global.tabs.c_str());
		printf("%sCurrent IMC File: %s\n", global.tabs.c_str(), song->imc);
		printf("%sInput: ", global.tabs.c_str());
		char newIMC[257];
		scanf_s(" %[^\n;]", newIMC, 256);
#pragma warning(suppress : 6031)
		getchar();
		if ((newIMC[1] != '\n' && newIMC[1] != '\0' && newIMC[1] != ';') || (newIMC[0] != 'q' && newIMC[0] != 'Q'))
		{
			do
			{
				printf("%sIs this correct?: /PROJECTS/STDATA/STAGE00/SONGDATA/%s [Y/N]\n", global.tabs.c_str(), newIMC);
				if (!strstr(newIMC, ".IMC"))
					printf("%s\".IMC\" will be added afterwards.\n", global.tabs.c_str());
				switch (menuChoices("yn"))
				{
				case 'y':
				{
					string strIMC = newIMC;
					if (strIMC.find(".IMC") == string::npos)
						strIMC += ".IMC";
					for (size_t i = 0; i < strIMC.length(); i++)
						song->imc[34 + i] = toupper(strIMC[i]);
					song->imc[34 + strIMC.length()] = 0;
					song->saved = 0;
					return;
					break;
				}
				case 'q':
					return;
				case 'n':
					global.quit = true;
				}
			} while (!global.quit);
			global.quit = false;
		}
		else
			global.quit = true;
	} while (!global.quit);
}

void CHC_Editor::audioSettings()
{
	size_t channel;
	do
	{
		if (!song->imc[0])
		{
			banner(" " + song->shortname + ".CHC - Audio Channels ");
			printf("%s          ||        Volume        ||        Paning        ||\n", global.tabs.c_str());
			printf("%s Channels ||  Value  ||  Percent  ||   Left   |   Right   ||\n", global.tabs.c_str());
			printf("%s==========================================================||\n", global.tabs.c_str());
			for (size_t index = 0; index < 8; index++)
			{
				printf("%s     %zu    ||", global.tabs.c_str(), index + 1);
				printf("%7lu  ||%7g%%   ||", song->audio[index].volume, ((double)song->audio[index].volume) * 100 / 32767);
				switch (song->audio[index].pan)
				{
				case 0:     printf("   Left   |           ||\n"); break;
				case 16383: printf("        Center        ||\n"); break;
				case 32767: printf("          |   Right   ||\n"); break;
				default: printf("%8g%% |%8g%% ||\n", 100 - (((double)song->audio[index].pan) * 100 / 32767), ((double)song->audio[index].pan) * 100 / 32767);
				}
			}
			printf("%s==========================================================||\n", global.tabs.c_str());
			printf("%sChoose the channel you wish to edit [Type 'Q' to exit audio settings]\n", global.tabs.c_str());
			channel = menuChoices("12345678", true);
		}
		else
		{
			banner(" " + song->shortname + ".CHC - Audio Channels [Duet Stages] ");
			printf("%s          ||        Volume        ||\n", global.tabs.c_str());
			printf("%s Channels ||  Value  ||  Percent  ||\n", global.tabs.c_str());
			printf("%s==================================||\n", global.tabs.c_str());
			printf("%s   1 & 2  ||        Unused        ||\n", global.tabs.c_str());
			for (size_t index = 2; index < 8; index++)
			{
				printf("%s     %zu    ||", global.tabs.c_str(), index + 1);
				printf("%7lu  ||%7g%%   ||", song->audio[index].volume, ((double)song->audio[index].volume) * 100 / 32767);
			}
			printf("%s==================================||\n", global.tabs.c_str());
			printf("%sChoose the channel you wish to edit [Type 'Q' to exit audio settings]\n", global.tabs.c_str());
			channel = menuChoices("345678", true);
		}
		switch (channel)
		{
		case 'q':
			global.quit = true;
			break;
		case '?':
		case '*':
			break;
		default:
			if (!song->imc[0])
				channel += 2;
			do
			{
				banner(" " + song->shortname + ".CHC - Audio Channel " + to_string(channel + 1));
				string choices = "vb";
				printf("%sV - Volume\n", global.tabs.c_str());
				if (!song->imc[0])
				{
					printf("%sP - Panning\n", global.tabs.c_str());
					choices += 'p';
				}
				printf("%sB - Choose Another Channel\n", global.tabs.c_str());
				printf("%sQ - Exit Audio Settings\n", global.tabs.c_str());
				switch (menuChoices(choices))
				{
				case 'v':
					do
					{
						banner(" " + song->shortname + ".CHC - Audio Channel " + to_string(channel + 1) + " - Volume");
						printf("%sProvide value for Volume (0 - 32767[100%%]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n", global.tabs.c_str());
						printf("%sCurrent Volume: %lu (%g%%)\n", global.tabs.c_str(), song->audio[channel].volume, 100.0 * song->audio[channel].volume / 32767);
						printf("%sInput: ", global.tabs.c_str());
						unsigned long oldVol = song->audio[channel].volume;
						switch (valueInsert(song->audio[channel].volume, false, 0UL, 32767UL, "b"))
						{
						case 'b':
							global.quit = true;
							break;
						case '!':
							if (song->audio[channel].volume != oldVol)
								song->saved = false;
							break;
						case 'q':
							return;
						case '-':
						case '>':
							printf("%sValue must be between 0 & 32767. Not adjusting.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
							clearIn();
							break;
						case '*':
							printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
							clearIn();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				case 'p':
					do
					{
						banner(" " + song->shortname + ".CHC - Audio Channel " + to_string(channel + 1) + " - Panning");
						printf("%sProvide value for Panning (Left[0] - Center[16383] - Right[32767]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n", global.tabs.c_str());
						printf("%sCurrent Panning: ", global.tabs.c_str());
						unsigned long oldPan = song->audio[channel].pan;
						switch (song->audio[channel].pan)
						{
						case 0: printf("Left (0)\n"); break;
						case 16383: printf("Center (16383)\n"); break;
						case 32767: printf("Right (32767)\n"); break;
						default: printf("%g%% Left | %g%% Right (%lu)\n", 100 - (song->audio[channel].pan * 100.0 / 32767), song->audio[channel].pan * 100.0 / 32767, song->audio[channel].pan);
						}
						printf("%sInput: ", global.tabs.c_str());
						switch (valueInsert(song->audio[channel].pan, false, 0UL, 32767UL, "b"))
						{
						case 'b':
							global.quit = true;
							break;
						case '!':
							if (song->audio[channel].pan != oldPan)
								song->saved = false;
							break;
						case 'q':
							return;
						case '-':
						case '>':
							printf("%sValue must be between 0 & 32767. Not adjusting.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
							clearIn();
							break;
						case '*':
							printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
							clearIn();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				case 'b':
					global.quit = true;
					break;
				case 'q':
					return;
				case '?':
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::winLossSettings()
{
	do
	{
		banner(" " + song->shortname + ".CHC - Win/Loss Animations ");
		static const string ssqStrings[] = { "Win A", "Win B", "Lost Pre", "Lost Post" };
		printf("%s            ||       Frames       ||\n", global.tabs.c_str());
		printf("%s Animations ||  First  ||   Last  ||\n", global.tabs.c_str());
		printf("%s==================================||\n", global.tabs.c_str());
		printf("%s 1 - Win A  ||%7g  ||%7g  ||\n", global.tabs.c_str(), song->events[0].first, song->events[0].last);
		printf("%s 2 - Win B  ||%7g  ||%7g  ||\n", global.tabs.c_str(), song->events[1].first, song->events[1].last);
		printf("%s3 - Lost Pre||%7g  ||%7g  ||\n", global.tabs.c_str(), song->events[2].first, song->events[2].last);
		printf("%s4 - Lt. Loop||%7g  ||%7g  ||\n", global.tabs.c_str(), song->events[3].first, song->events[3].last);
		printf("%s\n\t\t      ||Select an SSQ event ('1'/'2'/'3'/'4') [Type 'Q' to exit win/loss anim. settings]\n", global.tabs.c_str());
		size_t anim = menuChoices("1234", true);
		switch (anim)
		{
		case 'q':
			global.quit = true;
			break;
		case '?':
			printf("%s\n", global.tabs.c_str());
		case '*':
			break;
		default:
			do
			{
				banner(" " + song->shortname + ".CHC - SSQ Animation:" + ssqStrings[anim]);
				printf("%sF - Adjust first frame: %g\n", global.tabs.c_str(), song->events[anim].first );
				printf("%sL - Adjust last  frame: %g\n", global.tabs.c_str(), song->events[anim].last );
				printf("%sB - Choose different animation\n", global.tabs.c_str());
				printf("%sQ - Exit win/loss animation settings \n", global.tabs.c_str());
				switch (menuChoices("flb"))
				{
				case 'q':
					return;
				case '?':
					break;
				case 'b':
					global.quit = true;
					break;
				case 'f':
					do
					{
						banner(" " + song->shortname + ".CHC - SSQ Animation: " + ssqStrings[anim] + " - First Frame");
						printf("%sEnter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n", global.tabs.c_str());
						printf("%s(Can be a decimal)\n", global.tabs.c_str());
						float oldAnim = song->events[anim].first;
						switch (valueInsert(song->events[anim].first, false, "b"))
						{
						case 'q':
							return;
						case '!':
							if (song->events[anim].first != oldAnim)
								song->saved = false;
							break;
						case 'b':
							global.quit = true;
							break;
						case '-':
							printf("%sValue must be positive. Not adjusting.\n", global.tabs.c_str());
							clearIn();
							break;
						case '*':
							printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
							clearIn();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				case 'l':
					do
					{
						banner(" " + song->shortname + ".CHC - SSQ Animation: " + ssqStrings[anim] + " - Last Frame");
						printf("%sEnter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n", global.tabs.c_str());
						printf("%s(Can be a decimal)\n", global.tabs.c_str());
						float oldAnim = song->events[anim].last;
						switch (valueInsert(song->events[anim].last, false, "b"))
						{
						case 'q':
							return;
						case '!':
							if (song->events[anim].last != oldAnim)
								song->saved = false;
							break;
						case 'b':
							global.quit = true;
							break;
						case '-':
							printf("%sValue must be positive. Not adjusting.\n", global.tabs.c_str()); clearIn();
							break;
						case '*':
							printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
							clearIn();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::adjustSpeed()
{
	do
	{
		banner(" " + song->shortname + ".CHC - Gameplay Speed Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit speed settings]\n", global.tabs.c_str());
		printf("%sCan be a decimal... and/or negative with weird effects\n", global.tabs.c_str());
		printf("%sCurrent Value: %g\n", global.tabs.c_str(), song->speed );
		printf("%sInput: ", global.tabs.c_str());
		float oldSpeed = song->speed;
		switch (valueInsert(song->speed, true))
		{
		case '!':
			if (song->speed != oldSpeed)
				song->saved = false;
			break;
		case 'q':
			global.quit = true;
		case '>':
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::sectionMenu()
{
	typedef void(CHC_Editor::* functPointers)();
	functPointers results[5] = { &CHC_Editor::playOrder, &CHC_Editor::rearrange, &CHC_Editor::playerSwapAll, &CHC_Editor::sectionSubMenu, &CHC_Editor::fullPathTest };
	do
	{
		banner(" " + song->shortname + ".CHC - Sections ");
		printf("%sP - Set Unconditional Play Order\n", global.tabs.c_str());
		printf("%sR - Rearrange Sections\n", global.tabs.c_str());
		printf("%sS - Swap Player 1 & Player 2 for All Sections\n", global.tabs.c_str());
		printf("%sC - Choose a Single Section to Edit\n", global.tabs.c_str());
		printf("%sT - Test Section Accessibility\n", global.tabs.c_str());
		printf("%sQ - Back out to Modify Menu\n", global.tabs.c_str());
		size_t index = menuChoices("prsct", true);
		switch (index)
		{
		case 'q':
			return;
		case '?':
			printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
		case '*':
			break;
		default:
			global.adjustTabs(3);
			(this->*results[index])();
			global.adjustTabs(2);
		}
	} while (!global.quit);
}

void CHC_Editor::playerSwapAll()
{
	banner(" " + song->shortname + ".CHC - Complete Player Swap");
	for (size_t sectIndex = 0; sectIndex < song->sections.size(); sectIndex++)
		playerSwap(song->sections[sectIndex]);
	song->saved = false;
}

void CHC_Editor::playOrder()
{
	banner(" " + song->shortname + ".CHC - Play Order ");
	List<size_t> sectionIndexes;
	do
	{
		printf("%sType the index for each section in the order you wish them to be played - w/ spaces in-between.\n", global.tabs.c_str());
		for (size_t index = 0; index < song->sections.size(); index++)
			printf("%s%zu - %s\n", global.tabs.c_str(), index, song->sections[index].name );
		switch (listValueInsert(sectionIndexes, "yn", song->sections.size(), false))
		{
		case '?':
			break;
		case 'q':
			printf("%sPlay Order Selection cancelled.\n", global.tabs.c_str());
			return;
		case '!':
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%sNo sections have been selected. Quit Play Order Selection? [Y/N]\n", global.tabs.c_str());
					switch (menuChoices("yn"))
					{
					case 'q':
					case 'y':
						printf("%sPlay Order Selection cancelled.\n", global.tabs.c_str());
						return;
					case 'n':
						global.quit = true;
					}
				} while (!global.quit);
				global.quit = false;
			}
			else
			{
				global.quit = true;
				break;
			}
		case 'y':
			if (!sectionIndexes.size())
			{
				printf("%sPlay Order Selection cancelled.\n", global.tabs.c_str());
				return;
			}
			else
			{
				global.quit = true;
				break;
			}
		case 'n':
#pragma warning(suppress : 6031)
			getchar();
			printf("%sOk... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n", global.tabs.c_str());
		}
	} while (!global.quit);
	global.quit = false;
	printf("%s", global.tabs.c_str());
	for (size_t index = 0; index < sectionIndexes.size(); index++)
	{
		if (index + 1ULL < sectionIndexes.size())
		{
			song->saved = false;
			SongSection::Condition& cond = song->sections[sectionIndexes[index]].conditions.front();
			cond.type = 0;
			cond.trueEffect = (long)sectionIndexes[index + 1ULL];
		}
		else
		{
			if (!pathTest(sectionIndexes[index]))
			{
				SongSection::Condition& cond = song->sections[sectionIndexes[index]].conditions.front();
				cond.type = 0;
				cond.trueEffect = (long)song->sections.size();
			}
		}
		printf("%s ", song->sections[sectionIndexes[index]].name);
		song->saved = false;
	}
	putchar('\n');
}

void CHC_Editor::rearrange()
{
	banner(" " + song->shortname + ".CHC - Section Rearrangement ");
	size_t startIndex, numElements, position;
	do
	{
		printf("%sProvide the starting index for the range of sections you want to move.\n", global.tabs.c_str());
		for (size_t index = 0; index < song->sections.size(); index++)
			printf("%s%zu - %s\n", global.tabs.c_str(), index, song->sections[index].name );
		printf("%sInput: ", global.tabs.c_str());
		switch (valueInsert(startIndex, false, size_t(0), song->sections.size() - 1))
		{
		case '!':
			global.quit = true;
			break;
		case 'q':
			printf("%sSection rearrangement cancelled.\n", global.tabs.c_str());
			return;
		case '-':
			printf("%sGiven value cannot be negative\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			break;
		case '>':
			printf("%sGiven value cannot be greater than %zu\n%s\n", global.tabs.c_str(), song->sections.size() - 1, global.tabs.c_str());
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			break;
		}
	} while (!global.quit);
	global.quit = false;
	if (song->sections.size() - startIndex == 1)
		numElements = 1;
	else
	{
		do
		{
			printf("%sHow many elements do you want to move? [Max # of movable elements: %zu]\n", global.tabs.c_str(), song->sections.size() - startIndex);
			printf("%sInput: ", global.tabs.c_str());
			switch (valueInsert(numElements, false, size_t(1), song->sections.size() - startIndex))
			{
			case '!':
				global.quit = true;
				break;
			case 'q':
				printf("%sSection rearrangement cancelled.\n", global.tabs.c_str());
				return;
			case '-':
			case '<':
				printf("%sGiven value cannot be less than 1\n%s\n", global.tabs.c_str(), global.tabs.c_str());
				break;
			case '>':
				printf("%sGiven value cannot be greater than %zu\n%s\n", global.tabs.c_str(), song->sections.size() - startIndex, global.tabs.c_str());
				break;
			case '*':
				printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
				break;
			}
		} while (!global.quit);
		global.quit = false;
	}
	if (numElements < song->sections.size())
	{
		long alpha;
		if (song->sections.size() - numElements == 1)
		{
			if (!startIndex)
			{
				printf("%sMoving elements to the end of the list\n", global.tabs.c_str());
				position = song->sections.size();
				alpha = 1;
			}
			else
			{
				printf("%sMoving elements to the beginning of the list\n", global.tabs.c_str());
				position = 0;
				alpha = -1;
			}
		}
		else
		{
			do
			{
				printf("%sProvide the index that these elements will be moved to.\n", global.tabs.c_str());
				for (size_t index = 0; index < song->sections.size(); index++)
					if (index < startIndex || index > startIndex + numElements)
						printf("%s%zu - %s\n", global.tabs.c_str(), index, song->sections[index].name );
				if (song->sections.size() > startIndex + numElements)
					printf("%s%zu - End of the list\n", global.tabs.c_str(), song->sections.size());
				printf("%sInvalid position range: %zu - %zu\n", global.tabs.c_str(), startIndex, startIndex + numElements );
				printf("%sInput: ", global.tabs.c_str());
				switch (valueInsert(position, false, size_t(0), song->sections.size()))
				{
				case '!':
					if (position < startIndex)
					{
						alpha = long(position - startIndex);
						global.quit = true;
					}
					else if (position > startIndex + numElements)
					{
						alpha = long(position - (startIndex + numElements));
						global.quit = true;
					}
					else
						printf("%sCannot choose a value within the range of sections being moved\n", global.tabs.c_str());
					break;
				case 'q':
					printf("%sSection rearrangement cancelled.\n", global.tabs.c_str());
					return;
				case '-':
					printf("%sGiven value cannot be negative\n%s\n", global.tabs.c_str(), global.tabs.c_str());
					break;
				case '>':
					printf("%sGiven value cannot be greater than %zu\n%s\n", global.tabs.c_str(), song->sections.size(), global.tabs.c_str());
					break;
				case '*':
					printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
		for (size_t sectIndex = 0; sectIndex < song->sections.size(); sectIndex++)
		{
			for (size_t condIndex = 0; condIndex < song->sections[sectIndex].conditions.size(); condIndex++)
			{
				long* effect = &song->sections[sectIndex].conditions[condIndex].trueEffect;
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
		song->sections.moveElements(startIndex, position, numElements);
		for (size_t index = 0; index < song->sections.size(); index++)
			printf("%s%zu - %s\n", global.tabs.c_str(), index, song->sections[index].name );
	}
	else
		printf("%sReally? You didn't even do anything with that... *sigh*\n", global.tabs.c_str());
	printf("%s\n", global.tabs.c_str());
}

void CHC_Editor::adjustFactors()
{
	do
	{
		banner(" " + song->shortname + ".CHC - Damage/Energy Factors ", 1.53f);
		static const char* headers[] = { "||    Starting Energy   ||", "|| Initial-Press Energy ||", "|| Initial-Press Damage ||", "||   Guard Energy Gain  ||",
							 "||  Attack Miss Damage  ||", "||   Guard Miss Damage  ||", "|| Sustain Energy Coef. ||", "|| Sustain Damage Coef. ||" };
		for (size_t player = 0; player < 4; player += 2)
		{
			printf("%s        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||", global.tabs.c_str(), player + 1);
			printf("||        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n", player + 2);
			printf("%s%s||||%s||", global.tabs.c_str(), string(77, '=').c_str(), string(77, '=').c_str());
			for (size_t factor = 0; factor < 8; factor++)
			{
				printf("\n\t      ");
				for (size_t index = player; index < player + 2; index++)
				{
					printf(headers[factor]);
					for (size_t phase = 0; phase < 5; phase++)
					{
						float* val = &song->energyDamageFactors[index][phase].start + factor;
						printf("  %5g%% ||", (*val) * 100.0);
					}
				}
			}
			printf("\n%s%s||\n", global.tabs.c_str(), string(158, '=').c_str());
		}
		printf("%sSelect a player ('1'/'2'/'3'/'4') [Type 'Q' to exit factor settings]\n", global.tabs.c_str());
		size_t player = menuChoices("1234", true);
		switch (player)
		{
		case 'q':
			global.quit = true;
			break;
		case '?':
			printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
		case '*':
			break;
		default:
			do
			{
				banner(" " + song->shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " ");
				printf("%s        Player %zu      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n", global.tabs.c_str(), player + 1);
				printf("%s%s||", global.tabs.c_str(), string(77, '=').c_str());
				for (size_t factor = 0; factor < 8; factor++)
				{
					printf("\n\t      %s", headers[factor]);
					for (size_t phase = 0; phase < 5; phase++)
					{
						float* val = &song->energyDamageFactors[player][phase].start + factor;
						printf("  %5g%% ||", (*val) * 100.0);
					}
				}
				printf("\n%s%s||\n", global.tabs.c_str(), string(77, '=').c_str());
				printf("%sSelect a phase ('I'/'C'/'B'/'H'/'E') [Type 'P' to choose a different player | 'Q' to exit factor settings]\n", global.tabs.c_str());
				size_t phase = menuChoices("icbhep", true);
				const string phaseName[5] = { "Intro", "Charge", "Battle", "Harmony", "End" };
				switch (phase)
				{
				case 'q':
					return;
				case '?':
					printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
				case '*':
					break;
				case 5:
					global.quit = true;
					break;
				default:
					do
					{
						banner(" " + song->shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " | " + phaseName[phase] + " ");
						printf("%s          Player %zu        || %*s ||\n", global.tabs.c_str(), player + 1, unsigned(phaseName[phase].length() + 1), phaseName[phase].c_str());
						printf("%s%s||\n", global.tabs.c_str(), string(31 + phaseName[phase].length(), '=').c_str());
						printf("%s 1 - Starting Energy      || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].start * 100.0);
						printf("%s 2 - Initial-Press Energy || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].chargeInitial * 100.0);
						printf("%s 3 - Initial-Press Damage || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].attackInitial * 100.0);
						printf("%s 4 - Guard Energy Gain    || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].guardEnergy * 100.0);
						printf("%s 5 - Attack Miss Damage   || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].attackMiss * 100.0);
						printf("%s 6 - Guard  Miss Damage   || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].guardMiss * 100.0);
						printf("%s 7 - Sustain Energy Coef. || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].chargeRelease * 100.0);
						printf("%s 8 - Sustain Damage Coef. || %*g%% ||\n", global.tabs.c_str(), (unsigned)phaseName[phase].length(), song->energyDamageFactors[player][phase].attackRelease * 100.0);
						printf("%s%s||\n", global.tabs.c_str(), string(31 + phaseName[phase].length(), '=').c_str());
						printf("%sSelect a factor by number [Type 'B' to choose a different phase | 'Q' to exit factor settings]\n", global.tabs.c_str());
						size_t factor = menuChoices("12345678b", true);
						switch (factor)
						{
						case 'q':
							return;
						case '?':
							printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
						case '*':
							break;
						case 8: // 'b'
							global.quit = true;
							break;
						default:
							do
							{
								static const char* enders[] = { " | Starting Energy ", " | Initial-Press Energy ", " | Initial-Press Damage ", " | Guard Energy Gain ",
													" | Attack Miss Damage ", " | Guard Miss Damage ", " | Sustain Energy Coef. ", " | Sustain Damage Coef. " };
								banner(" " + song->shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " | " + phaseName[phase] + enders[factor]);
								float* val = &song->energyDamageFactors[player][phase].start + factor;
								printf("%s        Player %zu      || %s ||\n", global.tabs.c_str(), player + 1, phaseName[phase].c_str());
								printf("%s%s||\n", global.tabs.c_str(), string(26 + phaseName[phase].length(), '=').c_str());
								printf("\t      %s %*g ||\n", headers[factor], (unsigned)phaseName[phase].length(), *val);
								printf("%s%s||\n", global.tabs.c_str(), string(26 + phaseName[phase].length(), '=').c_str());
								printf("%sProvide a new value for this factor [Type 'B' to choose a different factor | 'Q' to exit factor settings]\n", global.tabs.c_str());
								printf("%sCan be a decimal... and/or negative with weird effects\n", global.tabs.c_str());
								printf("%sInput: ", global.tabs.c_str());
								float oldFac = *val;
								switch (valueInsert(*val, true, 0.0f, 0.0f, "b"))
								{
								case 'b':
									global.quit = true;
									break;
								case '!':
									if (*val != oldFac) song->saved = false;
									break;
								case 'q':
									return;
								case '*':
									printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
									clearIn();
								}
							} while (!global.quit);
							global.quit = false;
						}
					} while (!global.quit);
					global.quit = false;
				}
			} while (!global.quit);
			global.quit = false;
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::fullPathTest()
{
	banner(" " + song->shortname + ".CHC - Section Transversal ");
	pathTest(0, true);
}

bool CHC_Editor::pathTest(const size_t startIndex, const bool show)
{
	const size_t size = song->sections.size();
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
					for (size_t condIndex = 0; condIndex < song->sections[sectIndex].conditions.size(); sectIndex++)
						conditionTested[sectIndex][condIndex] = false;
				}
			}
			bool* reach = new bool[size]();
			testSection(startIndex, conditionTested, results, reach);
			printf("%s%s||\n", global.tabs.c_str(), string(40, '=').c_str());
			printf("%s Section || Accessible? || Can Reach End? ||\n", global.tabs.c_str());
			for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
			{
				printf("%s%8s ||      ", global.tabs.c_str(), song->sections[sectIndex].name);
				if (reach[sectIndex])
					printf("Y      ||        ");
				else
					printf("N      ||        ");
				if (results[sectIndex])
					printf("Y       ||\n");
				else
					printf("N       ||\n");
			}
			printf("%s%s||\n", global.tabs.c_str(), string(40, '=').c_str());
		}
		for (size_t sectIndex = 0; sectIndex < size; sectIndex++)
			if (conditionTested[sectIndex] != nullptr)
				delete[song->sections[sectIndex].conditions.size()] conditionTested[sectIndex];
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
			conditionTested[sectIndex] = new bool[song->sections[sectIndex].conditions.size()]();
		for (size_t c = 0; c < song->sections[sectIndex].conditions.size(); c++)
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
	SongSection::Condition& cond = song->sections[sectIndex].conditions[condIndex];
	if (cond.trueEffect >= 0)
	{
		if (cond.trueEffect >= (long)song->sections.size())
			results[sectIndex] = true;
		else if (testSection(cond.trueEffect, conditionTested, results, reach))
			results[sectIndex] = true;
	}
	else if(!conditionTested[sectIndex][condIndex - cond.trueEffect])
			traverseCondition(sectIndex, condIndex - cond.trueEffect, conditionTested, results, reach);
	if (cond.type)
	{
		if (cond.falseEffect >= 0)
		{
			if (cond.falseEffect >= (long)song->sections.size())
				results[sectIndex] = true;
			else if(testSection(cond.falseEffect, conditionTested, results, reach))
				results[sectIndex] = true;
		}
		else if (!conditionTested[sectIndex][condIndex - cond.falseEffect])
			traverseCondition(sectIndex, condIndex - cond.falseEffect, conditionTested, results, reach);
	}
}

void CHC_Editor::sectionSubMenu()
{
	do
	{
		banner(" " + song->shortname + ".CHC - Section Selection");
		printf("%sType the number for the section that you wish to edit\n", global.tabs.c_str());
		size_t val;
		for (size_t sectIndex = 0; sectIndex < song->sections.size(); sectIndex++)
			printf("%s%zu - %s\n", global.tabs.c_str(), sectIndex, song->sections[sectIndex].name );
		switch (valueInsert(val, false, size_t(0), song->sections.size() - 1))
		{
		case 'q':
			global.quit = true;
			break;
		case '-':
			printf("%sGiven section value must be positive.\n", global.tabs.c_str());
			clearIn();
			break;
		case '>':
			printf("%sGiven section value cannot exceed %zu\n", global.tabs.c_str(), song->sections.size() - 1 );
			clearIn();
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
			break;
		case '!':
			global.adjustTabs(4);
			do
			{
				banner(" " + song->shortname + ".CHC - " + song->sections[val].name + " - Modify ");
				string choices = "nafptdes"; //ENABLE CONDITION OPTION
				printf("%sN - Name\n", global.tabs.c_str());
				printf("%sA - Change the section of audio used: %s\n", global.tabs.c_str(), song->sections[val].audio);
				printf("%sF - Adjust SSQ frame range\n", global.tabs.c_str());
				printf("%sP - Phase: ", global.tabs.c_str());
				switch (song->sections[val].battlePhase)
				{
				case SongSection::Phase::INTRO: printf("INTRO\n"); break;
				case SongSection::Phase::CHARGE: printf("CHARGE\n"); break;
				case SongSection::Phase::BATTLE: printf("BATTLE\n");  break;
				case SongSection::Phase::FINAL_AG: printf("FINAL_AG\n");  break;
				case SongSection::Phase::HARMONY: printf("HARMONY\n"); break;
				case SongSection::Phase::END: printf("END\n");  break;
				default: printf("FINAL_I\n");
				}
				printf("%sT - Tempo: %g\n", global.tabs.c_str(), song->sections[val].tempo );
				printf("%sD - Duration: %lu\n", global.tabs.c_str(), song->sections[val].duration );
				//printf("%s", global.tabs.c_str(), "C - Modify Conditions (", song->sections[val].numConditions << ")\n";
				printf("%sE - Check if stage-end is reachable\n", global.tabs.c_str());
				if (!song->sections[val].organized)
				{
					printf("%sO - Organize this section\n", global.tabs.c_str());
					choices += 'o';
				}
				printf("%sS - Swap players\n", global.tabs.c_str());
				printf("%sQ - Choose another section\n", global.tabs.c_str());
				size_t index = menuChoices(choices);
				switch (index)
				{
				case 'q':
					global.quit = true;
					break;
				case '?':
					printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
				case '*':
					break;
				default:
					global.adjustTabs(5);
					switch (index)
					{
					case 'n':
						changeName(song->sections[val]);
						break;
					case 'a':
						changeAudio(song->sections[val]);
						break;
					case 'f':
						changeFrames(song->sections[val]);
						break;
					case 'p':
						changeFrames(song->sections[val]);
						break;
					case 't':
						adjustTempo(song->sections[val]);
						break;
					case 'd':
						adjustDuration(song->sections[val]);
						break;
					case 'c':
						conditionMenu(song->sections[val]);
						break;
					case 'e':
						banner(" " + song->shortname + ".CHC - " + song->sections[val].name + " - Transversal ");
						if (pathTest(val))
							printf("%s%s can reach the end of the stage.\n", global.tabs.c_str(), song->sections[val].name);
						else
							printf("%s%s is unable to reach the end of the stage.\n", global.tabs.c_str(), song->sections[val].name);
						break;
					case 'o':
						banner(" " + song->shortname + ".CHC - " + song->sections[val].name + " - Organize ");
						reorganize(song->sections[val]);
						break;
					case 's':
						banner(" " + song->shortname + ".CHC - " + song->sections[val].name + " - Player Swap ");
						playerSwap(song->sections[val]);
						song->saved = false;
					}
					global.adjustTabs(4);
				}
			} while (!global.quit);
			global.quit = false;
			global.adjustTabs(3);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::reorganize(SongSection& section)
{
	if (section.battlePhase == SongSection::Phase::CHARGE || (section.battlePhase == SongSection::Phase::BATTLE && !strstr(section.name, "BRK") && !strstr(section.name, "BREAK")))
	{
		typedef pair<long, Note*> sectNote;
		List<sectNote> notes[4];
		List<Chart> newCharts[4];
		for (size_t pl = 0; pl < 4; pl++)
		{
			size_t currentPlayer;
			if (song->imc[0]) currentPlayer = pl & 1;
			else currentPlayer = pl;
			List<sectNote>& player = notes[currentPlayer];
			for (size_t chIndex = 0; chIndex < section.numCharts; chIndex++)
			{
				size_t playerIndex, chartIndex;
				if (song->imc[0])
				{
					playerIndex = ((2 * ((pl >> 1) ^ (chIndex & 1))) + (pl & 1)) * section.numCharts;
					chartIndex = (section.numCharts >> 1) * (pl >> 1) + (((section.numCharts & 1) * (pl >> 1) + chIndex) >> 1);
				}
				else
				{
					playerIndex = pl * section.numCharts;
					chartIndex = chIndex;
				}
				Chart& ch = section.charts[(size_t)playerIndex + chartIndex];
				size_t index = 0;
				if (section.swapped >= 4 || ((!(pl & 1)) != (song->imc[0] && section.swapped & 1)))
				{
					for (size_t i = 0; i < ch.getNumGuards(); i++)
					{
						while (index < player.size())
						{
							if (ch.guards[i].pivotAlpha + ch.pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.guards[i].pivotAlpha + ch.pivotTime, &ch.guards[i]));
						index++;
					}
					index = 0;
					for (size_t i = 0; i < ch.phrases.size(); i++)
					{
						while (index < player.size())
						{
							if (ch.phrases[i].pivotAlpha + ch.pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.phrases[i].pivotAlpha + ch.pivotTime, &ch.phrases[i]));
						index++;
					}
					index = 0;
					if (ch.tracelines.size() > 1)
					{
						for (size_t i = 0; i < ch.tracelines.size(); i++)
						{
							while (index < player.size())
							{
								if (ch.tracelines[i].pivotAlpha + ch.pivotTime <= player[index].first)
									break;
								else
									index++;
							}
							player.insert(index, sectNote(ch.tracelines[i].pivotAlpha + ch.pivotTime, &ch.tracelines[i]));
							index++;
						}
					}
				}
				else
				{
					for (size_t i = 0; i < ch.phrases.size(); i++)
					{
						while (index < player.size())
						{
							if (ch.phrases[i].pivotAlpha + ch.pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.phrases[i].pivotAlpha + ch.pivotTime, &ch.phrases[i]));
						index++;
					}
					index = 0;
					if (ch.tracelines.size() > 1)
					{
						for (size_t i = 0; i < ch.tracelines.size(); i++)
						{
							while (index < player.size())
							{
								if (ch.tracelines[i].pivotAlpha + ch.pivotTime <= player[index].first)
									break;
								else
									index++;
							}
							player.insert(index, sectNote(ch.tracelines[i].pivotAlpha + ch.pivotTime, &ch.tracelines[i]));
							index++;
						}
					}
					index = 0;
					for (size_t i = 0; i < ch.getNumGuards(); i++)
					{
						while (index < player.size())
						{
							if (ch.guards[i].pivotAlpha + ch.pivotTime <= player[index].first)
								break;
							else
								index++;
						}
						player.insert(index, sectNote(ch.guards[i].pivotAlpha + ch.pivotTime, &ch.guards[i]));
						index++;
					}
				}
			}
		}
		const long double SAMPLES_PER_BEAT = 2880000.0L / section.tempo;
		auto adjustPhrases = [&](Chart* currentChart, const size_t traceIndex, const long& endAlpha, float& angle)
		{
			//Save angle value
			angle = currentChart->tracelines[traceIndex].angle;
			//Remove trace line
			printf("%s%s: ", global.tabs.c_str(), section.name);
			currentChart->remove(traceIndex, 't');
			if (currentChart->tracelines.size())
			{
				//Same idea for phrase bars
				for (size_t p = currentChart->phrases.size(); p > 0;)
				{
					//If the phrase bar is past the trace line
					if (currentChart->tracelines[traceIndex].getEndAlpha() <= currentChart->phrases[--p].pivotAlpha)
					{
						//Delete the phrase bar
						if (!currentChart->phrases[p].start)
							currentChart->phrases[p - 1ULL].end = true;
						printf("%s%s: ", global.tabs.c_str(), section.name);
						currentChart->remove(p, 'p');
					}
					else
					{
						//If inside the trace line
						if (currentChart->tracelines[traceIndex] <= currentChart->phrases[p])
							currentChart->phrases[p].changeEndAlpha(endAlpha);
						break;
					}
				}
			}
			else
			{
				for (size_t p = currentChart->phrases.size(); p > 0;)
				{
					printf("%s%s: ", global.tabs.c_str(), section.name);
					currentChart->remove(--p, 'p');
				}
			}
		};
		for (size_t pl = 0; pl < (song->imc[0] ? 2U : 4U); pl++)
		{
			size_t currentPlayer = pl;
			newCharts[currentPlayer].emplace_back();
			Chart* currentChart = &newCharts[currentPlayer][0];
			bool isPlayer = !(pl & 1) || section.swapped >= 4;
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
						notes[pl][startingIndex].second->setPivotAlpha(notes[pl][startingIndex].first - currentChart->pivotTime);
					}
					else
						//safety adjustment of the current note's pivot alpha to line up with this chart
						notes[pl][ntIndex].second->setPivotAlpha(notes[pl][ntIndex].first - currentChart->pivotTime);
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
								if (notes[pl][ntIndex].first + (long)tr->duration != notes[pl][ntIndex + 1ULL].first)
								{
									currentChart->setEndTime(notes[pl][ntIndex].first + tr->duration);
									makeNewChart = true;
								}
							}
							else if (dynamic_cast<Guard*>(notes[pl][ntIndex + 1ULL].second) != nullptr)	//If the next note is a guard mark
							{
								if (notes[pl][ntIndex].first + (long)tr->duration <= notes[pl][ntIndex + 1ULL].first)	//Outside the trace line
								{
									if (isPlayer != (song->imc[0] && section.swapped & 1))	//It's not the enemy's original charts
									{
										//Mark the end of the current chart
										currentChart->setEndTime(notes[pl][ntIndex].first + tr->duration);
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
											if (static_cast<Traceline*>(notes[pl][ntIndex + 2ULL].second)->duration == 1)
												notes[pl].erase(ntIndex + 2ULL);
									}
									//If it's the enemy's original charts or the next-next note, if it exists, is a guard mark
									if ((isPlayer == (song->imc[0] && section.swapped & 1)) || (ntIndex + 2ULL != notes[pl].size() && dynamic_cast<Path*>(notes[pl][ntIndex + 2ULL].second) == nullptr))
									{
										long endAlpha = notes[pl][ntIndex + 1ULL].first - SongSection::SAMPLE_GAP - 1 - currentChart->pivotTime;
										float angle = 0;
										for (size_t t = currentChart->tracelines.size(); t > 0;)
										{
											//If the new end point is less than or equal to the last trace line's pivot
											if (!currentChart->tracelines[--t].changeEndAlpha(endAlpha))
												adjustPhrases(currentChart, t, endAlpha, angle);
											else
												break;
										}
										if (currentChart->tracelines.size())
										{
											Traceline* tr2 = new Traceline(endAlpha, 1, angle);
											currentChart->add(tr2);
											delete tr2;
										}
										if (isPlayer != (song->imc[0] && section.swapped & 1)) //It's not the enemy's original charts
										{
											//Mark the end of the current chart
											currentChart->setEndTime(notes[pl][ntIndex + 1ULL].first - SongSection::SAMPLE_GAP);
											makeNewChart = true;
										}
									}
									else
										notes[pl].erase(ntIndex + 1ULL);
								}
							}
							else if (notes[pl][ntIndex].first + (long)tr->duration <= notes[pl][ntIndex + 1ULL].first)	//If the phrase bar is outside the trace line
								notes[pl].erase(ntIndex + 1ULL);
						}
						else if (dynamic_cast<Guard*>(notes[pl][ntIndex].second) != nullptr)	//If the current note is a guard mark
						{
							if (dynamic_cast<Guard*>(notes[pl][ntIndex + 1ULL].second) != nullptr)	//If the next note is a guard mark
							{
								//If there is enough distance between these guard marks in a duet or tutorial stage
								if ((!song->imc[0] || song->stage == 0 || song->stage == 11 || song->stage == 12) && notes[pl][ntIndex + 1ULL].first - notes[pl][ntIndex].first >= long(5.5 * SAMPLES_PER_BEAT))
								{
									if (!currentChart->tracelines.size())
									{
										//Move chartPivot in between these two notes
										//Calculate the value adjustment
										long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->pivotTime;
										//Adjust the chartAlpha
										currentChart->adjustPivotTime(pivotDifference);
										//Adjust the pivot alphas of inserted notes
										for (size_t grdIndex = 0; grdIndex < currentChart->guards.size(); grdIndex++)
											currentChart->guards[grdIndex].adjustPivotAlpha(-pivotDifference);
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
										if (tr->changePivotAlpha(tr->pivotAlpha + newFirst - notes[pl][ntIndex + 1ULL].first))
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
									if ((!song->imc[0] || song->stage == 0 || song->stage == 11 || song->stage == 12) && notes[pl][ntIndex + 1ULL].first - notes[pl][ntIndex].first >= long(5.5 * SAMPLES_PER_BEAT))
									{
										if (!currentChart->tracelines.size())
										{
											//Move chartPivot in between these two notes
											//Calculate the value adjustment
											long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->pivotTime;
											//Adjust the chartAlpha
											currentChart->adjustPivotTime(pivotDifference);
											//Adjust the pivot alphas of inserted notes
											for (size_t grdIndex = 0; grdIndex < currentChart->guards.size(); grdIndex++)
												currentChart->guards[grdIndex].adjustPivotAlpha(-pivotDifference);
										}
										currentChart->setEndTime(notes[pl][ntIndex].first + long(SAMPLES_PER_BEAT));
										makeNewChart = true;
									}
								}
								else
								{
									if (isPlayer != (song->imc[0] && section.swapped & 1))	//If it's not the enemy's original charts
									{
										//Move chartPivot in between these two notes
										//Calculate the value adjustment
										long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->pivotTime;
										//Adjust the chartAlpha
										currentChart->adjustPivotTime(pivotDifference);
										//Adjust the pivot alphas of inserted notes
										for (size_t grdIndex = 0; grdIndex < currentChart->guards.size(); grdIndex++)
											currentChart->guards[grdIndex].adjustPivotAlpha(-pivotDifference);
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
										if (static_cast<Traceline*>(notes[pl][ntIndex + 2ULL].second)->duration == 1)
											notes[pl].erase(ntIndex + 2ULL);
								}
								//If it's the enemy's original charts or the next-next note, if it exists, is a guard mark
								if ((isPlayer == (song->imc[0] && section.swapped & 1)) || (ntIndex + 2ULL != notes[pl].size() && dynamic_cast<Path*>(notes[pl][ntIndex + 2ULL].second) == nullptr))
								{
									long endAlpha = notes[pl][ntIndex + 1ULL].first - SongSection::SAMPLE_GAP - 1 - currentChart->pivotTime;
									float angle = 0;
									for (size_t t = currentChart->tracelines.size(); t > 0;)
									{
										//If the new end point is less than or equal to the last trace line's pivot
										if (!currentChart->tracelines[--t].changeEndAlpha(endAlpha))
											adjustPhrases(currentChart, t, endAlpha, angle);
										else
										{
											for (size_t p = currentChart->phrases.size(); p > 0;)
											{
												//If the phrase bar is past the trace line
												if (currentChart->tracelines[t].getEndAlpha() <= currentChart->phrases[--p].pivotAlpha)
												{
													//Delete the phrase bar
													if (!currentChart->phrases[p].start)
														currentChart->phrases[p - 1ULL].end = true;
													printf("%s%s: ", global.tabs.c_str(), section.name);
													currentChart->remove(p, 'p');
												}
												else
												{
													//If inside the trace line
													if (currentChart->tracelines[t] <= currentChart->phrases[p])
														currentChart->phrases[p].changeEndAlpha(endAlpha);
													break;
												}
											}
											break;
										}
									}
									if (currentChart->tracelines.size())
									{
										Traceline* tr = new Traceline(endAlpha, 1, angle);
										currentChart->add(tr);
										delete tr;
									}
									if (isPlayer != (song->imc[0] && section.swapped & 1)) //It's not the enemy's original charts
									{
										//Mark the end of the current chart
										currentChart->setEndTime(notes[pl][ntIndex + 1ULL].first - SongSection::SAMPLE_GAP);
										makeNewChart = true;
									}
								}
								else
									notes[pl].erase(ntIndex + 1ULL);
							}
						}
						if (makeNewChart)
						{
							if (song->imc[0] && song->stage > 0)
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
					currentChart->setEndTime(notes[pl].back().first + static_cast<Path*>(notes[pl].back().second)->duration);
				else
					currentChart->setEndTime(notes[pl].back().first);
			}
		}
		section.numCharts = 1;
		//Finds the proper new value for "section.numcharts"
		for (size_t pl = 0; pl < 4; pl++)
		{
			size_t perChart = newCharts[pl].size();
			for (size_t ch = 0; ch < newCharts[pl].size() && perChart > section.numCharts; ch++)
				if (!(newCharts[pl][ch].guards.size() + newCharts[pl][ch].phrases.size()) && newCharts[pl][ch].tracelines.size() <= 1)
					perChart--;
			if (perChart > section.numCharts)
				section.numCharts = (unsigned long)perChart;
		}
		size_t total;
		if (song->imc[0])
		{
			size_t total1 = 0, total2 = 0;
			for (unsigned pl = 0; pl < 4; pl += 2)
			{
				for (size_t ch = 0; ch < newCharts[pl].size(); ch++)
					if (newCharts[pl][ch].guards.size() + newCharts[pl][ch].phrases.size() || newCharts[pl][ch].tracelines.size() <= 1)
						total1++;
				for (size_t ch = 0; ch < newCharts[pl + 1].size(); ch++)
					if (newCharts[pl + 1][ch].guards.size() + newCharts[pl + 1][ch].phrases.size() || newCharts[pl + 1][ch].tracelines.size() <= 1)
						total2++;
			}
			total = total1 >= total2 ? total1 : total2;
		}
		section.charts.clear();
		section.size = unsigned long(64 + 16 * section.conditions.size());
		for (unsigned pl = 0; pl < 4; pl++)
		{
			while (newCharts[pl].size() > section.numCharts)
				newCharts[pl].pop_back();
			while (newCharts[pl].size() < section.numCharts)
				newCharts[pl].emplace_back();
			for (size_t i = 0; i < newCharts[pl].size(); i++)
			{
				section.size += newCharts[pl][i].size;
				section.charts.push_back(newCharts[pl][i]);
			}
		}
		printf("%s%s organized - # of charts per player: %lu", global.tabs.c_str(), section.name, section.numCharts);
		if (song->imc[0])
			printf(" (Pair total: %zu)\n", total);
		else
			putchar('\n');
		song->saved = false;
	}
	else
		printf("%s%s: [INTRO, HARMONY, END, & BRK sections are organized by default]\n", global.tabs.c_str(), section.name);
	section.organized = true;
	song->unorganized--;
}

void CHC_Editor::playerSwap(SongSection& section)
{
	if (song->imc[0])
	{
		if (section.battlePhase != SongSection::Phase::HARMONY && section.battlePhase != SongSection::Phase::END)
		{
			for (long playerIndex = section.numPlayers - 1; playerIndex > 0; playerIndex -= 2)
				section.charts.moveElements((size_t)playerIndex * section.numCharts, (playerIndex - 1ULL) * section.numCharts, section.numCharts);
			if (!(section.swapped & 1))
			{
				if (section.swapped == 0)
					printf("%s%s: P1/P3 -> P2/P4\n", global.tabs.c_str(), section.name);
				else if (section.swapped == 2)
					printf("%s%s: P3/P1 -> P4/P2\n", global.tabs.c_str(), section.name);
				else if (section.swapped == 4)
					printf("%s%s: P1/P3 -> P2D/P4D (Duet->PS2 conversion)\n", global.tabs.c_str(), section.name);
				else
					printf("%s%s: P3/P1 -> P4D/P2D (Duet->PS2 conversion)\n", global.tabs.c_str(), section.name);
				section.swapped++;
			}
			else
			{
				if (section.swapped == 1)
					printf("%s%s: P2/P4 -> P1/P3\n", global.tabs.c_str(), section.name);
				else if (section.swapped == 3)
					printf("%s%s: P4/P2 -> P3/P1\n", global.tabs.c_str(), section.name);
				else if (section.swapped == 5)
					printf("%s%s: P2D/P4D -> P1/P3 (Duet->PS2 conversion)\n", global.tabs.c_str(), section.name);
				else
					printf("%s%s: P4D/P2D -> P3/P1 (Duet->PS2 conversion)\n", global.tabs.c_str(), section.name);
				section.swapped--;
			}
		}
		else
		{
			do
			{
				printf("%sHow do you want to swap %s?\n", global.tabs.c_str(), section.name);
				printf("%sA - Swap P1/P3 with P2/P4 respectively\n", global.tabs.c_str());
				printf("%sB - Swap P1/P2 with P3/P4 respectively\n", global.tabs.c_str());
				printf("%sCurrent Format: ", global.tabs.c_str());
				if (!section.swapped)
					printf("P1/P2/P3/P4\n");
				else if (section.swapped == 1)
					printf("P2/P1/P4/P3\n");
				else if (section.swapped == 2)
					printf("P3/P4/P1/P2\n");
				else if (section.swapped == 3)
					printf("P4/P3/P2/P1\n");
				switch (menuChoices("ab"))
				{
				case 'q':
					return;
				case 'a':
					for (long playerIndex = section.numPlayers - 1; playerIndex > 0; playerIndex -= 2)
						section.charts.moveElements((size_t)playerIndex * section.numCharts, (playerIndex - 1ULL) * section.numCharts, section.numCharts);
					if (!(section.swapped & 1))
					{
						if ((section.swapped & 2) == 0)
							printf("%s%s: P1/P3 -> P2/P4\n", global.tabs.c_str(), section.name);
						else
							printf("%s%s: P3/P1 -> P4/P2\n", global.tabs.c_str(), section.name);
						section.swapped++;
					}
					else
					{
						if ((section.swapped & 2) == 0)
							printf("%s%s: P2/P4 -> P1/P3\n", global.tabs.c_str(), section.name);
						else
							printf("%s%s: P4/P2 -> P3/P1\n", global.tabs.c_str(), section.name);
						section.swapped--;
					}
					global.quit = true;
					break;
				case 'b':
					section.charts.moveElements(2ULL * section.numCharts, 0, 2ULL * section.numCharts);
					if ((section.swapped & 2) == 0)
					{
						if (!(section.swapped & 1))
							printf("%s%s: P1/P2 -> P3/P4\n", global.tabs.c_str(), section.name);
						else
							printf("%s%s: P2/P1 -> P4/P3\n", global.tabs.c_str(), section.name);
						section.swapped += 2;
					}
					else
					{
						if (!(section.swapped & 1))
							printf("%s%s: P3/P4 -> P1/P2\n", global.tabs.c_str(), section.name);
						else
							printf("%s%s: P4/P3 -> P2/P1\n", global.tabs.c_str(), section.name);
						section.swapped -= 2;
					}
					global.quit = true;
				}
			} while (!global.quit);
			global.quit = false;
		}
	}
	else
	{
		section.charts.moveElements(0, 2ULL * section.numCharts, section.numCharts);
		section.charts.moveElements(2ULL * section.numCharts, 0, section.numCharts);
		if (!section.swapped)
		{
			printf("%s%s: P1 -> P3\n", global.tabs.c_str(), section.name);
			section.swapped = 2;
		}
		else
		{
			printf("%s%s: P3 -> P1\n", global.tabs.c_str(), section.name);
			section.swapped = 0;
		}

	}
}

void CHC_Editor::changeName(SongSection& section)
{
	banner(" " + song->shortname + ".CHC - " + section.name + " - Name Change ");
	printf("%sProvide a new name for this section (16 character max) ('Q' to back out to the Section Menu)\n", global.tabs.c_str());
	printf("%sInput: ", global.tabs.c_str());
	char newName[17];
	scanf_s(" %[^\n;]s", newName, 16);
#pragma warning(suppress : 6031)
	getchar();
	if ((newName[1] != '\n' && newName[1] != '\0' && newName[1] != ';') || (newName[0] != 'q' && newName[0] != 'Q'))
	{
		memcpy_s(section.audio, 17, newName, 16);
		song->saved = false;
	}
}

void CHC_Editor::changeAudio(SongSection& section)
{
	banner(" " + song->shortname + ".CHC - " + section.name + " - Audio Change ");
	printf("%sProvide the name for the section of audio you want to use (16 character max) ('Q' to back out to the Section Menu)\n", global.tabs.c_str());
	printf("%sInput: ", global.tabs.c_str());
	char newAudio[17];
	scanf_s(" %[^\n;]s", newAudio, 16);
#pragma warning(suppress : 6031)
	getchar();
	if ((newAudio[1] != '\n' && newAudio[1] != '\0' && newAudio[1] != ';') || (newAudio[0] != 'q' && newAudio[0] != 'Q'))
	{
		memcpy_s(section.audio, 17, newAudio, 16);
		song->saved = false;
	}
}

void CHC_Editor::changeFrames(SongSection& section)
{
	do
	{
		banner(" " + song->shortname + ".CHC - " + section.name + " - SSQ Starting Index ");
		printf("%sProvide a value for the starting SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n", global.tabs.c_str());
		printf("%sCan be a decimal\n", global.tabs.c_str());
		printf("%sCurrent Value for Starting Index: %g\n", global.tabs.c_str(), section.frames.first );
		printf("%sInput: ", global.tabs.c_str());
		float oldFirst = section.frames.first;
		switch (valueInsert(section.frames.first, false, "u"))
		{
		case 'q':
			return;
		case '!':
			if (section.frames.first != oldFirst)
				song->saved = false;
		case 2:
			global.quit = true;
			break;
		case '-':
			printf("%sProvided value *must* be a zero or greater.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
		}
	} while (!global.quit);
	global.quit = false;
	printf("%s\n%sStarting index saved.\n%s\n", global.tabs.c_str(), global.tabs.c_str(), global.tabs.c_str());
	do
	{
		banner(" " + song->shortname + ".CHC - " + section.name + " - SSQ Ending Index ");
		printf("%sProvide a value for the Ending SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n", global.tabs.c_str());
		printf("%sCan be a decimal\n", global.tabs.c_str());
		printf("%sCurrent Value for Ending Index: %g\n", global.tabs.c_str(), section.frames.last );
		printf("%sInput: ", global.tabs.c_str());
		float oldLast = section.frames.last;
		switch (valueInsert(section.frames.last, false, "u"))
		{
		case 'q':
			return;
		case '!':
			if (section.frames.last != oldLast)
				song->saved = false;
		case 2:
			global.quit = true;
			break;
		case '-':
			printf("%sProvided value *must* be a zero or greater.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
		}
	} while (!global.quit);
	global.quit = false;
	printf("%s\n%sEnding index saved.\n", global.tabs.c_str(), global.tabs.c_str());
}

void CHC_Editor::switchPhase(SongSection& section)
{
	string choices = "";
	do
	{
		banner(" " + song->shortname + ".CHC - " + section.name + " - Phase Selection ");
		printf("%sSelect a phase type for this section\n", global.tabs.c_str());
		printf("%sCurrent Phase: ", global.tabs.c_str());
		switch (section.battlePhase)
		{
		case SongSection::Phase::INTRO: printf("INTRO\n"); break;
		case SongSection::Phase::CHARGE: printf("CHARGE\n"); break;
		case SongSection::Phase::BATTLE: printf("BATTLE\n");  break;
		case SongSection::Phase::FINAL_AG: printf("FINAL_AG\n");  break;
		case SongSection::Phase::HARMONY: printf("HARMONY\n"); break;
		case SongSection::Phase::END: printf("END\n");  break;
		default: printf("FINAL_I\n");
		}
		printf("%sI/0 - Intro\n", global.tabs.c_str());
		printf("%sC/1 - Charge\n", global.tabs.c_str());
		printf("%sB/2 - Battle\n", global.tabs.c_str());
		printf("%sH/4 - Harmony\n", global.tabs.c_str());
		printf("%sE/5 - End\n", global.tabs.c_str());
		size_t ph = menuChoices("i0c1b2h4e5", true);
		switch (ph)
		{
		case 'q':
			return;
		case '?':
			printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			break;
		case '*':
			global.quit = true;
			break;
		default:
			ph /= 2;
			if (ph > 2)
				ph++;
			if (section.battlePhase != static_cast<SongSection::Phase>(ph))
				song->saved = false;
			section.battlePhase = static_cast<SongSection::Phase>(ph);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::adjustTempo(SongSection& section)
{
	do
	{
		banner(" " + song->shortname + ".CHC - " + section.name + " - Tempo Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit tempo settings]\n", global.tabs.c_str());
		printf("%sCan be a decimal... and/or negative with weird effects on PSP\n", global.tabs.c_str());
		printf("%sCurrent Value: %g\n", global.tabs.c_str(), section.tempo );
		printf("%sInput: ", global.tabs.c_str());
		float oldTempo = section.tempo;
		switch (valueInsert(section.tempo, true))
		{
		case '!':
			if (section.tempo != oldTempo)
				song->saved = false;
			break;
		case 'q':
			global.quit = true;
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::adjustDuration(SongSection& section)
{
	do
	{
		banner(" " + song->shortname + ".CHC - " + section.name + " - Duration Change ");
		printf("%sProvide a value for the change [Type 'Q' to exit duration settings]\n", global.tabs.c_str());
		printf("%sCurrent Value: %lu\n", global.tabs.c_str(), section.duration );
		printf("%sInput: ", global.tabs.c_str());
		unsigned long oldDuration = section.duration;
		switch (valueInsert(section.duration, false))
		{
		case '!':
			if (section.duration != oldDuration)
				song->saved = false;
			break;
		case 'q':
			global.quit = true;
		case '-':
			printf("%sProvided value *must* be a zero or greater.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::conditionMenu(SongSection& section)
{
	do
	{
		banner(" " + song->shortname + ".CHC - Section " + section.name + " - Condition Selection ", 1.5);
		printf(" i ||Condition Type || Argument ||      True Effect      ||      False Effect     ||\n");
		printf("==================================================================================||\n");
		for (size_t condIndex = 0; condIndex < section.conditions.size(); condIndex++)
		{
			printf("%s %zu || ", global.tabs.c_str(), condIndex);
			switch (section.conditions[condIndex].type)
			{
			case 'q': printf("Unconditional ||   n/a    || "); break;
			case '!': printf("Left Side < # ||   %-4g   || ", section.conditions[condIndex].argument); break;
			case 2: printf("Right Side < #||   %-4g   || ", section.conditions[condIndex].argument); break;
			case 3: printf("Random < #    ||   %-4g   || ", section.conditions[condIndex].argument); break;
			case 4: printf("No Player 3?  ||   n/a    || "); break;
			case 5: printf("No Player 4?  ||   n/a    || "); break;
			case 6: printf("Left < Right? ||   n/a    || "); break;
			}
			if (section.conditions[condIndex].trueEffect >= 0)
				printf("Go to Section %7s || ", song->sections[section.conditions[condIndex].trueEffect].name);
			else
				printf("Go to Condition %5zu || ", condIndex - section.conditions[condIndex].trueEffect);
			if (section.conditions[condIndex].type > 0)
			{
				if (section.conditions[condIndex].falseEffect >= 0)
					printf("Go to Section %7s ||\n", song->sections[section.conditions[condIndex].falseEffect].name);
				else
					printf("Go to Condition %5zu ||\n", condIndex - section.conditions[condIndex].falseEffect);
			}
			else
				printf("         n/a         ||\n");
		}
		printf("==================================================================================||\n");
		printf("%sType the number for the condition that you wish to edit\n", global.tabs.c_str());
		size_t val;
		switch (valueInsert(val, false, size_t(0), section.conditions.size() - 1))
		{
		case 'q':
			global.quit = true;
			break;
		case '-':
			printf("%sGiven section value must be positive.\n", global.tabs.c_str());
			clearIn();
			break;
		case '>':
			printf("%sGiven section value cannot exceed %zu\n", global.tabs.c_str(), song->sections.size() - 1 );
			clearIn();
			break;
		case '*':
			printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
			clearIn();
			break;
		case '!':
			global.adjustTabs(6);
			do
			{
				banner(" " + song->shortname + ".CHC - Section " + section.name + " - Condition " + to_string(val) + ' ', 1.5);
				string choices = "ct";
				printf("%sc - Type: ", global.tabs.c_str());
				switch (section.conditions[val].type)
				{
				case 'q': printf("Unconditional\n"); break;
				case '!': printf("Left Side < #\n"); break;
				case 2: printf("Right Side < #\n"); break;
				case 3: printf("Random < #\n"); break;
				case 4: printf("No Player 3?\n"); break;
				case 5: printf("No Player 4?\n"); break;
				case 6: printf("Left < Right?\n"); break;
				}
				if (section.conditions[val].type > 0 && section.conditions[val].type < 4)
				{
					printf("%sA - Argument: %g\n", global.tabs.c_str(), section.conditions[val].argument );
					choices += 'a';
				}
				printf("%sT - True Effect: ", global.tabs.c_str());
				if (section.conditions[val].trueEffect >= 0)
					printf("Go to Section %s\n", song->sections[section.conditions[val].trueEffect].name );
				else
					printf("Go to Condition %zu\n", val - section.conditions[val].trueEffect );
				if (section.conditions[val].type > 0)
				{
					printf("%sF - False Effect: ", global.tabs.c_str());
					if (section.conditions[val].falseEffect >= 0)
						printf("Go to Section %s\n", song->sections[section.conditions[val].falseEffect].name );
					else
						printf("Go to Condition %zu\n", val - section.conditions[val].falseEffect );
					choices += 'f';
				}
				if (section.conditions.size() > 1)
				{
					printf("%sD - Delete this condition\n", global.tabs.c_str());
					choices += 'd';
				}
				printf("%sQ - Choose another condition\n", global.tabs.c_str());
				size_t index = menuChoices(choices);
				switch (index)
				{
				case 'q':
					global.quit = true;
					break;
				case '?':
					printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
				case '*':
					break;
				default:
					global.adjustTabs(7);
					switch (index)
					{
					case 'c':
						changeName(song->sections[val]);
						break;
					case 'a':
						changeAudio(song->sections[val]);
						break;
					case 't':
						changeFrames(song->sections[val]);
						break;
					case 'f':
						changeFrames(song->sections[val]);
						break;
					case 'd':
						adjustTempo(song->sections[val]);
						break;
					}
					global.adjustTabs(6);
				}
			} while (!global.quit);
			global.quit = false;
			global.adjustTabs(3);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::conditionDelete(SongSection& section, size_t index)
{
	List<SongSection::Condition> oldcond = section.conditions;
	for (size_t condIndex = 0; condIndex < index; condIndex++)
	{
		if (section.conditions[condIndex].trueEffect < 0)
		{
			if (condIndex - section.conditions[condIndex].trueEffect == index)
			{
				printf("%sCondition %zu's true effect points to this condition.", global.tabs.c_str(), condIndex);
			}
			else if ((size_t)condIndex - section.conditions[condIndex].trueEffect > index)
				section.conditions[condIndex].trueEffect++;
		}
	}
}