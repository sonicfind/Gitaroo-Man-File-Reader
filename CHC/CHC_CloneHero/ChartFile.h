#pragma once
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
#include "NoteTrack.h"

class ChartFile
{
protected:
	std::string file;
	FILE* chart;
public:
	ChartFile() : file(""), chart(nullptr) {}
	ChartFile(std::string filename, const bool write = false);
	virtual bool open(std::string filename, const bool write = false);
	int close() { return fclose(chart); }
};

class ChartFileExporter : public ChartFile
{
public:
	ChartFileExporter() : ChartFile() {}
	ChartFileExporter(std::string filename) : ChartFile(filename, true) {}
	bool open(std::string filename) { return ChartFile::open(filename, true); }
	void write(LinkedList::List<SyncTrack>& sync, LinkedList::List<Event>& events, NoteTrack(&notes)[2], const bool modchart);
	void writeIni(const unsigned char stageNumber, const double totalDuration, const bool jap = true);
};
