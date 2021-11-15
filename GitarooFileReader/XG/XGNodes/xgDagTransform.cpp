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
#include "xgDagTransform.h"
#include <glm/gtx/transform.hpp>
unsigned long xgDagTransform::read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
{
	PString test(inFile);
	if (!strchr(test.m_pstring, '}'))
	{
		m_inputMatrix.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}

void xgDagTransform::create(FILE* outFile) const
{
	XGNode::create(outFile);

	PString::push('{', outFile);
	if (m_inputMatrix)
	{
		PString::push("inputMatrix", outFile);
		m_inputMatrix->push(outFile);
		PString::push("outputMatrix", outFile);
	}
	PString::push('}', outFile);
}

void xgDagTransform::write_to_txt(FILE* txtFile, const char* tabs) const
{
	XGNode::write_to_txt(txtFile, tabs);

	if (m_inputMatrix)
		fprintf_s(txtFile, "\t\t%s      Input Matrix: %s\n", tabs, m_inputMatrix->getName().m_pstring);
	else
		fprintf_s(txtFile, "\t\t%s      No Input Matrix\n", tabs);
}

const size_t xgDagTransform::getSize() const
{
	size_t size = XGNode::getSize();
	if (m_inputMatrix)
		size += PSTRING_LEN("inputMatrix") + PSTRING_LEN("outPutMatrix")
			+ m_inputMatrix->getName().getSize();
	return size;
}

glm::mat4 xgDagTransform::getModelMatrix(bool animated) const
{
	if (m_inputMatrix)
	{
		glm::vec3 translation(0.0f);
		glm::quat rotation(1, 0, 0, 0);
		glm::vec3 scale(1.0f);
		m_inputMatrix->applyTransformations(translation, rotation, scale, animated);

		glm::mat4 result = glm::toMat4(conjugate(rotation));
		result[0] *= scale.x;
		result[1] *= scale.y;
		result[2] *= scale.z;
		result[3] = glm::vec4(translation, 1);
		return result;
	}
	else
		return glm::identity<glm::mat4>();
}
