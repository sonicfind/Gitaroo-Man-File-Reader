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
#include "CHC.h"

struct CHCType : FileType
{
	const char* getType() { return ".CHC"; }
	const size_t size() { return 5; };
	bool loadSingle(std::string filename);
	bool loadMulti();
};

class CHC_Main
{
private:
	CHC m_song;
public:
	CHC_Main(std::string filename) : m_song(filename) {}
	bool menu(size_t fileCount = 0);
	void saveFile(bool onExit = false);
	void writeTxt();
	bool applyChanges(const bool fix, const bool swap = false, const bool save = false);
	void edit(const bool multi = false);
	bool makeTAS();
	bool exportChart();
	bool importChart();
	bool createColorTemplate();
};
