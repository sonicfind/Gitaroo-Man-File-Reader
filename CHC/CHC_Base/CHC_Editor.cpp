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

bool CHC_Editor::editSong(bool multi)
{
	do
	{
		banner(" " + song.shortname + ".CHC - Editor ");
		//Holds addresses to the various functions that can be chosen in this menu prompt
		List<void(CHC_Editor::*)()> results;

		string choices = "";
		results.push_back(&CHC_Editor::saveFile); choices += 's';
		cout << global.tabs << "S - Save File\n";
		if (!song.optimized)
		{
			results.push_back(&CHC_Editor::fixNotes); choices += 'f';
			cout << global.tabs << "F - Fix any problematic notes or trace lines\n";
		}
		if (song.unorganized)
		{
			results.push_back(&CHC_Editor::organizeAll); choices += 'o';
			cout << global.tabs << "O - Organize All Unorganized Sections\n";
		}
		if (strstr(song.imc, ".IMC"))
		{
			results.push_back(&CHC_Editor::swapIMC); choices += 'i';
			cout << global.tabs << "I - Swap IMC file (Current file: " << song.imc + 34 << ")\n";
		}
		else
		{
			results.push_back(&CHC_Editor::PSPToPS2); choices += 'p';
			cout << global.tabs << "P - Convert for PS2-Version Compatibility\n";
		}
		results.push_back(&CHC_Editor::audioSettings); choices += 'a';
		cout << global.tabs << "V - Volume & Pan Settings\n";
		results.push_back(&CHC_Editor::winLossSettings); choices += 'w';
		cout << global.tabs << "A - Win/Loss Animations\n";
		results.push_back(&CHC_Editor::adjustSpeed); choices += 'g';
		cout << global.tabs << "G - Adjust Gameplay Speed: " << song.speed << endl;
		results.push_back(&CHC_Editor::sectionMenu); choices += 'c';
		cout << global.tabs << "C - SongSections (" << song.numSections << ")\n";
		results.push_back(&CHC_Editor::adjustFactors); choices += 'd';
		cout << global.tabs << "D - Player Damage/Energy Factors\n";
		cout << global.tabs << "? - Help info\n";
		if (multi)
			cout << global.tabs << "Q - Close this file" << endl;
		else
			cout << global.tabs << "Q - Exit Detail Editor" << endl;
		size_t index = menuChoices(choices);
		switch (index)
		{
		case -1:
			if (!saved)
			{
				cout << global.tabs << endl;
				cout << global.tabs << "Recent changes have not been saved externally to a file. Which action will you take?\n";
				cout << global.tabs << "S - Save (two sub options), and Exit\n";
				cout << global.tabs << "Q - Exit without saving\n";
				cout << global.tabs << "C - Cancel" << endl;
				switch (menuChoices("sc"))
				{
				case 0:
					saveFile();
					if (!saved)
						break;
				case -1:
					global.quit = true;
				}
			}
			else
				global.quit = true;
			break;
		case -3:
			cout << global.tabs << endl;
			cout << global.tabs << "S - Save File:\n";
			cout << global.tabs << "Pretty self-explanatory - save the file as an overwrite or as a new file.\n" << global.tabs << '\n';

			if (strstr(song.imc, ".IMC"))
			{
				cout << global.tabs << "I - Swap IMC file (Current file: " << song.imc + 34 << "):\n";
				cout << global.tabs << "PS2 compatible CHC files contain a string that points to the location of the .IMC audio file used for the stage.\n";
				cout << global.tabs << "Use this option to change which specific .IMC file the CHC will point to;\n" << global.tabs << '\n';
			}
			else
			{
				cout << global.tabs << "P - Convert for PS2-Version Compatibility:\n";
				cout << global.tabs << "Use this option to convert this (assumed) duet-made CHC into a PS2 compatible CHC. Doing so will open up a few other features\n";
				cout << global.tabs << "- most prominently being TASing capability - at the cost of one of the player tracks: Player 1, Player 2, or the Enemy.\n";
				cout << global.tabs << "This will also reorganize all unorganized sections for optimal compatibility.\n" << global.tabs << '\n';
			}
			cout << global.tabs << "V - Volume & Pan Settings:\n";
			cout << global.tabs << "Allows you to adjust the volume and pan of the 8 available audio channels.\n";
			cout << global.tabs << "Note that you can change channel 1 & 2 and channel pan altogether only if the CHC in question is PS2 compatible.\n" << global.tabs << '\n';

			cout << global.tabs << "A - Win/Loss Animations:\n";
			cout << global.tabs << "Every stage in the game has a set of win and loss animations. These animations start at set points in a stage's frame data (located in the stage's SSQ file).\n";
			cout << global.tabs << "This option will allow you to change what frame any of the four animations (two wins, two losses) start on - although you'll most likely never use this.\n" << global.tabs << '\n';

			cout << global.tabs << "G - Adjust Gameplay Speed (Current speed: " << song.speed << "):\n";
			cout << global.tabs << "The speed of which trace lines, phrae bars, and guard marks approach the middle of the screen is determined by this value.\n";
			cout << global.tabs << "This option will allow you to adjust the value to whatever you want (negative even). Note: lower = faster; higher = slower.\n" << global.tabs << '\n';

			cout << global.tabs << "C - SongSections (" << song.numSections << "):\n";
			cout << global.tabs << "In the future, this option will be the gateway to digging into the nitty gritty of each section. It will allow you manually adjust a wide range\n";
			cout << global.tabs << "of parameters, including a section's duration, conditions, and phase type while also allowing for manual adjustments on subsections and\n";
			cout << global.tabs << "their trace lines, phrase bars, and guard marks. Currently, however, it will give you a choice of two features:\n";
			cout << global.tabs << "1. If there are any sections still unorganized, it will give you the option to reorganize every section that needs it.\n";
			cout << global.tabs << "2. Change the order of which sections will proceed in gameplay using unconditional conditions. Useful for when you need to test only certain sections.\n" << global.tabs << '\n';

			cout << global.tabs << "F - Player Damage/Energy Factors:\n";
			cout << global.tabs << "For each player separately, each phase type has values pertaining to 8 different HP related factors.\n";
			cout << global.tabs << "This will bring up a series of menus so that you can edit any factors you like - although some factors have no effect in certain phase types.\n";
			cout << global.tabs << "(No dealing or receiving damage in charge phases for example).\n" << global.tabs << '\n';
		case -2:
			break;
		default:
			adjustTabs(2);
			(this->*results[index])();
			adjustTabs(1);
		}
	} while (!global.quit);
	global.quit = false;
	return overwritten;
}

void CHC_Editor::organizeAll()
{
	banner(" " + song.shortname + ".CHC - Complete Reorganization ");
	for (unsigned long sectIndex = 0; song.unorganized && sectIndex < song.numSections; sectIndex++)
	{
		if (!song.sections[sectIndex].organized)
			reorganize(song.sections[sectIndex]);
	}
	cout << global.tabs << "All sections organized\n";
}

