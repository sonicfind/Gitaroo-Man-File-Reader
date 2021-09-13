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
#include "Model_Setup.h"
PlayerModelSetup::PlayerModelSetup(FILE* inFile, char(&name)[16])
	: ModelSetup(inFile, name)
{
	if (m_headerVersion >= 0x1300)
	{
		unsigned long numControllable;
		fread(&numControllable, 4, 1, inFile);
		m_player_controllable.resize(numControllable);
		fread(&m_player_controllable.front(), sizeof(Struct48_2f), numControllable, inFile);
		m_player_read4Entry.resize(numControllable);
		for (auto& r4e : m_player_read4Entry)
		{
			unsigned long num;
			fread(&num, 4, 1, inFile);
			if (num)
			{
				r4e.m_vals.resize(num);
				fread(&r4e.m_vals.front(), 4, num, inFile);
			}
		}
		m_player_ulongs.resize(numControllable);
		fread(&m_player_ulongs.front(), 4, numControllable, inFile);
	}
}

void PlayerModelSetup::create(FILE* outFile) const
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1300)
	{
		unsigned long size = (unsigned long)m_player_controllable.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(&m_player_controllable.front(), sizeof(Struct48_2f), size, outFile);
		for (auto& r4e : m_player_read4Entry)
		{
			unsigned long num = (unsigned long)r4e.m_vals.size();
			fwrite(&num, 4, 1, outFile);
			if (num)
				fwrite(&r4e.m_vals.front(), 4, num, outFile);
		}
		fwrite(&m_player_ulongs.front(), 4, size, outFile);
	}
}
