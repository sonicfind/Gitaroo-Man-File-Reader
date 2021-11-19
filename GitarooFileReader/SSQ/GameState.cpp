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
#include "Sub Types/SSQ_BaseStructs.h"
#include "Viewer/InputHandler.h"
void GameState::reset()
{
	m_mode = Mode::Attack_Charge;
	for (auto& model : m_activeModels)
		model = false;
}

void GameState::setGlobalStates()
{
	if (InputHandling::g_input_keyboard.KEY_APOSTROPHE.isPressed())
	{
		if (m_mode == Mode::Attack_Charge)
			m_mode = Mode::Guard;
		else
			m_mode = Mode::Attack_Charge;
	}
	bool changed = false;
	for (int i = 0, playerIndex = 0; i < 4; ++i)
	{
		if (glfwJoystickPresent(i))
		{
			m_players[playerIndex].set(static_cast<int>(m_mode), playerIndex++, i);
			int axisCount = 0;
			const float* axes = glfwGetJoystickAxes(i, &axisCount);
			if (!changed && axisCount > 5)
			{
				m_activeModels[7] = m_activeModels[5] = axes[4] >= -.5;
				m_activeModels[8] = m_activeModels[6] = axes[5] >= -.5;
				changed = true;
			}
		}

		if ((&InputHandling::g_input_keyboard.KEY_5 + i)->isPressed())
			m_activeModels[5 + i] = !m_activeModels[5 + i];
	}
}

void GameState::Player::set(int mode, int playerIndex, int padIndex)
{
	playerEvent = 0;
	eventDescriptor = 0;
	angle = 0;

	int buttonCount = 0, axisCount = 0;

	// 0 - A
	// 1 - B
	// 2 - X
	// 3 - Y
	// 7 - Start
	const unsigned char* buttons = glfwGetJoystickButtons(padIndex, &buttonCount);
	const float* axes = glfwGetJoystickAxes(padIndex, &axisCount);

	// Decides whether the current player is in the attack/charge phase or the guard phase
	//
	// True if attack/Charge
	if ((playerIndex & 1) == mode)
	{
		if (abs(axes[0]) >= .1 || abs(axes[1]) >= .1)
		{
			playerEvent = 512;
			angle = atan2(-axes[1], -axes[0]);
		}

		// Buttons are setup this way purely for testing purposes
		for (int buttonIndex = 0; buttonIndex < buttonCount; ++buttonIndex)
			if (buttons[buttonIndex])
			{
				if (buttonIndex == 0)
					playerEvent = 4;
				else
				{
					playerEvent = 9;
					eventDescriptor = buttonIndex;
				}
				break;
			}
	}
	else
	{
		for (int buttonIndex = 0; buttonIndex < buttonCount; ++buttonIndex)
			if (buttons[buttonIndex])
			{
				playerEvent = 64;
				static const int descriptors[] = { 2, 3, 1, 4 };
				eventDescriptor = descriptors[buttonIndex];
				break;
			}
	}
}