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
#include "InputHandler.h"
#include "Camera.h"

static const float s_delayTime = 0.8f;
static const float s_activePressDelta = 1.0f / 12;

namespace InputHandling
{
	Keyboard g_input_keyboard;

	void processKey(GLFWwindow* window, KeyPair* key, const float time)
	{
		if (glfwGetKey(window, key->m_glfwValue) == GLFW_PRESS)
		{
			switch (key->m_status)
			{
			case KeyStatus::Unheld:
			case KeyStatus::Released:
				key->m_status = KeyStatus::Pressed;
				key->m_pressStartTime = time;
				break;
			case KeyStatus::Pressed:
			case KeyStatus::DelayToHold:
				if (time - key->m_pressStartTime < s_delayTime)
					key->m_status = KeyStatus::DelayToHold;
				else
				{
					key->m_status = KeyStatus::Heldtick;
					key->m_pressStartTime = time;
				}
				break;
			default:
				if (time - key->m_pressStartTime >= s_activePressDelta)
				{
					key->m_status = KeyStatus::Heldtick;
					key->m_pressStartTime = time;
				}
				else
					key->m_status = KeyStatus::Held;
			}
		}
		else
		{
			switch (key->m_status)
			{
			case KeyStatus::Held:
			case KeyStatus::Pressed:
				key->m_status = KeyStatus::Released;
				key->m_pressStartTime = 0;
				break;
			default:
				key->m_status = KeyStatus::Unheld;
			}
		}

	}

	void processInputs(GLFWwindow* window, const float time)
	{
		const unsigned int numKeys = sizeof(Keyboard) / sizeof(KeyPair);
		KeyPair* keys = (KeyPair*)&g_input_keyboard;
		for (int index = 0; index < numKeys; ++index)
			processKey(window, keys + index, time);
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		g_camera.turnCamera(xpos, ypos);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		g_camera.zoom(xoffset, yoffset);
	}

	void resetInputs()
	{
		const unsigned int numKeys = sizeof(Keyboard) / sizeof(KeyPair);
		KeyPair* keys = (KeyPair*)&g_input_keyboard;
		for (int index = 0; index < numKeys; ++index)
		{
			keys[index].m_status = KeyStatus::Unheld;
			keys[index].m_pressStartTime = 0;
		}
	}
}