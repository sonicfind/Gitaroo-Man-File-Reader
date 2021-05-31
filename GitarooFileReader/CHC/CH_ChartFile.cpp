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
#include "CH_ChartFile.h"
using namespace std;

ChartFile::ChartFile(string filename, bool write)
	: m_file(filename)
{
	if (write)
	{
		if (fopen_s(&m_chart, m_file.c_str(), "w"))
			throw "File is currently in use by another process and thus could not be opened.";
	}
	else
#pragma warning(suppress : 4996)
		m_chart = fopen(m_file.c_str(), "r");
}

bool ChartFile::open(string filename, const bool write)
{
	if (m_chart != nullptr)
		close();
	m_file = filename;
	if (write)
		return !fopen_s(&m_chart, m_file.c_str(), "w");
	else
	{
#pragma warning(suppress : 4996)
		m_chart = fopen(m_file.c_str(), "r");
		return m_chart != nullptr;
	}
}
