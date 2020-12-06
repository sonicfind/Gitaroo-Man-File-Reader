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
#include "IMX.h"

struct IMXType : FileType
{
	const char* getType() { return ".IMX"; }
	const size_t size() { return 5; };
	bool loadSingle(std::string filename);
	bool loadMulti();
};

class IMX_Main
{
	friend class XGM_Main;
private:
	IMX imx;
public:
	IMX_Main(std::string filename) : imx(filename) {}
	IMX_Main(IMX& other)
	{
		imx = other;
		imx.m_directory = other.m_directory;
		imx.m_fromXGM = true;
	}
	bool menu(size_t fileCount = 0);
	void saveFile(bool onExit = false);
	void writeTxt();
	void writeTxt(FILE* outTXT, FILE* outSimpleTXT);
};
