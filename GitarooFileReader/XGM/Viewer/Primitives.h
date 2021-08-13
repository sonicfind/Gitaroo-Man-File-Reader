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
#include "XG/XG_Nodes.h"
#include <glad/glad.h>

class Primitives
{
protected:
	const GLenum m_mode;
	GLsizei m_numPrimitives = 0;
	GLsizei* m_vertCounts = nullptr;

public:
	Primitives(GLenum mode) : m_mode(mode) {}
	~Primitives();
	operator bool();
	virtual void set(const xgDagMesh::Data& data) = 0;
	virtual void draw() = 0;
};

class TriElements : public Primitives
{
	GLuint m_EBO = 0;
	unsigned long** m_indices = nullptr;

public:
	TriElements(GLenum mode) : Primitives(mode) {}
	~TriElements();
	void set(const xgDagMesh::Data& data);
	void draw();
};

class TriArrays : public Primitives
{
	GLint* m_indices = nullptr;

public:
	TriArrays(GLenum mode) : Primitives(mode) {}
	~TriArrays();
	void set(const xgDagMesh::Data& data);
	void draw();
};
