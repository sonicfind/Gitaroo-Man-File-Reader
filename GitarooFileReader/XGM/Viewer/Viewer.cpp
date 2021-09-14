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
#include <glad/glad.h>
#include "Viewer.h"
#include "InputHandler.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

AspectRatioMode Viewer::s_aspectRatio = AspectRatioMode::Widescreen;
unsigned int Viewer::s_screenWidth = 1280;
unsigned int Viewer::s_screenHeight = 720;

Viewer::Viewer()
	: m_previous(0)
	, m_isPaused(false)
	, m_showNormals(false)
	, m_view(glm::identity<glm::mat4>())
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(s_screenWidth, s_screenHeight, "XG Viewer", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		throw "Failed to create GLFW window";
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw "Failed to initialize GLAD";

	glViewport(0, 0, s_screenWidth, s_screenHeight);

	g_shaders.createPrograms("base.vert", "material.frag", "geometry.vert", "geometry.geo", "geometry.frag");
	g_boneShaders.createPrograms("bones.vert", "material.frag", "geometry - bones.vert", "geometry.geo", "geometry.frag");

	glfwSetFramebufferSizeCallback(m_window, InputHandling::framebuffer_size_callback);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CW);
	// Enable color blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Generate view matrix uniform
	glGenBuffers(1, &m_viewUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_viewUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

	g_shaders.bindUniformBlock(1, "View");
	g_boneShaders.bindUniformBlock(1, "View");

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_viewUBO);

	// Generate projection matrix uniform
	glGenBuffers(1, &m_projectionUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

	g_shaders.bindUniformBlock(2, "Projection");
	g_boneShaders.bindUniformBlock(2, "Projection");

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_projectionUBO);

	// Generate light structure uniform
	glGenBuffers(1, &m_lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);

	g_shaders.m_base.bindUniformBlock(3, "Lights");
	g_boneShaders.m_base.bindUniformBlock(3, "Lights");

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	xgEnvelope::generateBoneUniform();
	Model::resetTime();
	g_camera.reset();
}

Viewer::~Viewer()
{
	xgEnvelope::deleteBoneUniform();
	g_shaders.closePrograms();
	g_boneShaders.closePrograms();
	InputHandling::resetInputs();
	glfwTerminate();
}

Viewer_XGM::Viewer_XGM(const std::vector<XG*>& models)
	: m_showAnimation(true)
	, m_lightPos(0, 100, 100)
	, m_lightAmbient(.5, .5, .5)
	, m_lightDiffuse(1, 1, 1)
	, m_lightSpecular(.5, .5, .5)
	, m_lightConstant(1.0f)
	, m_lightLinear(0.007f)
	, m_lightQuadratic(0.0002f)
{
	for (auto model : models)
		m_models.emplace_back(model);

	// Set light values
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, glm::value_ptr(m_lightAmbient));
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, glm::value_ptr(m_lightDiffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 4, glm::value_ptr(m_lightSpecular));
	glBufferSubData(GL_UNIFORM_BUFFER, 44, 4, &m_lightConstant);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &m_lightLinear);
	glBufferSubData(GL_UNIFORM_BUFFER, 52, 4, &m_lightQuadratic);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
	glfwSetScrollCallback(m_window, InputHandling::scroll_callback);
	m_isMouseActive = true;
}

Viewer_XGM::~Viewer_XGM()
{
	Model::resetLoop();
	Model::resetTime();
}

float Viewer_SSQ::s_startFrame = 0;

Viewer_SSQ::Viewer_SSQ(SSQ* ssq)
	: m_ssq(ssq)
	, m_hasFreeMovement(false)
{
	ssq->loadbuffers();

	SSQ::setFrame(s_startFrame);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(m_window, NULL);
	glfwSetScrollCallback(m_window, NULL);
	m_isMouseActive = false;
}

Viewer_SSQ::~Viewer_SSQ()
{
	m_ssq->unloadBuffers();
}

std::string Viewer::getAspectRatioString()
{
	switch (s_aspectRatio)
	{
	case AspectRatioMode::SDTV:
		return "4x3";
	case AspectRatioMode::Widescreen:
		return "16x9";
	case AspectRatioMode::UltraWide:
		return "21x9";
	}
	return "";
}

void Viewer::switchAspectRatio()
{
	switch (s_aspectRatio)
	{
	case AspectRatioMode::SDTV:
		s_aspectRatio = AspectRatioMode::Widescreen;
		break;
	case AspectRatioMode::Widescreen:
		s_aspectRatio = AspectRatioMode::UltraWide;
		break;
	case AspectRatioMode::UltraWide:
		s_aspectRatio = AspectRatioMode::SDTV;
	}
	setWidth();
}