void CHC_Editor::saveFile()
{
	banner(" " + song.shortname + ".CHC Save Prompt ");
	string ext = "_T";
	string filename = song.name.substr(0, song.name.length() - 4);
	do
	{
		cout << global.tabs << "S - Save & Overwrite Original File" << endl;
		cout << global.tabs << "A - Save as \"" << song.shortname << "_T.CHC\"" << endl;
		cout << global.tabs << "Q - Back Out" << endl;
		switch (menuChoices("sa"))
		{
		case -1:
			return;
		case 0:
			cout << global.tabs << "Saving " << song.shortname + ".CHC" << endl;
			song.create(song.name);
			saved = true;
			overwritten = true;
			global.quit = true;
			break;
		case 1:
			do
			{
				switch (fileOverwriteCheck(filename + ext + ".CHC"))
				{
				case -1:
					return;
				case 0:
					cout << global.tabs << endl;
					ext += "_T";
					break;
				case 1:
					song.create(filename + ext + ".CHC");
					saved = true;
					do
					{
						cout << global.tabs << endl;
						cout << global.tabs << "Swap loaded file to " << filename + ext << ".CHC? [Y/N]" << endl;
						switch (menuChoices("yn"))
						{
						case 0:
							song.name = filename + ext + ".CHC";
							song.shortname += ext;
							overwritten = true;
						case 1:
						case -1:
							global.quit = true;
						}
					} while (!global.quit);
				}
			} while (!global.quit);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::fixNotes()
{
	banner(" Fixing " + song.shortname + ".CHC ");
	unsigned long tracelinesCurved = 0, tracelinesDeleted = 0, phrasesDeleted = 0, phrasesShortened = 0, guardsDeleted = 0;
	bool duet = song.imc[0] == 0;
	for (unsigned sectIndex = 0; sectIndex < song.numSections; sectIndex++) //SongSections
	{
		SongSection& section = song.sections[sectIndex];
		if (section.battlePhase == SongSection::Phase::INTRO)
			continue;
		for (unsigned playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
		{
			for (unsigned chartIndex = 0; chartIndex < section.numCharts; chartIndex++)
			{
				Chart& chart = section.charts[(unsigned long long)playerIndex * section.numCharts + chartIndex];
				for (unsigned traceIndex = 1; traceIndex < chart.numTracelines; traceIndex++)
				{
					if (chart.tracelines[traceIndex - 1].angle == chart.tracelines[traceIndex].angle && !chart.tracelines[traceIndex - 1].curve)
					{
						chart.tracelines[traceIndex - 1].curve = true;
						cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
						cout << "Trace line #" << traceIndex - 1 << "'s curve flag set to true" << endl;
						tracelinesCurved++;
					}
				}
				unsigned long barsRemoved = 0, linesRemoved = 0;
				for (unsigned phraseIndex = 0, traceIndex = 0; phraseIndex < chart.numPhrases; phraseIndex++)
				{
					Phrase& phrase = chart.phrases[phraseIndex];
					if (phraseIndex && phrase.start)
					{
						if (phrase.pivotAlpha - chart.phrases[phraseIndex - 1].getEndAlpha() < SongSection::SAMPLE_GAP)
						{
							chart.phrases[phraseIndex - 1].changeEndAlpha((long)round(phrase.pivotAlpha - SongSection::SAMPLE_GAP));
							cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
							cout << "Phrase bar #" << phraseIndex + barsRemoved - 1 << " shortened" << endl;
							phrasesShortened++;
						}
					}
					for (; traceIndex < chart.numTracelines; traceIndex++)
						if (chart.tracelines[traceIndex].contains(phrase.pivotAlpha))
							break;
					while ((phrase.start && !phrase.end)
						&& (chart.tracelines[traceIndex].getEndAlpha() - phrase.pivotAlpha < 3000)
						&& (phrase.pivotAlpha - chart.tracelines[traceIndex].pivotAlpha > 800))
					{
						phrase.duration += chart.phrases[phraseIndex + 1ULL].duration;
						phrase.end = chart.phrases[phraseIndex + 1ULL].end;
						for (size_t i = 0; i < 12; i++)
							phrase.junk[i] = chart.phrases[phraseIndex + 1ULL].junk[i];
						for (; traceIndex < chart.numTracelines; traceIndex++)
						{
							if (chart.tracelines[traceIndex].contains(chart.phrases[phraseIndex + 1ULL].pivotAlpha))
							{
								if (!chart.tracelines[traceIndex - 1].changeEndAlpha(phrase.pivotAlpha))
								{
									cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
									chart.remove(--traceIndex, 't', linesRemoved);
									linesRemoved++;
								}
								chart.tracelines[traceIndex].changePivotAlpha(phrase.pivotAlpha);
								break;
							}
						}
						cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
						chart.remove(phraseIndex + 1, 'p', barsRemoved);
						barsRemoved++;
					}
				}
				tracelinesDeleted += linesRemoved;
				phrasesDeleted += barsRemoved;
				for (unsigned guardIndex = 0; guardIndex < chart.numGuards; guardIndex++)
				{
					while (guardIndex + 1 < chart.numGuards && chart.guards[guardIndex].pivotAlpha + 1600 > chart.guards[guardIndex + 1ULL].pivotAlpha)
					{
						cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
						chart.remove(guardIndex + 1, 'g');
						guardsDeleted++;
					}
				}
			}
		}
		if (section.battlePhase == SongSection::Phase::END || sectIndex + 1 == song.numSections)
			continue;
		if (duet || section.battlePhase == SongSection::Phase::HARMONY)
		{
			for (unsigned playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
			{
				for (long chartIndex = (long)section.numCharts - 1; chartIndex >= 0; chartIndex--)
				{
					if (section.charts[(unsigned long long)playerIndex * section.numCharts + chartIndex].getNumPhrases() > 0)
					{
						for (unsigned condIndex = 0; condIndex < section.numConditions; condIndex++)
						{
							long* effect = &section.conditions[condIndex].trueEffect;
							for (size_t i = 0; i < 2; i++, effect++)
							{
								if (*effect >= 0)
								{
									Chart& chart = section.charts[(unsigned long long)playerIndex * section.numCharts + chartIndex];
									long endTime = section.duration - (chart.phrases[chart.numPhrases - 1].getEndAlpha() + chart.pivotTime);
									Chart& chart2 = song.sections[*effect].charts[(unsigned long long)playerIndex * song.sections[*effect].numCharts];
									if (chart2.numPhrases)
									{
										long startTime = chart2.phrases[0].pivotAlpha + chart2.pivotTime;
										if (startTime + endTime < section.SAMPLE_GAP)
										{
											chart.phrases[chart.numPhrases - 1].changeEndAlpha((long)(section.duration - startTime - section.SAMPLE_GAP - chart.pivotTime));
											cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
											cout << "Phrase bar #" << chart.numPhrases - 1 << " shortened" << endl;
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
			for (int p = 0; p < 2; p++)
			{
				for (long chartIndex = (long)section.numCharts - 1; !global.quit && chartIndex >= 0; chartIndex--)
				{
					for (int playerIndex = section.numPlayers - 2 + p; !global.quit && playerIndex >= 0; playerIndex -= 2)
					{
						if (section.charts[(unsigned long long)playerIndex * section.numCharts + chartIndex].numPhrases != 0)
						{
							for (unsigned condIndex = 0; condIndex < section.numConditions; condIndex++)
							{
								long* effect = &section.conditions[condIndex].trueEffect;
								for (size_t i = 0; i < 2; i++, effect++)
								{
									if (*effect >= 0)
									{
										Chart& chart = section.charts[(unsigned long long)playerIndex * section.numCharts + chartIndex];
										long endTime = section.duration - (chart.phrases[chart.numPhrases - 1].getEndAlpha() + chart.pivotTime);
										Chart& chart2 = song.sections[*effect].charts[(unsigned long long)p * song.sections[*effect].numCharts];
										if (chart2.numPhrases)
										{
											long startTime = chart2.phrases[0].pivotAlpha + chart2.pivotTime;
											if (startTime + endTime < section.SAMPLE_GAP)
											{
												chart.phrases[chart.numPhrases - 1].changeEndAlpha((long)(section.duration - startTime - section.SAMPLE_GAP - chart.pivotTime));
												cout << global.tabs << section.name << " - Subsection #" << playerIndex * section.numCharts + chartIndex << ": ";
												cout << "Phrase bar #" << chart.numPhrases - 1 << " shortened" << endl;
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
	if (tracelinesCurved)
		cout << global.tabs << tracelinesCurved << (tracelinesCurved > 1 ? " straight Trace lines had their curve flags" : " straight Trace line had its curve flag") << " set to TRUE as a safety measure\n";
	if (tracelinesDeleted)
		cout << global.tabs << tracelinesDeleted << (tracelinesDeleted > 1 ? " Trace lines" : " Trace line") << " deleted to accommodate deleted phrase bars\n";
	if (phrasesDeleted)
		cout << global.tabs << phrasesDeleted << (phrasesDeleted > 1 ? " Phrase bars" : " Phrase bar") << " deleted for starting with durations under 3000 samples\n";
	if (phrasesShortened)
		cout << global.tabs << phrasesShortened << (phrasesShortened > 1 ? " Phrase bars" : " Phrase bar") << " Phrase Bars shortened for ending too close to following Phrase Bars\n";
	if (guardsDeleted)
		cout << global.tabs << guardsDeleted << (guardsDeleted > 1 ? " Guard marks" : " Guard mark") << " deleted for being within 1600 samples of a preceeding Guard Mark\n";
	if (tracelinesCurved || phrasesDeleted || phrasesShortened || guardsDeleted)
	{
		cout << global.tabs << '\n' << global.tabs << "Changes will be applied if you choose to save the file" << endl;
		saved = false;
	}
	else
		cout << global.tabs << "No changes made" << endl;
	song.optimized = true;
}

void CHC_Editor::PSPToPS2()
{
	banner(" " + song.shortname + ".CHC - Non-Duet Conversion ");
	bool p1 = false, p2 = false;
	do
	{
		cout << global.tabs << "Which players do you want to keep?\n";
		cout << global.tabs << "1 - Player 1 & Enemy\n";
		cout << global.tabs << "2 - Player 1 & Player 2\n";
		cout << global.tabs << "3 - Player 2 & Enemy" << endl;
		size_t value = menuChoices("123");
		switch (value)
		{
		case -1:
			return;
		case -3:
		case -2:
			break;
		default:
			p1 = value == 2;
			p2 = value & 1;
			song.name = song.name.substr(0, song.name.length() - 4) + "_PS2_" + to_string(p1 + 1) + 'v' + (!p2 ? 'E' : '2') + ".CHC";
			{
				size_t pos = song.name.find_last_of("\\");
				if (pos != string::npos)
					song.shortname = song.name.substr(pos + 1, song.name.length() - pos - 5);
				else
					song.shortname = song.shortname.substr(0, song.shortname.length() - 4);
			}
			global.quit = true;
		}
	} while (!global.quit);
	global.quit = false;
	snprintf(song.imc, 44, "/PROJECTS/STDATA/STAGE%02d/SONGDATA/ST%02d.IMC", song.stage, song.stage);
	song.audio[2].pan = song.audio[3].pan = song.audio[6].volume = song.audio[7].volume = 0;
	song.audio[4].pan = song.audio[5].pan = 32767;
	for (size_t i = 0; i < song.numSections; i++)
	{
		SongSection& section = song.sections[i];
		if (!section.organized) //Organize with the duet setting
			reorganize(section);
		section.charts.erase(3ULL * section.numCharts, section.numCharts);
		if (p1 || p2)
			section.charts.moveElements(2ULL * section.numCharts, section.numCharts, section.numCharts);
		if (!p1)
			section.charts.erase(2ULL * section.numCharts, section.numCharts);
		else
			section.charts.erase(0, section.numCharts);
		if (section.numCharts & 1)
		{
			section.charts.emplace_back();
			section.charts.insert(section.numCharts, Chart());
			section.numCharts++;
		}
		for (size_t c = 1; c < section.numCharts >> 1; c++)
		{
			section.charts.moveElements(c << 1, c);
			section.charts.moveElements((c << 1) + section.numCharts, c + section.numCharts);
		}
		section.numCharts >>= 1;
		section.charts.moveElements((unsigned long long)section.numCharts << 1, section.numCharts, section.numCharts);
		if (p2)
			section.swapped = (section.swapped >> 1) + 4;
	}
	for (size_t s = 0; s < 5; s++)
	{
		song.energyDamageFactors[0][s].attackInitial *= 2;
		song.energyDamageFactors[0][s].attackRelease *= 2;
		song.energyDamageFactors[0][s].attackMiss *= 2;
		song.energyDamageFactors[0][s].chargeInitial *= 2;
		song.energyDamageFactors[0][s].chargeRelease *= 2;
		song.energyDamageFactors[0][s].guardMiss *= 2;
	}
	duet = false;
	saved = false;
}

void CHC_Editor::swapIMC()
{
	banner(" " + song.shortname + ".CHC - IMC Swap ");
	string newIMC;
	cout << global.tabs << "Provide the name (just the name) of the .IMC file you wish to use (Or 'Q' to back out to Modify Menu)\n";
	cout << global.tabs << "Current IMC File: " << song.imc << endl;
	cout << global.tabs << "Input: ";
	cin >> ws >> newIMC;
	if (newIMC.length() != 1 || tolower(newIMC[0]) != 'q')
	{
		for (unsigned i = 0; i < newIMC.length(); i++)
			newIMC[i] = toupper(newIMC[i]);
		if (newIMC.find(".IMC") == string::npos)
			newIMC += ".IMC";
		memcpy(song.imc + 34, newIMC.c_str(), newIMC.length());
		song.imc[34 + newIMC.length()] = 0;
		saved = false;
	}
}

void CHC_Editor::audioSettings()
{
	bool duet = strstr(song.imc, ".IMC");
	size_t channel;
	do
	{
		if (duet)
		{
			banner(" " + song.shortname + ".CHC - Audio Channels ");
			cout << global.tabs << "          ||        Volume        ||        Paning        ||\n";
			cout << global.tabs << " Channels ||  Value  ||  Percent  ||   Left   |   Right   ||\n";
			cout << global.tabs << "==========================================================||\n";
			for (unsigned index = 0; index < 8; index++)
			{
				cout << global.tabs << "     " << index + 1 << "    ||";
				cout << setw(7) << song.audio[index].volume << "  ||" << setw(7) << ((double)song.audio[index].volume) * 100 / 32767 << "%   ||";
				switch (song.audio[index].pan)
				{
				case 0:     cout << "   Left   |           ||\n"; break;
				case 16383: cout << "        Center        ||\n"; break;
				case 32767: cout << "          |   Right   ||\n"; break;
				default: cout << setw(8) << 100 - (((double)song.audio[index].pan) * 100 / 32767) << "% |" << setw(8) << ((double)song.audio[index].pan) * 100 / 32767 << "% ||\n";
				}
			}
			cout << global.tabs << "==========================================================||\n";
			cout << global.tabs << "Choose the channel you wish to edit [Type 'Q' to exit audio settings]" << endl;
			channel = menuChoices("12345678");
		}
		else
		{
			banner(" " + song.shortname + ".CHC - Audio Channels [Duet Stages] ");
			cout << global.tabs << "          ||        Volume        ||\n";
			cout << global.tabs << " Channels ||  Value  ||  Percent  ||\n";
			cout << global.tabs << "==================================||\n";
			cout << global.tabs << "   1 & 2  ||        Unused        ||\n";
			for (unsigned index = 2; index < 8; index++)
				cout << global.tabs << "     " << index + 1 << "    ||" << setw(7) << song.audio[index].volume << "  ||\n";
			cout << global.tabs << "==================================||" << endl;
			cout << global.tabs << "Choose the channel you wish to edit [Type 'Q' to exit audio settings]" << endl;
			channel = menuChoices("345678");
		}
		switch (channel)
		{
		case -1:
			global.quit = true;
			break;
		case -3:
		case -2:
			break;
		default:
			if (duet) channel += 2;
			do
			{
				banner(" " + song.shortname + ".CHC - Audio Channel " + to_string(channel + 1));
				string choices = "v";
				cout << global.tabs << "V - Volume\n";
				if (duet)
				{
					cout << global.tabs << "P - Panning\n";
					choices += 'p';
				}
				cout << global.tabs << "B - Choose Another Channel\n";
				cout << global.tabs << "Q - Exit Audio Settings" << endl;
				switch (menuChoices(choices + 'b'))
				{
				case 0:
					do
					{
						banner(" " + song.shortname + ".CHC - Audio Channel " + to_string(channel + 1) + " - Volume");
						cout << global.tabs << "Provide value for Volume (0 - 32767[100%]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n";
						cout << global.tabs << "Current Volume: " << song.audio[channel].volume << " (" << 100.0 * song.audio[channel].volume / 32767 << "%)" << endl;
						cout << global.tabs << "Input: "; unsigned long oldVol = song.audio[channel].volume;
						switch (valueInsert(song.audio[channel].volume, false, 0UL, 32767UL, "b"))
						{
						case 2:
							global.quit = true;
							break;
						case 1:
							if (song.audio[channel].volume != oldVol)
								saved = false;
							break;
						case 0:
							return;
						case -1:
						case -3:
							cout << global.tabs << "Value must be between 0 & 32767. Not adjusting." << endl << global.tabs << '\n';
							cin.clear();
							break;
						case -4:
							cout << global.tabs << "\"" << global.invalid << "\" is not a valid response." << endl << global.tabs << '\n';
							cin.clear();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				case 1:
					do
					{
						banner(" " + song.shortname + ".CHC - Audio Channel " + to_string(channel + 1) + " - Panning");
						cout << global.tabs << "Provide value for Panning (Left[0] - Center[16383] - Right[32767]) ['B' for Volume/Panning Menu | 'Q' to exit audio settings]\n";
						cout << global.tabs << "Current Panning: "; unsigned long oldPan = song.audio[channel].pan;
						switch (song.audio[channel].pan)
						{
						case 0: cout << "Left (0)\n"; break;
						case 16383: cout << "Center (16383)\n"; break;
						case 32767: cout << "Right (32767)\n"; break;
						default: cout << 100 - (song.audio[channel].pan * 100.0 / 32767) << "% Left | "
							<< song.audio[channel].pan * 100.0 / 32767 << "% Right (" << song.audio[channel].pan << ")" << endl;
						}
						cout << global.tabs << "Input: ";
						switch (valueInsert(song.audio[channel].pan, false, 0UL, 32767UL, "b"))
						{
						case 2:
							global.quit = true;
							break;
						case 1:
							if (song.audio[channel].pan != oldPan)
								saved = false;
							break;
						case 0:
							return;
						case -1:
						case -3:
							cout << global.tabs << "Value must be between 0 & 32767. Not adjusting.\n" << global.tabs << endl;
							cin.clear();
							break;
						case -4:
							cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
							cin.clear();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				case 2:
					global.quit = true;
					break;
				case -1:
					return;
				case -3:
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
		banner(" " + song.shortname + ".CHC - Win/Loss Animations ");
		static const string ssqStrings[] = { "Win A", "Win B", "Lost Pre", "Lost Post" };
		cout << global.tabs << "            ||       Frames       ||\n";
		cout << global.tabs << " Animations ||  First  ||   Last  ||\n";
		cout << global.tabs << "==================================||\n";
		cout << global.tabs << " 1 - Win A  ||" << setw(7) << song.events[0].first << "  ||" << setw(7) << song.events[0].last << "  ||\n";
		cout << global.tabs << " 2 - Win B  ||" << setw(7) << song.events[1].first << "  ||" << setw(7) << song.events[1].last << "  ||\n";
		cout << global.tabs << "3 - Lost Pre||" << setw(7) << song.events[2].first << "  ||" << setw(7) << song.events[2].last << "  ||\n";
		cout << global.tabs << "4 - Lt. Loop||" << setw(7) << song.events[3].first << "  ||" << setw(7) << song.events[3].last << "  ||\n";
		cout << global.tabs << "\n\t\t      ||Select an SSQ event ('1'/'2'/'3'/'4') [Type 'Q' to exit win/loss anim. settings]" << endl;
		size_t anim = menuChoices("1234");
		switch (anim)
		{
		case -1:
			global.quit = true;
			break;
		case -3:
		case -2:
			break;
		default:
			do
			{
				banner(" " + song.shortname + ".CHC - SSQ Animation:" + ssqStrings[anim]);
				cout << global.tabs << "F - Adjust first frame: " << song.events[anim].first << '\n';
				cout << global.tabs << "L - Adjust last  frame: " << song.events[anim].last << '\n';
				cout << global.tabs << "b - Choose different animation\n";
				cout << global.tabs << "Q - Exit win/loss animation settings " << endl;
				switch (menuChoices("flb"))
				{
				case -1:
					return;
				case -3:
					break;
				case 2:
					global.quit = true;
					break;
				case 0:
					do
					{
						banner(" " + song.shortname + ".CHC - SSQ Animation: " + ssqStrings[anim] + " - First Frame");
						cout << global.tabs << "Enter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n";
						cout << global.tabs << "(Can be a decimal)" << endl; float oldAnim = song.events[anim].first;
						switch (valueInsert(song.events[anim].first, false, "b"))
						{
						case 0:
							return;
						case 1:
							if (song.events[anim].first != oldAnim)
								saved = false;
							break;
						case 2:
							global.quit = true;
							break;
						case -1:
							cout << global.tabs << "Value must be positive. Not adjusting." << endl; cin.clear();
							break;
						case -4:
							cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
							cin.clear();
						}
					} while (!global.quit);
					global.quit = false;
					break;
				case 1:
					do
					{
						banner(" " + song.shortname + ".CHC - SSQ Animation: " + ssqStrings[anim] + " - Last Frame");
						cout << global.tabs << "Enter a positive value to change to [Type 'b' to choose a different frame | 'Q' to exit win/loss anim. settings]\n";
						cout << global.tabs << "(Can be a decimal)" << endl; float oldAnim = song.events[anim].last;
						switch (valueInsert(song.events[anim].last, false, "b"))
						{
						case 0:
							return;
						case 1:
							if (song.events[anim].last != oldAnim)
								saved = false;
							break;
						case 2:
							global.quit = true;
							break;
						case -1:
							cout << global.tabs << "Value must be positive. Not adjusting." << endl; cin.clear();
							break;
						case -4:
							cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
							cin.clear();
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
		banner(" " + song.shortname + ".CHC - Gameplay Speed Change ");
		cout << global.tabs << "Provide a value for the change [Type 'Q' to exit speed settings]\n";
		cout << global.tabs << "Can be a decimal... and/or negative with weird effects\n";
		cout << global.tabs << "Current Value: " << song.speed << endl;
		cout << global.tabs << "Input: "; float oldSpeed = song.speed;
		switch (valueInsert(song.speed, true))
		{
		case 1:
			if (song.speed != oldSpeed)
				saved = false;
			break;
		case 0:
			global.quit = true;
		case -3:
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
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
		banner(" " + song.shortname + ".CHC - Sections ");
		cout << global.tabs << "P - Set Unconditional Play Order\n";
		cout << global.tabs << "R - Rearrange Sections\n";
		cout << global.tabs << "S - Swap Player 1 & Player 2 for All Sections\n";
		cout << global.tabs << "C - Choose a Single Section to Edit\n";
		cout << global.tabs << "T - Test Section Accessibility\n";
		cout << global.tabs << "Q - Back out to Modify Menu" << endl;
		size_t index = menuChoices("prsct");
		switch (index)
		{
		case -1:
			return;
		case -3:
			cout << global.tabs << "? [TBD]" << '\n' << global.tabs << '\n';
		case -2:
			break;
		default:
			adjustTabs(3);
			(this->*results[index])();
			adjustTabs(2);
		}
	} while (!global.quit);
}

void CHC_Editor::playerSwapAll()
{
	banner(" " + song.shortname + ".CHC - Complete Player Swap");
	for (unsigned long sectIndex = 0; sectIndex < song.numSections; sectIndex++)
		playerSwap(song.sections[sectIndex]);
	saved = false;
}

void CHC_Editor::playOrder()
{
	banner(" " + song.shortname + ".CHC - Play Order ");
	List<unsigned long> sectionIndexes;
	do
	{
		cout << global.tabs << "Type the index for each section in the order you wish - w/ spaces in-between.\n";
		for (unsigned index = 0; index < song.numSections; index++)
			cout << global.tabs << index << " - " << song.sections[index].name << endl;
		switch (vectorValueInsert(sectionIndexes, "yn", song.numSections, false))
		{
		case -1:
			break;
		case 0:
			cout << global.tabs << "Play Order Selection cancelled." << endl;
			return;
		case 1:
			if (!sectionIndexes.size())
			{
				do
				{
					cout << global.tabs << "No sections have been selected. Quit Play Order Selection? [Y/N]" << endl;
					switch (menuChoices("yn"))
					{
					case -1:
					case 0:
						cout << global.tabs << "Play Order Selection cancelled." << endl;
						return;
					case 1:
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
		case 2:
			if (!sectionIndexes.size())
			{
				cout << global.tabs << "Play Order Selection cancelled." << endl;
				return;
			}
			else
			{
				global.quit = true;
				break;
			}
		case 3:
			cin.get();
			cout << global.tabs << "Ok... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n";
		}
	} while (!global.quit);
	global.quit = false;
	cout << global.tabs;
	for (unsigned index = 0; index < sectionIndexes.size(); index++)
	{
		if (index + 1ULL < sectionIndexes.size())
		{
			saved = false;
			SongSection::Condition& cond = song.sections[sectionIndexes[index]].conditions.front();
			cond.type = 0;
			cond.trueEffect = sectionIndexes[index + 1ULL];
		}
		else
		{
			if (!pathTest(sectionIndexes[index]))
			{
				SongSection::Condition& cond = song.sections[sectionIndexes[index]].conditions.front();
				cond.type = 0;
				cond.trueEffect = song.numSections;
			}
		}
		cout << song.sections[sectionIndexes[index]].name << " ";
		saved = false;
	}
	cout << endl;
}

void CHC_Editor::rearrange()
{
	banner(" " + song.shortname + ".CHC - Section Rearrangement ");
	unsigned long startIndex, numElements, position;
	do
	{
		cout << global.tabs << "Provide the starting index for the range of sections you want to move.\n";
		for (unsigned index = 0; index < song.numSections; index++)
			cout << global.tabs << index << " - " << song.sections[index].name << '\n';
		cout << global.tabs << "Input: ";
		switch (valueInsert(startIndex, false, 0UL, song.numSections - 1))
		{
		case 1:
			global.quit = true;
			break;
		case 0:
			cout << global.tabs << "Section rearrangement cancelled." << endl;
			return;
		case -1:
			cout << global.tabs << "Given value cannot be negative\n" << global.tabs << '\n';
			break;
		case -3:
			cout << global.tabs << "Given value cannot be greater than " << song.numSections - 1 << '\n' << global.tabs << '\n';
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response\n" << global.tabs << '\n';
			break;
		}
	} while (!global.quit);
	global.quit = false;
	if (song.numSections - startIndex == 1)
		numElements = 1;
	else
	{
		do
		{
			cout << global.tabs << "How many elements do you want to move? [Max # of movable elements: " << song.numSections - startIndex << "]\n";
			cout << global.tabs << "Input: ";
			switch (valueInsert(numElements, false, 1UL, song.numSections - startIndex))
			{
			case 1:
				global.quit = true;
				break;
			case 0:
				cout << global.tabs << "Section rearrangement cancelled." << endl;
				return;
			case -1:
			case -2:
				cout << global.tabs << "Given value cannot be less than 1\n" << global.tabs << '\n';
				break;
			case -3:
				cout << global.tabs << "Given value cannot be greater than " << song.numSections - startIndex << '\n' << global.tabs << '\n';
				break;
			case -4:
				cout << global.tabs << "\"" << global.invalid << "\" is not a valid response\n" << global.tabs << '\n';
				break;
			}
		} while (!global.quit);
		global.quit = false;
	}
	if (numElements < song.numSections)
	{
		long alpha;
		if (song.numSections - numElements == 1)
		{
			if (!startIndex)
			{
				cout << global.tabs << "Moving elements to the end of the list" << endl;
				position = song.numSections;
				alpha = 1;
			}
			else
			{
				cout << global.tabs << "Moving elements to the beginning of the list" << endl;
				position = 0;
				alpha = -1;
			}
		}
		else
		{
			do
			{
				cout << global.tabs << "Provide the index that these elements will be moved to.\n";
				for (unsigned index = 0; index < song.numSections; index++)
				{
					if (index < startIndex || index > startIndex + numElements)
						cout << global.tabs << index << " - " << song.sections[index].name << '\n';
				}
				if (song.numSections > startIndex + numElements)
					cout << global.tabs << song.numSections << " - End of the list\n";
				cout << global.tabs << "Invalid position range: " << startIndex << " - " << startIndex + numElements << '\n';
				cout << global.tabs << "Input: ";
				switch (valueInsert(position, false, 0UL, song.numSections))
				{
				case 1:
					if (position < startIndex)
					{
						alpha = position - startIndex;
						global.quit = true;
					}
					else if (position > startIndex + numElements)
					{
						alpha = position - (startIndex + numElements);
						global.quit = true;
					}
					else
						cout << global.tabs << "Cannot choose a value within the range of sections being moved" << endl;
					break;
				case 0:
					cout << global.tabs << "Section rearrangement cancelled." << endl;
					return;
				case -1:
					cout << global.tabs << "Given value cannot be negative\n" << global.tabs << '\n';
					break;
				case -3:
					cout << global.tabs << "Given value cannot be greater than " << song.numSections << '\n' << global.tabs << '\n';
					break;
				case -4:
					cout << global.tabs << "\"" << global.invalid << "\" is not a valid response\n" << global.tabs << '\n';
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
		for (unsigned sectIndex = 0; sectIndex < song.numSections; sectIndex++)
		{
			for (unsigned condIndex = 0; condIndex < song.sections[sectIndex].numConditions; condIndex++)
			{
				long* effect = &song.sections[sectIndex].conditions[condIndex].trueEffect;
				for (size_t eff = 0; eff < 2; eff++, effect++)
				{
					if (*effect >= 0)
					{
						if (*effect < (long)startIndex)
						{
							if (*effect >= (long)position)
								*effect += numElements;
						}
						else if (*effect < long(startIndex + numElements))
							*effect += alpha;
						else if (*effect < (long)position)
							*effect -= numElements;
					}
				}
			}
		}
		song.sections.moveElements(startIndex, position, numElements);
		for (unsigned index = 0; index < song.numSections; index++)
			cout << global.tabs << index << " - " << song.sections[index].name << '\n';
	}
	else
		cout << global.tabs << "Really? You didn't even do anything with that... *sigh*\n";
	cout << global.tabs << endl;
}

void CHC_Editor::adjustFactors()
{
	do
	{
		banner(" " + song.shortname + ".CHC - Damage/Energy Factors ", 1.53f);
		static const string headers[] = { "||    Starting Energy   ||", "|| Initial-Press Energy ||", "|| Initial-Press Damage ||", "||   Guard Energy Gain  ||",
							 "||  Attack Miss Damage  ||", "||   Guard Miss Damage  ||", "|| Sustain Energy Coef. ||", "|| Sustain Damage Coef. ||" };
		for (unsigned player = 0; player < 4; player += 2)
		{
			cout << global.tabs << "        Player " << player + 1 << "      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||";
			cout << "||        Player " << player + 2 << "      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n";
			cout << global.tabs << setfill('=') << setw(81) << "||||" << setw(79) << "||" << setfill(' ');
			for (char factor = 0; factor < 8; factor++)
			{
				cout << endl << "\t      ";
				for (unsigned index = player; index < player + 2; index++)
				{
					cout << headers[factor];
					for (unsigned phase = 0; phase < 5; phase++)
					{
						float* val = &song.energyDamageFactors[index][phase].start + factor;
						cout << "  " << setw(5) << (*val) * 100 << "% ||";
					}
				}
			}
			cout << endl << global.tabs << setw(160) << setfill('=') << "||" << setfill(' ') << '\n';
		}
		cout << global.tabs << "Select a player ('1'/'2'/'3'/'4') [Type 'Q' to exit factor settings]" << endl;
		size_t player = menuChoices("1234");
		switch (player)
		{
		case -1:
			global.quit = true;
			break;
		case -3:
		case -2:
			break;
		default:
			do
			{
				banner(" " + song.shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " ");
				cout << global.tabs << "        Player " << player + 1 << "      ||  Intro  ||  Charge ||  Battle || Harmony ||   End   ||\n";
				cout << global.tabs << setw(79) << setfill('=') << "||" << setfill(' ');
				for (char factor = 0; factor < 8; factor++)
				{
					cout << endl << "\t      " << headers[factor];
					for (unsigned phase = 0; phase < 5; phase++)
					{
						float* val = &song.energyDamageFactors[player][phase].start + factor;
						cout << "  " << setw(5) << *val * 100 << "% ||";
					}
				}
				cout << endl << global.tabs << setw(79) << setfill('=') << "||" << setfill(' ') << '\n';
				cout << global.tabs << "Select a phase ('I'/'C'/'B'/'H'/'E') [Type 'P' to choose a different player | 'Q' to exit factor settings]" << endl;
				size_t phase = menuChoices("icbhep"); string phaseName[5] = { "Intro", "Charge", "Battle", "Harmony", "End" };
				switch (phase)
				{
				case -1:
					return;
				case -3:
				case -2:
					break;
				case 5:
					global.quit = true;
					break;
				default:
					do
					{
						banner(" " + song.shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " | " + phaseName[phase] + " ");
						cout << global.tabs << "          Player " << player + 1 << "        || " << setw(7) << phaseName[phase] << " ||\n";
						cout << global.tabs << setw(39) << setfill('=') << "||" << setfill(' ') << '\n';
						cout << global.tabs << " 1 - Starting Energy      ||  " << setw(5) << song.energyDamageFactors[player][phase].start * 100 << "% ||\n";
						cout << global.tabs << " 2 - Initial-Press Energy ||  " << setw(5) << song.energyDamageFactors[player][phase].chargeInitial * 100 << "% ||\n";
						cout << global.tabs << " 3 - Initial-Press Damage ||  " << setw(5) << song.energyDamageFactors[player][phase].attackInitial * 100 << "% ||\n";
						cout << global.tabs << " 4 - Guard Energy Gain    ||  " << setw(5) << song.energyDamageFactors[player][phase].guardEnergy * 100 << "% ||\n";
						cout << global.tabs << " 5 - Attack Miss Damage   ||  " << setw(5) << song.energyDamageFactors[player][phase].attackMiss * 100 << "% ||\n";
						cout << global.tabs << " 6 - Guard  Miss Damage   ||  " << setw(5) << song.energyDamageFactors[player][phase].guardMiss * 100 << "% ||\n";
						cout << global.tabs << " 7 - Sustain Energy Coef. ||  " << setw(5) << song.energyDamageFactors[player][phase].chargeRelease * 100 << "% ||\n";
						cout << global.tabs << " 8 - Sustain Damage Coef. ||  " << setw(5) << song.energyDamageFactors[player][phase].attackRelease * 100 << "% ||\n";
						cout << global.tabs << setw(39) << setfill('=') << "||\n";
						cout << global.tabs << "Select a factor by number [Type 'P' to choose a different phase | 'Q' to exit factor settings]" << endl;
						size_t factor = menuChoices("12345678p");
						switch (factor)
						{
						case -1:
							return;
						case -3:
						case -2:
							break;
						case 8:
							global.quit = true;
							break;
						default:
							do
							{
								static const string enders[] = { " | Starting Energy ", " | Initial-Press Energy ", " | Initial-Press Damage ", " | Guard Energy Gain ",
													" | Attack Miss Damage ", " | Guard Miss Damage ", " | Sustain Energy Coef. ", " | Sustain Damage Coef. " };
								banner(" " + song.shortname + ".CHC - Damage/Energy Factors | Player " + to_string(player + 1) + " | " + phaseName[phase] + enders[factor]);
								float* val = &song.energyDamageFactors[player][phase].start + factor;
								cout << global.tabs << "        Player " << player + 1 << "      || " << setfill(' ') << setw(7) << phaseName[phase] << " ||\n";
								cout << global.tabs << setw(35) << setfill('=') << "||" << setfill(' ') << '\n';
								cout << "\t      " << headers[factor] << " " << setw(6) << *val << "  ||\n";
								cout << global.tabs << setw(35) << setfill('=') << "||" << setfill(' ') << '\n';
								cout << global.tabs << "Provide a new value for this factor [Type 'F' to choose a different factor | 'Q' to exit factor settings]\n";
								cout << global.tabs << "Can be a decimal... and/or negative with weird effects" << endl;
								cout << global.tabs << "Input: "; float oldFac = *val;
								switch (valueInsert(*val, true, 0.0f, 0.0f, "f"))
								{
								case 2:
									global.quit = true;
									break;
								case 1:
									if (*val != oldFac) saved = false;
									break;
								case 0:
									return;
								case -4:
									cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
									cin.clear();
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
	banner(" " + song.shortname + ".CHC - Section Transversal ");
	pathTest(0, true);
}

bool CHC_Editor::pathTest(size_t startIndex, bool show)
{
	if (startIndex < song.numSections)
	{
		bool** conditionTested = new bool* [song.numSections]();
		bool* results = new bool[song.numSections]();
		for (size_t sectIndex = song.numSections; sectIndex > startIndex;)
			testSection(--sectIndex, conditionTested, results, nullptr);
		if (show) //Only used for the full path test
		{
			for (size_t sectIndex = 0; sectIndex < song.numSections; sectIndex++)
			{
				if (conditionTested[sectIndex] != nullptr)
				{
					for (size_t condIndex = 0; condIndex < song.sections[sectIndex].numConditions; sectIndex++)
						conditionTested[sectIndex][condIndex] = false;
				}
			}
			bool* reach = new bool[song.numSections]();
			testSection(startIndex, conditionTested, results, reach);
			cout << global.tabs << string(42, '=') << "||\n";
			cout << global.tabs << " Section || Accessible? || Can Reach End? ||\n";
			cout << setfill(' ');
			for (size_t sectIndex = 0; sectIndex < song.numSections; sectIndex++)
			{
				cout << global.tabs << setw(8) << right << song.sections[sectIndex].name << " ||      ";
				if (reach[sectIndex])
					cout << "Y      ||        ";
				else
					cout << "N      ||        ";
				if (results[sectIndex])
					cout << "Y       ||\n";
				else
					cout << "N       ||\n";
			}
			cout << global.tabs << string(42, '=') << "||" << endl;
		}
		for (size_t sectIndex = 0; sectIndex < song.numSections; sectIndex++)
			if (conditionTested[sectIndex] != nullptr)
				delete[song.sections[sectIndex].numConditions] conditionTested[sectIndex];
		bool res = results[startIndex];
		delete[song.numSections] results;
		delete[song.numSections] conditionTested;
		return res;
	}
	else
		return false;
}

char CHC_Editor::testSection(size_t sectIndex, bool** conditionTested, bool* results, bool* reach)
{
	if (!results[sectIndex] || reach != nullptr)
	{
		if (reach != nullptr)
			reach[sectIndex] = true;
		if (conditionTested[sectIndex] == nullptr)
			conditionTested[sectIndex] = new bool[song.sections[sectIndex].numConditions]();
		for (size_t c = 0; c < song.sections[sectIndex].numConditions; c++)
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

void CHC_Editor::traverseCondition(size_t sectIndex, size_t condIndex,bool** conditionTested, bool* results, bool* reach)
{
	conditionTested[sectIndex][condIndex] = true;
	SongSection::Condition& cond = song.sections[sectIndex].conditions[condIndex];
	if (cond.trueEffect >= 0)
	{
		if (cond.trueEffect >= (long)song.numSections)
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
			if (cond.falseEffect >= (long)song.numSections)
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
		banner(" " + song.shortname + ".CHC - Section Selection");
		cout << global.tabs << "Type the number for the section that you wish to edit\n";
		unsigned long val;
		for (unsigned long sectIndex = 0; sectIndex < song.numSections; sectIndex++)
			cout << global.tabs << sectIndex << " - " << song.sections[sectIndex].name << '\n';
		switch (valueInsert(val, false, 0UL, song.numSections - 1))
		{
		case 0:
			global.quit = true;
			break;
		case -1:
			cout << global.tabs << "Given section value must be positive." << endl;
			cin.clear();
			break;
		case -3:
			cout << global.tabs << "Given section value cannot exceed " << song.numSections - 1 << endl;
			cin.clear();
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
			break;
		case 1:
			adjustTabs(4);
			do
			{
				banner(" " + song.shortname + ".CHC - " + song.sections[val].name + " - Modify ");
				string choices = "nafptde"; //ENABLE CONDITION OPTION
				cout << global.tabs << "N - Name\n";
				cout << global.tabs << "A - Change the section of audio used\n";
				cout << global.tabs << "F - Adjust SSQ frame range\n";
				cout << global.tabs << "P - Phase: ";
				switch (song.sections[val].battlePhase)
				{
				case SongSection::Phase::INTRO: cout << "INTRO\n"; break;
				case SongSection::Phase::CHARGE: cout << "CHARGE\n"; break;
				case SongSection::Phase::BATTLE: cout << "BATTLE\n";  break;
				case SongSection::Phase::FINAL_AG: cout << "FINAL_AG\n";  break;
				case SongSection::Phase::HARMONY: cout << "HARMONY\n"; break;
				case SongSection::Phase::END: cout << "END\n";  break;
				default: cout << "FINAL_I\n";
				}
				cout << global.tabs << "T - Tempo: " << song.sections[val].tempo << '\n';
				cout << global.tabs << "D - Duration: " << song.sections[val].duration << '\n';
				//cout << global.tabs << "C - Modify Conditions (" << song.sections[val].numConditions << ")\n";
				cout << global.tabs << "E - Check if stage-end is reachable\n";
				if (!song.sections[val].organized)
				{
					cout << global.tabs << "O - Organize this section\n";
					choices += 'o';
				}
				cout << global.tabs << "S - Swap players\n";
				choices += 's';
				cout << global.tabs << "Q - Choose another section" << endl;
				size_t index = menuChoices(choices);
				switch (index)
				{
				case -1:
					global.quit = true;
					break;
				case -3:
					cout << global.tabs << "? [TBD]" << '\n' << global.tabs << '\n';
				case -2:
					break;
				default:
					adjustTabs(5);
					switch (choices[index])
					{
					case 'n':
						changeName(song.sections[val]);
						break;
					case 'a':
						changeAudio(song.sections[val]);
						break;
					case 'f':
						changeFrames(song.sections[val]);
						break;
					case 'p':
						changeFrames(song.sections[val]);
						break;
					case 't':
						adjustTempo(song.sections[val]);
						break;
					case 'd':
						adjustDuration(song.sections[val]);
						break;
					case 'c':
						conditionMenu(song.sections[val]);
						break;
					case 'e':
						banner(" " + song.shortname + ".CHC - " + song.sections[val].name + " - Transversal ");
						if (pathTest(val))
							cout << global.tabs << song.sections[val].name << " can reach the end of the stage.\n";
						else
							cout << global.tabs << song.sections[val].name << " is unable to reach the end of the stage.\n";
						break;
					case 'o':
						banner(" " + song.shortname + ".CHC - " + song.sections[val].name + " - Organize ");
						reorganize(song.sections[val]);
						break;
					case 's':
						banner(" " + song.shortname + ".CHC - " + song.sections[val].name + " - Player Swap ");
						playerSwap(song.sections[val]);
						saved = false;
					}
					adjustTabs(4);
				}
			} while (!global.quit);
			global.quit = false;
			adjustTabs(3);
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
		for (unsigned pl = 0; pl < 4; pl++)
		{
			unsigned char currentPlayer;
			if (!duet) currentPlayer = pl & 1;
			else currentPlayer = pl;
			List<sectNote>& player = notes[currentPlayer];
			for (unsigned chIndex = 0; chIndex < section.numCharts; chIndex++)
			{
				unsigned playerIndex, chartIndex;
				if (!duet)
				{
					playerIndex = ((2 * ((pl >> 1) ^ (chIndex & 1))) + (pl & 1)) * section.numCharts;
					chartIndex = (section.numCharts >> 1) * (pl >> 1) + (((section.numCharts & 1) * (pl >> 1) + chIndex) >> 1);
				}
				else
				{
					playerIndex = pl * section.numCharts;
					chartIndex = chIndex;
				}
				Chart& ch = section.charts[(unsigned long long)playerIndex + chartIndex];
				unsigned index = 0;
				if (section.swapped >= 4 || ((!(pl & 1)) != (!duet && section.swapped & 1)))
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
					for (size_t i = 0; i < ch.numPhrases; i++)
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
					if (ch.numTracelines > 1)
					{
						for (size_t i = 0; i < ch.numTracelines; i++)
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
					for (size_t i = 0; i < ch.numPhrases; i++)
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
					if (ch.numTracelines > 1)
					{
						for (size_t i = 0; i < ch.numTracelines; i++)
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
		section.numCharts = 1;
		const long double SAMPLES_PER_BEAT = 2880000.0L / section.tempo;
		for (unsigned pl = 0; pl < (!duet ? 2U : 4U); pl++)
		{
			unsigned chartDistance = 0;
			unsigned currentPlayer = pl;
			newCharts[currentPlayer].emplace_back();
			Chart* currentChart = &newCharts[currentPlayer][0];
			bool isPlayer = !(pl & 1) || section.swapped >= 4;
			if (notes[pl].size())
			{
				currentChart->clearTracelines();
				for (unsigned long ntIndex = 0, startingIndex = 0; ntIndex < notes[pl].size(); ntIndex++)
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
									if (isPlayer != (!duet && section.swapped & 1))	//It's not the enemy's original charts
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
									if ((isPlayer == (!duet && section.swapped & 1)) || (ntIndex + 2ULL != notes[pl].size() && dynamic_cast<Path*>(notes[pl][ntIndex + 2ULL].second) == nullptr))
									{
										long endAlpha = notes[pl][ntIndex + 1ULL].first - SongSection::SAMPLE_GAP - 1 - currentChart->pivotTime;
										float angle = 0;
										for (int t = currentChart->numTracelines - 1; t >= 0;)
										{
											//If the new end point is less than or equal to the last trace line's pivot
											if (!currentChart->tracelines[t].changeEndAlpha(endAlpha))
											{
												//Save angle value
												angle = currentChart->tracelines[t].angle;
												//Remove trace line
												cout << global.tabs << section.name << ": ";
												currentChart->remove(t--, 't');
												if (currentChart->numTracelines)
												{
													//Same idea for phrase bars
													for (int p = currentChart->numPhrases - 1; p >= 0;)
													{
														//If the phrase bar is past the trace line
														if (currentChart->tracelines[t].getEndAlpha() <= currentChart->phrases[p].pivotAlpha)
														{
															//Delete the phrase bar
															if (!currentChart->phrases[p].start)
																currentChart->phrases[p - 1ULL].end = true;
															cout << global.tabs << section.name << ": ";
															currentChart->remove(p--, 'p');
														}
														else
														{
															//If inside the trace line
															if (currentChart->tracelines[t] <= currentChart->phrases[p])
																currentChart->phrases[p].changeEndAlpha(endAlpha);
															break;
														}
													}
												}
												else
												{
													for (int p = currentChart->numPhrases - 1; p >= 0;)
													{
														cout << global.tabs << section.name << ": ";
														currentChart->remove(p--, 'p');
													}
												}
											}
											else
												break;
										}
										if (currentChart->numTracelines)
										{
											Traceline* tr2 = new Traceline(endAlpha, 1, angle);
											currentChart->add(tr2);
											delete tr2;
										}
										if (isPlayer != (!duet && section.swapped & 1)) //It's not the enemy's original charts
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
								if ((duet || song.stage == 0 || song.stage == 11 || song.stage == 12) && notes[pl][ntIndex + 1ULL].first - notes[pl][ntIndex].first >= long(5.5 * SAMPLES_PER_BEAT))
								{
									if (!currentChart->numTracelines)
									{
										//Move chartPivot in between these two notes
										//Calculate the value adjustment
										long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->pivotTime;
										//Adjust the chartAlpha
										currentChart->adjustPivotTime(pivotDifference);
										//Adjust the pivot alphas of inserted notes
										for (unsigned long grdIndex = 0; grdIndex < currentChart->numGuards; grdIndex++)
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
									if ((duet || song.stage == 0 || song.stage == 11 || song.stage == 12) && notes[pl][ntIndex + 1ULL].first - notes[pl][ntIndex].first >= long(5.5 * SAMPLES_PER_BEAT))
									{
										if (!currentChart->numTracelines)
										{
											//Move chartPivot in between these two notes
											//Calculate the value adjustment
											long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->pivotTime;
											//Adjust the chartAlpha
											currentChart->adjustPivotTime(pivotDifference);
											//Adjust the pivot alphas of inserted notes
											for (unsigned long grdIndex = 0; grdIndex < currentChart->numGuards; grdIndex++)
												currentChart->guards[grdIndex].adjustPivotAlpha(-pivotDifference);
										}
										currentChart->setEndTime(notes[pl][ntIndex].first + long(SAMPLES_PER_BEAT));
										makeNewChart = true;
									}
								}
								else
								{
									if (isPlayer != (!duet && section.swapped & 1))	//If it's not the enemy's original charts
									{
										//Move chartPivot in between these two notes
										//Calculate the value adjustment
										long pivotDifference = ((notes[pl][ntIndex + 1ULL].first + notes[pl][ntIndex].first) >> 1) - currentChart->pivotTime;
										//Adjust the chartAlpha
										currentChart->adjustPivotTime(pivotDifference);
										//Adjust the pivot alphas of inserted notes
										for (unsigned long grdIndex = 0; grdIndex < currentChart->numGuards; grdIndex++)
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
								if ((isPlayer == (!duet && section.swapped & 1)) || (ntIndex + 2ULL != notes[pl].size() && dynamic_cast<Path*>(notes[pl][ntIndex + 2ULL].second) == nullptr))
								{
									long endAlpha = notes[pl][ntIndex + 1ULL].first - SongSection::SAMPLE_GAP - 1 - currentChart->pivotTime;
									float angle = 0;
									for (int t = currentChart->numTracelines - 1; t >= 0;)
									{
										//If the new end point is less than or equal to the last trace line's pivot
										if (!currentChart->tracelines[t].changeEndAlpha(endAlpha))
										{
											//Save angle value
											angle = currentChart->tracelines[t].angle;
											//Remove trace line
											cout << global.tabs << section.name << ": ";
											currentChart->remove(t--, 't');
											if (currentChart->numTracelines)
											{
												//Same idea for phrase bars
												for (int p = currentChart->numPhrases - 1; p >= 0;)
												{
													//If the phrase bar is past the trace line
													if (currentChart->tracelines[t].getEndAlpha() <= currentChart->phrases[p].pivotAlpha)
													{
														//Delete the phrase bar
														if (!currentChart->phrases[p].start)
															currentChart->phrases[p - 1ULL].end = true;
														cout << global.tabs << section.name << ": ";
														currentChart->remove(p--, 'p');
													}
													else
													{
														//If inside the trace line
														if (currentChart->tracelines[t] <= currentChart->phrases[p])
															currentChart->phrases[p].changeEndAlpha(endAlpha);
														break;
													}
												}
											}
											else
											{
												for (int p = currentChart->numPhrases - 1; p >= 0;)
												{
													cout << global.tabs << section.name << ": ";
													currentChart->remove(p--, 'p');
												}
											}
										}
										else
										{
											for (int p = currentChart->numPhrases - 1; p >= 0;)
											{
												//If the phrase bar is past the trace line
												if (currentChart->tracelines[t].getEndAlpha() <= currentChart->phrases[p].pivotAlpha)
												{
													//Delete the phrase bar
													if (!currentChart->phrases[p].start)
														currentChart->phrases[p - 1ULL].end = true;
													cout << global.tabs << section.name << ": ";
													currentChart->remove(p--, 'p');
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
									if (currentChart->numTracelines)
									{
										Traceline* tr = new Traceline(endAlpha, 1, angle);
										currentChart->add(tr);
										delete tr;
									}
									if (isPlayer != (!duet && section.swapped & 1)) //It's not the enemy's original charts
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
							if (!duet && song.stage > 0)
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
							if (newCharts[currentPlayer].size() > section.numCharts)
								section.numCharts = (unsigned long)newCharts[currentPlayer].size();
						}
					}
				}
				if (dynamic_cast<Path*>(notes[pl].back().second) != nullptr)
					currentChart->setEndTime(notes[pl].back().first + static_cast<Path*>(notes[pl].back().second)->duration);
				else
					currentChart->setEndTime(notes[pl].back().first);
			}
		}
		size_t chartCount = 0;
		for (unsigned pl = 0; pl < 4; pl++)
		{
			size_t perChart = newCharts[pl].size();
			for (unsigned ch = 0; ch < newCharts[pl].size(); ch++)
				if (perChart > 1 && !(newCharts[pl][ch].numGuards + newCharts[pl][ch].numPhrases) && newCharts[pl][ch].numTracelines <= 1)
					perChart--;
			if (perChart > chartCount)
				chartCount = newCharts[pl].size();
		}
		if (chartCount != section.numCharts)
			section.numCharts = (unsigned long)chartCount;
		section.charts.clear();
		unsigned long total;
		if (!duet)
		{
			unsigned long total1 = 0, total2 = 0;
			for (unsigned pl = 0; pl < 4; pl += 2)
			{
				for (unsigned ch = 0; ch < newCharts[pl].size(); ch++)
					if (newCharts[pl][ch].numGuards + newCharts[pl][ch].numPhrases || newCharts[pl][ch].numTracelines <= 1)
						total1++;
				for (unsigned ch = 0; ch < newCharts[pl + 1].size(); ch++)
					if (newCharts[pl + 1][ch].numGuards + newCharts[pl + 1][ch].numPhrases || newCharts[pl + 1][ch].numTracelines <= 1)
						total2++;
			}
			total = total1 >= total2 ? total1 : total2;
		}
		section.size = 64 + 16 * section.numConditions;
		for (unsigned pl = 0; pl < 4; pl++)
		{
			while (newCharts[pl].size() > section.numCharts)
				newCharts[pl].pop_back();
			while (newCharts[pl].size() < section.numCharts)
				newCharts[pl].emplace_back();
			for (size_t i = 0; i < newCharts[pl].size(); i++)
			{
				Chart ch = newCharts[pl][i];
				section.charts.push_back(ch);
				section.size += ch.size;
			}
		}
		cout << global.tabs << section.name << " organized - # of charts per player: " << section.numCharts;
		if (!duet)
			cout << " (Pair total: " << total << ')' << endl;
		else
			cout << endl;
		saved = false;
	}
	else
		cout << global.tabs << section.name << ": [INTRO, HARMONY, END, & BRK sections are organized by default]" << endl;
	section.organized = true;
	song.unorganized--;
}

void CHC_Editor::playerSwap(SongSection& section)
{
	if (!duet)
	{
		if (section.battlePhase != SongSection::Phase::HARMONY)
		{
			for (long playerIndex = section.numPlayers - 1; playerIndex > 0; playerIndex -= 2)
				section.charts.moveElements((unsigned long long)playerIndex * section.numCharts, (playerIndex - 1ULL) * section.numCharts, section.numCharts);
			if (!(section.swapped & 1))
			{
				if (section.swapped == 0)
					cout << global.tabs << section.name << ": P1/P3 -> P2/P4" << endl;
				else if (section.swapped == 2)
					cout << global.tabs << section.name << ": P3/P1 -> P4/P2" << endl;
				else if (section.swapped == 4)
					cout << global.tabs << section.name << ": P1/P3 -> P2D/P4D (Duet->PS2 conversion)" << endl;
				else
					cout << global.tabs << section.name << ": P3/P1 -> P4D/P2D (Duet->PS2 conversion)" << endl;
				section.swapped++;
			}
			else
			{
				if (section.swapped == 1)
					cout << global.tabs << section.name << ": P2/P4 -> P1/P3" << endl;
				else if (section.swapped == 3)
					cout << global.tabs << section.name << ": P4/P2 -> P3/P1" << endl;
				else if (section.swapped == 5)
					cout << global.tabs << section.name << ": P2D/P4D -> P1/P3 (Duet->PS2 conversion)" << endl;
				else
					cout << global.tabs << section.name << ": P4D/P2D -> P3/P1 (Duet->PS2 conversion)" << endl;
				section.swapped--;
			}
		}
		else
		{
			do
			{
				cout << global.tabs << "How do you want this non-duet harmony section swapped?\n";
				cout << global.tabs << "A - Swap P1/P3 with P2/P4 respectively\n";
				cout << global.tabs << "B - Swap P1/P2 with P3/P4 respectively\n";
				cout << global.tabs << "Current Format: ";
				if (!section.swapped)
					cout << "P1/P2/P3/P4\n";
				else if (section.swapped == 1)
					cout << "P2/P1/P4/P3\n";
				else if (section.swapped == 2)
					cout << "P3/P4/P1/P2\n";
				else if (section.swapped == 3)
					cout << "P4/P3/P2/P1\n";
				switch (menuChoices("ab"))
				{
				case -1:
					return;
				case 0:
					for (long playerIndex = section.numPlayers - 1; playerIndex > 0; playerIndex -= 2)
						section.charts.moveElements((unsigned long long)playerIndex * section.numCharts, (playerIndex - 1ULL) * section.numCharts, section.numCharts);
					if (!(section.swapped & 1))
					{
						if ((section.swapped & 2) == 0)
							cout << global.tabs << section.name << ": P1/P3 -> P2/P4" << endl;
						else
							cout << global.tabs << section.name << ": P3/P1 -> P4/P2" << endl;
						section.swapped++;
					}
					else
					{
						if ((section.swapped & 2) == 0)
							cout << global.tabs << section.name << ": P2/P4 -> P1/P3" << endl;
						else
							cout << global.tabs << section.name << ": P4/P2 -> P3/P1" << endl;
						section.swapped--;
					}
					global.quit = true;
					break;
				case 1:
					section.charts.moveElements(2ULL * section.numCharts, 0, 2ULL * section.numCharts);
					if ((section.swapped & 2) == 0)
					{
						if (!(section.swapped & 1))
							cout << global.tabs << section.name << ": P1/P2 -> P3/P4" << endl;
						else
							cout << global.tabs << section.name << ": P2/P1 -> P4/P3" << endl;
						section.swapped += 2;
					}
					else
					{
						if (!(section.swapped & 1))
							cout << global.tabs << section.name << ": P3/P4 -> P1/P2" << endl;
						else
							cout << global.tabs << section.name << ": P4/P3 -> P2/P1" << endl;
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
			cout << global.tabs << section.name << ": P1 -> P3" << endl;
			section.swapped = 2;
		}
		else
		{
			cout << global.tabs << section.name << ": P3 -> P1" << endl;
			section.swapped = 0;
		}

	}
}

void CHC_Editor::changeName(SongSection& section)
{
	banner(" " + song.shortname + ".CHC - " + section.name + " - Name Change ");
	string newName;
	cout << global.tabs << "Provide a new name for this section (16 character max) ('Q' to back out to the Section Menu)\n";
	cout << global.tabs << "Input: ";
	cin >> ws >> newName;
	if (newName.length() != 1 || tolower(newName[0]) != 'q')
	{
		if (newName.length() < 16)
		{
			memcpy(section.name, newName.c_str(), newName.length());
			section.name[newName.length()] = 0;
		}
		else
			memcpy(section.name, newName.c_str(), 16);
		saved = false;
	}
}

void CHC_Editor::changeAudio(SongSection& section)
{
	banner(" " + song.shortname + ".CHC - " + section.name + " - Audio Change ");
	string newAudio;
	cout << global.tabs << "Provide the name for the section of audio you want to use (16 character max) ('Q' to back out to the Section Menu)\n";
	cout << global.tabs << "Input: ";
	cin >> ws >> newAudio;
	if (newAudio.length() != 1 || tolower(newAudio[0]) != 'q')
	{
		if (newAudio.length() < 16)
		{
			memcpy(section.audio, newAudio.c_str(), newAudio.length());
			section.audio[newAudio.length()] = 0;
		}
		else
			memcpy(section.audio, newAudio.c_str(), 16);
		saved = false;
	}
}

void CHC_Editor::changeFrames(SongSection& section)
{
	do
	{
		banner(" " + song.shortname + ".CHC - " + section.name + " - SSQ Starting Index ");
		cout << global.tabs << "Provide a value for the starting SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n";
		cout << global.tabs << "Can be a decimal\n";
		cout << global.tabs << "Current Value for Starting Index: " << section.frames.first << endl;
		cout << global.tabs << "Input: "; float oldFirst = section.frames.first;
		switch (valueInsert(section.frames.first, false, "u"))
		{
		case 0:
			return;
		case 1:
			if (section.frames.first != oldFirst)
				saved = false;
		case 2:
			global.quit = true;
			break;
		case -1:
			cout << global.tabs << "Provided value *must* be a zero or greater.\n" << global.tabs << '\n';
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
		}
	} while (!global.quit);
	global.quit = false;
	cout << global.tabs << '\n' << global.tabs << "Starting index saved.\n" << global.tabs << endl;
	do
	{
		banner(" " + song.shortname + ".CHC - " + section.name + " - SSQ Ending Index ");
		cout << global.tabs << "Provide a value for the Ending SSQ index [Type 'U' to leave this value unchanged; 'Q' to back out]\n";
		cout << global.tabs << "Can be a decimal\n";
		cout << global.tabs << "Current Value for Ending Index: " << section.frames.last << endl;
		cout << global.tabs << "Input: "; float oldLast = section.frames.last;
		switch (valueInsert(section.frames.last, false, "u"))
		{
		case 0:
			return;
		case 1:
			if (section.frames.last != oldLast)
				saved = false;
		case 2:
			global.quit = true;
			break;
		case -1:
			cout << global.tabs << "Provided value *must* be a zero or greater.\n" << global.tabs << '\n';
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
		}
	} while (!global.quit);
	global.quit = false;
	cout << global.tabs << '\n' << global.tabs << "Ending index saved." << endl;
}

void CHC_Editor::switchPhase(SongSection& section)
{
	string choices = "";
	do
	{
		banner(" " + song.shortname + ".CHC - " + section.name + " - Phase Selection ");
		cout << global.tabs << "Select a phase type for this section\n";
		cout << global.tabs << "Current Phase: ";
		switch (section.battlePhase)
		{
		case SongSection::Phase::INTRO: cout << "INTRO\n"; break;
		case SongSection::Phase::CHARGE: cout << "CHARGE\n"; break;
		case SongSection::Phase::BATTLE: cout << "BATTLE\n";  break;
		case SongSection::Phase::FINAL_AG: cout << "FINAL_AG\n";  break;
		case SongSection::Phase::HARMONY: cout << "HARMONY\n"; break;
		case SongSection::Phase::END: cout << "END\n";  break;
		default: cout << "FINAL_I\n";
		}
		cout << global.tabs << "I/0 - Intro\n";
		cout << global.tabs << "C/1 - Charge\n";
		cout << global.tabs << "B/2 - Battle\n";
		cout << global.tabs << "H/4 - Harmony\n";
		cout << global.tabs << "E/5 - End\n";
		size_t ph = menuChoices("i0c1b2h4e5");
		switch (ph)
		{
		case -3:
			cout << global.tabs << "? [TBD]" << '\n' << global.tabs << '\n';
			break;
		case -1:
			global.quit = true;
			break;
		default:
			ph /= 2;
			if (ph > 2)
				ph++;
			if (section.battlePhase != static_cast<SongSection::Phase>(ph))
				saved = false;
			section.battlePhase = static_cast<SongSection::Phase>(ph);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::adjustTempo(SongSection& section)
{
	do
	{
		banner(" " + song.shortname + ".CHC - " + section.name + " - Tempo Change ");
		cout << global.tabs << "Provide a value for the change [Type 'Q' to exit tempo settings]\n";
		cout << global.tabs << "Can be a decimal... and/or negative with weird effects on PSP\n";
		cout << global.tabs << "Current Value: " << section.tempo << endl;
		cout << global.tabs << "Input: "; float oldTempo = section.tempo;
		switch (valueInsert(section.tempo, true))
		{
		case 1:
			if (section.tempo != oldTempo)
				saved = false;
			break;
		case 0:
			global.quit = true;
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::adjustDuration(SongSection& section)
{
	do
	{
		banner(" " + song.shortname + ".CHC - " + section.name + " - Duration Change ");
		cout << global.tabs << "Provide a value for the change [Type 'Q' to exit duration settings]\n";
		cout << global.tabs << "Current Value: " << section.duration << endl;
		cout << global.tabs << "Input: "; unsigned long oldDuration = section.duration;
		switch (valueInsert(section.duration, false))
		{
		case 1:
			if (section.duration != oldDuration)
				saved = false;
			break;
		case 0:
			global.quit = true;
		case -1:
			cout << global.tabs << "Provided value *must* be a zero or greater.\n" << global.tabs << '\n';
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::conditionMenu(SongSection& section)
{
	do
	{
		banner(" " + song.shortname + ".CHC - Section " + section.name + " - Condition Selection ", 1.5);
		cout << " i ||Condition Type || Argument ||      True Effect      ||      False Effect     ||\n";
		cout << "======================================================================================||\n";
		for (unsigned long condIndex = 0; condIndex < section.numConditions; condIndex++)
		{
			cout << global.tabs << ' ' << condIndex << " || ";
			switch (section.conditions[condIndex].type)
			{
			case 0: cout << "Unconditional ||   n/a    || "; break;
			case 1: cout << "Left Side < # ||   " << setw(4) << left << section.conditions[condIndex].argument << "   || "; break;
			case 2: cout << "Right Side < #||   " << setw(4) << left << section.conditions[condIndex].argument << "   || "; break;
			case 3: cout << "Random < #    ||   " << setw(4) << left << section.conditions[condIndex].argument << "   || "; break;
			case 4: cout << "No Player 3?  ||   n/a    || "; break;
			case 5: cout << "No Player 4?  ||   n/a    || "; break;
			case 6: cout << "Left < Right? ||   n/a    || "; break;
			}
			if (section.conditions[condIndex].trueEffect >= 0)
				cout << "Go to Section " << setw(7) << song.sections[section.conditions[condIndex].trueEffect].name << " || ";
			else
				cout << "Go to Condition " << setw(5) << condIndex - section.conditions[condIndex].trueEffect << " || ";
			if (section.conditions[condIndex].type > 0)
			{
				if (section.conditions[condIndex].falseEffect >= 0)
					cout << "Go to Section " << setw(7) << song.sections[section.conditions[condIndex].falseEffect].name << " ||\n";
				else
					cout << "Go to Condition " << setw(5) << condIndex - section.conditions[condIndex].falseEffect << " ||\n";
			}
			else
				cout << "         n/a         ||\n";
		}
		cout << "======================================================================================||\n";
		cout << global.tabs << "Type the number for the condition that you wish to edit\n";
		unsigned long val;
		switch (valueInsert(val, false, 0UL, section.numConditions - 1))
		{
		case 0:
			global.quit = true;
			break;
		case -1:
			cout << global.tabs << "Given section value must be positive." << endl;
			cin.clear();
			break;
		case -3:
			cout << global.tabs << "Given section value cannot exceed " << song.numSections - 1 << endl;
			cin.clear();
			break;
		case -4:
			cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << endl;
			cin.clear();
			break;
		case 1:
			adjustTabs(6);
			do
			{
				banner(" " + song.shortname + ".CHC - Section " + section.name + " - Condition " + to_string(val) + ' ', 1.5);
				string choices = "ct";
				cout << global.tabs << "c - Type: ";
				switch (section.conditions[val].type)
				{
				case 0: cout << "Unconditional\n"; break;
				case 1: cout << "Left Side < #\n"; break;
				case 2: cout << "Right Side < #\n"; break;
				case 3: cout << "Random < #\n"; break;
				case 4: cout << "No Player 3?\n"; break;
				case 5: cout << "No Player 4?\n"; break;
				case 6: cout << "Left < Right?\n"; break;
				}
				if (section.conditions[val].type > 0 && section.conditions[val].type < 4)
				{
					cout << global.tabs << "A - Argument: " << section.conditions[val].argument << '\n';
					choices += 'a';
				}
				cout << global.tabs << "T - True Effect: ";
				if (section.conditions[val].trueEffect >= 0)
					cout << "Go to Section " << song.sections[section.conditions[val].trueEffect].name << '\n';
				else
					cout << "Go to Condition " << val - section.conditions[val].trueEffect << '\n';
				if (section.conditions[val].type > 0)
				{
					cout << global.tabs << "F - False Effect: ";
					if (section.conditions[val].falseEffect >= 0)
						cout << "Go to Section " << song.sections[section.conditions[val].falseEffect].name << '\n';
					else
						cout << "Go to Condition " << val - section.conditions[val].falseEffect << '\n';
					choices += 'f';
				}
				if (section.numConditions > 1)
				{
					cout << global.tabs << "D - Delete this condition\n";
					choices += 'd';
				}
				cout << global.tabs << "Q - Choose another condition" << endl;
				size_t index = menuChoices(choices);
				switch (index)
				{
				case -1:
					global.quit = true;
					break;
				case -3:
					cout << global.tabs << "? [TBD]" << '\n' << global.tabs << '\n';
				case -2:
					break;
				default:
					adjustTabs(7);
					switch (choices[index])
					{
					case 'c':
						changeName(song.sections[val]);
						break;
					case 'a':
						changeAudio(song.sections[val]);
						break;
					case 't':
						changeFrames(song.sections[val]);
						break;
					case 'f':
						changeFrames(song.sections[val]);
						break;
					case 'd':
						adjustTempo(song.sections[val]);
						break;
					}
					adjustTabs(6);
				}
			} while (!global.quit);
			global.quit = false;
			adjustTabs(3);
		}
	} while (!global.quit);
	global.quit = false;
}

void CHC_Editor::conditionDelete(SongSection& section, size_t index)
{
	List<SongSection::Condition> oldcond = section.conditions;
	for (unsigned long condIndex = 0; condIndex < index; condIndex++)
	{
		if (section.conditions[condIndex].trueEffect < 0)
		{
			if (condIndex - section.conditions[condIndex].trueEffect == index)
			{
				cout << global.tabs << "Condition " << condIndex << "'s true effect points to this condition.";
			}
			else if (condIndex - section.conditions[condIndex].trueEffect > index)
				section.conditions[condIndex].trueEffect++;
		}
	}
}