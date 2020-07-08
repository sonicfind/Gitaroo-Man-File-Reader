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
#include "CHC_CloneHero.h"
using namespace std;

SyncTrack::SyncTrack(FILE* inFile) : timeSig(4)
{
	char ignore[30];
	fscanf_s(inFile, " %lf%[^B]s", &position, ignore, 30);
	fseek(inFile, 1, SEEK_CUR);
	fscanf_s(inFile, " %lu", &bpm);
}

Event::Event(FILE* inFile)
{
	char bagel[100];
	fscanf_s(inFile, " %lf%[^\"]s", &position, bagel, 4);
	fscanf_s(inFile, " %[^\n]s", bagel, 100);
	name = bagel;
	if (name[0] == '\"')
		name = name.substr(1, name.length() - 2); //Gets rid of ""
};

CHNote::CHNote(FILE* inFile)
{
	char type = 0;
	//Only the second "type" insertion matters; First one is '=' character removal
	fscanf_s(inFile, " %lf %c %c", &position, &type, 1, &type, 1);
	if (type == 'E')
	{
		char bagel[100];
		fscanf_s(inFile, " %[^\n]s", bagel, 100);
		name = bagel;
		this->type = NoteType::EVENT;
		fret = 1;
		sustain = 0;
		mod = Modifier::NORMAL;
	}
	else
	{
		name = "";
		if (type == 'S')
			this->type = NoteType::STAR;
		else
			this->type = NoteType::NOTE;
		fscanf_s(inFile, " %c", &fret, 1);
		if (fret == '5')
			mod = Modifier::FORCED;
		else if (fret == '6')
			mod = Modifier::TAP;
		else
			mod = Modifier::NORMAL;
		fscanf_s(inFile, " %lf", &sustain);
	}
};

FILE* operator<<(FILE* outFile, SyncTrack sync)
{
	if (sync.timeSig)
		fprintf(outFile, "  %lu = TS %lu\n", (unsigned long)round(sync.position), sync.timeSig);
	if (sync.bpm)
		fprintf(outFile, "  %lu = B %lu\n", (unsigned long)round(sync.position), sync.bpm);
	return outFile;
};

FILE* operator<<(FILE* outFile, Event ev)
{
	fprintf(outFile, "  %lu = E \"section %s\"\n", (unsigned long)round(ev.position), ev.name.c_str());
	return outFile;
};

FILE* operator<<(FILE* outFile, CHNote note)
{
	switch (note.type)
	{
	case CHNote::NoteType::STAR:
		fprintf(outFile, "  %lu = S 2 %lu\n", (unsigned long)round(note.position), (unsigned long)round(note.sustain));
		break;
	case CHNote::NoteType::NOTE:
		fprintf(outFile, "  %lu = N %u' '", (unsigned long)round(note.position), (unsigned)note.fret);
		if (note.name.compare("n"))
			fprintf(outFile, "%ul\n", (unsigned long)round(note.sustain));
		else
			fprintf(outFile, "%ul\n", 0UL);
		switch (note.mod)
		{
		case CHNote::Modifier::FORCED:
			fprintf(outFile, "  %lu = S 5 0\n", (unsigned long)round(note.position));
			break;
		case CHNote::Modifier::TAP:
			fprintf(outFile, "  %lu = S 6 0\n", (unsigned long)round(note.position));
		}
		if (note.name.length() <= 1)
			break;
	case CHNote::NoteType::EVENT:
		fprintf(outFile, "  %lu = E %s\n", (unsigned long)round(note.position), note.name.c_str());
	}
	return outFile;
};

bool exportChart(CHC& song)
{
	Charter charter(song);
	return charter.exportChart();
}

bool importChart(CHC& song)
{
	Charter charter(song);
	if (charter.importChart())
	{
		song = charter.getSong();
		return true;
	}
	else
		return false;
}

