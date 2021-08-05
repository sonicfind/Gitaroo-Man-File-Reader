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
#include "Timeline.h"
#include "XG/XG.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct Slope
{
	float m_slope = 1;
	float m_recip = 1;
};
extern Slope g_BPM;

class Animator
{
	XG* m_xg;
	size_t m_animIndex;
	bool m_loop;
	float m_startTime;
	float m_coefficient;
	Slope m_localSlope;
	Slope* m_currSlope;
	unsigned long m_startingKeyframe;
	unsigned long m_endingKeyframe;
	unsigned long m_keyFrame;
	bool m_paused;
	bool m_isActive;

	static float s_tempo;

public:
	Timeline m_timeline;
	operator bool() const { return m_isActive; }
	void load(XG* xg);
	float getCoefficient() { return m_coefficient; }
	void setStartTime(const float time);
	void setCurrentAnimation(float time, float coefficient = 0);
	void changeAnimation(size_t index, bool loop, float time, float coefficient = 0);
	void update(const float time);
	void Interpolate();
	void staticFrame();
	void staticMatrix(glm::vec3& trn, glm::quat& rot, glm::vec3& scl, const xgBgMatrix* matrix);
	void mixFrames(const unsigned long next);
	void mixMatrix(glm::vec3& trn, glm::quat& rot, glm::vec3& scl, const xgBgMatrix* matrix, const unsigned long next);
	glm::vec3 mixVec3(float* first, float* second);
	glm::quat mixQuat(float* first, float* second);

	static float getBPM() { return s_tempo; }
	static bool setBPM();
};
