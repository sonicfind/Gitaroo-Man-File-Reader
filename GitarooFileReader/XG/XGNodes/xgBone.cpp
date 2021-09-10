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
#include "xgBone.h"
#include <glm/gtc/type_ptr.hpp>
unsigned long xgBone::read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(glm::value_ptr(m_restMatrix), sizeof(m_restMatrix), 1, inFile);

	PString::pull(inFile);
	m_inputMatrix.fill(inFile, nodeList);

	PString::pull(inFile);
	PString::pull(inFile);
	return 0;
}

void xgBone::create(FILE* outFile) const
{
	XGNode::create(outFile);

	PString::push('{', outFile);
	PString::push("restMatrix", outFile);
	fwrite(glm::value_ptr(m_restMatrix), sizeof(m_restMatrix), 1, outFile);

	PString::push("inputMatrix", outFile);
	m_inputMatrix->push(outFile);
	PString::push("outputMatrix", outFile);

	PString::push('}', outFile);
}

void xgBone::write_to_txt(FILE* txtFile, const char* tabs) const
{
	XGNode::write_to_txt(txtFile, tabs);

	fprintf_s(txtFile, "\t\t\t%s Rest Matrix:\n", tabs);
	for (int row = 0; row < 4; ++row)
		fprintf_s(txtFile, "\t\t\t%s      Row %u: %g, %g, %g, %g\n", tabs, row + 1,
			m_restMatrix[row].x, m_restMatrix[row].y, m_restMatrix[row].z, m_restMatrix[row].w);
	fprintf_s(txtFile, "\t\t\t%sInput Matrix: %s\n", tabs, m_inputMatrix->getName().m_pstring);
}

#include <glm/gtx/transform.hpp>
glm::mat4 xgBone::getBoneMatrix() const
{
	glm::vec3 translation(0);
	glm::quat rotation(1, 0, 0, 0);
	glm::vec3 scale(1.0f);
	if (m_inputMatrix)
		m_inputMatrix->applyTransformations(translation, rotation, scale);
	
	return glm::translate(translation) * glm::toMat4(conjugate(rotation)) * glm::scale(scale) * m_restMatrix;
}