bool Charter::exportChart()
{
	List<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to chart - w/ spaces inbetween.\n", global.tabs.c_str());
		for (size_t sectIndex = 0; sectIndex < song.sections.size(); sectIndex++)
			printf("%s%zu - %s\n", global.tabs.c_str(), sectIndex, song.sections[sectIndex].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent List: ", global.tabs.c_str());
			for (size_t index = 0; index < sectionIndexes.size(); index++)
				printf("%s ", song.sections[sectionIndexes[index]].getName());
			putchar('\n');
		}
		switch (listValueInsert(sectionIndexes, "yn", song.sections.size()))
		{
		case '?':

			break;
		case 'q':
			printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
			return false;
		case 'y':
			if (sectionIndexes.size())
			{
				global.quit = true;
				break;
			}
		case '!':
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%s\n", global.tabs.c_str());
					printf("%sNo sections have been selected. Quit CH chart creation? [Y/N]\n", global.tabs.c_str());
					switch (menuChoices("yn"))
					{
					case 'q':
					case 'y':
						printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
						return false;
					case 'n':
						printf("%s\n", global.tabs.c_str());
						global.quit = true;
					}
				} while (!global.quit);
				global.quit = false;
			}
			else
				global.quit = true;
			break;
		case 'n':
			printf("%s\n", global.tabs.c_str());
			printf("%sOk... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n", global.tabs.c_str());
			printf("%s\n", global.tabs.c_str());
		}
	} while (!global.quit);
	global.quit = false;
	printf("%s", global.tabs.c_str());
	for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
		printf("%s ", song.sections[sectionIndexes[sect]].getName());
	putchar('\n');
	bool multiplayer = toupper(song.shortname[song.shortname.length() - 1]) == 'M';
	bool sectOrientation = false, chartOrientation = false, chartColor = false, noteColor = false, phraseColor = false;
	size_t orientation = 3;
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
	{
		bool phrFound = false, grdFound = false;
		//Goes through each chosen section to see whether any guard marks and/or phrase bars are present
		//If any are found, prompt the user for their choice customization.
		for (size_t sect = 0; sect < sectionIndexes.size() && (!phrFound || !grdFound); sect++)
		{
			SongSection& section = song.sections[sectionIndexes[sect]];
			for (size_t playerIndex = 0; playerIndex < section.getNumPlayers() && (!phrFound || !grdFound); playerIndex++)
			{
				if (!(playerIndex & 1) || multiplayer)
				{
					for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && (!phrFound || !grdFound); chartIndex++)
					{
						Chart& chart = section.getChart(playerIndex * (size_t)section.getNumCharts() + chartIndex);
						if (modchart && !grdFound && chart.getNumGuards())
						{
							do
							{
								printf("%sHow will guard phrases be handled? [Only effects the modchart version]\n", global.tabs.c_str());
								printf("%sB - Base Orientation Only\n", global.tabs.c_str());
								printf("%sG - Guitar Hero Conversion\n", global.tabs.c_str());
								printf("%sS - Determined per Section\n", global.tabs.c_str());
								printf("%sC - Determined per Chart\n", global.tabs.c_str());
								switch (menuChoices("bgsc"))
								{
								case 'b':
									orientation = 0;
								case 'g':
									printf("%s\n", global.tabs.c_str());
									grdFound = true;
									global.quit = true;
									break;
								case 's':
									printf("%s\n", global.tabs.c_str());
									sectOrientation = grdFound = global.quit = true;
									break;
								case 'c':
									printf("%s\n", global.tabs.c_str());
									chartOrientation = grdFound = global.quit = true;
									break;
								case 'q':
									printf("%s\n", global.tabs.c_str());
									printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
									return false;
								}
							} while (!global.quit);
							global.quit = false;
						}
						if (!phrFound && chart.getNumPhrases())
						{
							do
							{
								printf("%sHow will strum phrases be handled?\n", global.tabs.c_str());
								printf("%sY - Test color only (Yellow)\n", global.tabs.c_str());
								printf("%sC - Determined per Chart\n", global.tabs.c_str());
								printf("%sN - Determined per Note\n", global.tabs.c_str());
								printf("%sP - Determined per Phrase Bar\n", global.tabs.c_str());
								//printf("%s", global.tabs.c_str(), "Note for C, N, & P: if a phrase bar has a pre-set color from a .chart import, that color will be used.\n";
								switch (menuChoices("ycnp"))
								{
								case 'y':
									printf("%s\n", global.tabs.c_str());
									phrFound = true;
									global.quit = true;
									break;
								case 'c':
									printf("%s\n", global.tabs.c_str());
									phrFound = true;
									chartColor = global.quit = true;
									break;
								case 'n':
									printf("%s\n", global.tabs.c_str());
									phrFound = true;
									noteColor = global.quit = true;
									break;
								case 'p':
									printf("%s\n", global.tabs.c_str());
									phrFound = true;
									phraseColor = global.quit = true;
									break;
								case 'q':
									printf("%s\n", global.tabs.c_str());
									printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
									return false;
								}
							} while (!global.quit);
							global.quit = false;
						}
					}
				}
			}
		}
	}
	bool duet = song.imc[0] == 0;
	const double TICKS_PER_BEAT = 480.0, GUARD_GAP = 8000.0;
	List<SyncTrack> sync;
	double position = 0;
	if (modchart)
	{
		//Set starting position for gnerating the modchart
		sync.emplace_back( 0, 2, 12632 );
		position += TICKS_PER_BEAT;
		if (song.shortname.find("ST01") != string::npos)
			sync.emplace_back( position, 2, 77538 );
		else if (song.shortname.find("ST02") != string::npos)
		{
			if (song.shortname.find("ST02_HE") != string::npos)
				sync.emplace_back( position, 2, 73983 );
			else
				sync.emplace_back( position, 2, 74070 );
		}
		else if (song.shortname.find("ST03") != string::npos)
			sync.emplace_back( position, 2, 76473 );
		else if (song.shortname.find("ST04") != string::npos)
			sync.emplace_back( position, 2, 79798 );
		else if (song.shortname.find("ST05") != string::npos)
			sync.emplace_back( position, 2, 74718 );
		else if (song.shortname.find("ST06") != string::npos)
			sync.emplace_back( position, 2, 79658 );
		else if (song.shortname.find("ST07") != string::npos)
			sync.emplace_back( position, 2, 73913 );
		else if (song.shortname.find("ST08") != string::npos)
			sync.emplace_back( position, 2, 76523 );
		else if (song.shortname.find("ST09") != string::npos)
			sync.emplace_back( position, 2, 74219 );
		else if (song.shortname.find("ST10") != string::npos)
			sync.emplace_back( position, 2, 75500 );
		else if (song.shortname.find("ST11") != string::npos)
			sync.emplace_back( position, 2, 80000 );
		else if (song.shortname.find("ST12") != string::npos)
			sync.emplace_back( position, 2, 80000 );
		position += TICKS_PER_BEAT;
	}
	//Expert for modchart, reimport for... well you get it
	List<CHNote> expert[2];
	List<CHNote> reimport[2];

	List<Event> events;
	//Yellow (GRYBO)
	size_t strumFret = 2;
	//Essentially selectin which controller setup to correspond to
	auto getOrientation = [&](const char* sectionName, size_t player = 0, size_t chart = 0)
	{
		while (true)
		{
			printf("%sChoose orientation for %s", global.tabs.c_str(), sectionName);
			player ? printf(": Player %zu - Chart %zu\n", player + 1, chart) : putchar('\n');
			printf("%s   ||U|L|R|D||   T - Triangle\n", global.tabs.c_str());
			printf("%s===============  S - Square\n", global.tabs.c_str());
			printf("%s 1 ||T|S|O|X||   O - Circle\n", global.tabs.c_str());
			printf("%s 2 ||S|X|T|O||   X - Cross\n", global.tabs.c_str());
			printf("%s 3 ||O|T|X|S||\n", global.tabs.c_str());
			orientation = menuChoices("123", true);
			switch (orientation)
			{
			case 'q':
				printf("%s\n", global.tabs.c_str());
				return false;
			case '?':
				printf("%s\n", global.tabs.c_str());
			case '*':
				break;
			default:
				printf("%s\n", global.tabs.c_str());
				return true;
			}
		};
	};
	/*
	Sets phrase bar/note color
	0-4 - GRYBO
	5 --- No Color
	If first note & piece:
	====6-10 - GRYBO tap version
	Else If not first piece:
	====6 - Continue previous color for one extra piece
	====7 - Extend previous color until an end piece
	*/
	auto fretting = [&](const char* sectionName, SongSection::Phase phase, size_t player, size_t chart, size_t note = 0, size_t piece = 1)
	{
		while (true)		//Ask for the fret color in each iteration
		{
			printf("%sChoose the fret for %s: Player %zu - Chart %zu\n", global.tabs.c_str(), sectionName, player + 1, chart );
			if (note)
				printf("%sNote %zu\n", global.tabs.c_str(), note );
			if (piece > 1)
				printf("%sPiece %zu\n", global.tabs.c_str(), piece );
			string choices = "grybon";
			printf("%s==============================================================||\n", global.tabs.c_str());
			printf("%s                         No Color (N)                         ||\n", global.tabs.c_str());
			printf("%s Green (G) || Red (R) || Yellow (Y) || Blue (B) || Orange (O) ||\n", global.tabs.c_str());
			if (phase == SongSection::Phase::BATTLE && note == 1 && piece == 1)
			{
				printf("%s   Tap (1) || Tap (2) ||   Tap (3)  ||  Tap (4) ||    Tap (5) ||\n", global.tabs.c_str());
				choices += "12345";
			}
			else if (piece > 1)
			{
				printf("%s              Continue with the previous color (C)            ||\n", global.tabs.c_str());
				printf("%s        Extend previous color to the end of the note (E)      ||\n", global.tabs.c_str());
				choices += "ce";
			}
			printf("%s==============================================================||\n", global.tabs.c_str());
			strumFret = (char)menuChoices(choices, true);
			switch (strumFret)
			{
			case 'q':
				printf("%s\n", global.tabs.c_str());
				return false;
			case '?':
				printf("%s\n", global.tabs.c_str());
			case '*':
				break;
			default:
				printf("%s\n", global.tabs.c_str());
				return true;
			}
		};
	};
	//Used for modchart song.ini file that accompanies the .chart
	//Fills in the "song_length" tag
	double totalDur = 0;
	for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
	{
		SongSection& section = song.sections[sectionIndexes[sect]];
		switch (section.getPhase())
		{
		case SongSection::Phase::INTRO:
			events.emplace_back( position, "INTRO - " + string(section.getName())); break;
		case SongSection::Phase::CHARGE: 
			events.emplace_back( position, "CHARGE - " + string(section.getName())); break;
		case SongSection::Phase::BATTLE: 
			events.emplace_back( position, "BATTLE - " + string(section.getName())); break;
		case SongSection::Phase::FINAL_AG: 
			events.emplace_back( position, "FINAL_AG - " + string(section.getName())); break;
		case SongSection::Phase::HARMONY: 
			events.emplace_back( position, "HARMONY - " + string(section.getName())); break;
		case SongSection::Phase::END: 
			events.emplace_back( position, "END - " + string(section.getName())); break;
		default: 
			events.emplace_back( position, "FINAL_I - " + string(section.getName()));
		}
		if (sync.size() == 0 || unsigned long(section.getTempo() * 1000) != sync.back().bpm)
			sync.emplace_back( position, 4, unsigned long(section.getTempo() * 1000));
		if (section.getPhase() != SongSection::Phase::INTRO && !strstr(section.getName(), "BRK")) //If not INTRO phase or BRK section
		{
			if (modchart && sectOrientation)
			{
				for (size_t playerIndex = 0; playerIndex < section.getNumPlayers() && !global.quit; playerIndex++)
				{
					if (!(playerIndex & 1) || multiplayer)
					{
						for (size_t chartIndex = 0; chartIndex < section.getNumCharts() && !global.quit; chartIndex++)
						{

							if (!getOrientation(section.getName()))
							{
								printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
								return false;
							}
							global.quit = true;
						}
					}
				}
			}
			global.quit = false;
			const double TICKS_PER_SAMPLE = section.getTempo() * TICKS_PER_BEAT / SAMPLES_PER_MIN;
			const double GUARD_OPEN_TICK_DISTANCE = GUARD_GAP * TICKS_PER_SAMPLE;
			//Marking where each the list the current section starts
			size_t startIndex[2][2] = { { expert[0].size(), expert[1].size() },
										  { reimport[0].size(), reimport[1].size()} };

			for (size_t chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
			{
				for (size_t playerIndex = 0, currentPlayer = 0; playerIndex < section.getNumPlayers(); playerIndex++)
				{
					Chart& chart = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
					if (!(playerIndex & 1))
					{
						if (playerIndex == 2 && duet)
							currentPlayer = 1;
						else
							currentPlayer = 0;
					}
					else if (multiplayer)
						currentPlayer = 1;
					else
						continue;
					List<CHNote>& player = expert[currentPlayer];
					List<CHNote>& rein = reimport[currentPlayer];
					if (modchart && chartOrientation && chart.getNumGuards() && !getOrientation(section.getName(), playerIndex, chartIndex))
					{
						printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
						return false;
					}
					size_t markIndex = rein.size();
					if (chartColor && chart.getNumPhrases() && !fretting(section.getName(), section.getPhase(), playerIndex, chartIndex))
					{
						printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
						return false;
					}
					size_t index = startIndex[0][currentPlayer], index2 = startIndex[1][currentPlayer];
					size_t grdIndex;
					for (size_t i = 0; i < chart.getNumGuards(); i++)
					{
						char fret, modfret;
						switch (orientation)	//Determine button based on orientation
						{
						case 'q':
							switch (chart.getGuard(i).getButton())
							{
							case 'q':		//Red
								modfret = 1;
								break;
							case '!':		//Blue
								modfret = 3;
								break;
							case 2:		//Orange
								modfret = 4;
								break;
							case 3:		//Green
								modfret = 0;
							}
							break;
						case '!':
							switch (chart.getGuard(i).getButton())
							{
							case 'q':		//Blue
								modfret = 3;
								break;
							case '!':		//Orange
								modfret = 4;
								break;
							case 2:		//Green
								modfret = 0;
								break;
							case 3:		//Red
								modfret = 1;
							}
							break;
						case 2:
							switch (chart.getGuard(i).getButton())
							{
							case 'q':		//Orange
								modfret = 4;
								break;
							case '!':		//Green
								modfret = 0;
								break;
							case 2:		//Red
								modfret = 1;
								break;
							case 3:		//Blue
								modfret = 3;
							}
							break;
						case 3:
							switch (chart.getGuard(i).getButton())
							{
							case 'q':		//Red
								modfret = 3;
								break;
							case '!':		//Green
								modfret = 0;
								break;
							case 2:		//Blue
								modfret = 1;
								break;
							case 3:		//Orange
								modfret = 4;
							}
						}
						switch (chart.getGuard(i).getButton())
						{
						case 'q':		//Red
							fret = 3;
							break;
						case '!':		//Green
							fret = 0;
							break;
						case 2:		//Blue
							fret = 1;
							break;
						case 3:		//Orange
							fret = 4;
						}
						double pos = position + TICKS_PER_SAMPLE * (chart.getGuard(i).getPivotAlpha() + double(chart.getPivotTime()));
						if (modchart)
						{
							while (index < player.size())
							{
								if (pos <= player[index].position)
									break;
								else
									index++;
							}
							player.emplace(index, 1, pos, modfret, 0, CHNote::Modifier::TAP);
							if (i == 0)
								grdIndex = index;
							index++;
							if (i + 1 != chart.getNumGuards())
							{
								CHNote::Modifier mod;
								long dif = chart.getGuard(i + 1).getPivotAlpha() - chart.getGuard(i).getPivotAlpha();
								if (dif >= 480000)		//If dif is >= ten seconds
								{
									pos += 240000 * TICKS_PER_SAMPLE;
									mod = CHNote::Modifier::FORCED;
								}
								else if (dif >= 240000) //If 5 seconds <= dif < ten seconds
								{
									pos += (dif >> 1) * TICKS_PER_SAMPLE;
									mod = CHNote::Modifier::FORCED;
								}
								else if (dif >= 2 * GUARD_GAP)
								{
									pos += GUARD_OPEN_TICK_DISTANCE;
									if (GUARD_OPEN_TICK_DISTANCE >= 162.5)
										mod = CHNote::Modifier::FORCED;
									else
										mod = CHNote::Modifier::NORMAL;
								}
								else
								{
									pos += (dif >> 1) * TICKS_PER_SAMPLE;
									if ((dif >> 1) * TICKS_PER_SAMPLE >= 162.5)
										mod = CHNote::Modifier::FORCED;
									else
										mod = CHNote::Modifier::NORMAL;
								}
								player.emplace(index, 1, pos, 7, 0, mod);
								index++;
							}
							pos -= 960;
						}
						while (index2 < rein.size())
						{
							if (pos <= rein[index2].position)
								break;
							else
								index2++;
						}
						rein.emplace(index2, 1, pos, fret, 0, CHNote::Modifier::NORMAL);
						index2++;
						
					}
					index2 = startIndex[1][currentPlayer];
					if (chart.getNumTracelines() > 1)
					{
						for (size_t i = 0; i < chart.getNumTracelines(); i++)
						{
							CHNote trace;
							double pos;
							if (modchart)
								pos = position + TICKS_PER_SAMPLE * (chart.getTraceline(i).getPivotAlpha() + double(chart.getPivotTime())) - 960;
							else
								pos = position + TICKS_PER_SAMPLE * (chart.getTraceline(i).getPivotAlpha() + double(chart.getPivotTime()));
							trace.type = CHNote::NoteType::EVENT;
							while (index2 < rein.size())
							{
								if (pos <= rein[index2].position)
									break;
								else
									index2++;
							}
							if (i + 1 != chart.getNumTracelines())
							{
								if (chart.getTraceline(i).getAngle() == 0)
									rein.emplace(index2, 1, pos, 0, 0, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, "Trace");
								else
									rein.emplace(index2, 1, pos, 0, 0, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, "Trace_" + to_string(radiansToDegrees(chart.getTraceline(i).getAngle())));
								index2++;
								if (chart.getTraceline(i).getCurve())
								{
									rein.emplace(index2, 1, pos, 0, 0, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, "Trace_curve");
									index2++;
								}
							}
							else
							{
								if (chart.getTraceline(i).getPivotAlpha() + chart.getPivotTime() >= (long)section.getDuration())
									rein.emplace(index2, 1, pos, 0, 0, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, "Trace_endP");
								else
									rein.emplace(index2, 1, pos, 0, 0, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, "Trace_end");
							}
						}
					}
					index = startIndex[0][currentPlayer], index2 = startIndex[1][currentPlayer];
					size_t phrIndex;
					for (size_t i = 0, note = 1, piece = 1; i < chart.getNumPhrases(); i++)
					{
						double pos = position + TICKS_PER_SAMPLE * (chart.getPhrase(i).getPivotAlpha() + double(chart.getPivotTime()));
						if (noteColor || chartColor || !phraseColor)
						{
							Phrase& phrase = chart.getPhrase(i);
							if (chartColor)
							{
								//Only ask about the first note
								if (note == 1)
								{
									//Not currently implemented, so it essentially calls fretting regardless
									if (phrase.getColor() != -1)
										strumFret = (char)phrase.getColor();
									else if (!fretting(section.getName(), section.getPhase(), playerIndex, chartIndex))
									{
										printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
										return false;
									}
								}
								else if (strumFret > 5)
									strumFret -= 6;
							}
							else if (noteColor)
							{
								//Not currently implemented, so it essentially calls fretting regardless
								if (phrase.getColor() != -1)
									strumFret = (char)phrase.getColor();
								else if (!fretting(section.getName(), section.getPhase(), playerIndex, chartIndex, note))
								{
									printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
									return false;
								}
							}
							double sus;
							//Combine all pieces into one Note
							while (i < chart.getNumPhrases())
							{
								if (chart.getPhrase(i).getEnd() || i + 1 == chart.getNumPhrases())
								{
									if (i + 1 != chart.getNumPhrases())
									{
										//Most likely won't be used much, if at all, but it's still a good safety feature
										//(Won't be used as a fixed CHC is required to with the exporter to begin with)
										if ((double)chart.getPhrase(i + 1).getPivotAlpha() - chart.getPhrase(i).getEndAlpha() < SongSection::SAMPLE_GAP)
											phrase.changeEndAlpha(long(chart.getPhrase(i + 1).getPivotAlpha() - SongSection::SAMPLE_GAP));
										else
											phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
									}
									else
										phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
									sus = TICKS_PER_SAMPLE * phrase.getDuration();
									break;
								}
								else
									i++;
							}
							CHNote::Modifier mod = CHNote::Modifier::NORMAL;
							if (note == 1)
							{
								//With note == 1, fret > 5 can only mean it's the TAP variant
								if (strumFret > 5)
								{
									mod = CHNote::Modifier::TAP;
									strumFret -= 6;
								}
								else
								{
									//Open note is set to fret 7
									if (strumFret == 5)
										strumFret = 7;
									if (modchart)
									{
										//Ensures that the modchart has the note set to strum
										//Skips any tracelines
										for (size_t p = index; p > 0;)
										{
											if (player[--p].type == CHNote::NoteType::NOTE)
											{
												if (strumFret != player[p].fret && pos - player[p].position < 162.5)
													mod = CHNote::Modifier::FORCED;
												break;
											}
										}
									}
									
								}
							}
							else
							{
								//Open note is set to fret 7
								if (strumFret == 5)
									strumFret = 7;
								if (modchart)
								{
									//Ensures that the modchart has the note set to strum
									if (strumFret != player[index - 1].fret && pos - player[index - 1].position < 162.5)
										mod = CHNote::Modifier::FORCED;
								}
							}
							//Find note placement in timeline
							if (modchart)
							{
								while (index < player.size())
								{
									if (pos <= player[index].position)
										break;
									else
										index++;
								}
								//In other words, disable the sustain for the modchart
								if (phrase.getDuration() < 4800)
									player.emplace(index, 1, pos, (char)strumFret, sus, mod, CHNote::NoteType::NOTE, "n");
								else
									player.emplace(index, 1, pos, (char)strumFret, sus, mod);
								if (note == 1)
									phrIndex = index;
								index++;
								//To account for the two beats added by the extra synctracks
								pos -= 960;
							}
							while (index2 < rein.size())
							{
								if (pos <= rein[index2].position)
									break;
								else
									index2++;
							}
							//For modchart exporting, TAP is used only on the first note of a subsection
							//and only for a note too close to the guard mark that preceeded it.
							//For normal exporting, the first note is instead forced.
							if (mod == CHNote::Modifier::TAP)
								mod = CHNote::Modifier::FORCED;
							//Modchart requires all notes regular notes to be strums, so forcing is needed
							//Normal export has no need for that rule.
							else if (mod == CHNote::Modifier::FORCED)
								mod = CHNote::Modifier::NORMAL;
							//Override any names of "n"
							if (phrase.getAnimation() > 0)
								rein.emplace(index2++, 1, pos, (char)strumFret, sus, mod, CHNote::NoteType::NOTE, "Anim_" + to_string(phrase.getAnimation()));
							else
								rein.emplace(index2++, 1, pos, (char)strumFret, sus, mod, CHNote::NoteType::NOTE, "");
							note++;
						}
						else //In other words, if phraseColor is true
						{
							//Not currently implemented, so it essentially calls fretting regardless
							if (chart.getPhrase(i).getColor() != -1)
								strumFret = (char)chart.getPhrase(i).getColor();
							else if (!fretting(section.getName(), section.getPhase(), playerIndex, chartIndex, note, piece))
							{
								printf("%sCH chart creation cancelled.\n", global.tabs.c_str());
								return false;
							}
							bool hopo = false;
							CHNote::Modifier mod = CHNote::Modifier::NORMAL;
							if (strumFret > 5)
							{
								//With piece == 1, strumFret > 5 is one of the tap options
								//This also means that note == 1 as options 6-10 don't appear otherwise
								if (piece == 1)
								{
									mod = CHNote::Modifier::TAP;
									strumFret -= 6;
								}
								else
								{
									//With piece > 1, strumFret > 5 is one of the continuation options
									//This means that everything in this scope must be done in reference
									//to the previous phrase bar
									Phrase& phrase = chart.getPhrase(i - 1);
									//Connect all phrase bars until an end cap is reached
									if (strumFret == 7)
									{
										while (i < chart.getNumPhrases())
										{
											if (!chart.getPhrase(i).getEnd())
												i++;
											else
											{
												if (i + 1 != chart.getNumPhrases())
												{
													//Most likely won't be used much, if at all, but it's still a good safety feature
													//(Won't be used as a fixed CHC is required to with the exporter to begin with)
													if ((double)chart.getPhrase(i + 1).getPivotAlpha() - chart.getPhrase(i).getEndAlpha() < SongSection::SAMPLE_GAP)
														phrase.changeEndAlpha(long(chart.getPhrase(i + 1).getPivotAlpha() - SongSection::SAMPLE_GAP));
													else
														phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
												}
												else
													phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
												break;
											}
										}
									}
									else
									{
										//Extend the note color by one more phrase bar
										if (i + 1 != chart.getNumPhrases())
										{
											//Most likely won't be used much, if at all, but it's still a good safety feature
											//(Won't be used as a fixed CHC is required to with the exporter to begin with)
											if ((double)chart.getPhrase(i + 1).getPivotAlpha() - chart.getPhrase(i).getEndAlpha() < SongSection::SAMPLE_GAP)
												phrase.changeEndAlpha(long(chart.getPhrase(i + 1).getPivotAlpha() - SongSection::SAMPLE_GAP));
											else
												phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
										}
										else
											phrase.changeEndAlpha(chart.getPhrase(i).getEndAlpha());
									}
									if (modchart)
									{
										//Reverse a disbabled modchart sustain
										if (phrase.getDuration() >= 4800)
											player[index - 1].name = "";
										player[index - 1].sustain = TICKS_PER_SAMPLE * phrase.getDuration();
									}
									rein[index2 - 1].sustain = TICKS_PER_SAMPLE * phrase.getDuration();
									if (chart.getPhrase(i).getEnd())
									{
										note++;
										piece = 1;
									}
									else
										piece++;
									//Seeing as this was a continuation, no new notes are inserted, so we
									//can skip over to the next iteration of the loop without incrementing
									//either indexes
									continue;
								}
							}
							else
							{
								if (strumFret == 5)
									strumFret = 7;
								if (piece == 1)
								{
									if (modchart)
									{
										//Ensures that the modchart has the note set to strum
										if (note == 1)
										{
											//Skips any tracelines
											for (size_t p = index; p > 0; p--)
											{
												if (player[--p].type == CHNote::NoteType::NOTE)
												{
													if (strumFret != player[p].fret && pos - player[p].position < 162.5)
														mod = CHNote::Modifier::FORCED;
													break;
												}
											}
										}
										else if (strumFret != player[index - 1].fret && pos - player[index - 1].position < 162.5)
											mod = CHNote::Modifier::FORCED;
									}
								}
								else
								{
									//Ensures that the modchart has the note set to a hopo... even in unusual cases
									if (modchart && (pos - rein[index2 - 1].position >= 162.5 || strumFret == player[index - 1].fret))
										mod = CHNote::Modifier::FORCED;
									hopo = true;
								}
							}
							double sus = TICKS_PER_SAMPLE * chart.getPhrase(i).getDuration();
							if (modchart)
							{
								while (index < player.size())
								{
									if (pos <= player[index].position)
										break;
									else
										index++;
								}
								if (note == 1 && piece == 1)
									phrIndex = index;
								//Disables the note's sustain for the modchart
								if (chart.getPhrase(i).getDuration() < 4800)
									player.emplace(index++, 1, pos, (char)strumFret, sus, mod, CHNote::NoteType::NOTE, "n");
								else
									player.emplace(index++, 1, pos, (char)strumFret, sus, mod);
								//To account for the two beats added by the extra synctracks
								pos -= 960;
							}
							while (index2 < rein.size())
							{
								if (pos <= rein[index2].position)
									break;
								else
									index2++;
							}
							//All hopos are taps... yea that's about it
							if (hopo)
								mod = CHNote::Modifier::TAP;
							//For modchart exporting, TAP is used only on the first note of a subsection
							//and only for a note too close to the guard mark that preceeded it.
							//For normal exporting, the first note is instead forced.
							else if (mod == CHNote::Modifier::TAP)
								mod = CHNote::Modifier::FORCED;
							//Modchart requires all notes regular notes to be strums, so forcing is needed
							//Normal export has no need for that rule.
							else if (mod == CHNote::Modifier::FORCED)
								mod = CHNote::Modifier::NORMAL;
							//Overrides a disabled modchart sustain
							if (piece == 1 && chart.getPhrase(i).getAnimation() > 0)
								rein.emplace(index2++, 1, pos, (char)strumFret, sus, mod, CHNote::NoteType::NOTE, "Anim_" + to_string(chart.getPhrase(i).getAnimation()));
							else
								rein.emplace(index2++, 1, pos, (char)strumFret, sus, mod, CHNote::NoteType::NOTE, "");
							if (chart.getPhrase(i).getEnd())
							{
								note++;
								piece = 1;
							}
							else
								piece++;
						}
					}
					index = startIndex[0][currentPlayer];
					//Firstly, determines whether a "start" marker should be placed
					if ((chartIndex != 0 || (playerIndex >= 2 && !duet)) && (chart.getNumTracelines() > 1 || chart.getNumGuards()))
					{
						CHNote chartMarker;
						chartMarker.type = CHNote::NoteType::EVENT;
						chartMarker.name = "start";
						//AKA, if any notes or trace lines were added
						if (markIndex < rein.size())
						{
							chartMarker.position = (rein[markIndex].position + rein[markIndex - 1].position) / 2;
							rein.emplace(markIndex, 1, chartMarker);
						}
						else
						{
							if (markIndex > 0)
								chartMarker.position = rein[markIndex - 1].position + 160;
							else
								chartMarker.position = position + 160;
							rein.emplace_back(chartMarker);
						}
					}
					//Sets star power phrases
					if (modchart)
					{
						switch (section.getPhase())
						{
						case SongSection::Phase::BATTLE:
							if (chart.getNumGuards())
								//Encapsulate all the guard marks in the subsection
								player.emplace(grdIndex, 1, position + TICKS_PER_SAMPLE * (chart.getGuard(0).getPivotAlpha() + double(chart.getPivotTime())),
														 1, 20 + TICKS_PER_SAMPLE * ((double)chart.getGuard(chart.getNumGuards() - 1).getPivotAlpha() - chart.getGuard(0).getPivotAlpha()),
														 CHNote::Modifier::NORMAL, CHNote::NoteType::STAR);
							break;
						case SongSection::Phase::CHARGE:
							if (chart.getNumPhrases())
								//Encapsulate all the phrase bars in the subsection
								player.emplace(grdIndex, 1, position + TICKS_PER_SAMPLE * (chart.getPhrase(0).getPivotAlpha() + double(chart.getPivotTime())),
													     1, TICKS_PER_SAMPLE * ((double)chart.getPhrase(chart.getNumPhrases() - 1).getEndAlpha() - chart.getPhrase(0).getPivotAlpha()),
														 CHNote::Modifier::NORMAL, CHNote::NoteType::STAR);
						}
					}
				}
			}
		}
		printf("%s%s converted\n", global.tabs.c_str(), section.getName());
		position += TICKS_PER_BEAT * round(double(section.getDuration()) * section.getTempo() / SAMPLES_PER_MIN);
		if (modchart)
			totalDur += (SAMPLES_PER_MIN / section.getTempo()) * round(double(section.getDuration()) * section.getTempo() / SAMPLES_PER_MIN);
	}
	printf("%s\n", global.tabs.c_str());
	string filename = song.name.substr(0, song.name.length() - 4);
	FILE* outFile;
	if (modchart)
	{
		string filenameMod = filename;
		do
		{
			switch (fileOverwriteCheck(filenameMod + ".chart"))
			{
			case 'q':
				for (size_t i = 0; i < sync.size();)
				{
					sync[i].position -= 960;
					//Remove the two extra synctracks
					if (sync[i].position < 960)
						sync.erase(i);
					else
						i++;
				}
				for (size_t i = 0; i < events.size(); i++)
					events[i].position -= 960;
				global.quit = true;
				break;
			case 'n':
				filenameMod += "_T";
				printf("%s\n", global.tabs.c_str());
				break;
			case 'y':
				FILE * outFile;
				fopen_s(&outFile, (song.name.substr(0, song.name.length() - 4) + ".ini").c_str(), "w");
				//Generate the ini file if it's a chart from the original games (stage 2 also including separate EN & JP charts)
				fputs("[song]\n", outFile);
				if (song.shortname.find("ST01") != string::npos)
				{
					fputs("artist = COIL, Tomohiro Harada, Kaleb James\n", outFile);
					fputs("name = 1. Twisted Reality (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Rock\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Panpeus: his axe is an axe. The Axe Gitaroo rips right through space-time, so watch out. Panpeus may have a baby face, but not baby skills.\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t       Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 3\n", outFile);
					fputs("preview_start_time = 29000\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 3\n", outFile);
					fputs("playlist_track = 5\n", outFile);
					fputs("video_start_time = 0\n", outFile);
					fputs("lyrics = 1\n", outFile);
				}
				else if (song.shortname.find("ST02") != string::npos)
				{
					fputs("artist = Tomohiro Harada, YUAN, a - li\n", outFile);
					if (song.shortname.find("ST02_HE") != string::npos)
						fputs("name = 2E. Flyin' to Your Heart (EN) (MODCHART)\n", outFile);
					else
						fputs("name = 2. Flyin' to Your Heart (JP) (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = J - Pop\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Flyin-O: a loyal soldier of the empire. His synth gitaroo shoots 'Dance-Until-Death' rays. His henchmen, the Little-Os, are nothing to be scoffed at either.\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   DISCLAIMER : EPILEPSY WARNING\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 3\n", outFile);
					fputs("preview_start_time = 23460\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 4\n", outFile);
					fputs("playlist_track = 7\n", outFile);
					fputs("video_start_time = 0\n", outFile);
					fputs("lyrics = 1\n", outFile);
				}
				else if (song.shortname.find("ST03") != string::npos)
				{
					fputs("artist = COIL, Tomohiro Harada, Keiichi Yano\n", outFile);
					fputs("name = 3. Bee Jam Blues (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Funk\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Mojo King Bee: he rules the darkness with his trumpet Gitaroo. No one has ever seen the man behind the shades.\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   DISCLAIMER : EPILEPSY WARNING\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 4\n", outFile);
					fputs("preview_start_time = 34260\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 5\n", outFile);
					fputs("playlist_track = 9\n", outFile);
					fputs("video_start_time = 0\n", outFile);
					fputs("lyrics = 1\n", outFile);
				}
				else if (song.shortname.find("ST04") != string::npos)
				{
					fputs("artist = Tomohiro Harada, k - nzk\n", outFile);
					fputs("name = 4. VOID (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Industrial\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Ben-K: As a space shark, he's a rough, tough, customer just looking for trouble!\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 4\n", outFile);
					fputs("preview_start_time = 13650\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 6\n", outFile);
					fputs("playlist_track = 11\n", outFile);
					fputs("video_start_time = 0\n", outFile);
				}
				else if (song.shortname.find("ST05") != string::npos)
				{
					fputs("artist = COIL, Tomohiro Harada, feat.NAHKI\n", outFile);
					fputs("name = 5. Nuff Respect (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Reggae\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Ben-K: He attacks by scratching the turntable Gitaroo on his chest - but his Karate moves prove he's a man of many talents!\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 3\n", outFile);
					fputs("preview_start_time = 38480\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 7\n", outFile);
					fputs("playlist_track = 13\n", outFile);
					fputs("video_start_time = 0\n", outFile);
				}
				else if (song.shortname.find("ST06") != string::npos)
				{
					fputs("artist = COIL, Tomohiro Harada\n", outFile);
					fputs("name = 6. The Legendary Theme (Acoustic) (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Acoustic\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Kirah: a shy and reserved girl with the heart of a warrior. She excels at baking Gitaroo Apple Pies.\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 2\n", outFile);
					fputs("preview_start_time = 26120\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 8\n", outFile);
					fputs("playlist_track = 15\n", outFile);
					fputs("video_start_time = 0\n", outFile);
					fputs("delay = -100\n", outFile);
				}
				else if (song.shortname.find("ST07") != string::npos)
				{
					fputs("artist = Tomohiro Harada, Steve Eto, Kazuki Abe\n", outFile);
					fputs("name = 7. Born To Be Bone (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Flamenco\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Sanbone Trio: Carrot, Soda, & Pine are three mariachis who use their bones as Gitaroos. Their bare-knuckled fighting style is truly praiseworthy!\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   DISCLAIMER : EPILEPSY WARNING\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 5\n", outFile);
					fputs("preview_start_time = 84330\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 9\n", outFile);
					fputs("playlist_track = 17\n", outFile);
					fputs("video_start_time = 0\n", outFile);
				}
				else if (song.shortname.find("ST08") != string::npos)
				{
					fputs("artist = Tomohiro Harada, Kozo Suganuma, Yoshifumi Yamaguchi\n", outFile);
					fputs("name = 8. Tainted Lovers (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Metal\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Gregorio (Seigfried) Wilhelm III: that would be me - at your service. The Great Cathedral itself is my Gitaroo, with which I share my fate. I find my inspiration for my art in tragedy and pain.\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   DISCLAIMER : EPILEPSY WARNING\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 5\n", outFile);
					fputs("preview_start_time = 34730\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 10\n", outFile);
					fputs("playlist_track = 19\n", outFile);
					fputs("video_start_time = 0\n", outFile);
				}
				else if (song.shortname.find("ST09") != string::npos)
				{
					fputs("artist = Tomohiro Harada, Yoshifumi Yamaguchi, Yusuke Nishikawa\n", outFile);
					fputs("name = 9. Overpass (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Rock\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Kirah: the most powerful Gravillian warrior, who was trained from childhood to be a Gitaroo Master. No one can defeat her - not even me!\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t       Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 2\n", outFile);
					fputs("preview_start_time = 22750\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 11\n", outFile);
					fputs("playlist_track = 21\n", outFile);
					fputs("video_start_time = 0\n", outFile);
				}
				else if (song.shortname.find("ST10") != string::npos)
				{
					fputs("artist = COIL, Tomohiro Harada\n", outFile);
					fputs("name = 10. Resurrection (MODCHART)\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Hard Rock\n", outFile);
					fputs("year = 2001\n", outFile);
					fputs("loading_phrase = \"Lord Zowie: he has revealed to us his true power! His Armored Gitaroo, the apex of Gravillian technology, will now rev into life!\"", outFile);
					fputs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t       Video Start Time = [Audio Offset x -1]\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Modchart Full\n", outFile);
					fputs("diff_guitar = 5\n", outFile);
					fputs("preview_start_time = 23830\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 12\n", outFile);
					fputs("playlist_track = 23\n", outFile);
					fputs("video_start_time = 0\n", outFile);
				}
				else if (song.shortname.find("ST11") != string::npos)
				{
					fputs("artist = Tomozuin H, Andy MacKinlay, Satoshi Izumi\n", outFile);
					fputs("name = 11. Metal Header (MODCHART) [MP-Only]\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Rock\n", outFile);
					fputs("year = 2006\n", outFile);
					fputs("loading_phrase =\n", outFile);
					fputs("diff_guitar = 4\n", outFile);
					fputs("diff_rhythm = 4\n", outFile);
					fputs("preview_start_time = 0\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 14\n", outFile);
					fputs("playlist_track = 25\n", outFile);
					fputs("video_start_time = 0\n", outFile);
					fputs("lyrics = 1\n", outFile);
				}
				else if (song.shortname.find("ST12") != string::npos)
				{
					fputs("artist = Tomozuin H, Yordanka Farres, Kenjiro Imawara, Tetsuo Koizumi\n", outFile);
					fputs("name = 12. Toda Pasion (MODCHART) [SP/Online MP]\n", outFile);
					fputs("charter = Sonicfind (Assisted by CyclopsDragon)\n", outFile);
					fputs("album = Gitaroo Man: Warrior of Music [OST]\n", outFile);
					fputs("genre = Latin Rock\n", outFile);
					fputs("year = 2006\n", outFile);
					fputs("loading_phrase =\n", outFile);
					fputs("diff_guitar = 4\n", outFile);
					fputs("diff_rhythm = 4\n", outFile);
					fputs("preview_start_time = 0\n", outFile);
					fputs("icon = gitaroo\n", outFile);
					fputs("album_track = 15\n", outFile);
					fputs("playlist_track = 26\n", outFile);
					fputs("video_start_time = 0\n", outFile);
					fputs("lyrics = 1\n", outFile);
				}
				fputs("modchart = 1\n", outFile);
				//Converting totalDur to milliseconds
				fprintf(outFile, "song_length = %lu\n", (unsigned long)ceil(totalDur / 48));
				fputs("Property of Koei Co. Ltd. Gitaroo Man (C) KOEI 2001", outFile);
				fclose(outFile);
				//Switch file from the .ini to the .chart
				fopen_s(&outFile, (filenameMod + ".chart").c_str(), "w");
				fputs("[Song]\n{\n", outFile);
				fputs("  Offset = 0\n", outFile);
				fputs("  Resolution = 480\n", outFile);
				fputs("  Player2 = bass\n", outFile);
				fputs("  Difficulty = 0\n", outFile);
				fputs("  PreviewStart = 0\n", outFile);
				fputs("  PreviewEnd = 0\n", outFile);
				fputs("  Genre = \"rock\"\n", outFile);
				fputs("  MediaType = \"cd\"\n}\n", outFile);
				fputs("[SyncTrack]\n{\n", outFile);
				for (size_t i = 0; i < sync.size(); i++)
				{
					outFile << sync[i];
					sync[i].position -= 960;
					//Remove the two extra synctracks
					if (sync[i].position < 960)
						sync.erase(i);
					else
						i++;
				}
				fputs("}\n[Events]\n{\n", outFile);
				for (size_t i = 0; i < events.size(); i++)
				{
					outFile << events[i];
					events[i].position -= 960;
				}
				fputs("}\n", outFile);
				const char* headers[2] = { "[ExpertSingle]\n{\n", "[ExpertDoubleRhythm]\n{\n" };
				for (char i = 0; i < 2; i++)
				{
					if (!expert[i].size()) continue;
					fputs(headers[i], outFile);
					for (size_t n = 0; n < expert[i].size(); n++)
						outFile << expert[i][n];
					fputs("}\n", outFile);
				}
				fclose(outFile);
				global.quit = true;
			}
			printf("%s\n", global.tabs.c_str());
		} while (!global.quit);
		global.quit = false;
	}
	filename += "_For_Reimporting";
	do
	{
		switch (fileOverwriteCheck(filename + ".chart"))
		{
		case 'q':
			global.quit = true;
			break;
		case 'n':
			printf("%s\n", global.tabs.c_str());
			filename += "_T";
			break;
		case 'y':
			fopen_s(&outFile, (filename + ".chart").c_str(), "w");
			fputs("[CHC]\n{\n", outFile);
			fputs("  Offset = 0\n", outFile);
			fputs("  Resolution = 480\n", outFile);
			fputs("  Player2 = bass\n", outFile);
			fputs("  Difficulty = 0\n", outFile);
			fputs("  PreviewStart = 0\n", outFile);
			fputs("  PreviewEnd = 0\n", outFile);
			fputs("  Genre = \"rock\"\n", outFile);
			fputs("  MediaType = \"cd\"\n}\n", outFile);
			fputs("[SyncTrack]\n{\n", outFile);
			for (size_t i = 0; i < sync.size(); i++)
				outFile << sync[i];
			fputs("}\n[Events]\n{\n", outFile);
			for (size_t i = 0; i < events.size(); i++)
				outFile << events[i];
			fputs("}\n", outFile);
			const char* headers[2] = { "[ExpertSingle]\n{\n", "[ExpertDoubleRhythm]\n{\n" };
			for (char i = 0; i < 2; i++)
			{
				if (!reimport[i].size()) continue;
				fputs(headers[i], outFile);
				for (size_t n = 0; n < reimport[i].size(); n++)
					outFile << reimport[i][n];
				fputs("}\n", outFile);
			}
			fclose(outFile);
			return true;
		}
	} while (!global.quit);
	return false;
}

