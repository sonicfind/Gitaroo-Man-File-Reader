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
#include "Model.h"
#include "Global_Functions.h"
Model::Model(XG* xg)
	: m_xg(xg)
	, m_animIndex(0)
	, m_currAnimStartTime(0)
	, m_length(xg->getAnimationLength(m_animIndex))
{
	m_xg->initializeViewerState();
	s_isLooping = false;
}

Model::~Model()
{
	m_xg->uninitializeViewerState();
}

// Sets all vertex and bone matrix values to their defaults
void Model::restPose()
{
	m_xg->restPose();
}

// Checks whether to proceed to the next animation (or to loop if that flag is set)
// Then updates the model data
void Model::update(float time)
{
	while (time >= m_length + m_currAnimStartTime)
	{
		if (s_isLooping)
		{
			m_currAnimStartTime += m_length;
			GlobalFunctions::printf_tab("Loop\n");
		}
		else
			nextAnimation(m_currAnimStartTime + m_length);
	}

	m_xg->animate(time - m_currAnimStartTime, m_animIndex);
}

// Sets the handler to the provided animation index
void Model::setAnimation(float time, size_t animIndex)
{
	m_currAnimStartTime = time;
	if (m_animIndex != animIndex)
	{
		m_animIndex = animIndex;
		m_length = m_xg->getAnimationLength(m_animIndex);
		GlobalFunctions::printf_tab("Animation: %zu\n", m_animIndex);
	}
}

// Skip to next animation
void Model::nextAnimation(float time, bool forced)
{
	if (m_animIndex < m_xg->m_animations.size() - 1)
		setAnimation(time, m_animIndex + 1);
	else
		setAnimation(time, 0);
	if (forced)
		m_xg->animate(0, m_animIndex);
}

// Skip back to the previoes animation
void Model::prevAnimation(float time, bool forced)
{
	if (m_animIndex > 0)
		setAnimation(time, m_animIndex - 1);
	else
		setAnimation(time, m_xg->m_animations.size() - 1);
	if (forced)
		m_xg->animate(0, m_animIndex);
}

// Resets the current animation
void Model::setStartTime(float time)
{
	m_currAnimStartTime = time;
	m_xg->animate(0, m_animIndex);
}

// Jumps to the first animation
void Model::reset()
{
	setAnimation(0, 0);
	m_xg->animate(0, 0);
}

// This is one isn't obvious at all
void Model::toggleLoop()
{
	s_isLooping = !s_isLooping;
}

// Draws all vertex data to the current framebuffer
void Model::draw(const glm::mat4 view, const bool showNormals, const bool doTransparents) const
{
	m_xg->draw(view, showNormals, doTransparents);
}