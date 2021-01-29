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

class Camera
{
private:
	glm::vec3 m_front;
	glm::vec3 m_up;
	bool m_firstMouse;
	float m_lastX;
	float m_lastY;
	float m_yaw;
	float m_pitch;
public:
	glm::vec3 m_position;
	float m_fov;
	float m_sens;

	Camera();
	Camera(const glm::vec3 position);
	void reset(const glm::vec3 position);
	glm::mat4 getViewMatrix();
	void moveCamera(float delta);
	void turnCamera(double xpos, double ypos);
	void zoom(double xoffset, double yoffset);
};

extern Camera g_camera;
