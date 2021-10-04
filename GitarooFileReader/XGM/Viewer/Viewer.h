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
#include "Model.h"
#include "SSQ/SSQ.h"
#include <GLFW/glfw3.h>

enum class AspectRatioMode
{
	SDTV,
	Widescreen,
	UltraWide
};

class Viewer
{
protected:
	// settings
	static AspectRatioMode s_aspectRatio;
	static unsigned int s_screenWidth;
	static unsigned int s_screenHeight;

	GLFWwindow* m_window;
	unsigned int m_viewUBO;
	unsigned int m_projectionUBO;

	unsigned int m_useLights;

	float m_previous;
	bool m_isPaused;
	bool m_showNormals;
	bool m_isMouseActive;

	Camera m_cameraControl;
	glm::mat4 m_view;
	glm::mat4 m_projection;

public:
	Viewer(const char* windowName);
	virtual ~Viewer();
	int view();
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static std::string getAspectRatioString();
	static unsigned int getScreenHeight() { return s_screenHeight; }
	static void switchAspectRatio();
	static bool changeHeight();

protected:
	virtual void update(float current) = 0;
	virtual void draw() = 0;
	static void setWidth();
};

class Viewer_XGM : public Viewer
{
	unsigned int m_lightUBO;

	bool m_showAnimation;
	std::list<Model> m_models;
public:
	Viewer_XGM(const std::vector<XG*>& models);
	~Viewer_XGM();

private:
	void update(float current);
	void draw();
};

class Viewer_SSQ : public Viewer
{
	SSQ* m_ssq;
	bool m_hasFreeMovement;

public:
	Viewer_SSQ(SSQ* ssq);
	~Viewer_SSQ();

private:
	void update(float current);
	void draw();
};
