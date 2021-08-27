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
#include "pch.h"
#include "xgTime.h"
unsigned long xgTime::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_numFrames, 4, 1, inFile);
	PString::pull(inFile);
	fread(&m_time, 4, 1, inFile);
	PString::pull(inFile);
	return 0;
}

void xgTime::create(FILE* outFile, bool full) const
{
	PString::push("xgTime", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("numFrames", outFile);
		fwrite(&m_numFrames, 4, 1, outFile);
		PString::push("time", outFile);
		fwrite(&m_time, 4, 1, outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgTime::write_to_txt(FILE* txtFile, const char* tabs)
{
	fprintf_s(txtFile, "\t\t%s   # of Frames: %g\n", tabs, m_numFrames);
	fprintf_s(txtFile, "\t\t\t%s  Time: %g (Starting point??)\n", tabs, m_time);
}

void xgTime::setTime(float time)
{
	if (time < m_numFrames - 1)
		m_time = time;
	else
		m_time = m_numFrames - 1;
}

size_t xgTime::getFrame()
{
	return (size_t)m_time;
}

float xgTime::getCoefficient()
{
	return m_time - (unsigned)m_time;
}
