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
#include "SSQ/SSQ.h"
#include "InputHandler.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

float Viewer::s_aspectRatio = 16.0f / 9;
unsigned int Viewer::s_screenWidth = 1280;
unsigned int Viewer::s_screenHeight = 720;
void Viewer::initialize(const char* windowName)
{
	glfwInit();
	// Version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(s_screenWidth, s_screenHeight, windowName, NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		throw "Failed to create GLFW window";
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw "Failed to initialize GLAD";

	glViewport(0, 0, s_screenWidth, s_screenHeight);

	g_shaderList.createPrograms();

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CW);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Generate view matrix uniform
	glGenBuffers(1, &m_viewUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_viewUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

	g_shaderList.bindUniformBlock(1, "View");

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_viewUBO);

	// Generate projection matrix uniform
	glGenBuffers(1, &m_projectionUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

	g_shaderList.bindUniformBlock(2, "Projection");

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_projectionUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &m_lightVBO);
	glGenVertexArrays(1, &m_lightVAO);
	glBindVertexArray(m_lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	xgDagMesh::generateMatrixUniform();
	xgMaterial::generateMaterialUniform();
	xgEnvelope::generateBoneUniform();
	m_view = glm::identity<glm::mat4>();
	m_controlledCamera = new Camera;
}

void Viewer::uninitialize()
{
	delete m_controlledCamera;

	xgDagMesh::deleteMatrixUniform();
	xgMaterial::deleteMaterialUniform();
	xgEnvelope::deleteBoneUniform();

	glDeleteBuffers(1, &m_lightVBO);
	glDeleteVertexArrays(1, &m_lightVAO);
	glDeleteBuffers(1, &m_projectionUBO);
	glDeleteBuffers(1, &m_viewUBO);

	g_shaderList.closePrograms();
	InputHandling::resetInputs();
	glfwTerminate();
}

void XGM::initialize(const char* windowName)
{
	Viewer::initialize(windowName);

	glBindBuffer(GL_ARRAY_BUFFER, m_lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), glm::value_ptr(glm::vec3(0, 40, 200)), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate light structure uniform
	glGenBuffers(1, &m_lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, 112, NULL, GL_STATIC_DRAW);

	g_shaderList.m_baseShaders.m_base.bindUniformBlock(3, "Lights");
	g_shaderList.m_boneShaders.m_base.bindUniformBlock(3, "Lights");
	g_shaderList.m_lightShader.bindUniformBlock(3, "Lights");

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_lightUBO);

	// Set light values
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(unsigned int), &m_viewerControls->useLights);
	unsigned long numLights = 1;
	glBufferSubData(GL_UNIFORM_BUFFER, 4, sizeof(unsigned long), &numLights);
	// Vertex Color Diffuse
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(glm::vec3(0)));
	// Stage Ambience
	glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec3), glm::value_ptr(glm::vec3(92.0f / 255, 104.0f / 255, 111.0f / 255)));
	// Light Direction
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 12, glm::value_ptr(glm::vec3(1, 0, 0)));
	// Light Diffuse
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 12, glm::value_ptr(glm::vec3(1)));
	// Light Specular
	glBufferSubData(GL_UNIFORM_BUFFER, 80, 12, glm::value_ptr(glm::vec3(.5)));
	float coeff = 1;
	glBufferSubData(GL_UNIFORM_BUFFER, 96, 4, &coeff);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
	glfwSetScrollCallback(m_window, InputHandling::scroll_callback);

	for (auto& model : ((ViewerControls_XGM*)m_viewerControls)->m_models)
		model.init();
}

void XGM::uninitialize()
{
	delete (ViewerControls_XGM*)m_viewerControls;
	glDeleteBuffers(1, &m_lightUBO);
	Viewer::uninitialize();
}

XGM::ViewerControls_XGM::ModelInfo::ModelInfo(XG* model)
	: m_model(model)
	, m_animIndex(0)
	, m_length(m_model->getAnimationLength(m_animIndex))
	, m_frame(0) {}

XGM::ViewerControls_XGM::ModelInfo::~ModelInfo()
{
	m_model->uninitializeViewerState();
}

void XGM::ViewerControls_XGM::ModelInfo::init()
{
	m_model->initializeViewerState();
}

void XGM::ViewerControls_XGM::ModelInfo::update(float delta, bool loop)
{
	if (m_length > 0)
	{
		m_frame += delta;
		while (m_frame >= m_length)
		{
			m_frame -= m_length;
			if (m_model->m_animations.size() != 1)
			{
				if (loop)
					GlobalFunctions::printf_tab("%s - Loop\n", m_model->getName());
				else
					nextAnim(false);
			}
		}
	}

	m_model->resetInstanceCount();
	m_model->animate(m_frame, m_animIndex, glm::identity<glm::mat4>());
}

