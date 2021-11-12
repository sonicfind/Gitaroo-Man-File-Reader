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
PlayerModelSetup::PlayerModelSetup(FILE* inFile, ModelType type, glm::mat4& mat)
	: ModelSetup(inFile, type, mat)
{
	if (m_headerVersion >= 0x1300)
	{
		fread(&m_numControllables, 4, 1, inFile);

		m_controllables.reserve(m_numControllables);
		m_controllables.resize(m_numControllables);
		fread(m_controllables.data(), sizeof(Controllable), m_numControllables, inFile);
		//m_controllables[1].ulong_h = 1;

		m_connections.reserve(m_numControllables);
		m_connections.resize(m_numControllables);
		for (auto& set : m_connections)
		{
			fread(&set.size, 4, 1, inFile);
			if (set.size)
			{
				set.controllableList.reserve(set.size);
				set.controllableList.resize(set.size);
				fread(set.controllableList.data(), 4, set.size, inFile);
			}
		}

		m_defaults.reserve(m_numControllables);
		m_defaults.resize(m_numControllables);
		fread(m_defaults.data(), 4, m_numControllables, inFile);
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

		fwrite(m_defaults.data(), 4, m_numControllables, outFile);
	}
}

void PlayerModelSetup::animateFromGameState(const float frame)
{
	const Controllable* current = &m_controllables[m_controllableIndex];
	float length = m_xg->getAnimationLength(current->animIndex) + current->holdTime;
	if (current->eventFlag == 64 ||
		(current->eventFlag == 4 && 
			current->angleMin == -3.14159298f &&
			current->angleMax == 3.14159298f))
		length += m_xg->getAnimationLength(12);

	const auto player = g_gameState.getPlayerState(static_cast<int>(m_type) - 1);
	while (current->interruptible || frame >= length + m_controllableStartFrame)
	{
		long index = -1;
		bool connected = current->randomize;
		if (!connected)
		{
			for (const auto& connection : m_connections[m_controllableIndex].controllableList)
			{
				const auto& compare = m_controllables[connection];
				if (compare.angleMin <= player.getAngle() &&
					player.getAngle() < compare.angleMax &&
					player.getDescriptor() == compare.descriptor &&
					player.getEvent() <= compare.eventFlag)
				{
					if (player.getEvent() == compare.eventFlag ||
						(player.getEvent() != 0 &&
							(index == -1 || compare.eventFlag < m_controllables[index].eventFlag)))
					{
						connected = true;
						index = connection;
						if (player.getEvent() == compare.eventFlag)
							break;
					}
				}
			}

			if (!connected)
			{
				index = m_defaults[m_controllableIndex];
				if (index == -1)
					break;
			}
		}
		else
			index = m_connections[m_controllableIndex].controllableList[rand() % m_connections[m_controllableIndex].size];

		m_controllableIndex = index;
		if (!current->interruptible && !m_controllables[m_controllableIndex].interruptible)
			m_controllableStartFrame = length + m_controllableStartFrame;
		else if (current->interruptible && !m_controllables[m_controllableIndex].interruptible)
			m_controllableStartFrame = frame;
		else
			m_controllableStartFrame = m_bpmStartFrame;
		current = &m_controllables[m_controllableIndex];

		length = m_xg->getAnimationLength(current->animIndex) + current->holdTime;
		if (current->eventFlag == 64 ||
			(current->eventFlag == 4 &&
				current->angleMin == -3.14159298f &&
				current->angleMax == 3.14159298f))
			length += m_xg->getAnimationLength(12);

		if (!connected && current->interruptible)
			break;
	}
	
	const float delta = frame - m_controllableStartFrame;
	if (length > m_xg->getAnimationLength(current->animIndex) + current->holdTime)
	{
		float anim12 = .5f * m_xg->getAnimationLength(12);
		if (delta < anim12)
			m_xg->animate(2 * delta, 12, m_matrix, 1);
		else if (delta < length - anim12)
			m_xg->animate(delta - anim12, current->animIndex, m_matrix, 1);
		else
			m_xg->animate(2 * (delta - (length - anim12)), 12, m_matrix, 0);
	}
	else
		m_xg->animate(fmod(delta, length), current->animIndex, m_matrix, current->playbackDirection);
}
