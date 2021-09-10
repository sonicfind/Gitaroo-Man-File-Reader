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
#include "xgBgMatrix.h"
#include <glm/gtc/type_ptr.hpp>
unsigned long xgBgMatrix::read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(glm::value_ptr(m_position), sizeof(m_position), 1, inFile);

	PString::pull(inFile);
	fread(glm::value_ptr(m_rotation), sizeof(m_rotation), 1, inFile);

	PString::pull(inFile);
	fread(glm::value_ptr(m_scale), sizeof(m_scale), 1, inFile);

	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputPosition"))
			m_inputPosition.fill(inFile, nodeList);
		else if (strstr(test.m_pstring, "inputRotation"))
			m_inputRotation.fill(inFile, nodeList);
		else if (strstr(test.m_pstring, "inputScale"))
			m_inputScale.fill(inFile, nodeList);
		else
			m_inputParentMatrix.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}

void xgBgMatrix::create(FILE* outFile) const
{
	XGNode::create(outFile);

	PString::push('{', outFile);
	PString::push("position", outFile);
	fwrite(glm::value_ptr(m_position), sizeof(m_position), 1, outFile);

	PString::push("rotation", outFile);
	fwrite(glm::value_ptr(m_rotation), sizeof(m_rotation), 1, outFile);

	PString::push("scale", outFile);
	fwrite(glm::value_ptr(m_scale), sizeof(m_scale), 1, outFile);

	if (m_inputPosition)
	{
		PString::push("inputPosition", outFile);
		m_inputPosition->push(outFile);
		PString::push("outputVec3", outFile);
	}

	if (m_inputRotation)
	{
		PString::push("inputRotation", outFile);
		m_inputRotation->push(outFile);
		PString::push("outputQuat", outFile);
	}

	if (m_inputScale)
	{
		PString::push("inputScale", outFile);
		m_inputScale->push(outFile);
		PString::push("outputVec3", outFile);
	}

	if (m_inputParentMatrix)
	{
		PString::push("inputParentMatrix", outFile);
		m_inputParentMatrix->push(outFile);
		PString::push("outputMatrix", outFile);
	}

	PString::push('}', outFile);
}

void xgBgMatrix::write_to_txt(FILE* txtFile, const char* tabs) const
{
	XGNode::write_to_txt(txtFile, tabs);

	fprintf_s(txtFile, "\t\t\t%s Grid Position (XYZ): %g, %g, %g\n", tabs, m_position[0], m_position[1], m_position[2]);
	fprintf_s(txtFile, "\t\t\t%sGrid Rotation (XYZW): %g, %g, %g, %g\n", tabs, m_rotation[0], m_rotation[1], m_rotation[2], m_rotation[3]);
	fprintf_s(txtFile, "\t\t\t%s    Grid Scale (XYZ): %g, %g, %g\n", tabs, m_scale[0], m_scale[1], m_scale[2]);
	if (m_inputPosition)
		fprintf_s(txtFile, "\t\t%s     Position overwritten by: %s\n", tabs, m_inputPosition->getName().m_pstring);
	if (m_inputRotation)
		fprintf_s(txtFile, "\t\t%s     Rotation overwritten by: %s\n", tabs, m_inputRotation->getName().m_pstring);
	if (m_inputScale)
		fprintf_s(txtFile, "\t\t\t%sScale overwritten by: %s\n", tabs, m_inputScale->getName().m_pstring);
	if (m_inputParentMatrix)
		fprintf_s(txtFile, "\t\t\t%s Parent Input Matrix: %s\n", tabs, m_inputParentMatrix->getName().m_pstring);
}

const size_t xgBgMatrix::getSize() const
{
	size_t size = XGNode::getSize()
		+ PSTRING_LEN_VAR("position", m_position)
		+ PSTRING_LEN_VAR("rotation", m_position)
		+ PSTRING_LEN_VAR("scale", m_position);

	if (m_inputPosition)
	{
		size += PSTRING_LEN("inputPosition") + PSTRING_LEN("outputVec3")
			+ m_inputPosition->getName().getSize();
	}

	if (m_inputRotation)
	{
		size += PSTRING_LEN("inputRotation") + PSTRING_LEN("outputQuat")
			+ m_inputRotation->getName().getSize();
	}

	if (m_inputScale)
	{
		size += PSTRING_LEN("inputScale") + PSTRING_LEN("outputVec3")
			+ m_inputScale->getName().getSize();
	}

	if (m_inputParentMatrix)
	{
		size += PSTRING_LEN("inputParentMatrix") + PSTRING_LEN("outputMatrix")
			+ m_inputParentMatrix->getName().getSize();
	}

	return size;
}

void xgBgMatrix::applyTransformations(glm::vec3& pos, glm::quat& rot, glm::vec3& scl)
{
	if (m_inputParentMatrix)
		m_inputParentMatrix->applyTransformations(pos, rot, scl);

	if (m_inputPosition)
		pos += m_inputPosition->interpolate();
	else
		pos += m_position;

	if (m_inputRotation)
		rot *= m_inputRotation->interpolate();
	else
		rot *= m_rotation;

	if (m_inputScale)
		scl *= m_inputScale->interpolate();
	else
		scl *= m_scale;
}
