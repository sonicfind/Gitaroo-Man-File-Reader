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

void Interpolation::write_to_txt(FILE* txtFile, const glm::quat quat)
{
	fprintf_s(txtFile, "%g, %g, %g, %g\n", quat.x, quat.y, quat.z, quat.w);
}

void Interpolation::write_to_txt(FILE* txtFile, const ListType<Vertex>& list, const char* tabs)
{
	fprintf_s(txtFile, "\t\t\t\t%sVertex Flags: %lu\n", tabs, list.m_vertexFlags);
	fprintf_s(txtFile, "\t\t\t%s       # of Vertices: %zu\n", tabs, list.m_vertices.size());
	for (unsigned long index = 0; index < list.m_vertices.size(); ++index)
	{
		fprintf_s(txtFile, "\t\t\t\t%s    Vertex %03lu\n", tabs, index + 1);
		const Vertex& vertex = list.m_vertices[index];
		// Position
		if (list.m_vertexFlags & 1)
			fprintf_s(txtFile, "\t\t\t\t\t\t%sPosition (XYZW): %g, %g, %g, %g\n", tabs, vertex.m_position.x, vertex.m_position.y, vertex.m_position.z, vertex.m_position.w);
		// Normal
		if (list.m_vertexFlags & 2)
			fprintf_s(txtFile, "\t\t\t\t\t\t%s   Normal (XYZ): %g, %g, %g\n", tabs, vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z);
		// Color
		if (list.m_vertexFlags & 4)
			fprintf_s(txtFile, "\t\t\t\t\t\t%s   Color (RGBA): %g, %g, %g, %g\n", tabs, vertex.m_color.r, vertex.m_color.g, vertex.m_color.b, vertex.m_color.a);
		// Texture Coordinate
		if (list.m_vertexFlags & 8)
			fprintf_s(txtFile, "\t\t\t\t\t%sTexture Coordinate (ST): %g, %g\n", tabs, vertex.m_texCoord.s, vertex.m_texCoord.t);
	}
}
