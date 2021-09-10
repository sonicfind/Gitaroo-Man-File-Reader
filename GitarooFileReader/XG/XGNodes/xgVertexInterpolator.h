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
#include "xgInterpolator.h"
class xgVertexInterpolator
	: public xgTargetedInterpolator<glm::vec3, Interpolation::vertStrings>
	, public xgTimedInterpolator<std::vector<glm::vec3>, Interpolation::vertStrings>
{
public:
	xgVertexInterpolator(const PString& type, const PString& name)
		: xgTargetedInterpolator(type, name)
		, xgTimedInterpolator(type, name)
		, xgInterpolator(type, name) {}
	static bool compareType(const PString& str) { return strcmp("xgVertexInterpolator", str.m_pstring) == 0; }
#pragma warning(suppress : 4250)
};
