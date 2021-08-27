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
#include "Model.h"
#include "XGM/XGM.h"
#include <GLFW/glfw3.h>

enum class AspectRatioMode
{
	SDTV,
	Widescreen,
	UltraWide
};

class Viewer
{
	// settings
	static AspectRatioMode s_aspectRatio;
	static unsigned int s_screenWidth;
	static unsigned int s_screenHeight;

	GLFWwindow* m_window;
	unsigned int m_lightUBO;
	unsigned int m_viewUBO;
	unsigned int m_projectionUBO;

	glm::vec3 m_lightPos;
	glm::vec3 m_lightAmbient;
	glm::vec3 m_lightDiffuse;
	glm::vec3 m_lightSpecular;
	float m_lightConstant;
	float m_lightLinear;
	float m_lightQuadratic;

	std::list<Model> m_models;

public:
	Viewer(XGM* xgmObject, const std::vector<size_t>& xgIndices);
	int viewXG();
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static std::string getAspectRatioString();
	static unsigned int getScreenHeight() { return s_screenHeight; }
	static void switchAspectRatio();
	static bool changeHeight();

private:
	static void setWidth();
};
