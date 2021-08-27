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
#include "xgTime.h"
#include <glm/gtx/quaternion.hpp>
#include "XG/Vertex Types/Vertex.h"
namespace Interpolation
{
	void write_to_txt(FILE* txtFile, const glm::vec2);
	void write_to_txt(FILE* txtFile, const glm::vec3);
	void write_to_txt(FILE* txtFile, const glm::quat);

	template<typename T>
	void write_to_txt(FILE* txtFile, const ListType<T>& vector, const char* tabs = "")
	{
		fprintf_s(txtFile, "\t\t\t%s      # of Elements: %zu\n", tabs, vector.m_values.size());
		for (unsigned long index = 0; index < vector.m_values.size(); ++index)
		{
			fprintf_s(txtFile, "\t\t\t\t%s       %lu: ", tabs, index + 1);
			write_to_txt(txtFile, vector.m_values[index]);
		}
	}
	void write_to_txt(FILE* txtFile, const ListType<Vertex>&, const char* tabs = "");
	glm::vec2 mix(const glm::vec2, const glm::vec2, const float coefficient);
	glm::vec3 mix(const glm::vec3, const glm::vec3, const float coefficient);
	glm::quat mix(const glm::quat, const glm::quat, const float coefficient);

	template<typename T>
	ListType<T> mix(const ListType<T>& vect_1, const ListType<T>& vect_2, const float coefficient)
	{
		ListType<T> mixedVect = vect_1;
		for (size_t i = 0; i < mixedVect.m_values.size(); ++i)
			mixedVect.m_values[i] = glm::mix(vect_1.m_values[i], vect_2.m_values[i], coefficient);
		return mixedVect;
	}
	ListType<Vertex> mix(const ListType<Vertex>&, const ListType<Vertex>&, const float coefficient);
}

template <class T>
class xgInterpolator : public XGNode
{
protected:
	unsigned long m_type = 0;
	std::vector<T> m_keys;
	SharedNode<xgTime> m_inputTime;

public:
	using XGNode::XGNode;
	virtual unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
	{
		PString::pull(inFile);
		fread(&m_type, 4, 1, inFile);
		return 0;
	}

	virtual void create(FILE* outFile, bool full) const
	{
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
	}

	virtual void write_to_txt(FILE* txtFile, const char* tabs = "")
	{
		fprintf_s(txtFile, "\t\t%s Interpolation: %s (%lu)\n", tabs, m_type ? "TRUE" : "FALSE", m_type);
		fprintf_s(txtFile, "\t\t%s    Input Time: %s\n", tabs, m_inputTime->getName().m_pstring);
	}

	const char* getType() { return "xgInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgInterpolator", str.m_pstring) == 0; }
	T interpolate() const
	{
		return mix(m_inputTime->getFrame(), m_inputTime->getFrame() + 1, m_inputTime->getCoefficient());
	}

protected:
	T mix(const size_t index, const size_t nextIndex, const float coefficient) const
	{
		if (m_type && nextIndex < m_keys.size())
			return Interpolation::mix(m_keys[index], m_keys[nextIndex], coefficient);
		else
			return m_keys[index];
	}
};
