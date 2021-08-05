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
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
	glm::vec3 m_front;
	glm::vec3 m_up;
	bool m_firstMouse;
	float m_lastX;
	float m_lastY;
public:
	glm::vec3 m_position;
	float m_yaw;
	float m_pitch;
	float m_fov;
	float m_sens;

	Camera() : Camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

	Camera(const glm::vec3 position)
	{
		m_position = position;
		m_front = glm::vec3(0.0f, 0.0f, -1.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_yaw = -90.0f;
		m_pitch = 0.0f;
		m_fov = 45.0f;
		m_sens = 10.0f;
		m_firstMouse = true;
		m_lastX = 400;
		m_lastY = 300;
	}

	void reset(const glm::vec3 position)
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

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	bool moveCamera(GLFWwindow* window, float delta)
	{
		const float cameraSpeed = m_sens * delta; // adjust accordingly
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			m_position += cameraSpeed * m_front;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			m_position -= cameraSpeed * m_front;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			m_position -= glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			m_position += glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			m_position.y += cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			m_position.y -= cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			m_sens += 100 * delta;

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			m_sens -= 100 * delta;

		return glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
	}

	void turnCamera(GLFWwindow* window, double xpos, double ypos)
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

	void zoom(GLFWwindow* window, double xoffset, double yoffset)
	{
		m_fov -= (float)yoffset;
		if (m_fov < 1.0f)
			m_fov = 1.0f;
		if (m_fov > 45.0f)
			m_fov = 45.0f;
	}
};