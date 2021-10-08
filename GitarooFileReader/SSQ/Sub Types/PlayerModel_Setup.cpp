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
		fread(&m_numControllables, 4, 1, inFile);

		m_controllables.reserve(m_numControllables);
		m_controllables.resize(m_numControllables);
		fread(m_controllables.data(), sizeof(Controllable), m_numControllables, inFile);
		//m_controllables[1].ulong_h = 1;

		m_connections.resize(m_numControllables);
		m_connections.reserve(m_numControllables);
		for (auto& set : m_connections)
		{
			fread(&set.size, 4, 1, inFile);
			if (set.size)
			{
				set.controllableList.resize(set.size);
				fread(set.controllableList.data(), 4, set.size, inFile);
			}
		}

		m_endings.reserve(m_numControllables);
		m_endings.resize(m_numControllables);
		fread(m_endings.data(), 4, m_numControllables, inFile);
	}
}

void PlayerModelSetup::create(FILE* outFile) const
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1300)
	{
		fwrite(&m_numControllables, 4, 1, outFile);
		fwrite(m_controllables.data(), sizeof(Controllable), m_numControllables, outFile);

		for (auto& set : m_connections)
		{
			fwrite(&set.size, 4, 1, outFile);
			if (set.size)
				fwrite(set.controllableList.data(), 4, set.size, outFile);
		}

		fwrite(m_endings.data(), 4, m_numControllables, outFile);
	}
}
