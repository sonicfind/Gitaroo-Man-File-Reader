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
#include "CHC_TAS.h"
#include <fstream>
#include <algorithm>
using namespace std;
using namespace GlobalFunctions;

struct
{
	std::string name = "VALUES.P2M2V";
	bool use = false;

	// Each stage is split into two sets
	// Set 1 is the number of samples to offset the recording by (48000 samples per second)
	// Set 2 is the number of total frames to offset the recording by
	int32_t values[13][2][6] =
	{ { { 0, 0, 0, 0, 0, 0 }, {373, 399, 0, 0, 0, 0} }
	, { { 44, 0, 0, 0, 0, 0 }, {646, 639, 646, 720, 735, 739} }
	, { { 0, 0, 0, 0, 0, 0 }, {553, 553, 547, 680, 693, 696} }
	, { { 0, 0, 0, 0, 0, 0 }, {712, 705, 711, 755, 767, 772} }
	, { { 0, 0, 0, 0, 0, 0 }, {304, 298, 304, 0, 0, 0} }
	, { { 0, 0, 0, 0, 0, 0 }, {741, 733, 739, 871, 885, 887} }
	, { { 0, 0, 0, 0, 0, 0 }, {419, 414, 420, 0, 0, 0} }
	, { { 0, 0, 0, 0, 0, 0 }, {695, 673, 682, 818, 827, 833} }
	, { { 0, 0, 0, 0, 0, 0 }, {622, 615, 622, 735, 746, 749} }
	, { { 0, 0, 0, 0, 0, 0 }, {595, 589, 595, 0, 0, 0} }
	, { { 0, 0, 0, 0, 0, 0 }, {656, 650, 657, 523, 0, 0} }
	, { { 0, 0, 0, 0, 0, 0 }, {256, 580, 574, 0, 0, 0} }
	, { { 0, 0, 0, 0, 0, 0 }, {241, 280, 406, 0, 0, 0} } };
} g_frameOffsets;

struct
{
	float samples_per_frame;
	float samples_per_frame_adjusted;
	float speed;
	std::vector<SectPoint> markers;
} g_TASValues;

// Square - 127
// X/Cross  - 191
// Circle - 223
// Triangle - 239
const int PlayerTrack::s_GUARD_ORIENTATIONS[4][4] =
{ 
	{127, 191, 223, 239},
	{127, 191, 223, 239},
	{191, 223, 239, 127},
	{239, 127, 191, 223}
};
const int PlayerTrack::s_PHRASE_ORIENTATIONS[4] = { 127, 127, 191, 239 };

bool loadOffsets(string filename = g_frameOffsets.name)
{
	const size_t pos = filename.find_last_of("\\");
	const string sub = pos != string::npos ? filename.substr(pos + 1) : filename;
	ifstream p2m2v(filename.c_str());
	if (p2m2v.is_open())
	{
		g_frameOffsets.name = filename;
		bool error = false, failed = true;
		for (int framerate = 0; framerate < 2; framerate++)
		{
			p2m2v.ignore(500, ';');
			for (int valueType = 0; valueType < 2; ++valueType)
			{
				while (!p2m2v.eof())
				{
					char line[128];
					p2m2v.getline(line, 128);

					// Used to skip to the next group of values
					if (strchr(line, ';'))
						break;

					std::stringstream str;
					str << line;
					unsigned stage;
					str >> stage;
					if (stage <= 12)
					{
						str.ignore(1);
						for (int difficulty = 0; !str.eof() && difficulty < 6; difficulty++)
						{
							switch (insertFromStream(str, g_frameOffsets.values[stage][valueType][difficulty], true))
							{
							case ResultType::Success:
								failed = false;
								break;
							case ResultType::Failed:
								printf_tab("Error: Invalid input (%s)\n", g_global.invalid.c_str());
								printf_tab("At %s FPS %s - Stage %u"
									, framerate == 0 ? "59.94 " : "50.00 "
									, !valueType ? "[Sample Offsets]" : "[Frame Offsets]"
									, stage);

								switch (difficulty)
								{
								case 0: printf(" (Hard) [1]\n"); break;
								case 1: printf(" (Normal) [2]\n"); break;
								case 2: printf(" (Easy) [3]\n"); break;
								default: printf(" (Multiplayer) [%u]\n", difficulty + 1);
								}

								printf_tab("\n");
								printf_tab("Read ended early\n");
								__fallthrough;
							case ResultType::Quit:
								goto Finish_Read;
							}
						}

						if (!p2m2v.eof() && (strchr(line, 'q') || strchr(line, 'Q')))
							goto Finish_Read;
					}

					if (valueType == 0 && p2m2v.eof())
					{
						printf_tab("Read ended early\n");
						goto Finish_Read;
					}
				}
			}
		}

	Finish_Read:
		p2m2v.close();
		if (!failed)
		{
			printf_tab("%s loaded%s\n", sub.c_str(), !error ? "" : " with error(s)");
			g_frameOffsets.use = true;
			return true;
		}
		else
		{
			printf_tab("%s failed to load\n", sub.c_str());
			return false;
		}
	}
	else
	{
		printf_tab("%s could not be located\n", sub.c_str());
		return false;
	}
}