bool Viewer::changeHeight()
{
	while (true)
	{
		GlobalFunctions::printf_tab("Current Screen Height: %u ['B' to leave unchanged]\n", s_screenHeight);
		GlobalFunctions::printf_tab("Input: ");
		switch (GlobalFunctions::valueInsert(s_screenHeight, false, "b"))
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::Success:
			setWidth();
			__fallthrough;
		case GlobalFunctions::ResultType::SpecialCase:
			return false;
		case GlobalFunctions::ResultType::InvalidNegative:
			GlobalFunctions::printf_tab("Value must be positive.\n");
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::Failed:
			GlobalFunctions::printf_tab("\"%s\" is not a valid response.\n", g_global.invalid.c_str());
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
		}
	}
}

void Viewer::setWidth()
{
	switch (s_aspectRatio)
	{
	case AspectRatioMode::SDTV:
		s_screenWidth = (unsigned int)round((4.0 * s_screenHeight) / 3);
		break;
	case AspectRatioMode::Widescreen:
		s_screenWidth = (unsigned int)round((16.0 * s_screenHeight) / 9);
		break;
	case AspectRatioMode::UltraWide:
		s_screenWidth = (unsigned int)round((21.0 * s_screenHeight) / 9);
	}
}

bool Viewer_SSQ::changeStartFrame()
{
	while (true)
	{
		GlobalFunctions::printf_tab("Current Starting Fame: %g ['B' to leave unchanged]\n", s_startFrame);
		GlobalFunctions::printf_tab("Input: ");
		switch (GlobalFunctions::valueInsert(s_startFrame, false, "b"))
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::Success:
		case GlobalFunctions::ResultType::SpecialCase:
			return false;
		case GlobalFunctions::ResultType::InvalidNegative:
			GlobalFunctions::printf_tab("Value must be positive.\n");
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
			break;
		case GlobalFunctions::ResultType::Failed:
			GlobalFunctions::printf_tab("\"%s\" is not a valid response.\n", g_global.invalid.c_str());
			GlobalFunctions::printf_tab("\n");
			GlobalFunctions::clearIn();
		}
	}
}

float Viewer_SSQ::getStartFrame()
{
	return s_startFrame;
}

void Viewer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	g_camera.turnCamera(xpos, ypos);
}

void Viewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_camera.zoom(xoffset, yoffset);
}

