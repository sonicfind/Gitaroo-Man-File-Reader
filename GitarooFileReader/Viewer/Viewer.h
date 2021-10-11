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
#include "Camera.h"
#include <GLFW/glfw3.h>
class Viewer
{
protected:
	struct ViewerControls
	{
		unsigned int useLights = 1;

		bool isPaused = false;
		bool showNormals = false;
		bool isMouseActive;
		ViewerControls(bool mouse) : isMouseActive(mouse) {}
	};

	// settings
	static float s_aspectRatio;
	static unsigned int s_screenWidth;
	static unsigned int s_screenHeight;

	GLFWwindow* m_window;
	unsigned int m_viewUBO;
	unsigned int m_projectionUBO;

	Camera* m_controlledCamera;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	std::unique_ptr<ViewerControls> m_viewerControls;

public:
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static const char* getAspectRatioString();
	static void switchAspectRatio();
	static bool changeHeight();

protected:
	int startDisplay(const char* windowName);
	virtual void initialize(const char* windowName);
	virtual void uninitialize();
	virtual void update(float delta) = 0;
	virtual void draw() = 0;
};