void XGM::ViewerControls_XGM::ModelInfo::draw(bool showNormals, bool doTransparents) const
{
	m_model->draw(showNormals, doTransparents);
}

void XGM::ViewerControls_XGM::ModelInfo::animReset()
{
	m_frame = 0;
	m_model->resetInstanceCount();
	m_model->animate(m_frame, m_animIndex, glm::identity<glm::mat4>());
}

void XGM::ViewerControls_XGM::ModelInfo::nextAnim(bool animate)
{
	if (m_animIndex < m_model->m_animations.size() - 1)
		++m_animIndex;
	else
		m_animIndex = 0;
	animChanged(animate);
}

void XGM::ViewerControls_XGM::ModelInfo::prevAnim()
{
	if (m_animIndex > 0)
		--m_animIndex;
	else
		m_animIndex = m_model->m_animations.size() - 1;
	animChanged(true);
}

void XGM::ViewerControls_XGM::ModelInfo::animChanged(bool animate)
{
	m_frame = 0;
	m_length = m_model->getAnimationLength(m_animIndex);

	if (animate)
	{
		m_model->resetInstanceCount();
		m_model->animate(m_frame, m_animIndex, glm::identity<glm::mat4>());
	}
}

void XGM::ViewerControls_XGM::ModelInfo::fullReset()
{
	m_frame = 0;
	m_animIndex = 0;
	m_length = m_model->getAnimationLength(0);

	m_model->resetInstanceCount();
	m_model->animate(m_frame, m_animIndex, glm::identity<glm::mat4>());
}

void XGM::ViewerControls_XGM::ModelInfo::restPose()
{
	m_model->restPose();
}

void XGM::ViewerControls_XGM::ModelInfo::swap(XG* model)
{
	m_model->uninitializeViewerState();
	m_model = model;
	m_model->initializeViewerState();

	m_frame = 0;
	m_animIndex = 0;
	m_length = m_model->getAnimationLength(0);
}

void XGM::ViewerControls_XGM::ModelInfo::changeTitle(GLFWwindow* window, bool animate, bool loop)
{
	if (animate)
	{
		std::string title = std::string(m_model->getName())
										+ " | Anim: " + std::to_string(m_animIndex)
										+ " / " + std::to_string(m_model->m_animations.size())
										+ " | Frame: " + std::to_string(m_frame)
										+ " / " + std::to_string(m_length);

		if (m_model->m_animations.size() != 1 && loop)
			title += " | Loop Enabled";

		glfwSetWindowTitle(window, title.c_str());
	}
	else
		glfwSetWindowTitle(window, m_model->getName());
}

