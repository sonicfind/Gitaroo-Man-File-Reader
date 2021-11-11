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
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 12, glm::value_ptr(glm::vec3(0)));
	// Light Specular
	glBufferSubData(GL_UNIFORM_BUFFER, 80, 12, glm::value_ptr(glm::vec3(.5)));
	unsigned long min = 0;
	glBufferSubData(GL_UNIFORM_BUFFER, 92, 4, &min);
	float coeff = 1;
	glBufferSubData(GL_UNIFORM_BUFFER, 96, 4, &coeff);
	unsigned long max = 1;
	glBufferSubData(GL_UNIFORM_BUFFER, 100, 4, &max);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
	glfwSetScrollCallback(m_window, InputHandling::scroll_callback);

	for (auto& model : ((ViewerControls_XGM*)m_viewerControls.get())->m_models)
		m_models[model.modelIndex].initializeViewerState();
}

void XGM::uninitialize()
{
	for (auto& model : ((ViewerControls_XGM*)m_viewerControls.get())->m_models)
		m_models[model.modelIndex].uninitializeViewerState();
	glDeleteBuffers(1, &m_lightUBO);
	Viewer::uninitialize();
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

	m_camera.generateBuffers(s_aspectRatio);
	for (auto& texAnim : m_texAnimations)
		texAnim.loadCuts();

	m_sprites.generateSpriteBuffers();
}

void SSQ::uninitialize()
{
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

	m_camera.deleteBuffers();
	for (auto& texAnim : m_texAnimations)
		texAnim.unloadCuts();

	m_sprites.deleteSpriteBuffers();
	g_gameState.reset();
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

#ifdef _DEBUG
	double lastFPSTime = glfwGetTime();
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
	ViewerControls_XGM* controls = (ViewerControls_XGM*)m_viewerControls.get();
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
				auto& model = controls->m_models.front();
				m_models[model.modelIndex].uninitializeViewerState();
				if (InputHandling::g_input_keyboard.KEY_PERIOD.isTicked())
				{
					if (model.modelIndex < m_models.size() - 1)
						++model.modelIndex;
					else
						model.modelIndex = 0;
				}
				else
				{
					if (model.modelIndex > 0)
						--model.modelIndex;
					else
						model.modelIndex = m_models.size() - 1;
				}
				m_models[model.modelIndex].initializeViewerState();

				if (controls->animate && !InputHandling::g_input_keyboard.KEY_O.isPressed())
				{
					model.frame = 0;
					model.animIndex = 0;
					model.length = m_models[model.modelIndex].getAnimationLength(0);
				}
				else
					m_models[model.modelIndex].restPose();
				glfwSetWindowTitle(m_window, m_models[model.modelIndex].getName());
			}
			delta = 0;
		}
	}

	// Toggling whether to play animations
	if (InputHandling::g_input_keyboard.KEY_O.isPressed())
	{
		controls->animate = !controls->animate;

		if (!controls->animate)
			for (auto& model : controls->m_models)
				m_models[model.modelIndex].restPose();
		else
		{
			for (auto& model : controls->m_models)
			{
				model.frame = 0;
				model.animIndex = 0;
				model.length = m_models[model.modelIndex].getAnimationLength(0);
				controls->isPaused = false;
			}
		}
	}

	if (controls->animate)
	{
		if (InputHandling::g_input_keyboard.KEY_P.isPressed())
			controls->isPaused = !controls->isPaused;

		if (InputHandling::g_input_keyboard.KEY_L.isPressed())
			controls->loop = !controls->loop;

		for (auto& model : controls->m_models)
		{
			if (InputHandling::g_input_keyboard.KEY_R.isActive())
			{
				model.frame = 0;
				// Reset current animation
				if (InputHandling::g_input_keyboard.KEY_R.isHeld())
				{
					model.animIndex = 0;
					model.length = m_models[model.modelIndex].getAnimationLength(model.animIndex);
				}
			}
			else if (InputHandling::g_input_keyboard.KEY_RIGHT.isActive() || InputHandling::g_input_keyboard.KEY_LEFT.isActive())
			{
				model.frame = 0;
				// Skip to the next animation
				if (InputHandling::g_input_keyboard.KEY_RIGHT.isTicked())
				{
					if (model.animIndex < m_models[model.modelIndex].m_animations.size() - 1)
						++model.animIndex;
					else
						model.animIndex = 0;
				}
				else if (InputHandling::g_input_keyboard.KEY_LEFT.isTicked())
				{
					if (model.animIndex > 0)
						--model.animIndex;
					else
						model.animIndex = m_models[model.modelIndex].m_animations.size() - 1;
				}

				model.length = m_models[model.modelIndex].getAnimationLength(model.animIndex);
			}
			else if (!controls->isPaused)
				model.frame += 30 * delta;

			if (model.length > 0)
			{
				while (model.frame >= model.length)
				{
					model.frame -= model.length;
					if (controls->loop)
						GlobalFunctions::printf_tab("Loop\n");
					else
					{
						if (model.animIndex < m_models[model.modelIndex].m_animations.size() - 1)
							++model.animIndex;
						else
							model.animIndex = 0;
						model.length = m_models[model.modelIndex].getAnimationLength(model.animIndex);
					}
				}
			}

			m_models[model.modelIndex].resetInstanceCount();
			m_models[model.modelIndex].animate(model.frame, model.animIndex, glm::identity<glm::mat4>());
		}
	}
}

