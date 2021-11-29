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
#include "Camera_Setup.h"
void CameraSetup::read(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMPX"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMPX' Tag (Camera) at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_size, 4, 1, inFile);
	fread(m_unk, 1, 8, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(&m_baseGlobalValues, sizeof(BaseGlobalValues), 1, inFile);

	unsigned long numPositions, numRotations;
	fread(&numPositions, 4, 1, inFile);
	fread(&numRotations, 4, 1, inFile);

	m_positions.fill(numPositions, inFile);
	for (auto& pos : m_positions.m_vect)
		pos.position.z *= -1;

	m_rotations.fill(numRotations, inFile);

	unsigned long numSettings;
	fread(&numSettings, 4, 1, inFile);
	if (numSettings > 1)
		m_projections.fill(numSettings, inFile);

	unsigned long numAmbientColors;
	fread(&numAmbientColors, 4, 1, inFile);
	if (numAmbientColors > 1)
		m_ambientColors.fill(numAmbientColors, inFile);

	unsigned long numlights;
	fread(&numlights, 4, 1, inFile);

	for (unsigned long i = 0; i < numlights; ++i)
		m_lights.emplace_back(inFile);

	if (m_headerVersion >= 0x1200)
	{
		unsigned long unk3;
		fread(&unk3, 4, 1, inFile);
		if (unk3 > 1)
			m_64bytes_v.fill(unk3, inFile);
	}
}

void CameraSetup::create(FILE* outFile)
{
	fprintf(outFile, "GMPX");
	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_size, 4, 1, outFile);
	fwrite(m_unk, 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_baseGlobalValues, sizeof(BaseGlobalValues), 1, outFile);

	unsigned long numPositions = (unsigned long)m_positions.size();
	unsigned long numRotations = (unsigned long)m_rotations.size();

	fwrite(&numPositions, 4, 1, outFile);
	fwrite(&numRotations, 4, 1, outFile);
	std::vector<Position> tmp = m_positions.m_vect;
	for (auto& pos : tmp)
		pos.position.z *= -1;
	fwrite(tmp.data(), sizeof(Position), numPositions, outFile);
	m_rotations.write(outFile);

	m_projections.write_conditioned(outFile);
	m_ambientColors.write_conditioned(outFile);

	unsigned long size = (unsigned long)m_lights.size();
	fwrite(&size, 4, 1, outFile);

	for (auto& light : m_lights)
		light.create(outFile);

	if (m_headerVersion >= 0x1200)
		m_64bytes_v.write_conditioned(outFile);
}

float CameraSetup::getLastFrame() const
{
	return m_positions.back().frame;
}

#include "Viewer/Shaders/Shaders.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
void CameraSetup::generateBuffers(float aspectRatio)
{
	m_viewerAspectRatio = 3 * aspectRatio / 4;
	// Fills both light and sprite UBOs
	glGenBuffers(2, &m_lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, 304, NULL, GL_DYNAMIC_DRAW);

	g_shaderList.m_baseShaders.m_base.bindUniformBlock(3, "Lights");
	g_shaderList.m_boneShaders.m_base.bindUniformBlock(3, "Lights");
	g_shaderList.m_lightShader.bindUniformBlock(3, "Lights");

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_lightUBO);
	unsigned long numLights = (unsigned long)m_lights.size();
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &numLights);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &m_baseGlobalValues.coefficient);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &m_baseGlobalValues.useDiffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(glm::vec3(m_baseGlobalValues.vertColorDiffuse) / 255.0f));
	glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec3), glm::value_ptr(glm::vec3(m_baseGlobalValues.baseAmbience) / 255.0f));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CameraSetup::deleteBuffers()
{
	// Deletes both light and sprite UBOs
	glDeleteBuffers(2, &m_lightUBO);
}

glm::vec3 CameraSetup::getClearColor(const float frame)
{
	// Temporary as I am so sure that something must control the clear color mid-stage
	return m_baseGlobalValues.clearColor;
}

glm::mat4 CameraSetup::getProjectionMatrix(const float frame)
{
	if (m_projections.empty())
		return glm::perspective(glm::radians(m_baseGlobalValues.fov),
								m_baseGlobalValues.aspectRatio * m_viewerAspectRatio,
								m_baseGlobalValues.zNear,
								m_baseGlobalValues.zFar);
	else
	{
		const auto iter = m_projections.update(frame);
		if (!iter->doInterpolation || iter + 1 == m_projections.end())
			return glm::perspective(glm::radians(iter->fov),
									iter->aspectRatio * m_viewerAspectRatio,
									iter->zNear,
									iter->zFar);
		else
		{
			const float coefficient = (frame - iter->frame) * iter->coefficient;
			const auto next = iter + 1;
			return glm::perspective(glm::radians(mix(iter->fov, next->fov, coefficient)),
									mix(iter->aspectRatio, next->aspectRatio, coefficient) * m_viewerAspectRatio,
									mix(iter->zNear, next->zNear, coefficient),
									mix(iter->zFar, next->zFar, coefficient));
		}
	}
}

glm::mat4 CameraSetup::getViewMatrix(const float frame)
{
	const auto posIter = m_positions.update(frame);
	glm::vec3 position;
	if (!posIter->doInterpolation || posIter + 1 == m_positions.end())
		position = posIter->position;
	else
		position = glm::mix(posIter->position, (posIter + 1)->position, (frame - posIter->frame) * posIter->coefficient);

	const auto rotIter = m_rotations.update(frame);
	glm::quat rotation;
	if (!rotIter->doInterpolation || rotIter + 1 == m_rotations.end())
		rotation = rotIter->rotation;
	else
		rotation = glm::slerp(rotIter->rotation, (rotIter + 1)->rotation, (frame - rotIter->frame) * rotIter->coefficient);

	return glm::toMat4(rotation) * glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

glm::vec3 CameraSetup::getAmbientColor(const float frame)
{
	if (m_ambientColors.empty())
		return glm::vec3(m_baseGlobalValues.baseAmbience) / 255.0f;
	else
	{
		const auto iter = m_ambientColors.update(frame);
		if (!iter->doInterpolation || iter + 1 == m_ambientColors.end())
			return iter->color;
		else
			return glm::mix(iter->color, (iter + 1)->color, (frame - iter->frame) * iter->coefficient);
	}
}

void CameraSetup::setLights(const float frame, const unsigned int doLights)
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(unsigned int), &doLights);
	if (doLights)
	{
		// Scene Ambience
		glBufferSubData(GL_UNIFORM_BUFFER, 32, 12, glm::value_ptr(getAmbientColor(frame)));

		std::vector<LightSetup::LightForBuffer> lightStructs;
		for (auto& light : m_lights)
		{
			// It is very possible that there is more to lights than just these
			// Further testing will be required
			lightStructs.push_back(light.getLight(frame));
		}
		// All lights
		glBufferSubData(GL_UNIFORM_BUFFER, 48, sizeof(LightSetup::LightForBuffer) * lightStructs.size(), lightStructs.data());
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
