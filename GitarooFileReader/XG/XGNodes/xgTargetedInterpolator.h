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
#include "xgTimedInterpolator.h"
template<typename T>
class xgTargetedInterpolator : public xgTimedInterpolator<T>
{
protected:
	std::vector<unsigned long> m_targets;

public:
	xgTargetedInterpolator(const PString& name)
		: xgTimedInterpolator<T>(name) {}

	const char* getType() { return "xgTargetedInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgTargetedInterpolator", str.m_pstring) == 0; }

	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
	{
		xgTimedInterpolator<T>::read(inFile, nodeList);

		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_targets.reserve(size);
		m_targets.resize(size);
		fread(m_targets.data(), 4, size, inFile);

		unsigned long sizechange = 0;
		PString test(inFile);
		while (!strchr(test.m_pstring, '}'))
		{
			// Removes duplicate xgTime node pointers
			if (this->m_inputTime)
				sizechange += 22 + this->m_inputTime->getName().m_size;
			this->m_inputTime.fill(inFile, nodeList);
			PString::pull(inFile);
			test.fill(inFile);
		}
		return sizechange;
	}

	void create(FILE* outFile, bool full) const
	{
		PString::push('{', outFile);
		xgTimedInterpolator<T>::create(outFile, full);

		PString::push("targets", outFile);
		unsigned long size = (unsigned long)m_targets.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_targets.data(), 4, m_targets.size(), outFile);

		PString::push("inputTime", outFile);
		this->m_inputTime->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	
	void write_to_txt(FILE* txtFile, const char* tabs = "")
	{
		xgTimedInterpolator<T>::write_to_txt(txtFile, tabs);

		fprintf_s(txtFile, "\t\t%s     # of Keys: %zu\n", tabs, this->m_keys.size());
		for (unsigned long key = 0; key < this->m_keys.size(); ++key)
		{
			fprintf_s(txtFile, "\t\t\t%s   Key %lu:\n", tabs, key + 1);
			Interpolation::write_to_txt(txtFile, this->m_keys[key]);
		}

		fprintf_s(txtFile, "\t\t%s  # of Targets: %zu\n", tabs, m_targets.size());
		for (unsigned long index = 0; index < m_targets.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s  Target %lu: %lu\n", tabs, index + 1, m_targets[index]);
	}

	ListType<T> interpolate() const
	{
		ListType<T> mixed = xgTimedInterpolator<T>::interpolate();
		ListType<T> finalMix;
		finalMix.m_values = mixed.m_values;
		for (unsigned long targetIndex = 0; targetIndex < m_targets.size(); ++targetIndex)
			finalMix.m_values[m_targets[targetIndex]] = mixed.m_values[targetIndex];
		finalMix.m_values.shrink_to_fit();
		return finalMix;
	}
};