void SSQ::initialize(const char* windowName)
{
	Viewer::initialize(windowName);

	glBindBuffer(GL_ARRAY_BUFFER, m_lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), glm::value_ptr(glm::vec3(0, 150, 0)), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setFrame(m_startFrame);
	for (auto& model : m_modelSetups)
		model->reset();

	IMXEntry::generateSpriteBuffer(m_IMXentries);
	for (auto& entry : m_IMXentries)
		if (entry.m_imxPtr)
			entry.m_imxPtr->m_data->generateTexture();

	for (size_t i = 0; i < m_modelSetups.size(); ++i)
		if (!m_XGentries[i].m_isClone)
			m_XGentries[i].m_xg->initializeViewerState();

	if (m_shadowPtr)
	{
		m_shadowPtr->m_data->generateTexture();
		glGenBuffers(1, &m_shadowVBO);
		glGenVertexArrays(1, &m_shadowVAO);
		glBindVertexArray(m_shadowVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_shadowVBO);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(4 * sizeof(float)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(8 * sizeof(float)));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(12 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glBufferData(GL_ARRAY_BUFFER, m_modelMatrices.size() * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (m_skyPtr)
	{
		m_skyPtr->m_data->generateTexture();
		glGenBuffers(1, &m_skyVBO);
		glGenVertexArrays(1, &m_skyVAO);
		glBindVertexArray(m_skyVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);

		static const float quadVertices[] = {
			// positions // texCoords
			-1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 0.0f
		};

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_camera.generateBuffers(s_aspectRatio);
	for (auto& texAnim : m_texAnimations)
		texAnim.loadCuts();

	m_sprites.generateSpriteBuffers();
}

void SSQ::uninitialize()
{
	glDeleteBuffers(1, &m_shadowVBO);
	glDeleteVertexArrays(1, &m_shadowVAO);
	m_shadowVBO = 0;
	m_shadowVAO = 0;

	glDeleteBuffers(1, &m_skyVBO);
	glDeleteVertexArrays(1, &m_skyVAO);
	m_skyVBO = 0;
	m_skyVAO = 0;

	for (auto& entry : m_XGentries)
		entry.m_dropShadow = false;

	for (auto& model : m_modelSetups)
		model->reset();
	IMXEntry::deleteSpriteBuffer();
	for (auto& entry : m_IMXentries)
		if (entry.m_imxPtr)
			entry.m_imxPtr->m_data->deleteTexture();

	for (size_t i = 0; i < m_modelSetups.size(); ++i)
		if (!m_XGentries[i].m_isClone)
			m_XGentries[i].m_xg->uninitializeViewerState();

	if (m_shadowPtr)
		m_shadowPtr->m_data->deleteTexture();

	if (m_skyPtr)
		m_skyPtr->m_data->deleteTexture();

	m_camera.deleteBuffers();
	for (auto& texAnim : m_texAnimations)
		texAnim.unloadCuts();

	m_sprites.deleteSpriteBuffers();
	g_gameState.reset();

	delete (ViewerControls_SSQ*)m_viewerControls;
	Viewer::uninitialize();
}

const char* Viewer::getAspectRatioString()
{
	if (s_aspectRatio == 4.0f / 3)
		return "4x3";
	else if (s_aspectRatio == 16.0f / 9)
		return "16x9";
	else
		return "21x9";
}

void Viewer::switchAspectRatio()
{
	if (s_aspectRatio == 4.0f / 3)
		s_aspectRatio = 16.0f / 9;
	else if (s_aspectRatio == 16.0f / 9)
		s_aspectRatio = 21.0f / 9;
	else
		s_aspectRatio = 4.0f / 3;
	s_screenWidth = (unsigned int)round(s_aspectRatio * s_screenHeight);
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
			s_screenWidth = (unsigned int)round(s_aspectRatio * s_screenHeight);
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

void Viewer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	m_controlledCamera->turnCamera(xpos, ypos);
}

void Viewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_controlledCamera->zoom(xoffset, yoffset);
}

int Viewer::startDisplay(const char* windowName)
{
	initialize(windowName);

	double lastFPSTime = glfwGetTime();
#ifdef _DEBUG
	int nbFrames = 0;
#endif
	float previousTime = (float)lastFPSTime;
	while (!glfwWindowShouldClose(m_window))
	{
		float currentTime = (float)glfwGetTime();

#ifdef _DEBUG
		nbFrames++;
		if (currentTime - lastFPSTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			GlobalFunctions::printf_tab("%f ms/frame\n", 1000.0 / double(nbFrames));
			GlobalFunctions::printf_tab("%i frames\n", nbFrames);
			nbFrames = 0;
			lastFPSTime += 1.0;
		}
#endif
		InputHandling::processInputs(m_window, currentTime);

		if (InputHandling::g_input_keyboard.KEY_ESCAPE.isPressed())
			break;

		if (InputHandling::g_input_keyboard.KEY_N.isPressed())
			m_viewerControls->showNormals = !m_viewerControls->showNormals;

		if (InputHandling::g_input_keyboard.KEY_U.isPressed())
			m_viewerControls->useLights = m_viewerControls->useLights ? 0 : 1;

		update(currentTime - previousTime);

		// Update view matrix buffer
		glBindBuffer(GL_UNIFORM_BUFFER, m_viewUBO);
		m_view[2] *= -1.0f;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(m_view));

		// Update projection matrix buffer
		glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(m_projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		drawOpaques();
		drawLights();
		drawTranparents();

		// Check calls
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		previousTime = currentTime;
	}

	uninitialize();
	return 0;
}

// Called last
void Viewer::drawLights()
{
	if (m_viewerControls->showNormals)
	{
		glLineWidth(20);
		g_shaderList.m_lightShader.use();
		glBindBuffer(GL_ARRAY_BUFFER, m_lightVBO);
		glBindVertexArray(m_lightVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(1);
	}
}

void XGM::update(float delta)
{
	ViewerControls_XGM* controls = (ViewerControls_XGM*)m_viewerControls;
	if (InputHandling::g_input_keyboard.KEY_M.isPressed())
	{
		controls->isMouseActive = !controls->isMouseActive;
		if (!controls->isMouseActive)
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
			m_controlledCamera->setFirstMouse();
		}
	}

	if (controls->isMouseActive)
		m_controlledCamera->moveCamera(delta);

	if (InputHandling::g_input_keyboard.KEY_U.isPressed())
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &controls->useLights);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	m_view = m_controlledCamera->getViewMatrix();
	m_projection = glm::perspective(glm::radians(m_controlledCamera->m_fov), s_aspectRatio, 1.0f, 1800000.0f);

	if (controls->m_models.size() == 1)
	{
		if (InputHandling::g_input_keyboard.KEY_PERIOD.isActive() || InputHandling::g_input_keyboard.KEY_COMMA.isActive())
		{
			if (InputHandling::g_input_keyboard.KEY_PERIOD.isTicked() || InputHandling::g_input_keyboard.KEY_COMMA.isTicked())
			{
				if (InputHandling::g_input_keyboard.KEY_PERIOD.isTicked())
				{
					if (controls->modelIndex < m_models.size() - 1)
						++controls->modelIndex;
					else
						controls->modelIndex = 0;
				}
				else
				{
					if (controls->modelIndex > 0)
						--controls->modelIndex;
					else
						controls->modelIndex = m_models.size() - 1;
				}
				
				controls->m_models.front().swap(&m_models[controls->modelIndex]);
				if (!controls->animate)
					controls->m_models.front().restPose();
			}
			delta = 0;
		}
	}
	delta *= 30;

	if (InputHandling::g_input_keyboard.KEY_P.isPressed())
		controls->isPaused = !controls->isPaused;

	// Toggling whether to play animations
	if (InputHandling::g_input_keyboard.KEY_O.isPressed())
	{
		controls->animate = !controls->animate;

		if (!controls->animate)
		{
			for (auto& model : controls->m_models)
				model.restPose();
		}
		else
		{
			controls->isPaused = false;
			for (auto& model : controls->m_models)
				model.fullReset();
		}
	}
	else if (controls->animate)
	{
		if (InputHandling::g_input_keyboard.KEY_L.isPressed())
			controls->loop = !controls->loop;

		for (auto& model : controls->m_models)
		{
			if (InputHandling::g_input_keyboard.KEY_R.isActive())
			{
				// Reset current animation
				if (InputHandling::g_input_keyboard.KEY_R.isHeld())
					model.fullReset();
				else if (InputHandling::g_input_keyboard.KEY_R.isPressed())
					model.animReset();
			}
			else if (InputHandling::g_input_keyboard.KEY_RIGHT.isActive() || InputHandling::g_input_keyboard.KEY_LEFT.isActive())
			{
				// Skip to the next animation
				if (InputHandling::g_input_keyboard.KEY_RIGHT.isTicked())
					model.nextAnim(true);
				else if (InputHandling::g_input_keyboard.KEY_LEFT.isTicked())
					model.prevAnim();
			}
			else if (!controls->isPaused)
				model.update(delta, controls->loop);
		}
	}

	if (controls->m_models.size() == 1)
		controls->m_models.front().changeTitle(m_window, controls->animate, controls->loop);
}

void XGM::drawOpaques()
{
	const ViewerControls_XGM* controls = (ViewerControls_XGM*)m_viewerControls;
	// Clear color and depth buffers
	glClearColor(0.2f, 0.5f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Disable color blending
	glDisable(GL_BLEND);
	// Draw opaque meshes
	for (auto& model : controls->m_models)
		model.draw(controls->showNormals, false);
}

void XGM::drawTranparents()
{
	const ViewerControls_XGM* controls = (ViewerControls_XGM*)m_viewerControls;
	// Enable color blending
	glEnable(GL_BLEND);
	// Draw transparent meshes
	// Does not draw normals
	for (auto& model : controls->m_models)
		model.draw(false, true);
}

void SSQ::update(float delta)
{
	ViewerControls_SSQ* controls = (ViewerControls_SSQ*)m_viewerControls;
	if (InputHandling::g_input_keyboard.KEY_F.isPressed())
	{
		controls->hasFreeMovement = !controls->hasFreeMovement;
		if (!controls->hasFreeMovement)
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
			m_controlledCamera->setFirstMouse();
			controls->isMouseActive = true;
		}
	}
	else if (controls->hasFreeMovement && InputHandling::g_input_keyboard.KEY_M.isPressed())
	{
		controls->isMouseActive = !controls->isMouseActive;
		if (!controls->isMouseActive)
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
			m_controlledCamera->setFirstMouse();
		}
	}

	if (controls->hasFreeMovement && controls->isMouseActive)
		m_controlledCamera->moveCamera(delta);

	{
		static bool toggleState = false;
		bool togglePause = false;

		if (InputHandling::g_input_keyboard.KEY_P.isPressed())
			togglePause = true;
		else
		{
			for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
			{
				int buttonCount;
				const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);
				if (buttons && buttons[7])
				{
					togglePause = true;
					break;
				}
			}
		}

		if (togglePause)
		{
			if (!toggleState)
			{
				controls->isPaused = !controls->isPaused;
				toggleState = true;
			}
		}
		else
			toggleState = false;
	}

	bool doFullUpdate = !controls->isPaused;
	if (InputHandling::g_input_keyboard.KEY_R.isHeld())
	{
		setFrame(m_startFrame);
		doFullUpdate = true;
	}
	else if (!controls->isPaused)
	{
		m_currFrame += 30 * delta;
		// Set this to the max for now
		if (m_currFrame >= m_endFrame)
			m_currFrame = m_endFrame;
	}

	g_gameState.setGlobalStates();

	m_camera.setLights(m_currFrame, controls->useLights);

	if (doFullUpdate)
	{
		for (size_t i = 0; i < m_modelSetups.size(); ++i)
		{
			if (!m_XGentries[i].m_isClone)
				m_XGentries[i].m_xg->resetInstanceCount();

			m_modelSetups[i]->updateMatrix(m_currFrame);
			m_XGentries[i].m_dropShadow = m_modelSetups[i]->animate(m_currFrame);
		}

		if (m_shadowPtr)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_shadowVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_modelMatrices.size() * sizeof(glm::mat4), m_modelMatrices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		for (auto& texAnim : m_texAnimations)
			texAnim.substitute(m_currFrame);

		m_sprites.update(m_currFrame);
		glfwSetWindowTitle(m_window, (m_filename + " | Frame: " + std::to_string(m_currFrame)).c_str());
	}

	if (!controls->hasFreeMovement)
	{
		m_view = m_camera.getViewMatrix(m_currFrame);
		m_projection = m_camera.getProjectionMatrix(m_currFrame);
	}
	else
	{
		m_view = m_controlledCamera->getViewMatrix();
		m_projection = glm::perspective(glm::radians(m_controlledCamera->m_fov), float(s_screenWidth) / s_screenHeight, 1.0f, 1800000.0f);
	}
}