bool CHC::buildTAS()
{
	if (m_imc[0] == 0)
	{
		printf_tab("%s.CHC is not compatible for PS2\n", m_filename.c_str());
		return false;
	}
	auto load = [&]()
	{
		do
		{
			string p2m2vTemp = "";
			printf_tab("Drag & drop a valid P2M2V file (or type only 'Q' to back out or only 'B' for base values):");
			switch (stringInsertion(p2m2vTemp, "b"))
			{
			case ResultType::Quit:
				throw "TAS creation cancelled";
			case ResultType::SpecialCase:
				g_global.quit = true;
				break;
			case ResultType::Success:
				if (p2m2vTemp.find(".P2M2V") == string::npos)
					p2m2vTemp += ".P2M2V";

				if (loadOffsets(p2m2vTemp))
					g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
		return true;
	};

	banner(" " + m_filename + " - TAS creation ");

	if (!g_frameOffsets.use)
	{
		FILE* p2m2v;
		if (!fopen_s(&p2m2v, g_frameOffsets.name.c_str(), "r"))
		{
			fclose(p2m2v);
			size_t pos = g_frameOffsets.name.find_last_of("\\");
			do
			{
				printf_tab("Use \"%s\"? [Y/N]\n", g_frameOffsets.name.substr(pos != string::npos ? pos + 1 : 0).c_str());
				switch (menuChoices("yn"))
				{
				case ResultType::Success:
					if (g_global.answer.character == 'y')
					{
						loadOffsets();
						g_global.quit = true;
						break;
					}
					else if (load())
					{
						g_global.quit = true;
						break;
					}
					__fallthrough;
				case ResultType::Quit:
					throw "TAS creation cancelled";
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}
		else if (!load())
			return false;
	}
	else if (!loadOffsets() && !load())
		return false;

	try
	{
		PCSX2 pcsx2(m_directory + m_filename, m_stage);

		g_TASValues.samples_per_frame = 48000.0f / 59.94f;
		g_TASValues.samples_per_frame_adjusted = 3.0f * m_speed * g_TASValues.samples_per_frame;

		vector<size_t> sectionIndexes = indexInsertionDialogue(m_sections
				, "Type the index for each section that you wish to TAS - in chronological order and w/ spaces in-between."
				, "TAS Creation"
				, "tmv");
		
		if (sectionIndexes.size() == 0)
			return false;

		bool endReached = false;
		uint32_t totalDuration = 0;

		// Places every single note that will appear in all chosen sections
		// into one huge timeline.
		for (const size_t sectIndex : sectionIndexes)
		{
			SongSection& section = m_sections[sectIndex];
			g_TASValues.markers.push_back({ pcsx2.m_position, SectPoint::VisualType::Technical, (int32_t)round(s_SAMPLES_PER_MIN / section.m_tempo) });
			printf_tab("%s\n", section.m_name);
			if (m_stage == 0
				|| (section.m_battlePhase != SongSection::Phase::INTRO
					&& !strstr(section.m_name, "BRK")
					&& !strstr(section.m_name, "BREAK"))) // If not INTRO phase or BRK section
			{
				if (section.m_battlePhase == SongSection::Phase::END || sectIndex + 1 == m_sections.size())
					endReached = true; // If END phase or last section

				// t - Technical
				// v - Visuals
				// m - Mixed
				for (size_t playerIndex = 0; playerIndex < section.m_numPlayers;
					pcsx2.m_difficulty == 3 ? ++playerIndex : playerIndex += 2)
				{
					for (size_t chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
					{
						if (section.m_charts[playerIndex * section.m_numCharts + chartIndex].getNumPhrases())
						{
							do
							{
								printf_tab("How should %s's phrase bars be played?\n", section.m_name);
								printf_tab("T - Technicality (Release at the end of all sustains)\n");
								printf_tab("V - Visually (Hold past/through the end of all sustains)\n");
								printf_tab("M - Mixed (Release only on sustain that exceeds a defined length)\n");
								switch (menuChoices("tvm", true))
								{
								case ResultType::Quit:
									throw "TAS creation cancelled";
								case ResultType::Help:
									printf_tab("\n");
									__fallthrough;
								case ResultType::Failed:
									break;
								default:
									printf_tab("\n");
									if (g_global.answer.index == 2)
									{
										do
										{
											printf_tab("Provide a value for the sustain limit coeffienct.\n");
											printf_tab("AKA, how many beats must a sustain be before the TAS programs a sustain-release at the end of the note?\n");
											printf_tab("Value can range from 0.5 to 4.0 [Default is 1].\n");
											printf_tab("Input: ");
											float sustainCoeffienct = 1; // Sustain limit set to a base of 1 beat
											switch (valueInsert(sustainCoeffienct, false, 0.5f, 4.0f))
											{
											case ResultType::Success:
												g_TASValues.markers.back().type = SectPoint::VisualType::Mixed;
												g_TASValues.markers.back().sustainLimit = (int32_t)round(sustainCoeffienct * s_SAMPLES_PER_MIN / section.m_tempo);
												g_global.quit = true;
												break;
											case ResultType::Quit:
												throw "TAS creation cancelled";
											case ResultType::InvalidNegative:
											case ResultType::MinExceeded:
												printf_tab("Provided value *must* be greater than or equal to 0.5.\n%s\n", g_global.tabs.c_str());
												break;
											case ResultType::MaxExceeded:
												printf_tab("Provided value *must* be less than or equal to 4.0.\n%s\n", g_global.tabs.c_str());
												break;
											case ResultType::Failed:
												printf_tab("\"%s\" is not a valid response.\n%s\n", g_global.invalid.c_str(), g_global.tabs.c_str());
												clearIn();
											}
										} while (!g_global.quit);
									}
									else
										g_TASValues.markers.back().type = static_cast<SectPoint::VisualType>(g_global.answer.index);
									g_global.quit = true;
								}
							} while (!g_global.quit);
							g_global.quit = false;

							goto Note_Insertion;
						}
					}
				}

			Note_Insertion:
				// Marking where in each list the current section starts
				const size_t startIndex[4][3] =
				{
					{ pcsx2.m_players[0].m_guards.size(), pcsx2.m_players[0].m_phrases.size(), pcsx2.m_players[0].m_tracelines.size() },
					{ pcsx2.m_players[1].m_guards.size(), pcsx2.m_players[1].m_phrases.size(), pcsx2.m_players[1].m_tracelines.size() },
					{ pcsx2.m_players[2].m_guards.size(), pcsx2.m_players[2].m_phrases.size(), pcsx2.m_players[2].m_tracelines.size() },
					{ pcsx2.m_players[3].m_guards.size(), pcsx2.m_players[3].m_phrases.size(), pcsx2.m_players[3].m_tracelines.size() }
				};

				for (size_t chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
				{
					for (size_t playerIndex = 0; playerIndex < section.m_numPlayers; pcsx2.m_difficulty == 3 ? ++playerIndex : playerIndex += 2)
					{
						Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
						size_t currentPlayer = (pcsx2.m_multi[playerIndex & 1] ? playerIndex : playerIndex & 1);

						auto& guardTrack = pcsx2.m_players[currentPlayer].m_guards;
						for (size_t i = 0; i < chart.getNumGuards(); ++i)
						{
							const GuardPoint point =
							{
								 chart.m_guards[i].m_pivotAlpha + chart.m_pivotTime + pcsx2.m_position,
								 chart.m_guards[i].m_button
							};
							guardTrack.insert(std::upper_bound(guardTrack.begin() + startIndex[currentPlayer][0], guardTrack.end(), point), point);
							++pcsx2.m_notes[playerIndex & 1];
						}

						if (chart.getNumTracelines() > 1)
						{
							auto& traceTrack = pcsx2.m_players[currentPlayer].m_tracelines;
							for (size_t i = 0; i < chart.getNumTracelines(); ++i)
							{
								const TracePoint point =
								{
									chart.m_tracelines[i].m_pivotAlpha + chart.m_pivotTime + pcsx2.m_position,
									i + 1 == chart.getNumTracelines(),
									chart.m_tracelines[i].m_angle,
									chart.m_tracelines[i].m_curve > 0
								};
								traceTrack.insert(std::upper_bound(traceTrack.begin() + startIndex[currentPlayer][1], traceTrack.end(), point), point);
							}

							auto& phraseTrack = pcsx2.m_players[currentPlayer].m_phrases;
							for (size_t i = 0; i < chart.getNumPhrases(); ++i)
							{
								Phrase phrase = chart.m_phrases[i];
								// Combine all pieces into one Note
								while (i < chart.getNumPhrases() && !chart.m_phrases[i].m_end)
									++i;

								if (i + 1 != chart.getNumPhrases())
								{
									switch (g_TASValues.markers.back().type)
									{
									case SectPoint::VisualType::Visual:
										if (chart.m_phrases[i + 1].m_pivotAlpha - chart.m_phrases[i].getEndAlpha() < g_TASValues.markers.back().sustainLimit)
											phrase.changeEndAlpha(chart.m_phrases[i + 1].m_pivotAlpha - int32_t(g_TASValues.samples_per_frame));
										else
											phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() + int32_t(2 * g_TASValues.samples_per_frame));
										break;
									case SectPoint::VisualType::Mixed:
										if (chart.m_phrases[i + 1].m_pivotAlpha - phrase.m_pivotAlpha < g_TASValues.markers.back().sustainLimit)
										{
											phrase.changeEndAlpha(chart.m_phrases[i + 1].m_pivotAlpha - int32_t(g_TASValues.samples_per_frame));
											break;
										}
										__fallthrough;
									default:
										phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());
									}
								}
								else if (g_TASValues.markers.back().type != SectPoint::VisualType::Visual
									|| !phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() - int32_t(5 * g_TASValues.samples_per_frame)))
									phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());

								const PhrasePoint point =
								{
									phrase.m_pivotAlpha + chart.m_pivotTime + pcsx2.m_position,
									i,
									i + 1 == chart.getNumPhrases(),
									phrase.m_duration
								};
								phraseTrack.insert(std::upper_bound(phraseTrack.begin() + startIndex[currentPlayer][2], phraseTrack.end(), point), point);
								++pcsx2.m_notes[playerIndex & 1];
							}
						}
					}
				}
			}
			pcsx2.m_position += section.m_duration;
			totalDuration += section.m_duration;
		}

		size_t numFrames = 0;
		if (pcsx2.m_stage == 0 && endReached && m_filename.find('B') == string::npos)
		{
			endReached = false;
			CHC* m_tutorialStageB = nullptr;
			try
			{
				m_tutorialStageB = new CHC(m_directory + "ST00B");

				do
				{
					printf_tab("Continue with ST00B? [Y/N]\n");
					switch (menuChoices("yn"))
					{
					case ResultType::Success:
						if (g_global.answer.character == 'n')
						{
							delete m_tutorialStageB;
							m_tutorialStageB = nullptr;
						}
						g_global.quit = true;
						break;
					case ResultType::Quit:
						throw "TAS creation cancelled";
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}
			catch (...)
			{
				do
				{
					string filename;
					printf_tab("Provide the ST00B.CHC file to use for this TAS (Or 'N' to only TAS ST00A): ");
					switch (stringInsertion(filename, "n"))
					{
					case ResultType::Quit:
						throw "TAS creation cancelled";
					case ResultType::SpecialCase:
						g_global.quit = true;
						break;
					case ResultType::Success:
						if (filename.find("ST00B") != string::npos)
						{
							if (filename.find(".CHC") != string::npos)
								filename = filename.substr(0, filename.length() - 4);
							try
							{
								m_tutorialStageB = new CHC(filename);
								printf_tab("\n");
								g_global.quit = true;
							}
							catch (...)
							{
								printf_tab("The given ST00B could not be successfully read.\n");
							}
						}
						else
							printf_tab("Only an ST00B.CHC file can be accepted.\n");
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}

			if (m_tutorialStageB != nullptr)
			{
				printf_tab("Stage ST00B: ");
				printf("%lu frames during intermission |", g_frameOffsets.values[0][1][1]);
				printf(" Post-intermission Displacement: %li samples\n%s\n", g_frameOffsets.values[0][0][1], g_global.tabs.c_str());
				pcsx2.m_position += int32_t((g_frameOffsets.values[0][1][1] - 1000) * g_TASValues.samples_per_frame) + g_frameOffsets.values[0][0][1];

				sectionIndexes = indexInsertionDialogue(m_sections
						, "Type the index for each section that you wish to TAS from ST00B - in chronological order and w/ spaces in-between."
						, "TAS Creation"
						, "tmv");

				if (sectionIndexes.size() == 0)
				{
					delete m_tutorialStageB;
					return false;
				}

				for (const size_t sectIndex : sectionIndexes)
				{
					SongSection& section = m_tutorialStageB->m_sections[sectIndex];
					g_TASValues.markers.push_back({ pcsx2.m_position, SectPoint::VisualType::Technical, (int32_t)round(s_SAMPLES_PER_MIN / section.m_tempo) });
					printf_tab("%s\n", section.m_name);

					if (sectIndex + 1 == m_tutorialStageB->m_sections.size())
						endReached = true;

					// 0 - Technical
					// 1 - Visuals
					// 2 - Mixed
					for (size_t playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex += 2)
					{
						for (size_t chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
						{
							if (section.m_charts[playerIndex * (size_t)section.m_numCharts + chartIndex].getNumPhrases())
							{
								do
								{
									printf_tab("How should %s's phrase bars be played?\n", section.m_name);
									printf_tab("T - Technicality (Release at the end of all sustains)\n", g_global.tabs.c_str());
									printf_tab("V - Visually (Hold past/through the end of all sustains)\n");
									printf_tab("M - Mixed (Release only on sustain that exceeds a defined length)\n");
									switch (menuChoices("tvm", true))
									{
									case ResultType::Quit:
										delete m_tutorialStageB;
										throw "TAS creation cancelled";
									case ResultType::Help:
										printf_tab("\n");
										__fallthrough;
									case ResultType::Failed:
										break;
									default:
										printf_tab("\n");
										if (g_global.answer.index == 2)
										{
											do
											{
												printf_tab("Provide a value for the sustain limit coeffienct.\n");
												printf_tab("AKA, how many beats must a sustain be before the TAS programs a sustain-release at the end of the note?\n");
												printf_tab("Value can range from 0.5 to 4.0 [Default is 1].\n");
												printf_tab("Input: ");
												float sustainCoeffienct = 1; // Sustain limit set to a base of 1 beat
												switch (valueInsert(sustainCoeffienct, false, 0.5f, 4.0f))
												{
												case ResultType::Success:
													g_TASValues.markers.back().type = SectPoint::VisualType::Mixed;
													g_TASValues.markers.back().sustainLimit = (int32_t)round(sustainCoeffienct * s_SAMPLES_PER_MIN / section.m_tempo);
													g_global.quit = true;
													break;
												case ResultType::Quit:
													delete m_tutorialStageB;
													throw "TAS creation cancelled";
												case ResultType::InvalidNegative:
												case ResultType::MinExceeded:
													printf_tab("Provided value *must* be greater than or equal to 0.5.\n%s\n", g_global.tabs.c_str());
													break;
												case ResultType::MaxExceeded:
													printf_tab("Provided value *must* be less than or equal to 4.0.\n%s\n", g_global.tabs.c_str());
													break;
												case ResultType::Failed:
													printf_tab("\"%s\" is not a valid response.\n%s\n", g_global.invalid.c_str(), g_global.tabs.c_str());
													clearIn();
												}
											} while (!g_global.quit);
										}
										else
											g_TASValues.markers.back().type = static_cast<SectPoint::VisualType>(g_global.answer.index);
										g_global.quit = true;
									}
								} while (!g_global.quit);
								g_global.quit = false;
								goto ST00B_Insertion;
							}
						}
					}

				ST00B_Insertion:
					const size_t startIndex[3] = 
					{ 
						pcsx2.m_players[0].m_guards.size(),
						pcsx2.m_players[0].m_phrases.size(),
						pcsx2.m_players[0].m_tracelines.size()
					};

					for (unsigned chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
					{
						for (unsigned playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
						{
							Chart& chart = section.m_charts[chartIndex];
							auto& guardTrack = pcsx2.m_players[0].m_guards;
							for (size_t i = 0; i < chart.getNumGuards(); ++i)
							{
								const GuardPoint point =
								{
									 chart.m_guards[i].m_pivotAlpha + chart.m_pivotTime + pcsx2.m_position,
									 chart.m_guards[i].m_button
								};
								guardTrack.insert(std::upper_bound(guardTrack.begin() + startIndex[0], guardTrack.end(), point), point);
								++pcsx2.m_notes[0];
							}

							if (chart.getNumTracelines() > 1)
							{
								auto& traceTrack = pcsx2.m_players[0].m_tracelines;
								for (size_t i = 0; i < chart.getNumTracelines(); ++i)
								{
									const TracePoint point =
									{
										chart.m_tracelines[i].m_pivotAlpha + chart.m_pivotTime + pcsx2.m_position,
										i + 1 == chart.getNumTracelines(),
										chart.m_tracelines[i].m_angle,
										chart.m_tracelines[i].m_curve > 0
									};
									traceTrack.insert(std::upper_bound(traceTrack.begin() + startIndex[1], traceTrack.end(), point), point);
								}

								auto& phraseTrack = pcsx2.m_players[0].m_phrases;
								for (size_t i = 0; i < chart.getNumPhrases(); ++i)
								{
									Phrase phrase = chart.m_phrases[i];
									// Combine all pieces into one Note
									while (i < chart.getNumPhrases() && !chart.m_phrases[i].m_end)
										++i;

									if (i + 1 != chart.getNumPhrases())
									{
										switch (g_TASValues.markers.back().type)
										{
										case SectPoint::VisualType::Visual:
											if (chart.m_phrases[i + 1].m_pivotAlpha - chart.m_phrases[i].getEndAlpha() < g_TASValues.markers.back().sustainLimit)
												phrase.changeEndAlpha(chart.m_phrases[i + 1].m_pivotAlpha - int32_t(g_TASValues.samples_per_frame));
											else
												phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() + int32_t(2 * g_TASValues.samples_per_frame));
											break;
										case SectPoint::VisualType::Mixed:
											if (chart.m_phrases[i + 1].m_pivotAlpha - phrase.m_pivotAlpha < g_TASValues.markers.back().sustainLimit)
											{
												phrase.changeEndAlpha(chart.m_phrases[i + 1].m_pivotAlpha - int32_t(g_TASValues.samples_per_frame));
												break;
											}
											__fallthrough;
										default:
											phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());
										}
									}
									else if (g_TASValues.markers.back().type != SectPoint::VisualType::Visual
										|| !phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha() - int32_t(5 * g_TASValues.samples_per_frame)))
										phrase.changeEndAlpha(chart.m_phrases[i].getEndAlpha());

									const PhrasePoint point =
									{
										phrase.m_pivotAlpha + chart.m_pivotTime + pcsx2.m_position,
										i,
										i + 1 == chart.getNumPhrases(),
										phrase.m_duration
									};
									phraseTrack.insert(std::upper_bound(phraseTrack.begin() + startIndex[2], phraseTrack.end(), point), point);
									++pcsx2.m_notes[0];
								}
							}
						}
					}

					pcsx2.m_position += section.m_duration;
					totalDuration += section.m_duration;
				}

				numFrames = (size_t)g_frameOffsets.values[0][1][1] - 1000;
				delete m_tutorialStageB;
			}
		}
		pcsx2.insertFrames(size_t(ceilf(totalDuration / g_TASValues.samples_per_frame)) + numFrames);

		if (endReached)
			pcsx2.resultScreen();

		std::thread threads[4];
		for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
			if (pcsx2.m_players[playerIndex])
				threads[playerIndex] = pcsx2.convertToFrames(playerIndex);

		for (auto& thr : threads)
			thr.join();

		return pcsx2.print();
	}
	catch (const char* str)
	{
		printf_tab("\n%s%s\n", g_global.tabs.c_str(), str);
		return false;
	}
}

TAS::TAS(const std::string& filename, const int stage)
	: m_filename(filename)
	, m_stage(stage)
{
	if (m_stage == 0)
		m_difficulty = 0;
	else
	{
		if (m_stage < 0 || m_stage > 12)
		{
			do
			{
				printf_tab("Which stage will be replaced? [1-12] ('Q' to back out to Main Menu)\n");
				printf_tab("Input: ");
				switch (valueInsert(m_stage, false, 1, 12))
				{
				case ResultType::Success:
					g_global.quit = true;
					break;
				case ResultType::InvalidNegative:
				case ResultType::MinExceeded:
					printf_tab("Given value cannot be less than 1\n%s\n", g_global.tabs.c_str());
					break;
				case ResultType::MaxExceeded:
					printf_tab("Given value cannot be greater than 12\n%s\n", g_global.tabs.c_str());
					break;
				case ResultType::Failed:
					printf_tab("\"%s\" is not a valid response.\n%s\n", g_global.invalid.c_str(), g_global.tabs.c_str());
					break;
				case ResultType::Quit:
					throw "TAS creation cancelled";
				}
			} while (!g_global.quit);
			g_global.quit = false;

			if (m_stage <= 10)
			{
				do
				{
					printf_tab("Which difficulty is this chart for? ('Q' to back out to Main Menu)\n");
					printf_tab(" 0 - Hard/Master Play\n");
					printf_tab(" 1 - Normal\n");
					printf_tab(" 2 - Easy\n");
					printf_tab(" 3 - Multiplayer\n");
					switch (menuChoices("0123", true))
					{
					case ResultType::Quit:
						throw "TAS creation cancelled";
					case ResultType::Help:
						printf_tab("\n");
						__fallthrough;
					case ResultType::Failed:
						break;
					default:
						m_difficulty = (int)g_global.answer.index;
						printf_tab("\n");
						g_global.quit = true;
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}
		}
		else
		{
			switch (filename.back())
			{
			case 'M':
				m_difficulty = 3;
				if (m_stage != 10)
				{
					do
					{
						printf_tab("How many players for this multiplayer TAS [2/3/4]? ('Q' to back out to Main Menu)\n");
						switch (menuChoices("234", true))
						{
						case ResultType::Quit:
							throw "TAS creation cancelled";
						case ResultType::Success:
							switch (g_global.answer.index)
							{
							case 2:
								m_multi[1] = true;
								__fallthrough;
							case 1:
								m_multi[0] = true;
								__fallthrough;
							default:
								printf_tab("\n");
								g_global.quit = true;
							}
						}
					} while (!g_global.quit);
					g_global.quit = false;
				}
				break;
			case 'H':
				m_difficulty = 0; break;
			case 'E':
				m_difficulty = 2; break;
			default:
				m_difficulty = 1;
			}
		}
	}

	m_position = g_frameOffsets.values[m_stage][0][m_difficulty + m_multi[0] + m_multi[1]];
	do
	{
		printf_tab("Which orientation for all guard phrases? ('Q' to back out to Main Menu)\n");
		printf_tab("0 - No Orientation Change\n");
		printf_tab("1 - No Change, but open the config menu in-game\n");
		printf_tab("2 - Change to Orientation 2\n");
		printf_tab("3 - Change to Orientation 3\n");
		switch (menuChoices("0123", true))
		{
		case  ResultType::Quit:
			throw "TAS creation cancelled";
		case  ResultType::Help:
			printf_tab("\n");
			__fallthrough;
		case ResultType::Failed:
			break;
		default:
			m_orientation = (int)g_global.answer.index;
			printf_tab("\n");
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;

	if (m_difficulty == 3)
		m_filename += "_" + to_string(m_orientation) + "_" + to_string(m_multi[0] + m_multi[1] + 2) + 'P';
	else
		m_filename += "_" + to_string(m_orientation) + "_SP";

	printf_tab("Stage %i - Diff. %i: ", m_stage, m_difficulty);
	printf("%lu frames | ", g_frameOffsets.values[m_stage][1][m_difficulty + m_multi[0] + m_multi[1]]);
	printf("Displacement: %li samples\n%s\n", g_frameOffsets.values[m_stage][0][m_difficulty + m_multi[0] + m_multi[1]], g_global.tabs.c_str());
}

PCSX2::PCSX2(const std::string& filename, const int stage)
	: TAS(filename, stage)
{
	printf_tab("Type the name of the author [255 character limit] (';' to notate the end of the name [for multi-step usage]) ('Q' to back out to Main Menu)\n");
	printf_tab("Input: ");
	if (charArrayInsertion(m_header.m_author, 255) == ResultType::Quit)
		throw "TAS creation cancelled";
}

void PCSX2::resultScreen()
{
	// stage 0 = tutorial
	static size_t resultFrames[2][13] = 
	{ {70, 611, 790, 790, 790, 790, 340, 1130, 175, 1125, 790, 790, 790},
	  {0,  500, 500, 500, 0,   500, 0,   500,  500, 0,    500, 500, 500} };

	if (m_stage != 0)
	{
		if (m_notes[0] > m_notes[1])
			m_players[0].m_frames.resize(m_players[0].m_frames.size() + resultFrames[m_difficulty == 3][m_stage] + m_notes[0] + 306);
		else
			m_players[0].m_frames.resize(m_players[0].m_frames.size() + resultFrames[m_difficulty == 3][m_stage] + m_notes[1] + 306);
	}
	else
		m_players[0].m_frames.resize(m_players[0].m_frames.size() + 70);

	if (m_difficulty == 3)
	{
		m_players[1].m_frames.resize(m_players[0].m_frames.size());
		if (m_multi[0])
		{
			m_players[2].m_frames.resize(m_players[0].m_frames.size());
			if (m_multi[1])
				m_players[3].m_frames.resize(m_players[0].m_frames.size());
		}
	}
}

bool PCSX2::print()
{
	do
	{
		switch (fileOverwriteCheck(m_filename + ".pirec"))
		{
		case ResultType::Quit:
			printf_tab("\n%sPCSX2 TAS was completed, but the file generation was cancelled.\n", g_global.tabs.c_str());
			return false;
		case ResultType::No:
			m_filename += "_T";
			break;
		case ResultType::Yes:
#pragma warning(suppress : 4996) 
			FILE* outp2m2 = fopen((m_filename + ".pirec").c_str(), "wb");
			fputc(m_header.m_version, outp2m2);
			fwrite(m_header.m_emulator, 1, 50, outp2m2);
			fwrite(m_header.m_author, 1, 255, outp2m2);
			fwrite(m_header.m_game, 1, 255, outp2m2);

			const size_t size = m_players[0].m_frames.size();
			fwrite(&size, 4, 1, outp2m2);
			fwrite("\0\0\0\0", 1, 4, outp2m2);
			fputc(1, outp2m2);

			char pads = 0b00000001; // Slot 1 Port 1
			if (m_players[1].m_frames.size() > 0) // Slot 2 Port 1
				pads += 0b00010000;

			if (m_players[2].m_frames.size() > 0) // Slot 2 Port 2
				pads += 0b00100000;

			if (m_players[3].m_frames.size() > 0) // Slot 2 Port 3
				pads += 0b01000000;
			fputc(pads, outp2m2);

			unsigned char sequence[18] = { 0 };
			for (size_t index = 0; index < m_players[0].m_frames.size(); index++)
			{
				for (int32_t player = 0; player < 2 + m_multi[0] + m_multi[1]; player++)
				{
					TAS_Frame& frame = m_players[player].m_frames[index];
					sequence[0] = frame.dpad;
					sequence[1] = frame.button;
					sequence[2] = frame.rightStickX;
					sequence[3] = frame.rightStickY;
					sequence[4] = frame.leftStickX;
					sequence[5] = frame.leftStickY;

					frame.dpad = ~frame.dpad;
					if (frame.dpad & 128) sequence[7] = 0xFF;	// Left
					if (frame.dpad & 64) sequence[9] = 0xFF;	// Down
					if (frame.dpad & 32) sequence[6] = 0xFF;	// Right
					if (frame.dpad & 16) sequence[8] = 0xFF;	// Up

					frame.button = ~frame.button;
					if (frame.button & 128) sequence[13] = 0xFF;
					if (frame.button & 64) sequence[12] = 0xFF;	// Cross
					if (frame.button & 32) sequence[11] = 0xFF;
					if (frame.button & 16) sequence[10] = 0xFF;	// Triangle
					fwrite(sequence, 1, 18, outp2m2);
					memset(sequence, 0, 14);
				}
				fflush(outp2m2);
			}
			fclose(outp2m2);
			printf_tab("PCSX2 TAS Completed.\n");
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return true;
}

void PCSX2::insertFrames(size_t numFrames)
{
	if (m_stage == 0)
	{
		m_players[0].m_frames.resize(628ULL + g_frameOffsets.values[0][1][0] + numFrames);
		m_players[0].m_frames[337].dpad &= 247;		// Start - Title Screen
		m_players[0].m_frames[639].button &= 191;	// X-button - Start Tutorial
	}
	else if (m_stage <= 10)
	{
		m_players[0].m_frames.resize(1589ULL + 22ULL * m_multi[0] + g_frameOffsets.values[m_stage][1][m_difficulty + m_multi[0] + m_multi[1]] + numFrames);

		size_t index = 337ULL + 3ULL * m_multi[0];
		m_players[0].m_frames[index++].dpad &= 247;	index += 302 + 4ULL * m_multi[0];		// Start - Title Screen
		m_players[0].m_frames[index++].dpad &= 239;											// D-pad Up - To Settings option
		m_players[0].m_frames[index++].button &= 191;	index += 239 + 3ULL * m_multi[0];	// X-button - Select Settings option
		m_players[0].m_frames[index++].dpad &= 223;	++index;								// D-pad Right - To Difficulty option
		m_players[0].m_frames[index++].dpad &= 223;											// D-pad Right - To Load Game option
		m_players[0].m_frames[index++].button &= 191;	index += 187 + 5ULL * m_multi[0];	// X-button - Select Load Game
		m_players[0].m_frames[index++].button &= 191;	index += 161;						// X-button - Select Memory Slot 1
		m_players[0].m_frames[index++].button &= 191;	index += 49;						// X-button - Select Save File 1
		m_players[0].m_frames[index++].button &= 191;	index += 197 + 3ULL * m_multi[0];	// X-button - Exit Loading Module

		if (m_difficulty == 1)
		{
			m_players[0].m_frames.resize(m_players[0].m_frames.size() + 4);
			m_players[0].m_frames[index++].dpad &= 223;				// D-pad Right - To Difficulty option
			m_players[0].m_frames[index++].dpad &= 191;				// D-pad Down - Difficulty to "Hard"
			m_players[0].m_frames[index++].dpad &= 127;	++index;	// D-pad Left - To Vibration option
		}

		if (m_orientation)
		{
			m_players[0].m_frames.resize(m_players[0].m_frames.size() + 87);
			m_players[0].m_frames[index++].dpad &= 127;		++index;		// D-pad Left - To Volume option
			m_players[0].m_frames[index++].dpad &= 127;						// D-pad Left - To Controller option
			m_players[0].m_frames[index++].button &= 191;	index += 34;	// X-button - Select Controller option
			switch (m_orientation)
			{
			case 1:
				index += 3;		// Do nothing on the Controller screen
				break;
			case 2:
				m_players[0].m_frames[index++].dpad &= 223; index += 2;	// D-pad Right - To Orientation 2
				break;
			case 3:
				m_players[0].m_frames[index++].dpad &= 223; ++index;		// D-pad Right - To Orientation 2
				m_players[0].m_frames[index++].dpad &= 223;				// D-pad Right - To Orientation 3
			}
			m_players[0].m_frames[index++].button &= 239;	index += 45;// /\-button - Exit Controller screen
		}

		m_players[0].m_frames[index++].button &= 239;	index += 130 + 4ULL * m_multi[0];	// /\-button - Exit Settings menu / To Main menu
		switch (m_difficulty)
		{
			case 0:		// Master
				m_players[0].m_frames.resize(m_players[0].m_frames.size() + 1);
				m_players[0].m_frames[index++].dpad &= 191;				// D-pad Down - To Master Play option
				__fallthrough;
			case 1:		// Hard		Stay on Single Play option
			case 2:		// Normal	Stay on Single Play option
			{
				m_players[0].m_frames[index++].button &= 191;			// X-button - Select Current option
				// Need to check if this is still necesssary to dodge a crash
				if (m_stage != 8)
					m_players[0].m_frames.resize(m_players[0].m_frames.size() + 150 + 2ULL * m_stage);
				else
					m_players[0].m_frames.resize(m_players[0].m_frames.size() + 151 + 2ULL * m_stage);

				index += 178;
				for (int stageIndex = 0; stageIndex < m_stage - 1; stageIndex++)
				{
					m_players[0].m_frames[index++].dpad &= 223;	// D-pad Right - stage Scrolling
					if (m_stage - stageIndex - 2)
						++index;
				}

				if (m_stage == 8)
					++index;

				m_players[0].m_frames[index].button &= 191;	// X-button - stage Start
				break;
			}
			case 3:		// Multi-
			{
				m_players[0].m_frames.resize(m_players[0].m_frames.size() + 3);
				m_players[0].m_frames[index++].dpad &= 191; ++index;		// D-pad Down - To Master Play option
				m_players[0].m_frames[index++].dpad &= 191;				// D-pad Down - To VS Play option
				m_players[0].m_frames[index++].button &= 191;			// X-button - Select Current option

				if (m_multi[1])
					m_players[0].m_frames.resize(m_players[0].m_frames.size() + 270U);
				else if (m_multi[0])
					m_players[0].m_frames.resize(m_players[0].m_frames.size() + 238U);
				else if (m_stage == 10)
					m_players[0].m_frames.resize(m_players[0].m_frames.size() + 205U);
				else
					m_players[0].m_frames.resize(m_players[0].m_frames.size() + 203U);
				m_players[1].m_frames.resize(m_players[0].m_frames.size());

				index += 161 + 3ULL * m_multi[0];
				m_players[0].m_frames[index++].button &= 191; index += 47;	// X-button - MP Intro Screen

				int scrollVal = m_stage - (m_stage - 1) / 3;
				for (int stageIndex = 0; stageIndex < scrollVal; stageIndex++)
				{
					++index;
					if (stageIndex + 1 != scrollVal)
						m_players[0].m_frames[index++].dpad &= 191;	// D-pad Down - stage Scrolling
				}

				m_players[0].m_frames[index++].button &= 191;		// X-button - stage Selection

				if (m_multi[1])
					index += 74ULL - (size_t(scrollVal) << 1);
				else if (m_multi[0])
					index += 42ULL - (size_t(scrollVal) << 1);
				else if (m_stage < 5)
					index += 10ULL - (size_t(scrollVal) << 1);
				else
					index += 2;

				m_players[0].m_frames[index].button &= 191;				// X-button - Multiplayer Character Selection
				m_players[1].m_frames[index++].button &= 191; ++index;
				m_players[0].m_frames[index].button &= 191;				// X-button - Health Handicap Selection/stage Start
				m_players[1].m_frames[index++].button &= 191; ++index;

				if (m_multi[0]) // If Player 3
				{
					m_players[2].m_frames = m_players[1].m_frames;
					if (m_multi[1])	// If Player 4
						m_players[3].m_frames = m_players[1].m_frames;
				}
			}
		}
	}
	else
	{
		m_players[0].m_frames.resize(659ULL + 7ULL * m_multi[0] + g_frameOffsets.values[m_stage][1][m_difficulty + m_multi[0] + m_multi[1]] + numFrames);

		size_t index = 337ULL + 3ULL * m_multi[0];
		m_players[0].m_frames[index++].dpad &= 247; index += 302ULL + 4ULL * m_multi[0]; // Start - Title Screen
		m_players[0].m_frames[index++].dpad &= 239; ++index;								// D-pad Up - To Settings option
		m_players[0].m_frames[index++].dpad &= 239;										// D-pad Up - To Collection option
		
		if (m_stage == 11)
		{
			++index;
			m_players[0].m_frames[index++].dpad &= 239;									// D-pad Up - To Theater option
		}

		m_players[0].m_frames[index].button &= 191;										// X-button - Select Theater or Collection option
	}

	m_initialFrame = m_players[0].m_frames.size() - numFrames;
}

std::thread TAS::convertToFrames(const size_t playerIndex)
{
	return std::thread(&PlayerTrack::convertToFrames
		, std::ref(m_players[playerIndex])
		, m_difficulty != 3 ? m_filename + ".txt" 
							: m_filename + "_P" + to_string(playerIndex + 1) + ".txt"
		, m_initialFrame
		, m_orientation);
}

void PlayerTrack::convertToFrames(const std::string& filename, const size_t initialFrame, const int orientation)
{
	// Handle stick movement
	for (size_t traceIndex = 0, guardIndex = 0; traceIndex < m_tracelines.size(); ++traceIndex)
	{
		while (guardIndex < m_guards.size() && m_guards[guardIndex] < m_tracelines[traceIndex])
			++guardIndex;

		if (traceIndex + 1 != m_tracelines.size()
			&& (!m_tracelines[traceIndex].last
				|| 10 * (m_tracelines[traceIndex + 1].position - (float)m_tracelines[traceIndex].position) <= g_TASValues.samples_per_frame_adjusted)
			&& (guardIndex >= m_guards.size()
				|| (m_guards[guardIndex] >= m_tracelines[traceIndex + 1])))
		{
			size_t currentFrame = initialFrame + (size_t)round(m_tracelines[traceIndex].position / g_TASValues.samples_per_frame);
			size_t endFrame = initialFrame + (size_t)round(m_tracelines[traceIndex + 1].position / g_TASValues.samples_per_frame);

			if (endFrame - currentFrame > 0)
			{
				float currentAngle = m_tracelines[traceIndex].angle;
				float angleDif = 0;

				if (!m_tracelines[traceIndex].last)
				{
					if (!m_tracelines[traceIndex + 1].last)
					{
						angleDif = m_tracelines[traceIndex + 1].angle - currentAngle;
						if (angleDif > M_PI)
							angleDif -= 2 * M_PI;
						else if (angleDif < -M_PI)
							angleDif += 2 * M_PI;
					}
				}
				else
					currentAngle = m_tracelines[traceIndex + 1].angle;

				if (orientation == 2)
					currentAngle += .5 * M_PI;
				else if (orientation == 3)
					currentAngle -= .5 * M_PI;

				if (!m_tracelines[traceIndex].curve)
				{
					// Iterate through all frames with a straight trace line, if any
					for (const size_t last = endFrame - size_t(g_TASValues.speed / 20) - 1; currentFrame < last; ++currentFrame)
					{
						// Only Orientation 2 uses the right stick for trace lines
						if (orientation == 2)
						{
							m_frames[currentFrame].rightStickX = (unsigned)round(127 - 127 * cos(currentAngle));
							m_frames[currentFrame].rightStickY = (unsigned)round(127 + 127 * sin(currentAngle));
						}
						else
						{
							m_frames[currentFrame].leftStickX = (unsigned)round(127 - 127 * cos(currentAngle));
							m_frames[currentFrame].leftStickY = (unsigned)round(127 + 127 * sin(currentAngle));
						}
					}
				}

				float angleIncrement = angleDif / (endFrame - currentFrame);
				for (; currentFrame < endFrame; ++currentFrame)
				{
					// Only Orientation 2 uses the right stick for trace lines
					if (orientation == 2)
					{
						m_frames[currentFrame].rightStickX = (unsigned)round(127 - 127 * cos(currentAngle));
						m_frames[currentFrame].rightStickY = (unsigned)round(127 + 127 * sin(currentAngle));
					}
					else
					{
						m_frames[currentFrame].leftStickX = (unsigned)round(127 - 127 * cos(currentAngle));
						m_frames[currentFrame].leftStickY = (unsigned)round(127 + 127 * sin(currentAngle));
					}
					currentAngle += angleIncrement;
				}
			}
		}
	}

#pragma warning(suppress : 4996)
	FILE* taslog = fopen(filename.c_str(), "w");

	fprintf(taslog, "Samples per frame: %g", g_TASValues.samples_per_frame);
	// Handle button presses
	for (size_t markIndex = 0, phraseIndex = 0, guardIndex = 0; markIndex < g_TASValues.markers.size();)
	{
		fprintf(taslog, "\n//// Section Marker %zu at sample %li\n\n", markIndex + 1, g_TASValues.markers[markIndex].position);
		while (phraseIndex < m_phrases.size() || guardIndex < m_guards.size())
		{
			while (phraseIndex < m_phrases.size()
				&& (guardIndex >= m_guards.size() || m_phrases[phraseIndex] < m_guards[guardIndex]))
			{
				if (markIndex + 1 != g_TASValues.markers.size() && g_TASValues.markers[markIndex + 1] <= m_phrases[phraseIndex])
					goto Iteration;

				size_t frame = initialFrame + (size_t)round(m_phrases[phraseIndex].position / g_TASValues.samples_per_frame);
				size_t endFrame = initialFrame + (size_t)ceilf((m_phrases[phraseIndex].position + m_phrases[phraseIndex].duration) / g_TASValues.samples_per_frame);

				fprintf(taslog, "Phrase Bar %03zu- Starting at sample %li | Frame #%zu\n", m_phrases[phraseIndex].index, m_phrases[phraseIndex].position, frame - initialFrame);
				fprintf(taslog, "\t      -   Ending at sample %li | Frame #%zu\n", m_phrases[phraseIndex].position + m_phrases[phraseIndex].duration, endFrame - initialFrame);

				if (m_phrases[phraseIndex].last && phraseIndex + 1 != m_phrases.size())
				{
					size_t test = initialFrame + (size_t)round(m_phrases[phraseIndex + 1].position / g_TASValues.samples_per_frame) - 1;
					if (m_frames[test].leftStickX != 127
						|| m_frames[test].rightStickX != 127
						|| m_frames[test].leftStickY != 127
						|| m_frames[test].rightStickY != 127)
					{
						if ((g_TASValues.markers[markIndex].type == SectPoint::VisualType::Mixed
								&& m_phrases[phraseIndex + 1].position - m_phrases[phraseIndex].position < g_TASValues.markers[markIndex].sustainLimit)
							|| (g_TASValues.markers[markIndex].type == SectPoint::VisualType::Visual
								&& m_phrases[phraseIndex + 1].position - m_phrases[phraseIndex].position < g_TASValues.markers[markIndex].sustainLimit + (int32_t)m_phrases[phraseIndex].duration))
						{
							endFrame = test;
							fprintf(taslog, "\t      - Extended to sample %li | Frame #%zu\n", m_phrases[phraseIndex + 1].position, endFrame - initialFrame);
						}
					}
				}

				m_frames[frame - 1].button = 255;
				while (frame <= endFrame)
					m_frames[frame++].button = s_PHRASE_ORIENTATIONS[orientation];
			}

			while (guardIndex < m_guards.size()
				&& (phraseIndex >= m_phrases.size() || m_guards[guardIndex] <= m_phrases[phraseIndex]))
			{
				if (markIndex + 1 != g_TASValues.markers.size() && g_TASValues.markers[markIndex + 1] <= m_guards[guardIndex])
					goto Iteration;

				size_t grdFrame = initialFrame + (size_t)round(m_guards[guardIndex].position / g_TASValues.samples_per_frame);

				if (guardIndex + 1 != m_guards.size())
				{
					switch (initialFrame + (size_t)round(m_guards[guardIndex + 1].position / g_TASValues.samples_per_frame) - grdFrame)
					{
					case 0:
						// Push the second mark back one frame
						m_guards[guardIndex + 1].position += int32_t(g_TASValues.samples_per_frame);
						// If pushing it back places it behind the note that *was* after it, fix the order
						if (guardIndex + 2 != m_guards.size()
							&& m_guards[guardIndex + 1].position >= m_guards[guardIndex + 2].position)
							std::swap(m_guards[guardIndex + 1], m_guards[guardIndex + 2]);
						__fallthrough;
					case 1:
						// Pull the current mark forwards one frame
						--grdFrame;
					}
				}

				// Clear any buttons from the prior frame
				m_frames[grdFrame - 1].button = 255;
				m_frames[grdFrame].button = s_GUARD_ORIENTATIONS[orientation][m_guards[guardIndex].button];

				fprintf(taslog, "Guard Mark %03zu: ", m_guards[guardIndex].index);
				switch (m_frames[grdFrame].button)
				{
				case 239:
					fprintf(taslog, "[Square] - Landing at sample %li | Frame #%zu\n", m_guards[guardIndex].position, grdFrame - initialFrame);
					break;
				case 223:
					fprintf(taslog, "[X/Cross] - Landing at sample %li | Frame #%zu\n", m_guards[guardIndex].position, grdFrame - initialFrame);
					break;
				case 191:
					fprintf(taslog, "[Circle] - Landing at sample %li | Frame #%zu\n", m_guards[guardIndex].position, grdFrame - initialFrame);
					break;
				case 127:
					fprintf(taslog, "[Triangle] - Landing at sample %li | Frame #%zu\n", m_guards[guardIndex].position, grdFrame - initialFrame);
				}
			}
		}
	Iteration:
		++markIndex;
		fflush(taslog);
	}

	fclose(taslog);
}
