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
#include "Animator.h"
#include "InputHandler.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
using namespace InputHandling;

// Base BPM of 120
Slope g_BPM = { 7.5f , 1.0f / 7.5f };
float Animator::s_tempo = 120;

bool Animator::setBPM()
{
	while (true)
	{
		GlobalFunctions::printf_tab("Current BPM: %g ['B' to leave unchanged]\n", s_tempo);
		GlobalFunctions::printf_tab("Input: ");
		switch (GlobalFunctions::valueInsert(s_tempo, false, "b"))
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::SpecialCase:
		case GlobalFunctions::ResultType::Success:
			g_BPM.m_slope = s_tempo * .0625f;
			g_BPM.m_recip = 16 / s_tempo;
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

void Animator::load(XG* xg)
{
	m_xg = xg;
	m_paused = false;
	m_isActive = true;
	changeAnimation(0, false, 0, 0);
}

void Animator::setStartTime(const float time) { m_startTime = time; }

void Animator::setCurrentAnimation(float time, float coefficient)
{
	m_keyFrame = m_startingKeyframe;
	if (!m_paused)
		m_startTime = time;
	m_coefficient = coefficient;

	staticFrame();
}

void Animator::changeAnimation(size_t index, bool loop, float time, float coefficient)
{
	m_animIndex = index;
	printf("Anim: %zu\n", m_animIndex);
	m_startingKeyframe = (unsigned long)m_xg->m_animations[m_animIndex].m_starting_keyframe;
	m_endingKeyframe = m_startingKeyframe + (unsigned long)(m_xg->m_animations[m_animIndex].m_length / m_xg->m_animations[m_animIndex].m_keyframe_interval);
	m_loop = loop;

	if (m_xg->m_animations[m_animIndex].m_non_tempo)
	{
		m_currSlope = &m_localSlope;
		m_currSlope->m_slope = m_xg->m_animations[m_animIndex].m_framerate / (2 * m_xg->m_animations[m_animIndex].m_keyframe_interval);
		m_currSlope->m_recip = 1 / m_currSlope->m_slope;
	}
	else
		m_currSlope = &g_BPM;

	setCurrentAnimation(time, coefficient);
}

void Animator::update(const float time)
{
	if (g_input_keyboard.KEY_L.isPressed())
		m_loop = !m_loop;

	if (!m_isActive)
	{
		if (g_input_keyboard.KEY_O.isPressed())
		{
			m_isActive = true;
			setCurrentAnimation(time);
		}
	}
	else if (g_input_keyboard.KEY_O.isPressed())
		m_isActive = false;
	else
	{
		if (g_input_keyboard.KEY_P.isPressed())
		{
			m_paused = !m_paused;
			m_startTime = time - m_startTime;
		}

		if (g_input_keyboard.KEY_R.isPressed() || g_input_keyboard.KEY_R.isDelayed())
			setCurrentAnimation(time);
		else if (g_input_keyboard.KEY_R.isHeld())
		{
			if (m_animIndex == 0)
				setCurrentAnimation(time);
			else
				changeAnimation(0, m_loop, time);
		}
		else if (g_input_keyboard.KEY_RIGHT.isTicked() && !g_input_keyboard.KEY_LEFT.isActive())
		{
			if (m_animIndex + 1 < m_xg->m_animations.size())
				changeAnimation(m_animIndex + 1, m_loop, time);
			else
				changeAnimation(0, m_loop, time);
		}
		else if (g_input_keyboard.KEY_LEFT.isTicked() && !g_input_keyboard.KEY_RIGHT.isActive())
		{
			if (m_animIndex > 0)
				changeAnimation(m_animIndex - 1, m_loop, time);
			else
				changeAnimation(m_xg->m_animations.size() - 1, m_loop, time);
		}
		else if (!m_paused)
		{
			while (m_currSlope->m_slope * (time - m_startTime) > 1.0f)
			{
				m_startTime += m_currSlope->m_recip;
				if (m_keyFrame + 1 >= m_endingKeyframe)
				{
					if (m_loop)
						m_keyFrame = m_startingKeyframe;
					else if (m_animIndex + 1 < m_xg->m_animations.size())
						changeAnimation(m_animIndex + 1, m_loop, m_startTime, m_coefficient);
					else
						break;
				}
				else
					++m_keyFrame;
			}

			m_coefficient = m_currSlope->m_slope * (time - m_startTime);
			Interpolate();
		}
		else if (g_input_keyboard.KEY_COMMA.isPressed())
		{
			if (m_keyFrame > m_startingKeyframe)
				--m_keyFrame;
			else
				m_keyFrame = m_endingKeyframe - 1;
			staticFrame();
		}
		else if (g_input_keyboard.KEY_PERIOD.isPressed())
		{
			if (m_keyFrame + 1 < m_endingKeyframe)
				++m_keyFrame;
			else
				m_keyFrame = m_startingKeyframe;
			staticFrame();
		}
	}
}

void Animator::Interpolate()
{
	if (m_coefficient < 0.01)
		// This frame, no mixing
		staticFrame();
	else if (m_keyFrame + 1 < m_endingKeyframe)
		// This frame mixed with next frame
		mixFrames(m_keyFrame + 1);
	else if (m_loop)
	{
		if (m_endingKeyframe - m_startingKeyframe > 1)
		{
			// This frame mixed with the first frame in the loop
			mixFrames(m_startingKeyframe);
		}
		else
			staticFrame();
	}
	else if (m_animIndex + 1 < m_xg->m_animations.size())
		// This frame mixed with the first frame of the next animation
		mixFrames((unsigned long)m_xg->m_animations[m_animIndex + 1].m_starting_keyframe);
	else
		// This frame, no mixing
		staticFrame();
}

void Animator::staticFrame()
{
	for (size_t boneIndex = 0; boneIndex < m_timeline.m_bones.size(); ++boneIndex)
	{
		const auto& bone = m_timeline.m_bones[boneIndex];
		glm::vec3 translation(0);
		glm::quat rotation(1, 0, 0, 0);
		glm::vec3 scale(1.0f);
		staticMatrix(translation, rotation, scale, bone.m_bone->m_inputMatrix.m_node);
		
		glm::mat4 tmp = glm::translate(translation) * glm::toMat4(conjugate(rotation)) * glm::scale(scale) * bone.m_rest;
		memcpy(m_timeline.m_boneMatrices[boneIndex], glm::value_ptr(tmp), 64);
		// Is this correct?? Even if it is, we need to optimize it as much as possible
	}

	for (auto& shape : m_timeline.m_shapes)
		for (unsigned long vertIndex = 0; vertIndex < shape.m_numVerts; ++vertIndex)
			memcpy(shape.m_animated.get() + vertIndex, shape.m_keys[shape.m_times[m_keyFrame]] + vertIndex, sizeof(Vertex));

	for (auto& transform : m_timeline.m_modelTransforms)
	{
		if (transform.m_transform)
		{
			glm::vec3 translation(0);
			glm::quat rotation(1, 0, 0, 0);
			glm::vec3 scale(1.0f);
			if (transform.m_transform->m_inputMatrices.size())
				staticMatrix(translation, rotation, scale, transform.m_transform->m_inputMatrices.front().m_node);

			transform.m_transformMatrix = glm::translate(translation) * glm::toMat4(conjugate(rotation)) * glm::scale(scale);
			// Is this correct?? Even if it is, we need to optimize it as much as possible
		}
		else
			transform.m_transformMatrix = glm::mat4(1.0f);
	}
}

void Animator::staticMatrix(glm::vec3& trn, glm::quat& rot, glm::vec3& scl, const xgBgMatrix* matrix)
{
	if (matrix->m_inputParentMatrix.isValid())
		staticMatrix(trn, rot, scl, matrix->m_inputParentMatrix.m_node);

	if (matrix->m_inputPosition.isValid())
		trn += glm::make_vec3(matrix->m_inputPosition->getKey(m_keyFrame));
	else
		trn += glm::make_vec3(matrix->m_position);

	if (matrix->m_inputRotation.isValid())
		rot *= glm::make_quat(matrix->m_inputRotation->getKey(m_keyFrame));
	else
		rot *= glm::make_quat(matrix->m_rotation);

	if (matrix->m_inputScale.isValid())
		scl *= glm::make_vec3(matrix->m_inputScale->getKey(m_keyFrame));
	else
		scl *= glm::make_vec3(matrix->m_scale);
}

void Animator::mixFrames(const unsigned long next)
{
	for (size_t boneIndex = 0; boneIndex < m_timeline.m_bones.size(); ++boneIndex)
	{
		const auto& bone = m_timeline.m_bones[boneIndex];
		glm::vec3 translation(0);
		glm::quat rotation(1, 0, 0, 0);
		glm::vec3 scale(1.0f);
		mixMatrix(translation, rotation, scale, bone.m_bone->m_inputMatrix.m_node, next);

		glm::mat4 tmp = glm::translate(translation) * glm::toMat4(conjugate(rotation)) * glm::scale(scale) * bone.m_rest;
		memcpy(m_timeline.m_boneMatrices[boneIndex], glm::value_ptr(tmp), 64);
		// Is this correct?? Even if it is, we need to optimize it as much as possible
	}

	for (auto& shape : m_timeline.m_shapes)
	{
		for (unsigned long vertIndex = 0; vertIndex < shape.m_numVerts; ++vertIndex)
		{
			memcpy(&shape.m_animated[vertIndex], shape.m_keys[shape.m_times[m_keyFrame]] + vertIndex, sizeof(Vertex));
			memcpy(shape.m_animated[vertIndex].nextVertex.m_position, shape.m_keys[shape.m_times[next]] + vertIndex, sizeof(Vertex));
		}
	}

	for (auto& transform : m_timeline.m_modelTransforms)
	{
		if (transform.m_transform)
		{
			glm::vec3 translation(0);
			glm::quat rotation(1, 0, 0, 0);
			glm::vec3 scale(1.0f);

			if (transform.m_transform->m_inputMatrices.size())
				mixMatrix(translation, rotation, scale, transform.m_transform->m_inputMatrices.front().m_node, next);

			transform.m_transformMatrix = glm::translate(translation) * glm::toMat4(conjugate(rotation)) * glm::scale(scale);
			// Is this correct?? Even if it is, we need to optimize it as much as possible
		}
		else
			transform.m_transformMatrix = glm::mat4(1.0f);
	}
}

void Animator::mixMatrix(glm::vec3& trn, glm::quat& rot, glm::vec3& scl, const xgBgMatrix* matrix, const unsigned long next)
{
	if (matrix->m_inputParentMatrix.isValid())
		mixMatrix(trn, rot, scl, matrix->m_inputParentMatrix.m_node, next);

	if (matrix->m_inputPosition.isValid())
		trn += mixVec3(matrix->m_inputPosition->getKey(m_keyFrame), matrix->m_inputPosition->getKey(next));
	else
		trn += glm::make_vec3(matrix->m_position);

	if (matrix->m_inputRotation.isValid())
		rot *= mixQuat(matrix->m_inputRotation->getKey(m_keyFrame), matrix->m_inputRotation->getKey(next));
	else
		rot *= glm::make_quat(matrix->m_rotation);

	if (matrix->m_inputScale.isValid())
		scl *= mixVec3(matrix->m_inputScale->getKey(m_keyFrame), matrix->m_inputScale->getKey(next));
	else
		scl *= glm::make_vec3(matrix->m_scale);
}

glm::vec3 Animator::mixVec3(float* first, float* second)
{
	return glm::mix(glm::make_vec3(first), glm::make_vec3(second), m_coefficient);
}

glm::quat Animator::mixQuat(float* first, float* second)
{
	return  glm::slerp(glm::make_quat(first), glm::make_quat(second), m_coefficient);
}