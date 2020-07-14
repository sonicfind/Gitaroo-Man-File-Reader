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
#include "CHC.h"
using namespace std;

//Creates a CHC object with 1 songsection
CHC::CHC() : sections(1), name(""), shortname(""), stage(0), speed(0), unorganized(0), optimized(false), saved(2) {}

CHC::CHC(const CHC& song)
	: sections(song.sections), name(song.name), shortname(song.shortname), stage(song.stage),
		speed(song.speed), unorganized(song.unorganized), optimized(song.optimized), saved(song.saved)
{
	memcpy_s(header, 36, song.header, 36);
	memcpy_s(imc, 256, song.imc, 256);
	memcpy_s(events, sizeof(SSQ) * 4, song.events, sizeof(SSQ) * 4);
	memcpy_s(audio, sizeof(AudioChannel) * 8, song.audio, sizeof(AudioChannel) * 8);
	memcpy_s(energyDamageFactors, sizeof(EnergyDamage) * 20, song.energyDamageFactors, sizeof(EnergyDamage) * 20);
}

CHC& CHC::operator=(CHC& song)
{
	name = song.name;
	shortname = song.shortname;
	stage = song.stage;
	memcpy_s(header, 36, song.header, 36);
	memcpy_s(imc, 256, song.imc, 256);
	memcpy_s(events, sizeof(SSQ) * 4, song.events, sizeof(SSQ) * 4);
	memcpy_s(audio, sizeof(AudioChannel) * 8, song.audio, sizeof(AudioChannel) * 8);
	speed = song.speed;
	unorganized = song.unorganized;
	optimized = song.optimized;
	sections = song.sections;
	memcpy_s(energyDamageFactors, sizeof(EnergyDamage) * 20, song.energyDamageFactors, sizeof(EnergyDamage) * 20);
	saved = song.saved;
	return *this;
}