int Viewer::view()
{
	m_previous = (float)glfwGetTime();
	double lastFPSTime = glfwGetTime();
	int nbFrames = 0;
	
	while (!glfwWindowShouldClose(m_window))
	{
		float currentTime = (float)glfwGetTime();
		nbFrames++;
		if (currentTime - lastFPSTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			GlobalFunctions::printf_tab("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastFPSTime += 1.0;
		}
		InputHandling::processInputs(m_window, currentTime);

		if (InputHandling::g_input_keyboard.KEY_ESCAPE.isPressed())
			break;

		if (InputHandling::g_input_keyboard.KEY_N.isPressed())
			m_showNormals = !m_showNormals;

		update(currentTime);
		draw();

		// Check calls
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		m_previous = currentTime;
	}
	return 0;
}

void Viewer_XGM::update(float current)
{
	if (InputHandling::g_input_keyboard.KEY_M.isPressed())
	{
		m_isMouseActive = !m_isMouseActive;
		if (!m_isMouseActive)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(m_window, NULL);
			glfwSetScrollCallback(m_window, NULL);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
			glfwSetScrollCallback(m_window, InputHandling::scroll_callback);
			g_camera.setFirstMouse();
		}
	}

	if (m_isMouseActive)
		g_camera.moveCamera(current - m_previous);

	if (InputHandling::g_input_keyboard.KEY_L.isPressed())
		Model::toggleLoop();

	// Toggling whether to play animations
	if (InputHandling::g_input_keyboard.KEY_O.isPressed())
	{
		m_showAnimation = !m_showAnimation;
		Model::resetTime();
		if (!m_showAnimation)
			for (auto& model : m_models)
				model.restPose();
		else
		{
			for (auto& model : m_models)
				model.resetModel();
			m_isPaused = false;
		}
	}
	else if (m_showAnimation)
	{
		if (InputHandling::g_input_keyboard.KEY_P.isPressed())
			m_isPaused = !m_isPaused;

		if (InputHandling::g_input_keyboard.KEY_R.isActive())
		{
			// Reset current animation
			if (InputHandling::g_input_keyboard.KEY_R.isPressed())
				for (auto& model : m_models)
					model.resetStartTime();
			// Reset to the first animation
			else if (InputHandling::g_input_keyboard.KEY_R.isHeld())
			{
				Model::resetTime();
				for (auto& model : m_models)
					model.resetModel();
			}
		}
		else if (InputHandling::g_input_keyboard.KEY_RIGHT.isActive())
		{
			// Skip to the next animation
			if (InputHandling::g_input_keyboard.KEY_RIGHT.isTicked())
			{
				for (auto& model : m_models)
					model.nextAnimation();
			}
		}
		else if (InputHandling::g_input_keyboard.KEY_LEFT.isActive())
		{
			// Skip down to the previous animation
			if (InputHandling::g_input_keyboard.KEY_LEFT.isTicked())
			{
				for (auto& model : m_models)
					model.prevAnimation();
			}
		}
		// Animates model data as normal
		else if (!m_isPaused)
		{
			// Update animations, obviously
			Model::adjustTime(current - m_previous);
			for (auto& model : m_models)
				model.update();
		}
	}

	// Update view matrix buffer
	glBindBuffer(GL_UNIFORM_BUFFER, m_viewUBO);
	m_view = g_camera.getViewMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(m_view));

	// Update projection matrix buffer
	glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);
	glm::mat4 projection = glm::perspective(glm::radians(g_camera.m_fov), float(s_screenWidth) / s_screenHeight, 1.0f, 40000.0f);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Viewer_XGM::draw()
{
	// Clear color and depth buffers
	glClearColor(0.2f, 0.5f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw opaque meshes
	for (auto& model : m_models)
		model.draw(m_view, m_showNormals, false, m_showAnimation);

	// Draw transparent meshes
	for (auto& model : m_models)
		model.draw(m_view, m_showNormals, true, m_showAnimation);
	glBindVertexArray(0);
}

void Viewer_SSQ::update(float current)
{
	if (InputHandling::g_input_keyboard.KEY_F.isPressed())
	{
		m_hasFreeMovement = !m_hasFreeMovement;
		if (!m_hasFreeMovement)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(m_window, NULL);
			glfwSetScrollCallback(m_window, NULL);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
			glfwSetScrollCallback(m_window, InputHandling::scroll_callback);
			g_camera.setFirstMouse();
			m_isMouseActive = true;
		}
	}
	else if (m_hasFreeMovement && InputHandling::g_input_keyboard.KEY_M.isPressed())
	{
		m_isMouseActive = !m_isMouseActive;
		if (!m_isMouseActive)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(m_window, NULL);
			glfwSetScrollCallback(m_window, NULL);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
			glfwSetScrollCallback(m_window, InputHandling::scroll_callback);
			g_camera.setFirstMouse();
		}
	}

	if (m_hasFreeMovement && m_isMouseActive)
		g_camera.moveCamera(current - m_previous);

	if (InputHandling::g_input_keyboard.KEY_P.isPressed())
	{
		m_isPaused = !m_isPaused;
		if (m_isPaused)
			GlobalFunctions::printf_tab("%g\n",SSQ::getFrame());
	}

	if (InputHandling::g_input_keyboard.KEY_R.isHeld())
	{
		SSQ::setFrame(s_startFrame);
		m_ssq->update();
	}
	// Animates model data as normal
	else if (!m_isPaused)
	{
		// Update animations, obviously
		SSQ::adjustFrame(current - m_previous);
		m_ssq->update();
	}

	// Update view matrix buffer
	glBindBuffer(GL_UNIFORM_BUFFER, m_viewUBO);

	if (m_hasFreeMovement)
		m_view = g_camera.getViewMatrix();
	else
		m_view = m_ssq->getViewMatrix();

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(m_view));

	// Update projection matrix buffer
	glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);

	if (m_hasFreeMovement)
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(glm::perspective(glm::radians(g_camera.m_fov), float(s_screenWidth) / s_screenHeight, 1.0f, 40000.0f)));
	else
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(m_ssq->getProjectionMatrix(s_screenWidth, s_screenHeight)));
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Viewer_SSQ::draw()
{
	// Clear color and depth buffers
	glm::vec4 color = m_ssq->getClearColor();
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw opaque meshes
	m_ssq->draw(m_view, m_showNormals, false);
	// Draw transparent meshes
	m_ssq->draw(m_view, m_showNormals, true);
	glBindVertexArray(0);
}