void SSQ::drawOpaques()
{
	if (m_skyPtr && InputHandling::g_input_keyboard.KEY_K.isPressed())
		m_doSkyBackground = !m_doSkyBackground;

	glDisable(GL_BLEND);
	if (!m_skyPtr || !m_doSkyBackground)
	{
		const glm::vec3 color = m_camera.getClearColor(m_currFrame);
		glClearColor(color.r, color.g, color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	// Draw the sky as the background
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_FALSE);

		glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
		glBindVertexArray(m_skyVAO);

		g_shaderList.m_skyShader.use();

		glActiveTexture(GL_TEXTURE0);
		m_skyPtr->m_data->bindTexture();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
	}

	// Draw opaque meshes
	draw(false);
}

void SSQ::drawTranparents()
{
	// Draw transparent meshes
	// Does not draw normals
	glEnable(GL_BLEND);
	draw(true);
}

void SSQ::draw(const bool doTransparents)
{
	for (size_t i = 0; i < m_modelSetups.size(); ++i)
		if (!m_XGentries[i].m_isClone && m_XGentries[i].m_xg->getInstanceCount())
			m_XGentries[i].m_xg->draw(m_viewerControls->showNormals && !doTransparents, doTransparents);

	if (m_shadowPtr &&
		(doTransparents || m_viewerControls->showNormals))
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_shadowVBO);
		glBindVertexArray(m_shadowVAO);

		if (!doTransparents)
			g_shaderList.m_shadowShaders.m_normals.use();
		else
		{
			g_shaderList.m_shadowShaders.m_base.use();
			glActiveTexture(GL_TEXTURE0);
			m_shadowPtr->m_data->bindTexture();
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
		}

		for (size_t i = 0; i < m_XGentries.size(); ++i)
			if (m_XGentries[i].m_dropShadow)
				glDrawArrays(GL_POINTS, i, 1);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	if (m_sprites.hasSprites())
	{
		static const std::string textures[] =
		{
			"textures[0]", "textures[1]", "textures[2]", "textures[3]", "textures[4]", "textures[5]", "textures[6]", "textures[7]",
		};

		g_shaderList.m_spriteShaders.m_base.use();
		for (size_t i = 0; i < m_IMXentries.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + int(i));
			m_IMXentries[i].m_imxPtr->m_data->bindTexture();
			Shader::setInt(textures[i], int(i));
		}

		m_sprites.draw(doTransparents);

		if (!doTransparents && m_viewerControls->showNormals)
		{
			g_shaderList.m_spriteShaders.m_normals.use();
			m_sprites.draw(doTransparents);
		}
	}
}
