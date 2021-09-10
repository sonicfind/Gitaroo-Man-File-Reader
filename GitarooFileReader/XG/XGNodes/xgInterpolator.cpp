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
#include "xgInterpolator.h"
void Interpolation::write_to_txt(FILE* txtFile, const glm::vec2 vec)
{
	fprintf_s(txtFile, "%g, %g\n", vec.s, vec.t);
}

void Interpolation::write_to_txt(FILE* txtFile, const glm::vec3 vec)
{
	fprintf_s(txtFile, "%g, %g, %g\n", vec.x, vec.y, vec.z);
}

void Interpolation::write_to_txt(FILE* txtFile, const glm::quat qua)
{
	fprintf_s(txtFile, "%g, %g\n", qua.x, qua.y);
}

template<>
void Interpolation::write_to_txt<VertexList, Interpolation::shapeStrings>(FILE* txtFile, const std::vector<VertexList>& vect, const char* tabs)
{
	fprintf_s(txtFile, "\t\t%s     # of %s: %zu\n", tabs, Interpolation::shapeStrings::plural, vect.size());
	for (size_t i = 0; i < vect.size(); ++i)
	{
		fprintf_s(txtFile, "\t\t\t%s   %s %zu:\n", tabs, Interpolation::shapeStrings::singular, i + 1);
		vect[i].write_to_txt(txtFile, "\t\t\t", tabs);
	}
}

template<>
void Interpolation::write_to_txt<std::vector<glm::vec2>, Interpolation::texStrings>(FILE* txtFile, const std::vector<std::vector<glm::vec2>>& vect, const char* tabs)
{
	write_to_txt_vect<glm::vec2, texStrings>(txtFile, vect, tabs);
}

template<>
void Interpolation::write_to_txt<std::vector<glm::vec3>, Interpolation::vertStrings>(FILE* txtFile, const std::vector<std::vector<glm::vec3>>& vect, const char* tabs)
{
	write_to_txt_vect<glm::vec3, vertStrings>(txtFile, vect, tabs);
}

template<>
void Interpolation::write_to_txt<std::vector<glm::vec3>, Interpolation::normStrings>(FILE* txtFile, const std::vector<std::vector<glm::vec3>>& vect, const char* tabs)
{
	write_to_txt_vect<glm::vec3, normStrings>(txtFile, vect, tabs);
}

template<>
glm::quat Interpolation::mix(const glm::quat& a, const glm::quat& b, const float coefficient)
{
	return glm::slerp(a, b, coefficient);
}

template<>
VertexList Interpolation::mix(const VertexList& a, const VertexList& b, const float coefficient)
{
	return VertexList(a, b, coefficient);
}

const char* Interpolation::vec3Strings::singular = "Vec3";
const char* Interpolation::vec3Strings::plural = "Vec3s";
const char* Interpolation::quatStrings::singular = "Quat";
const char* Interpolation::quatStrings::plural = "Quats";
const char* Interpolation::vertStrings::singular = "Position";
const char* Interpolation::vertStrings::plural = "Positions";
const char* Interpolation::normStrings::singular = "Normal";
const char* Interpolation::normStrings::plural = "Normals";
const char* Interpolation::texStrings::singular = "TexCoord";
const char* Interpolation::texStrings::plural = "TexCoords";
const char* Interpolation::shapeStrings::singular = "Geometry";
const char* Interpolation::shapeStrings::plural = "Geometries";
