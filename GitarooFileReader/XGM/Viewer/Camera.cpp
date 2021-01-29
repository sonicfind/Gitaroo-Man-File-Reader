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
#include <glm/gtc/matrix_transform.hpp>
using namespace InputHandling;

Camera g_camera;

Camera::Camera() : Camera(glm::vec3(0.0f, 0.0f, 200.0f)) {}

Camera::Camera(const glm::vec3 position)
{
	m_position = position;
	m_front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_yaw = -90.0f;
	m_pitch = 0.0f;
	m_fov = 45.0f;
	m_sens = 500.0f;
	m_firstMouse = true;
	m_lastX = 400;
	m_lastY = 300;
}

void Camera::reset(const glm::vec3 position)
{
	m_position = position;
	m_front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_yaw = -90.0f;
	m_pitch = 0.0f;
	m_fov = 45.0f;
	m_firstMouse = true;
	m_lastX = 400;
	m_lastY = 300;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::moveCamera(float delta)
{
	const float cameraSpeed = m_sens * delta; // adjust accordingly
	if (g_input_keyboard.KEY_W.isActive())
		m_position += cameraSpeed * m_front;

	if (g_input_keyboard.KEY_S.isActive())
		m_position -= cameraSpeed * m_front;

	if (g_input_keyboard.KEY_A.isActive())
		m_position -= glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;

	if (g_input_keyboard.KEY_D.isActive())
		m_position += glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;

	if (g_input_keyboard.KEY_SPACE.isActive())
		m_position.y += cameraSpeed;

	if (g_input_keyboard.KEY_LEFT_SHIFT.isActive())
		m_position.y -= cameraSpeed;

	if (g_input_keyboard.KEY_UP.isActive())
	{
		float sensChange = 1000 * delta;
		if (m_sens + sensChange < 100) // Overflow handling
			m_sens = 100;
		else
			m_sens += sensChange;
	}

	if (g_input_keyboard.KEY_DOWN.isActive())
	{
		float sensChange = 1000 * delta;
		if (m_sens - sensChange < 100)
			m_sens = 100;
		else
			m_sens -= sensChange;
	}
}

void Camera::turnCamera(double xpos, double ypos)
{
	if (m_firstMouse) // initially set to true
	{
		m_lastX = (float)xpos;
		m_lastY = (float)ypos;
		m_firstMouse = false;
	}
	float xoffset = (float)xpos - m_lastX;
	float yoffset = m_lastY - (float)ypos; // reversed: y ranges bottom to top
	m_lastX = (float)xpos;
	m_lastY = (float)ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	if (m_pitch > 89.9f)
		m_pitch = 89.9f;

	if (m_pitch < -89.9f)
		m_pitch = -89.9f;

	if (m_yaw >= 360.0f)
		m_yaw -= 360.0f;

	if (m_yaw <= -0.0f)
		m_yaw += 360.0f;

	glm::vec3 direction(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
		sin(glm::radians(m_pitch)),
		sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
	m_front = glm::normalize(direction);
}

void Camera::zoom(double xoffset, double yoffset)
{
	m_fov -= (float)yoffset;
	if (m_fov < 1.0f)
		m_fov = 1.0f;
	if (m_fov > 45.0f)
		m_fov = 45.0f;
}