//Creates a CHC object using values from the CHC file pointed to by the provided filename.
//
//Value names chosen to be kept are based off the CHC tab in the Gitaroo Pals shoutwiki
CHC::CHC(string filename) : name(filename + ".CHC"), saved(2)
{
	{
		size_t pos = filename.find_last_of('\\');
		shortname = filename.substr(pos != string::npos ? pos + 1 : 0);
	}
	banner(" Loading " + shortname + ".CHC ");
	try
	{
		stage = stoi(shortname.substr(2, 2));
	}
	catch (...)
	{
		do
		{
			printf("%sWhich number do you wish to give this stage? (Think:\"ST##\") ('Q' to cancel CHC load)\n", global.tabs.c_str());
			printf("%sWarning: setting it the 0 alters how some functions behave (especially organizing)\n", global.tabs.c_str());
			printf("%sInput: ", global.tabs.c_str());
			switch (valueInsert(stage, false))
			{
			case '!':
				global.quit = true;
				break;
			case '-':
				printf("%sGiven value cannot be negative\n%s\n", global.tabs.c_str(), global.tabs.c_str());
				break;
			case '*':
				printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
				break;
			case 'q':
				printf("%s\n", global.tabs.c_str());
				//printf("%s", global.tabs.c_str(), "CHC load cancelled\n";
				throw "Stage number not provided.";
			}
		} while (!global.quit);
		global.quit = false;
	}
	union {
		char c[4];
		unsigned ui;
	} u;
	FILE* inFile;
	if (fopen_s(&inFile, name.c_str(), "rb"))
		throw "Error: " + name + " does not exist.";
	fread(header, 1, 36, inFile);
	if (!strstr(header, "SNGS"))
	{
		fclose(inFile);
		throw "Error: No 'SNGS' Tag at byte 0.";
	}
	memcpy_s(u.c, 4, header + 4, 4);
	optimized = u.ui == 6145;
	fread(imc, 1, 256, inFile);
	bool duet = imc[0] == 0;
	fread(&events, sizeof(SSQ), 4, inFile);
	fread(&audio, sizeof(AudioChannel), 8, inFile);
	fread(&speed, 4, 1, inFile);
	fread(u.c, 4, 1, inFile);
	unorganized = u.ui;
	//Uses FILE* constructor to read section cue data
	for (unsigned sectIndex = 0; sectIndex < u.ui; sectIndex++)
		sections.emplace_back(inFile);
	fseek(inFile, 4, SEEK_CUR);
	bool reorganized = false;
	for (size_t sectIndex = 0; sectIndex < sections.size(); sectIndex++) //SongSections
	{
		fread(u.c, 1, 4, inFile);
		if (!strstr(u.c, "CHLS"))
		{
			fclose(inFile);
			throw "Error: No 'CHLS' Tag for section #" + to_string(sectIndex) + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
		}
		SongSection& section = sections[sectIndex];
		fread(u.c, 1, 4, inFile);
		if (u.ui & 1)
		{
			section.organized = true;
			unorganized--;
		}
		section.swapped = (u.ui - 4864) / 2;
		fread(&section.size, 4, 1, inFile);
		if (!section.organized)
		{
			fread(&section.organized, 4, 1, inFile);
			unorganized -= section.organized;
		}
		else
			fseek(inFile, 4, SEEK_CUR);
		if (!section.swapped)
			fread(&section.swapped, 4, 1, inFile);
		else
			fseek(inFile, 4, SEEK_CUR);
		if ((stage == 11 || stage == 12) && !duet && section.swapped < 4)
		{
			section.swapped += 4;
			printf("%sSection #%zu (%s)'s swap value was adjusted to match current implementation for Duet->PS2 conversions. Make sure to save this file to apply this change.\n", global.tabs.c_str(), sectIndex, section.name);
		}
		fread(section.junk, 1, 16, inFile);
		fread(&section.battlePhase, 4, 1, inFile);
		fread(&section.tempo, 4, 1, inFile);
		fread(&section.duration, 4, 1, inFile);
		fseek(inFile, 4, SEEK_CUR);
		fread(u.c, 4, 1, inFile);
		for (unsigned condIndex = 0; condIndex < u.ui; condIndex++)
			section.conditions.emplace_back(inFile);
		fread(&section.numPlayers, 4, 1, inFile);
		if (section.numPlayers != 4)
		{
			do
			{
				printf("%sAre you sure you want value for the number of players used to read %s to be %lu? [Y/N][Q to halt player swap from this point]\n", global.tabs.c_str(), section.name, section.numPlayers);
				printf("%sChoosing 'N' will force a read of 4 players\n", global.tabs.c_str());
				switch (menuChoices("yn"))
				{
				case '-':
					fclose(inFile);
					throw "Number of players for section " + to_string(sectIndex) + " left unspecified.";
				case '!':
					section.numPlayers = 4;
				case 'q':
					global.quit = true;
				}
			} while (!global.quit);
			global.quit = false;
		}
		fread(&section.numCharts, 4, 1, inFile);
		section.charts.clear();
		for (unsigned playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
		{
			for (unsigned chartIndex = 0; chartIndex < section.numCharts; chartIndex++)
			{
				fread(u.c, 1, 4, inFile);
				if (!strstr(u.c, "CHCH"))
				{
					fclose(inFile);
					throw "Error: No 'CHCH' Tag for section " + to_string(sectIndex) + " - subsection " + to_string(playerIndex * section.numCharts + chartIndex) +
						" [File offset: " + to_string(ftell(inFile) - 4) + "].";
				}
				Chart chart;
				//Skip Chart size as the embedded value can be wrong
				fseek(inFile, 16, SEEK_CUR);
				fread(chart.junk, 1, 16, inFile);
				fread(&chart.pivotTime, 4, 1, inFile);
				fread(&chart.endTime, 4, 1, inFile);
				chart.clearTracelines();
				fread(u.c, 1, 4, inFile); //Read number of trace lines
				//Uses Traceline FILE* constructor
				for (unsigned traceIndex = 0; traceIndex < u.ui; traceIndex++)
					chart.emplaceTraceline(inFile);
				fread(u.c, 1, 4, inFile); //Read number of Phrase bars
				//Uses Phrase FILE* constructor
				for (unsigned phraseIndex = 0; phraseIndex < u.ui; phraseIndex++)
					chart.emplacePhrase(inFile);
				fread(u.c, 1, 4, inFile); //Read number of Guard marks
				//Uses Guard FILE* constructor
				for (unsigned guardIndex = 0; guardIndex < u.ui; guardIndex++)
					chart.emplaceGuard(inFile);
				fseek(inFile, 4, SEEK_CUR);
				section.charts.push_back(chart);
			}
		}
	}
	fread(u.c, 1, 4, inFile);
	if (u.ui != 20)
	{
		fclose(inFile);
		throw "Error: Incorrect constant value found when attempting to read Player Damage / Energy Factors.\n" + 
			global.tabs + "Needed: '20' (or '0x00000014')| Found: " + to_string(u.ui) + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
	fread(&energyDamageFactors, sizeof(EnergyDamage), 20, inFile);
	fclose(inFile);
}

//Create or update a CHC file
void CHC::create(string filename)
{
	{
		size_t pos = filename.find_last_of('\\');
		banner(" Saving " + filename.substr(pos != string::npos ? pos + 1 : 0) + ' ');
	}
	FILE* outFile;
	fopen_s(&outFile, filename.c_str(), "wb");
	union {
		char c[4];
		unsigned ui;
	} u;
	u.ui = 6144UL + optimized;
	memcpy_s(header + 4, 4, u.c, 4);
	fwrite(header, 1, 36, outFile);
	fwrite(imc, 1, 256, outFile);
	fwrite(events, sizeof(SSQ), 4, outFile);
	fwrite(audio, sizeof(AudioChannel), 8, outFile);
	fwrite(&speed, 4, 1, outFile);
	fwrite((unsigned long*)&sections.size(), 4, 1, outFile);
	for (unsigned sectIndex = 0; sectIndex < sections.size(); sectIndex++)	//Cues
	{
		fwrite(&sections[sectIndex].index, 4, 1, outFile);
		fwrite(sections[sectIndex].name, 1, 16, outFile);
		fwrite(sections[sectIndex].audio, 1, 16, outFile);
		fwrite(&sections[sectIndex].frames, 4, 2, outFile);
		fwrite("\0\0\0\0", 1, 4, outFile);
	}
	fwrite((unsigned long*)&sections.size(), 4, 1, outFile);
	for (unsigned sectIndex = 0; sectIndex < sections.size(); sectIndex++) //SongSections
	{
		SongSection& section = sections[sectIndex];
		fputs("CHLS", outFile);
		u.ui = 4864UL;
		fwrite(u.c, 1, 4, outFile);
		fwrite(&section.size, 4, 1, outFile);
		fwrite(&section.organized, 4, 1, outFile);
		fwrite(&section.swapped, 4, 1, outFile);
		fwrite(section.junk, 1, 16, outFile);
		fwrite(&section.battlePhase, 4, 1, outFile);
		fwrite(&section.tempo, 4, 1, outFile);
		fwrite(&section.duration, 4, 1, outFile);
		fwrite("\0\0\0\0", 1, 4, outFile);
		fwrite((unsigned long*)&section.conditions.size(), 4, 1, outFile);
		for (size_t condIndex = 0; condIndex < section.conditions.size(); condIndex++)
			fwrite(&section.conditions[condIndex], 16, 1, outFile);
		fwrite(&section.numPlayers, 4, 1, outFile);
		fwrite(&section.numCharts, 4, 1, outFile);
		for (unsigned playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
		{
			for (unsigned chartIndex = 0; chartIndex < section.numCharts; chartIndex++)
			{
				Chart& chart = section.charts[(size_t)playerIndex * section.numCharts + chartIndex];
				fputs("CHCH", outFile);
				u.ui = 4864UL;
				fwrite(u.c, 1, 4, outFile);
				fwrite(&chart.size, 4, 1, outFile);
				fwrite("\0\0\0\0\0\0\0\0", 1, 8, outFile);
				fwrite(chart.junk, 1, 16, outFile);
				fwrite(&chart.pivotTime, 4, 1, outFile);
				fwrite(&chart.endTime, 4, 1, outFile);
				fwrite((unsigned long*)&chart.tracelines.size(), 4, 1, outFile);
				for (size_t traceIndex = 0; traceIndex < chart.tracelines.size(); traceIndex++)
				{
					fwrite(&chart.tracelines[traceIndex].pivotAlpha, 4, 1, outFile);
					fwrite(&chart.tracelines[traceIndex].duration, 4, 1, outFile);
					fwrite(&chart.tracelines[traceIndex].angle, 4, 1, outFile);
					fwrite(&chart.tracelines[traceIndex].curve, 4, 1, outFile);
				}
				fwrite((unsigned long*)&chart.phrases.size(), 4, 1, outFile);
				for (size_t phraseIndex = 0; phraseIndex < chart.phrases.size(); phraseIndex++)
				{
					fwrite(&chart.phrases[phraseIndex].pivotAlpha, 4, 1, outFile);
					fwrite(&chart.phrases[phraseIndex].duration, 4, 1, outFile);
					fwrite(&chart.phrases[phraseIndex].start, 4, 1, outFile);
					fwrite(&chart.phrases[phraseIndex].end, 4, 1, outFile);
					fwrite(&chart.phrases[phraseIndex].animation, 4, 1, outFile);
					if (chart.phrases[phraseIndex].color != -1)
					{
						fputs("CHARTCLR", outFile);
						fwrite(&chart.phrases[phraseIndex].color, 4, 1, outFile);
					}
					else
						fwrite(chart.phrases[phraseIndex].junk, 1, 12, outFile);
				}
				fwrite((unsigned long*)&chart.guards.size(), 4, 1, outFile);
				for (size_t guardIndex = 0; guardIndex < chart.guards.size(); guardIndex++)
				{
					fwrite(&chart.guards[guardIndex].pivotAlpha, 4, 1, outFile);
					fwrite(&chart.guards[guardIndex].button, 4, 1, outFile);
					fwrite("\0\0\0\0\0\0\0\0", 1, 8, outFile);
				}
				fwrite("\0\0\0\0", 1, 4, outFile);
			}
		}
		fflush(outFile);
	}
	u.ui = 20UL;
	fwrite(u.c, 1, 4, outFile);
	fwrite(energyDamageFactors, sizeof(EnergyDamage), 20, outFile);
	fclose(outFile);
	saved = 1;
}