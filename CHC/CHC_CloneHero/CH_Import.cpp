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
	bool duet = song.imc[0] == 0;
	ChartFileImporter chart;
	do
	{
		printf("%sProvide the name of the .CHART file you wish to use (Or 'Q' to exit): ", global.tabs.c_str());
		string chartName = "";
		switch (filenameInsertion(chartName))
		{
		case 'q':
			return false;
		case '!':
			if (chartName.find(".CHART") == string::npos && chartName.find(".chart") == string::npos)
				chartName += ".CHART";
#pragma warning(suppress : 4996)
			if (chart.open(chartName.c_str()))
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
	chart.read(*this);
	bool charted[2] = { notes[0].allNotes.size() > 0, notes[1].allNotes.size() > 0 };
	if (charted[0] || charted[1])
	{
		fillSections();
		for (size_t sectIndex = 0; sectIndex < song.sections.size(); sectIndex++)
		{
			SongSection& section = song.sections[sectIndex];
			if (!(section.getPhase() == SongSection::Phase::INTRO || strstr(section.getName(), "BRK") || strstr(section.getName(), "BREAK")))
			{
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
										if (val < sections[sectIndex2].subs[playerIndex & 1].size())
										{
											Chart& imp = sections[sectIndex2].subs[playerIndex & 1][val];
											if (imp.tracelines.size() > 1 || imp.guards.size())
											{
												section.setOrganized(false);
												Chart& ins = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
												long buf = ins.insertNotes(imp);
												if (lastNotes[playerIndex & 1] < buf)
													lastNotes[playerIndex & 1] = buf;
												if (section.getPhase() == SongSection::Phase::BATTLE)
												{
													if (ins.guards.size() && ins.tracelines.size() > 1)
													{
														//Determining which comes first
														if (ins.guards.front().getPivotAlpha() < ins.tracelines.front().getPivotAlpha())
														{
															if ((playerIndex & 1))
																swapped[1] = true;
														}
														else if (ins.tracelines.front().getPivotAlpha() < ins.guards.front().getPivotAlpha())
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
											//Checks if the data in any unchanged subsections needs to be deleted as to not interfere with inserted notes
											Chart& skipped = section.getChart(playerIndex * section.getNumCharts() + chartIndex);
											if (skipped.guards.size() && skipped.guards.front().getPivotAlpha() < lastNotes[playerIndex & 1]
												|| skipped.tracelines.size() > 1 && skipped.tracelines.front().getPivotAlpha() < lastNotes[playerIndex & 1])
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
										if (chartIndex < sections[sectIndex2].subs[playerIndex].size())
										{
											Chart& imp = sections[sectIndex2].subs[playerIndex][chartIndex];
											if (imp.tracelines.size() > 1 || imp.guards.size())
											{
												section.setOrganized(false);
												long buf = section.getChart(2 * playerIndex * section.getNumCharts() + chartIndex).insertNotes(imp);
												if (lastNotes[playerIndex & 1] < buf)
													lastNotes[playerIndex & 1] = buf;
											}
											else
											{
												//Checks if the data in any unchanged subsections needs to be deleted as to not interfere with inserted notes
												Chart& skipped = section.getChart(2 * playerIndex * section.getNumCharts() + chartIndex);
												if (skipped.guards.size() && skipped.guards.front().getPivotAlpha() < lastNotes[playerIndex & 1]
													|| skipped.tracelines.size() > 1 && skipped.tracelines.front().getPivotAlpha() < lastNotes[playerIndex & 1])
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
							song.unorganized++;
						printf("%sReplaced %s\n", global.tabs.c_str(), section.getName());
						break;
					}
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
				case 'c':
					song.name = song.name.substr(0, song.name.length() - 4) + "_IMPORTED.CHC";
					song.shortname += "_IMPORTED";
				case 'k':
					printf("%s\n", global.tabs.c_str());
					global.quit = true;
					break;
				}
			} while (!global.quit);
			global.quit = false;
		}
		else
		{
			do
			{
				printf("%sHow should the file be named?\n", global.tabs.c_str());
				printf("%sK - Keep filename as %s\n", global.tabs.c_str(), song.shortname.c_str());
				printf("%sC - Change filename to %s_T\n", global.tabs.c_str(), song.shortname.c_str());
				switch (menuChoices("kc"))
				{
				case 'q':
					return false;
				case 'c':
					song.name = song.name.substr(0, song.name.length() - 4) + "_T.CHC";
					song.shortname += "_T";
				case 'k':
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
		return true;
	}
	else
	{
		printf("%s\n", global.tabs.c_str());
		printf("%sNote import aborted.\n", global.tabs.c_str());
		return false;
	}
}

void ChartFileImporter::read(CH_Importer& importer)
{
	int chartVersion = 0;
	char ignore[400];
	//Skips the info section
	fscanf_s(chart, " %[^}]", ignore, 400);
	fscanf_s(chart, " %[^{]", ignore, 400);
	fseek(chart, 1, SEEK_CUR);
	char test = 0;
	List<SyncTrack> tempos;
	fscanf_s(chart, " %c", &test, 1);
	//Checking for the end of the synctrack section
	while (test != '}')
	{
		fseek(chart, -1, SEEK_CUR);
		//Creates new tempo object
		tempos.emplace_back(chart);
		fscanf_s(chart, " %c", &test, 1);
	}
	fscanf_s(chart, " %[^{]", ignore, 400);
	fseek(chart, 1, SEEK_CUR);
	size_t tempoIndex = 0;
	fscanf_s(chart, " %c", &test, 1);
	//Checking for the end of the event section
	while (test != '}')
	{
		fseek(chart, -1, SEEK_CUR);
		Event ev(chart);
		//For example, if the name is "BATTLE - something,"
		//only use "something."
		if (ev.name.find("section") != string::npos)
		{
			ev.name = ev.name.substr(ev.name.find_last_of(' ') + 1);
			//Add all tempos between the current section event and the previous section
			//to the previous section
			while (tempoIndex + 1ULL < tempos.size() && (ev.position >= tempos[tempoIndex + 1ULL].position || tempos[tempoIndex].bpm == 0))
			{
				if (ev.position > tempos[tempoIndex + 1ULL].position && tempos[tempoIndex + 1ULL].bpm > 0)
				{
					Section::Tempo& prev = importer.sections.back().tempos.back();
					double pos_samples = prev.position_samples + (tempos[tempoIndex + 1ULL].position - prev.position_ticks)
						* (SAMPLES_PER_MIN / (TICKS_PER_BEAT * prev.bpm / 1000));
					//Creates new tempo object
					importer.sections.back().tempos.emplace_back(tempos[tempoIndex + 1ULL].bpm, tempos[tempoIndex + 1ULL].position, pos_samples);
				}
				tempoIndex++;
			}
			double pos_samples = 0;
			//If this isn't the first section
			if (importer.sections.size())
			{
				Section::Tempo& prev = importer.sections.back().tempos.back();
				pos_samples = importer.sections.back().position_samples + prev.position_samples + (ev.position - prev.position_ticks)
					* (SAMPLES_PER_MIN / (TICKS_PER_BEAT * prev.bpm / 1000));
			}
			Section& section = importer.sections.emplace_back(ev.name, ev.position, pos_samples, tempos[tempoIndex].bpm);
			section.subs[0].emplace_front().clearTracelines();
			section.subs[1].emplace_front().clearTracelines();
		}
		else if (ev.name.find("GMFR EXPORT V2.0") != string::npos)
			chartVersion = 2;
		fscanf_s(chart, " %c", &test, 1);
	}
	if (!chartVersion)
	{
		do
		{
			printf("%sGMFR import, while compatible, works best with a .chart file updated to the current Moonscraper->CHC charting style. Create a converted version of this file? [Y/N]\n", global.tabs.c_str());
			printf("%sEnter 'C' if this file already utilizes this style (his will add the proper global event marker that defines the GMFR import version).\n", global.tabs.c_str());
			switch (menuChoices("ync"))
			{
			case 'n':
			case 'q':
				return;
			case 'c':
				chartVersion = 1;
			case 'y':
				global.quit = true;
			}
		} while (!global.quit);
		global.quit = false;
	}
	while (!feof(chart))
	{
		fscanf_s(chart, " %[^{]", ignore, 400);
		fseek(chart, 1, SEEK_CUR);
		if (feof(chart))
			break;
		fscanf_s(chart, " %c", &test, 1);
		size_t playerIndex;
		if (strstr(ignore, "[ExpertSingle]"))
			playerIndex = 0;
		else if (strstr(ignore, "[ExpertDoubleRhythm]"))
			playerIndex = 1;
		while (test != '}')
		{
			fseek(chart, -1, SEEK_CUR);
			CHNote note(chart);
			switch (note.type)
			{
			case CHNote::NoteType::EVENT:
				importer.notes[playerIndex].addEvent(note.position, note.name);
				break;
			case CHNote::NoteType::NOTE:
				if (note.mod == CHNote::Modifier::NORMAL)
				{
					note.fret.lane = size_t(1) << note.fret.lane;
					importer.notes[playerIndex].addNote(note);
				}
				else if (!chartVersion)
				{
					if (note.mod == CHNote::Modifier::TAP)
					{
						importer.notes[playerIndex].addModifier(note.position, CHNote::Modifier::FORCED);
						importer.notes[playerIndex].addModifier(note.position, CHNote::Modifier::TAP);
					}
				}
				else if (note.mod == CHNote::Modifier::FORCED)
					importer.notes[playerIndex].addModifier(note.position, CHNote::Modifier::FORCED);
			}
			fscanf_s(chart, " %c", &test, 1);
		}
		if (!feof(chart))
			fseek(chart, 1, SEEK_CUR);
		else
			break;
	}
	close();
	if (chartVersion < 2)
	{
		List<Event> events(1, 0, "GMFR EXPORT V2.0");
		for (size_t sectIndex = 0; sectIndex < importer.sections.size(); sectIndex++)
			events.emplace_back(importer.sections[sectIndex].position_ticks, importer.sections[sectIndex].name);
		ChartFileExporter converter;
		if (chartVersion == 0)
			converter.open(file + "v2.0 Convert");
		else
			converter.open(file);
		converter.write(tempos, events, importer.notes, false);
		converter.close();
	}
}

void CH_Importer::fillSections()
{
	for (size_t playerIndex = 0; playerIndex < 2; playerIndex++)
	{
		const size_t listSize = notes[playerIndex].allNotes.size();
		if (listSize)
		{
			size_t sectIndex = 0;
			size_t tempoIndex = 0;
			long double SAMPLES_PER_TICK = SAMPLES_PER_MIN / (TICKS_PER_BEAT * sections[0].tempos[0].bpm / 1000);
			Section* currSection = &sections[0];
			Chart* currChart = &currSection->subs[0].back();
			for (size_t itemIndex = 0; itemIndex < listSize; itemIndex++)
			{
				CHNote* note = notes[playerIndex].allNotes[itemIndex];
				while (sectIndex + 1 < sections.size() && note->position >= sections[sectIndex + 1].position_ticks)
				{
					currSection = &sections[++sectIndex];
					currChart = &currSection->subs[playerIndex].back();
					tempoIndex = 0;
					SAMPLES_PER_TICK = SAMPLES_PER_MIN / (TICKS_PER_BEAT * currSection->tempos[tempoIndex].bpm / 1000);
				}
				while (tempoIndex + 1 < currSection->tempos.size() && note->position >= currSection->tempos[tempoIndex + 1].position_ticks)
					SAMPLES_PER_TICK = SAMPLES_PER_MIN / (TICKS_PER_BEAT * currSection->tempos[++tempoIndex].bpm / 1000);
				switch (note->type)
				{
				case CHNote::NoteType::EVENT:
					//Signals the insertion of a new chart/subsection
					if (note->name.find("start") != string::npos)
					{
						currChart = &currSection->subs[playerIndex].emplace_back();
						currChart->clearTracelines();
					}
					//Signals a trace line related function or note
					else if (note->name.find("Trace") != string::npos)
					{
						//Set previous Trace line's curve set to true
						if (note->name.find("curve") != string::npos)
							currChart->tracelines.back().setCurve(true);
						else
						{
							try
							{
								addTraceLine(((note->position - currSection->tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK) + currSection->tempos[tempoIndex].position_samples,
									note->name, sectIndex, playerIndex, currChart);
							}
							catch (...)
							{
								printf("%sTrace line event at tick position %lu had extraneous data that could not be pulled.\n", global.tabs.c_str(), (unsigned long)note->position);
								printf("%sRemember: trace events *must* be formatted as \"Trace(P)\", \"Trace(P)_[float angle value]\", \"Trace(P)_end\", or \"Trace_curve\"\n", global.tabs.c_str());
							}
						}
					}
					else if (note->name.find("Anim") != string::npos)
						currChart->phrases.back().setAnimation(stoi(note->name.substr(5)));
					break;
				case CHNote::NoteType::NOTE:
					//If Phrase Bar
					if (note->fret.sustain)
					{
						addPhraseBar((long)round((note->position - currSection->tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK + currSection->tempos[tempoIndex].position_samples),
										(unsigned long)round(note->fret.sustain * SAMPLES_PER_TICK), 1UL << note->fret.lane, currChart, (long)round(SAMPLES_PER_TICK));
					}
					else
					{
						switch (note->mod)
						{
						case CHNote::Modifier::NORMAL: //Guard Mark
						{
							addGuardMark((long)round(((note->position - sections[sectIndex].tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK) + sections[sectIndex].tempos[tempoIndex].position_samples),
											(unsigned long)note->fret.lane, currChart);
						}
						break;
						case CHNote::Modifier::FORCED:
							applyForced((long)round(((note->position - sections[sectIndex].tempos[tempoIndex].position_ticks) * SAMPLES_PER_TICK) + sections[sectIndex].tempos[tempoIndex].position_samples),
											currChart, currSection, playerIndex);
						}
					}
				}
			}
			notes[playerIndex].clear();
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
		Section& prevSection = sections[sectIndex - 1];
		Section::Tempo& prevtempo = prevSection.tempos.back();
		if (name.find("end") != string::npos && pos == 0)
			pos--;
		else if (currChart->tracelines.size() > 0)
		{
			if (currChart->tracelines.front().getPivotAlpha() <= pos)
				pos = double(currChart->tracelines.front().getPivotAlpha()) - 1;
		}
		pos += (sections[sectIndex].position_ticks - prevtempo.position_ticks) * (SAMPLES_PER_MIN / (TICKS_PER_BEAT * prevtempo.bpm / 1000)) + prevtempo.position_samples;
		insert = &prevSection.subs[playerIndex].back();
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
	if (currChart->phrases.size() != 0)
	{
		for (size_t index = currChart->phrases.size(); index > 0 && sus > 0;)
		{
			Phrase* previous = &currChart->phrases[--index];
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
				for (; index < currChart->phrases.size(); index++)
				{
					previous = &currChart->phrases[index];
					if (previous->getDuration() <= sus - SAMPLES_PER_TICK_ROUNDED)
					{
						previous->addColor(lane);
						pos += previous->getDuration();
						sus -= previous->getDuration();
						if (index + 1 == currChart->phrases.size())
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
							if (index + 1 == currChart->phrases.size())
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
	if (currChart->guards.size() == 0 || currChart->guards.back().getPivotAlpha() < pos)
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
	size_t index = currChart->phrases.size();
	while (index > 0)
	{
		Phrase& base = currChart->phrases[--index];
		if (base.getPivotAlpha() == pos)
		{
			if (index == 0)
				base.addColor(64);
			else if (base.getStart())
			{
				Phrase& previous = currChart->phrases[index - 1];
				base.setStart(false);
				previous.setEnd(false);
				previous.changeEndAlpha(base.getPivotAlpha());
			}
			else
			{
				bool remove = false;
				while (index > 0)
				{
					Phrase& previous = currChart->phrases[--index];
					for (size_t color = 0; color < 6; color++)
					{
						unsigned long val = 1UL << color;
						if (!(base.getColor() & val) && (previous.getColor() & val))
						{
							if (remove)
							{
								printf("%s%s - Player %zu - Subsection %zu: ", global.tabs.c_str(), currSection->name.c_str(), playerIndex + 1, currSection->subs[playerIndex].size() - 1);
								currChart->remove(index + 1, 'p');
							}
							previous.setEnd(false);
							previous.changeEndAlpha(base.getPivotAlpha());
							global.quit = true;
							break;
						}
					}
					if (!(global.quit || remove || previous.getStart()))
						remove = true;
					else
						break;
				}
				global.quit = false;
			}
			break;
		}
	}
}