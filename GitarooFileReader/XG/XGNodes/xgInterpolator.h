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
};
