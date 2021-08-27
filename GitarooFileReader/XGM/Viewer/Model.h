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
#include "XG/XG.h"
class Model
{
	XG* m_xg;
	size_t m_animIndex;
	float m_currAnimStartTime;
	// The length of the animation in seconds
	float m_length;
	static bool s_isLooping;

public:
	Model(XG* xg);
	~Model();
	// Sets all vertex and bone matrix values to their defaults
	void restPose();
	// Checks whether to proceed to the next animation (or to loop if that flag is set)
	// Then updates the model data
	void update(float time);
	// Sets the handler to the provided animation index
	void setAnimation(float time, size_t animIndex);
	// Skip to next animation
	void nextAnimation(float time, bool forced = false);
	// Skip back to the previoes animation
	void prevAnimation(float time, bool forced = false);
	// Resets the current animation
	void setStartTime(float time);
	// Jumps to the first animation
	void reset();
	// This is one isn't obvious at all
	static void toggleLoop();
	// Draws all vertex data to the current framebuffer
	void draw(const glm::mat4 view, const bool showNormals, const bool doTransparents) const;
};
