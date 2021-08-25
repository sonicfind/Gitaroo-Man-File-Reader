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
#include "TextureAnimation.h"
TexAnim::TexAnim(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "\0\0\0\0"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No '\\0\\0\\0\\0' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk1, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(&m_offset_X, 4, 1, inFile);
	fread(&m_offset_Y, 4, 1, inFile);
	fread(m_texture, 1, 24, inFile);

	unsigned long numCutOuts, numTexFrames;
	fread(&numCutOuts, 4, 1, inFile);
	m_cutOuts.resize(numCutOuts);
	fread(&m_cutOuts.front(), sizeof(CutOut), numCutOuts, inFile);

	fread(&numTexFrames, 4, 1, inFile);
	m_textureFrames.resize(numTexFrames);
	fread(&m_textureFrames.front(), sizeof(TexFrame), numTexFrames, inFile);
}

void TexAnim::create(FILE* outFile)
{
	fwrite("\0\0\0\0", 1, 4, outFile);

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk1, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_offset_X, 4, 1, outFile);
	fwrite(&m_offset_Y, 4, 1, outFile);
	fwrite(m_texture, 1, 24, outFile);

	unsigned long numCutOuts = (unsigned long)m_cutOuts.size(), numTexFrames = (unsigned long)m_textureFrames.size();
	fwrite(&numCutOuts, 4, 1, outFile);
	fwrite(&m_cutOuts.front(), sizeof(CutOut), numCutOuts, outFile);

	fwrite(&numTexFrames, 4, 1, outFile);
	fwrite(&m_textureFrames.front(), sizeof(TexFrame), numTexFrames, outFile);
}