void XGM::drawOpaques()
{
	const ViewerControls_XGM* controls = (ViewerControls_XGM*)m_viewerControls.get();
	// Clear color and depth buffers
	glClearColor(0.2f, 0.5f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Disable color blending
	glDisable(GL_BLEND);
	// Draw opaque meshes
	for (auto& model : controls->m_models)
		m_models[model.modelIndex].draw(controls->showNormals, false, controls->animate);
}

void XGM::drawTranparents()
{
	const ViewerControls_XGM* controls = (ViewerControls_XGM*)m_viewerControls.get();
	// Enable color blending
	glEnable(GL_BLEND);
	// Draw transparent meshes
	// Does not draw normals
	for (auto& model : controls->m_models)
		m_models[model.modelIndex].draw(false, true, controls->animate);
}

void SSQ::update(float delta)
{
	ViewerControls_SSQ* controls = (ViewerControls_SSQ*)m_viewerControls.get();
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

	if (InputHandling::g_input_keyboard.KEY_P.isPressed())
	{
		controls->isPaused = !controls->isPaused;
		if (controls->isPaused)
			GlobalFunctions::printf_tab("%g\n", m_currFrame);
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
			auto& entry = m_XGentries[i];
			XG* xg;
			if (!entry.m_isClone)
			{
				xg = entry.m_xg;
				xg->resetInstanceCount();
			}
			else
				xg = m_XGentries[entry.m_cloneID].m_xg;

			const auto result = m_modelSetups[i]->animate(m_currFrame);
			m_XGentries[i].m_dropShadow = result.first;
			m_modelMatrices[i] = result.second;
		}


		for (auto& texAnim : m_texAnimations)
			texAnim.substitute(m_currFrame);

		m_sprites.update(m_currFrame);
	}

	if (!controls->hasFreeMovement)
	{
		m_view = m_camera.getViewMatrix(m_currFrame);
		m_projection = m_camera.getProjectionMatrix(m_currFrame, s_screenWidth, s_screenHeight);
	}
	else
	{
		m_view = m_controlledCamera->getViewMatrix();
		m_projection = glm::perspective(glm::radians(m_controlledCamera->m_fov), float(s_screenWidth) / s_screenHeight, 1.0f, 1800000.0f);
	}
}

void SSQ::drawOpaques()
{
	// Clear color and depth buffers
	const glm::vec3 color = m_camera.getClearColor(m_currFrame);
	glClearColor(color.r, color.g, color.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw opaque meshes
	glDisable(GL_BLEND);
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

	// Temporary solution for blending
	// Full solution will require figuring out how it decides if a sprite a blends or not
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
