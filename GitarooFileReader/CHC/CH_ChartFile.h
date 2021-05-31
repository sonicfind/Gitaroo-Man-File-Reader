#pragma once
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
#include "CH_NoteTrack.h"

class ChartFile
{
protected:
	std::string m_file;
	FILE* m_chart;
public:
	ChartFile() : m_file(""), m_chart(nullptr) {}
	ChartFile(std::string filename, const bool write = false);
	virtual bool open(std::string filename, const bool write = false);
	int close() { return fclose(m_chart); }
};

class ChartFileExporter : public ChartFile
{
	friend class CH_Exporter;
	friend class ChartFileImporter;
	std::vector<SyncTrack> m_sync;
	std::vector<Event> m_events;
	NoteTrack m_modchartNotes[2];
	NoteTrack m_reimportNotes[2];
public:
	ChartFileExporter() : ChartFile() {}
	ChartFileExporter(std::string filename) : ChartFile(filename, true) {}
	ChartFileExporter(std::vector<SyncTrack>& sync, std::vector<Event>& events, NoteTrack notes[2]);
	bool open(std::string filename) { return ChartFile::open(filename, true); }
	void writeHeader(const bool modchart);
	void write(const bool modchart);
	void writeDuetModchart();
	void writeIni(const unsigned char stageNumber, const unsigned long totalDuration, const bool jap = true);
};