bool Charter::importChart()
{
	bool duet = song.imc[0] == 0;
	FILE* inChart = nullptr;
	do
	{
		printf("%sProvide the name of the .CHART file you wish to use (Or 'Q' to exit): ", global.tabs.c_str());
		string chartName = "";
		char val = filenameInsertion(chartName);
		if (val == -1)
			return false;
		else if (val == 0)
		{
			if (chartName.find(".CHART") == string::npos)
				chartName += ".CHART";
			if (!fopen_s(&inChart, chartName.c_str(), "r"))
				global.quit = true;
			else
			{
				size_t pos = chartName.find_last_of('\\');
				if (pos != string::npos)
					printf("%s\"%s\" is not a valid file of extension \".CHART\"\n", global.tabs.c_str(), chartName.substr(pos + 1).c_str());
				else
					printf("%s\"%s\" is not a valid file of extension \".CHART\"\n", global.tabs.c_str(), chartName.c_str());
			}
		}
	} while (!global.quit);
	global.quit = false;
	char ignore[400];
	//Skips the info section
	fscanf_s(inChart, " %[^}]", ignore, 400);
	fscanf_s(inChart, " %[^{]", ignore, 400);
	fseek(inChart, 1, SEEK_CUR);
	struct SubSection
	{
		struct Color
		{
			long position;
			int color;
			Color(long pos, int clr) : position(pos), color(clr) {}
		};
		Chart chart;
		List<Color> colors;
	};
	
	struct Section
	{
		struct Tempo
		{
			unsigned long bpm;
			//In reference to the beginning of the song
			double position_ticks;
			//In reference to the beginning of the section is resides in
			double position_samples;
			Tempo(unsigned long bpm = 120, double pos_ticks = 0, double pos_samples = 0) : bpm(bpm), position_ticks(pos_ticks), position_samples(pos_samples) {}
		};
		string name = "";
		//In reference to the beginning of the song
		double position_ticks = 0;
		//In reference to the beginning of the song, but calculated 
		//using the previous songsection (if one exists) as a base
		double position_samples = 0;
		List<Tempo> tempos;
		//Two lists for two players
		List<SubSection> subs[2];
		Section(string nam, double pos_T = 0, double pos_S = 0, unsigned long bpm = 120) : name(nam), position_ticks(pos_T), position_samples(pos_S)
		{
			tempos.emplace_back( bpm, position_ticks );
		}
	};
	List<Section> sections;
	char test = 0;
	const long double SAMPLES_PER_MIN = 2880000, TICKS_PER_BEAT = 480;
	{
		List<SyncTrack> tempos;
		fscanf_s(inChart, " %c", &test, 1);
		//Checking for the end of the synctrack section
		while (test != '}')
		{
			fseek(inChart, -1, SEEK_CUR);
			//Creates new tempo object
			tempos.emplace_back(inChart);
			fscanf_s(inChart, " %c", &test, 1);
		}
		fscanf_s(inChart, " %[^{]", ignore, 400);
		fseek(inChart, 1, SEEK_CUR);
		size_t tempoIndex = 0;
		fscanf_s(inChart, " %c", &test, 1);
		//Checking for the end of the event section
		while (test != '}')
		{
			fseek(inChart, -1, SEEK_CUR);
			Event ev(inChart);
			//For example, if the name is "BATTLE - something,"
			//only use "something."
			ev.name = ev.name.substr(ev.name.find_last_of(' ') + 1);
			//Add all tempos between the current section event and the previous section
			//to the previous section
			while (tempoIndex + 1ULL < tempos.size() && ev.position >= tempos[tempoIndex + 1ULL].position)
			{
				if (ev.position > tempos[tempoIndex + 1ULL].position)
				{
					Section::Tempo& prev = sections.back().tempos.back();
					double pos_samples = prev.position_samples + (tempos[tempoIndex + 1ULL].position - prev.position_ticks)
																 * (SAMPLES_PER_MIN / (TICKS_PER_BEAT * prev.bpm / 1000));
					//Creates new tempo object
					sections.back().tempos.emplace_back(tempos[tempoIndex + 1ULL].bpm, tempos[tempoIndex + 1ULL].position, pos_samples);
				}
				tempoIndex++;
			}
			double pos_samples = 0;
			//If this isn't the first section
			if (sections.size())
			{
				Section::Tempo& prev = sections.back().tempos.back();
				pos_samples = sections.back().position_samples + prev.position_samples + (ev.position - prev.position_ticks)
																						 * (SAMPLES_PER_MIN / (TICKS_PER_BEAT * prev.bpm / 1000));
			}
			sections.emplace_back(ev.name, ev.position, pos_samples, tempos[tempoIndex].bpm);
			fscanf_s(inChart, " %c", &test, 1);
		}
	}
	unsigned numPlayersCharted = 0;
	for (size_t sectIndex = 0; numPlayersCharted < 2 && !feof(inChart); numPlayersCharted++, sectIndex = 0)
	{
		fscanf_s(inChart, " %[^{]", ignore, 400);
		fseek(inChart, 1, SEEK_CUR);
		if (feof(inChart))
			break;
		fscanf_s(inChart, " %c", &test, 1);
		long double SAMPLES_PER_TICK = SAMPLES_PER_MIN / (TICKS_PER_BEAT * sections[0].tempos[0].bpm / 1000);
		sections[0].subs[numPlayersCharted].emplace_back();
		SubSection* currSub = &sections[0].subs[numPlayersCharted].back();
		SubSection* prevSub = nullptr;
		currSub->chart.clearTracelines();
		Section::Tempo* prev = &sections[0].tempos[0];
		size_t tempoIndex = 0;
		while (test != '}')
		{
			fseek(inChart, -1, SEEK_CUR);
			CHNote note(inChart);
			while (sectIndex + 1ULL != sections.size() && note.position >= sections[sectIndex + 1ULL].position_ticks)
			{
				prevSub = currSub;
				prev = &sections[sectIndex].tempos[tempoIndex];
				sectIndex++;
				tempoIndex = 0;
				SAMPLES_PER_TICK = SAMPLES_PER_MIN / (TICKS_PER_BEAT * sections[sectIndex].tempos[0].bpm / 1000);
				sections[sectIndex].subs[numPlayersCharted].emplace_back();
				currSub = &sections[sectIndex].subs[numPlayersCharted].back();
				currSub->chart.clearTracelines();
			}
			while (tempoIndex + 1ULL != sections[sectIndex].tempos.size() && note.position >= sections[sectIndex].tempos[tempoIndex].position_ticks)
				SAMPLES_PER_TICK = SAMPLES_PER_MIN / (TICKS_PER_BEAT * sections[sectIndex].tempos[++tempoIndex].bpm / 1000);
			switch (note.type)
			{
			case CHNote::NoteType::EVENT:
				//Signals the insertion of a new chart/subsection
				if (!note.name.compare("start"))
				{
					prevSub = currSub;
					prev = &sections[sectIndex].tempos[0];
					sections[sectIndex].subs[numPlayersCharted].emplace_back();
					currSub = &sections[sectIndex].subs[numPlayersCharted].back();
					currSub->chart.clearTracelines();
				}
				//Signals a trace line related function or note
				else if (note.name.find("Trace") != string::npos)
				{
					//Set previous Trace line's curve set to true
					if (note.name.find("curve") != string::npos)
						currSub->chart.getTraceline(currSub->chart.getNumTracelines() - 1).setCurve(true);
					else
					{
						double pos = ((note.position - sections[sectIndex].tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK) + sections[sectIndex].tempos[tempoIndex].position_samples;
						//If endP
						if ((note.name.find('P') != string::npos || note.name.find('p') != string::npos) && prevSub != nullptr)
						{
							//Makes sure that the resulting trace line would not interrupted
							//If it would be, don't add it
							if (!currSub->chart.getNumGuards() && (!currSub->chart.getNumTracelines() || (long)round(pos) == currSub->chart.getTraceline(0).getPivotAlpha()))
							{
								if (sections[sectIndex].position_ticks != prev->position_ticks) //Specifically, greater than
								{
									pos += (sections[sectIndex].position_ticks - prev->position_ticks) * (SAMPLES_PER_MIN / (TICKS_PER_BEAT * sections[sectIndex].tempos[0].bpm / 1000));
									pos += prev->position_samples;
								}
								if (currSub->chart.getNumTracelines())
									pos -= 1;
								prevSub->chart.addTraceline_back((long)round(pos));
							}
						}
						else
						{
							//If the traceline is not and end piece AND has an angle tied to it
							if (note.name.find("end") == string::npos && note.name.length() > 5)
							{
								try
								{
									currSub->chart.addTraceline_back((long)round(pos), 1, float(stof(note.name.substr(6)) * M_PI / 180));
								}
								catch (...)
								{
									printf("%sTrace line event at tick position %lu had extraneous data that could not be pulled.\n", global.tabs.c_str(), (unsigned long)note.position);
									printf("%sRemember: trace events *must* be formatted as \"Trace\", \"Trace_[float angle value]\", \"Trace_end\", \"Trace_endP\", or \"Trace_curve\"\n", global.tabs.c_str());
									currSub->chart.addTraceline_back((long)round(pos));
								}
							}
							else
								currSub->chart.addTraceline_back((long)round(pos));
						}
					}
				}
				else if (note.name.find("Anim") != string::npos && currSub->chart.getNumPhrases() > 0)
					currSub->chart.getPhrase(currSub->chart.getNumPhrases() - 1).setAnimation(stoi(note.name.substr(5)));
				break;
			case CHNote::NoteType::NOTE:
				//If Phrase Bar
				if (note.sustain)
				{
					long pos = (long)round(((note.position - sections[sectIndex].tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK) + sections[sectIndex].tempos[tempoIndex].position_samples);
					if (currSub->chart.getNumPhrases() == 0 || currSub->chart.getPhrase(currSub->chart.getNumPhrases() - 1).getPivotAlpha() < pos)
					{
						currSub->chart.addPhrase_back(pos, (unsigned long)round(note.sustain * SAMPLES_PER_TICK));
						currSub->colors.emplace_back(pos, note.fret - 48);
					}
				}
				else
				{
					switch (note.mod)
					{
					case CHNote::Modifier::NORMAL: //Guard Mark
						{
							long pos = (long)round(((note.position - sections[sectIndex].tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK) + sections[sectIndex].tempos[tempoIndex].position_samples);
							if (currSub->chart.getNumGuards() == 0 || currSub->chart.getGuard(currSub->chart.getNumGuards() - 1).getPivotAlpha() < pos)
							{
								switch (note.fret)
								{
								case '0':
									currSub->chart.addGuard_back(pos, 1);
									break;
								case '1':
									currSub->chart.addGuard_back(pos, 2);
									break;
								case '3':
									currSub->chart.addGuard_back(pos, 0);
									break;
								case '4':
									currSub->chart.addGuard_back(pos, 3);
								}
							}
						}
						break;
					case CHNote::Modifier::FORCED: 
						currSub->colors.back().color += 8;
						break;
					case CHNote::Modifier::TAP:	//Continued Phrase Bar
						if (currSub->chart.getNumPhrases() >= 2)
						{
							currSub->chart.getPhrase(currSub->chart.getNumPhrases() - 1).setStart(false);
							currSub->chart.getPhrase(currSub->chart.getNumPhrases() - 2).setEnd(false);
						}
					}
				}
			}
			if (!feof(inChart))
				fscanf_s(inChart, " %c", &test, 1);
			else
			{
				fclose(inChart);
				break;
			}
		}
	}

	if (numPlayersCharted)
	{
		auto insertNotes = [&](Chart& imported, Chart& insertion)
		{
			insertion.clearPhrases();
			for (size_t phraseIndex = 0; phraseIndex < imported.getNumPhrases(); phraseIndex++)
			{
				//Create a new copy so that pivot alpha values are maintained for the loop iterations
				Phrase phr(imported.getPhrase(phraseIndex));
				//Pivot alpha was previous set to total displacement from the start of the section
				phr.adjustPivotAlpha(-insertion.getPivotTime());
				if (!phr.getStart() && insertion.getNumPhrases())
					insertion.getPhrase(insertion.getNumPhrases() - 1).changeEndAlpha(phr.getPivotAlpha());
				if (!phr.getEnd() && phraseIndex + 1 == imported.getNumPhrases())
					phr.setEnd(true);
				insertion.addPhrase_back(phr);
			}
			if (imported.getNumTracelines() > 1)
			{
				insertion.clearTracelines();
				for (size_t traceIndex = 0, phraseIndex = 0; traceIndex < imported.getNumTracelines(); traceIndex++)
				{
					Traceline trace = imported.getTraceline(traceIndex);
					trace.adjustPivotAlpha(-insertion.getPivotTime());
					if (traceIndex)
						insertion.getTraceline(insertion.getNumTracelines() - 1).changeEndAlpha(trace.getPivotAlpha());
					insertion.addTraceline_back(trace);
				}
			}
			//Go through every phrase bar & trace line to find places where phrase bars
			//should be split into two pieces
			for (size_t traceIndex = 0, phraseIndex = 0; traceIndex < insertion.getNumTracelines(); traceIndex++)
			{
				Traceline& trace = insertion.getTraceline(traceIndex);
				while (phraseIndex < insertion.getNumPhrases())
				{
					Phrase* phr = &insertion.getPhrase(phraseIndex);
					if (traceIndex == 0)
					{
						if (trace.getPivotAlpha() > phr->getPivotAlpha())
							trace.changePivotAlpha(phr->getPivotAlpha());
						break;
					}
					else if (trace.getPivotAlpha() >= phr->getEndAlpha())
						phraseIndex++;
					else if (trace.getPivotAlpha() > phr->getPivotAlpha())
					{

						unsigned long dur = phr->getEndAlpha() - trace.getPivotAlpha();
						bool end = phr->getEnd();
						phr->changeEndAlpha(trace.getPivotAlpha());
						phr->setEnd(false);
						insertion.addPhrase(trace.getPivotAlpha(), dur, false, end, 0, phr->getColor());
						phraseIndex++;
						break;
					}
					else if (traceIndex + 1 != insertion.getNumTracelines())
						break;
					//If the phrase bar lands at or after the last trace line, delete
					else if (phr->getPivotAlpha() >= trace.getPivotAlpha())
					{
						if (!phr->getStart())
							insertion.getPhrase(phraseIndex - 1).setEnd(true);
						insertion.remove(phraseIndex, 'p');
					}
					else if (phr->getEndAlpha() > trace.getPivotAlpha())
					{
						phr->changeEndAlpha(trace.getPivotAlpha());
						phr->setEnd(true);
						phraseIndex++;
					}
				}
			}
			insertion.clearGuards();
			for (size_t guardIndex = 0; guardIndex < imported.getNumGuards(); guardIndex++)
			{
				Guard& grd = imported.getGuard(guardIndex);
				insertion.addGuard(grd.getPivotAlpha() - insertion.getPivotTime(), grd.getButton());
			}
		};
		for (size_t sectIndex = 0; sectIndex < song.sections.size(); sectIndex++)
		{
			SongSection& section = song.sections[sectIndex];
			if (section.getPhase() == SongSection::Phase::INTRO || strstr(section.getName(), "BRK") || strstr(section.getName(), "BREAK"))
				continue;
			for (size_t sectIndex2 = 0; sectIndex2 < sections.size(); sectIndex2++)
			{
				if (string(section.getName()).substr(0, sections[sectIndex2].name.length()).find(sections[sectIndex2].name) != string::npos)
				{
					if (sectIndex2 + 1ULL != sections.size())
					{
						//Sets Section duration to an even and equal spacing based off marked tempo
						const long double numBeats = (long double)round((sections[sectIndex2 + 1ULL].position_ticks - sections[sectIndex2].position_ticks) / TICKS_PER_BEAT);
						section.setDuration((unsigned long)round(SAMPLES_PER_MIN * numBeats / section.getTempo()));
					}
					//This differentiation is needed due to the difference in how
					//subsections are split in PS2 charts vs. Duet charts
					if (!duet)
					{
						bool p1swapped, p2swapped;
						p1swapped = p2swapped = section.getSwapped() & 1;
						for (unsigned chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
						{
							for (unsigned playerIndex = 0; playerIndex < section.getNumPlayers(); playerIndex++)
							{
								if (!(playerIndex & 1) || numPlayersCharted == 2)
								{
									unsigned val = 2 * chartIndex + (playerIndex >> 1);
									if (val < sections[sectIndex2].subs[playerIndex & 1].size())
									{
										Chart& imp = sections[sectIndex2].subs[playerIndex & 1][val].chart;
										if (imp.getNumTracelines() > 1 || imp.getNumGuards())
										{
											section.setOrganized(false);
											insertNotes(imp, section.getChart((size_t)playerIndex* section.getNumCharts() + chartIndex));
											if (section.getPhase() == SongSection::Phase::BATTLE)
											{
												if (imp.getNumGuards() && imp.getNumTracelines() > 1)
												{
													//Determining which comes first
													if (imp.getGuard(0).getPivotAlpha() < imp.getTraceline(0).getPivotAlpha())
													{
														if ((playerIndex & 1))
															p2swapped = true;
													}
													else if (imp.getTraceline(0).getPivotAlpha() < imp.getGuard(0).getPivotAlpha())
													{
														if (!(playerIndex & 1))
															p1swapped = true;
													}
												}
											}
										}
									}
								}
							}
						}
						if (p1swapped && p2swapped)
							section.setSwapped(1);
						else if (section.getSwapped() >= 4 && !(p1swapped || p2swapped))
							section.setSwapped(0);
					}
					else
					{
						for (unsigned playerIndex = 0; playerIndex < numPlayersCharted; playerIndex++)
						{
							for (unsigned chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
							{
								if (chartIndex < sections[sectIndex2].subs[playerIndex].size())
								{
									Chart& imp = sections[sectIndex2].subs[playerIndex][chartIndex].chart;
									if (imp.getNumTracelines() > 1 || imp.getNumGuards())
									{
										section.setOrganized(false);
										insertNotes(imp, section.getChart(2ULL * playerIndex * section.getNumCharts() + chartIndex));
									}
								}
							}
						}
					}
					if (!section.getOrganized())
						song.unorganized++;
					printf("%sReplaced %s\n", global.tabs.c_str(), section.getName());
					break;
				}
			}
		}
		do
		{
			printf("%sFix & reorganize the newly imported notes for safety? [Y/N][or Q to cancel save]\n", global.tabs.c_str());
			printf("%sChoosing not to do so will leave all newly imported sections in an unorganized/possibly glitchy state.\n", global.tabs.c_str());
			switch (menuChoices("yn"))
			{
			case 'q':
				return false;
			case 'y':
				if (!loadProc(dlls[DLL_INDEX].libraries[0].dll, "quickFix", song))
					printf("%sAn error was ecountered while attempting to alter %s\n", global.tabs.c_str(), song.shortname.c_str());
			case 'n':
				printf("%s\n", global.tabs.c_str());
				global.quit = true;
				break;
			}
		} while (!global.quit);
		global.quit = false;
		if (song.name.find("IMPORTED") == string::npos)
		{
			do
			{
				printf("%sHow should the file be named?\n", global.tabs.c_str());
				printf("%sK - Keep filename as %s\n", global.tabs.c_str(), song.shortname.c_str());
				printf("%sC - Change filename to %s_IMPORTED\n", global.tabs.c_str(), song.shortname.c_str());
				switch (menuChoices("kc"))
				{
				case 'q':
					return false;
				case 'k':
					song.name = song.name.substr(0, song.name.length() - 4) + "_IMPORTED.CHC";
					song.shortname += "_IMPORTED";
				case 'c':
					printf("%s\n", global.tabs.c_str());
					global.quit = true;
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
		do
		{
			//Either yes or no will still overwrite the old CHC data held
			//in the current CHC_Main object
			printf("%sSave %s externally? [Y/N]\n", global.tabs.c_str(), song.shortname.c_str());
			printf("%sNote: after this point, import changes will be applied if 'Q' is not used.\n", global.tabs.c_str());
			switch (menuChoices("yn"))
			{
			case 'q':
				return false;
			case 'y':
			{
				printf("%s\n", global.tabs.c_str());
				string filename = song.name.substr(0, song.name.length() - 4);
				string ext = "";
				do
				{
					switch (fileOverwriteCheck(filename + ext + ".CHC"))
					{
					case 'n':
						printf("%s\n", global.tabs.c_str());
						ext += "_T";
						break;
					case 'y':
						song.create(filename + ext + ".CHC");
						song.name = filename + ext + ".CHC";
						song.shortname += ext;
						song.saved = 2;
						global.quit = true;
						break;
					case 'q':
						printf("%s\n", global.tabs.c_str());
						global.quit = true;
					}
				} while (!global.quit);
			}
			case 'n':
				global.quit = true;
				break;
			}
		} while (!global.quit);
		global.quit = false;
		if (!duet)
		{
			do
			{
				printf("%sGenerate a color sheet to go with the imported chart? [Y/N]\n", global.tabs.c_str());
				switch (menuChoices("yn"))
				{
				case 'y':
				{
					banner(" " + song.shortname + ".CHC - Color Sheet Creation From Import ");
					bool multiplayer = false;
					do
					{
						printf("%sIs this chart for multiplayer? [Y/N]\n", global.tabs.c_str());
						switch (menuChoices("yn"))
						{
						case 'q':
							return true;
						case 'y':
							multiplayer = true;
						case 'n':
							global.quit = true;
							break;
						}
					} while (!global.quit);
					global.quit = false;
					string filename = song.name.substr(0, song.name.length() - 4) + "_COLORDEF";
					string filename2 = song.name.substr(0, song.name.length() - 4) + "_COLORDEF_FRAGS";
					FILE *outSheet = nullptr, *outSheet2 = nullptr;
					do
					{
						switch (fileOverwriteCheck(filename + ".txt"))
						{
						case 'n':
							printf("%s\n", global.tabs.c_str());
							filename += "_T";
							break;
						case 'y':
							fopen_s(&outSheet, (filename + ".txt").c_str(), "w");
						case 'q':
							global.quit = true;
						}
					} while (!global.quit);
					printf("%s\n", global.tabs.c_str());
					global.quit = false;
					do
					{
						switch (fileOverwriteCheck(filename2 + ".txt"))
						{
						case 'n':
							printf("%s\n", global.tabs.c_str());
							filename2 += "_T";
							break;
						case 'y':
							fopen_s(&outSheet2,(filename2 + ".txt").c_str(), "w");
						case 'q':
							global.quit = true;
						}
					} while (!global.quit);
					if (outSheet != nullptr || outSheet2 != nullptr)
					{
						const char colorRange[] = {'G', 'R', 'Y', 'B', 'O', 0, 0, 'N', 'g', 'r', 'y', 'b', 'o'};
						fputs("[phrasemode fragments]\n", outSheet2);
						dualvfprintf_s(outSheet, outSheet2, "[attack point palette]\nG: 00ff00\nR: ff0000\nY: ffff00\nB: 0000ff\nO: ff7f00\nP: ff00ff\nN: f89b44\ng: ffffff\nr: ffffff\ny: ffffff\nb: ffffff\no: ffffff\np: ffffff\n\n");
						dualvfprintf_s(outSheet, outSheet2, "[phrase bar palette]\nG: 40ff40\nR: ff4040\nY: ffff40\nB: 4040c8\nO: ff9f40\nP: ff40ff\nN: f07b7b\ng: 40ff40\nr: ff4040\ny: ffff40\nb: 4040c8\no: ff9f40\np: ff40ff\n\n");
						size_t chartCount = 0;
						for (size_t sectIndex = 0; sectIndex < song.sections.size(); sectIndex++)
						{
							SongSection& section = song.sections[sectIndex];
							size_t sectIndex2 = 0;
							for (; sectIndex2 < sections.size(); sectIndex2++)
								if (string(section.getName()).substr(0, sections[sectIndex2].name.length()).find(sections[sectIndex2].name) != string::npos)
									break;
							for (unsigned long playerIndex = 0; playerIndex < section.getNumPlayers(); playerIndex++)
							{
								for (unsigned long chartIndex = 0; chartIndex < section.getNumCharts(); chartIndex++)
								{
									if (!(playerIndex & 1) || multiplayer)
									{
										Chart& chart = section.getChart((size_t)playerIndex * section.getNumCharts() + chartIndex);
										if (chart.getNumPhrases())
										{
											dualvfprintf_s(outSheet, outSheet2, "#SongSection %lu [%s], P%lu CHCH %lu\n", sectIndex, section.getName(), playerIndex + 1, chartIndex);
											dualvfprintf_s(outSheet, outSheet2, "[drawn chart %lu]\n", chartCount);
											if (sectIndex2 < sections.size() && 2ULL * chartIndex + (playerIndex >> 1) < sections[sectIndex2].subs[playerIndex & 1].size())
											{
												List<SubSection::Color>& colors = sections[sectIndex2].subs[playerIndex & 1][2ULL * chartIndex + (playerIndex >> 1)].colors;
												size_t colorIndex = 0;
												for (size_t phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
												{
													Phrase& phr = chart.getPhrase(phrIndex);
													while (colorIndex + 1ULL != colors.size() && long(chart.getPivotTime() + phr.getPivotAlpha()) >= colors[colorIndex + 1ULL].position)
														colorIndex++;
													//Prints color choice character from the colorRange array
													if (!phrIndex)
													{
														fprintf_s(outSheet, "%c", colorRange[colors[colorIndex].color]);
														fprintf_s(outSheet2, "!%c", colorRange[colors[colorIndex].color]);
													}
													else if (phr.getStart())
													{
														fprintf_s(outSheet, " %c", colorRange[colors[colorIndex].color]);
														fprintf_s(outSheet2, " !%c", colorRange[colors[colorIndex].color]);
													}
													else
														fprintf_s(outSheet2, "-%c", colorRange[colors[colorIndex].color]);
												}
											}
											else
											{
												for (size_t phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
												{
													if (!phrIndex)
													{
														fprintf_s(outSheet, "N");
														fprintf_s(outSheet2, "!N");
													}
													else if (chart.getPhrase(phrIndex).getStart())
													{
														fprintf_s(outSheet, " N");
														fprintf_s(outSheet2, " !N");
													}
													else
														fprintf_s(outSheet2, "-N");
												}
											}
											fprintf_s(outSheet, "\n\n");
											fprintf_s(outSheet2, "\n\n");
										}
										chartCount++;
									}
								}
							}
							if (sectIndex2 < sections.size())
								printf("%sColored %s\n", global.tabs.c_str(), section.getName());
						}
						if (outSheet != nullptr)
							fclose(outSheet);
						if (outSheet2 != nullptr)
							fclose(outSheet2);
					}
				}
				case 'n':
				case 'q':
					global.quit = true;
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
		return true;
	}
	else
	{
		printf("%s\n", global.tabs.c_str());
		printf("%sCould not import any notes.\n", global.tabs.c_str());
		return false;
	}
}