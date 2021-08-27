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
template <class T>
class xgTimedInterpolator : public xgInterpolator<ListType<T>>
{
protected:
	std::vector<float> m_times;

public:
	xgTimedInterpolator(const PString& name)
		: xgInterpolator<ListType<T>>(name) {}

	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
	{
		xgInterpolator<ListType<T>>::read(inFile, nodeList);

		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_times.reserve(size);
		m_times.resize(size);
		fread(m_times.data(), 4, size, inFile);

		PString::pull(inFile);
		fread(&size, 4, 1, inFile);
		this->m_keys.reserve(size);
		this->m_keys.resize(size);
		for (auto& key : this->m_keys)
			key.read(inFile);
		return 0;
	}

	void create(FILE* outFile, bool full) const
	{
		xgInterpolator<ListType<T>>::create(outFile, full);

		PString::push("times", outFile);
		unsigned long size = (unsigned long)m_times.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_times.data(), 4, size, outFile);

		PString::push("keys", outFile);
		size = (unsigned long)this->m_keys.size();
		fwrite(&size, 4, 1, outFile);
		for (const auto& key : this->m_keys)
			key.create(outFile);
	}

	void write_to_txt(FILE* txtFile, const char* tabs = "")
	{
		xgInterpolator<ListType<T>>::write_to_txt(txtFile, tabs);
		fprintf_s(txtFile, "\t\t%s    # of Times: %zu\n", tabs, m_times.size());
		for (unsigned long index = 0; index < m_times.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s     Time %lu: %g\n", tabs, index + 1, m_times[index]);

		fprintf_s(txtFile, "\t\t%s     # of Keys: %zu\n", tabs, this->m_keys.size());
		for (size_t index = 0; index < this->m_keys.size(); ++index)
		{
			fprintf_s(txtFile, "\t\t\t%s   Key %zu:\n", tabs, index + 1);
			Interpolation::write_to_txt(txtFile, this->m_keys[index], tabs);
		}
	}

	const char* getType() { return "xgTimedInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgTimedInterpolator", str.m_pstring) == 0; }
};
