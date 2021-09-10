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
#include "XG/Vertex Types/Vertex.h"
namespace Interpolation
{
	struct vec3Strings { static const char* singular;  static const char* plural; };
	struct quatStrings { static const char* singular;  static const char* plural; };
	struct vertStrings { static const char* singular;  static const char* plural; };
	struct normStrings { static const char* singular;  static const char* plural; };
	struct texStrings { static const char* singular;  static const char* plural; };
	struct shapeStrings { static const char* singular;  static const char* plural; };

	void write_to_txt(FILE* txtFile, const glm::vec2);
	void write_to_txt(FILE* txtFile, const glm::vec3);
	void write_to_txt(FILE* txtFile, const glm::quat);

	template<typename T, typename S>
	void write_to_txt_vect(FILE* txtFile, const std::vector<std::vector<T>>& vect, const char* tabs)
	{
		fprintf_s(txtFile, "\t\t%s     # of Keys: %zu\n", tabs, vect.size());
		for (size_t i = 0; i < vect.size(); ++i)
		{
			const auto& key = vect[i];
			fprintf_s(txtFile, "\t\t\t%s   Key %zu:\n", tabs, i + 1);
			fprintf_s(txtFile, "\t\t\t%s     # of %s: %zu\n", tabs, S::plural, key.size());
			for (size_t index = 0; index < key.size(); ++index)
			{
				fprintf_s(txtFile, "\t\t\t\t%s   %s %zu: ", tabs, S::singular, index + 1);
				write_to_txt(txtFile, key[index]);
			}
		}
	}

	template<typename T, typename S>
	void write_to_txt(FILE* txtFile, const std::vector<T>& vect, const char* tabs)
	{
		fprintf_s(txtFile, "\t\t%s     # of %s: %zu\n", tabs, S::plural, vect.size());
		for (size_t index = 0; index < vect.size(); ++index)
		{
			fprintf_s(txtFile, "\t\t\t%s   %s %zu: ", tabs, S::singular, index + 1);
			write_to_txt(txtFile, vect[index]);
		}
	}

	template<>
	void write_to_txt<VertexList, shapeStrings>(FILE* txtFile, const std::vector<VertexList>& vect, const char* tabs);

	template<>
	void write_to_txt<std::vector<glm::vec2>, texStrings>(FILE* txtFile, const std::vector<std::vector<glm::vec2>>& vect, const char* tabs);

	template<>
	void write_to_txt<std::vector<glm::vec3>, vertStrings>(FILE* txtFile, const std::vector<std::vector<glm::vec3>>& vect, const char* tabs);

	template<>
	void write_to_txt<std::vector<glm::vec3>, normStrings>(FILE* txtFile, const std::vector<std::vector<glm::vec3>>& vect, const char* tabs);

	template <typename T>
	T mix(const T& a, const T& b, const float coefficient)
	{
		return glm::mix(a, b, coefficient);
	}

	template<>
	glm::quat mix(const glm::quat& a, const glm::quat& b, const float coefficient);

	template<>
	VertexList mix(const VertexList& a, const VertexList& b, const float coefficient);

	template <typename T>
	std::vector<T> mix(const std::vector<T>& a, const std::vector<T>& b, const float coefficient)
	{
		std::vector<T> mixed(a);
		for (size_t i = 0; i < a.size(); ++i)
			mixed[i] = glm::mix(a[i], b[i], coefficient);
		return mixed;
	}
}

template <class T, class S>
class xgInterpolator : public XGNode
{
protected:
	unsigned long m_type;
	std::vector<T> m_keys;
	SharedNode<xgTime> m_inputTime;

	size_t m_frame = 0;
	size_t m_next = 1;
	float m_coefficient = 0;

public:
	using XGNode::XGNode;
	unsigned long read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
	{
		read_preKey(inFile);
		read_keys(inFile);
		return read_postKey(inFile, nodeList);
	}

	void create(FILE* outFile) const
	{
		create_preKey(outFile);
		create_keys(outFile);
		create_postKey(outFile);
	}

	void write_to_txt(FILE* txtFile, const char* tabs = "") const
	{
		write_preKey_to_txt(txtFile, tabs);
		Interpolation::write_to_txt<T, S>(txtFile, m_keys, tabs);
		write_postKey_to_txt(txtFile, tabs);
	}

	const auto interpolate()
	{
		setFrame();
		return mix();
	}
protected:
	virtual void setFrame()
	{
		m_frame = m_inputTime->getFrame();
		m_next = m_frame + 1;
		m_coefficient = m_inputTime->getCoefficient();
	}

	virtual const T mix() const
	{
		if (m_type && m_next < m_keys.size())
			return Interpolation::mix(m_keys[m_frame], m_keys[m_next], m_coefficient);
		else
			return m_keys[m_frame];
	}


	virtual void read_preKey(FILE* inFile)
	{
		PString::pull(inFile);
		fread(&m_type, 4, 1, inFile);
	}

	virtual void create_preKey(FILE* outFile) const
	{
		XGNode::create(outFile);
		PString::push('{', outFile);
		PString::push("type", outFile);
		fwrite(&m_type, 4, 1, outFile);
	}

	virtual void write_preKey_to_txt(FILE* txtFile, const char* tabs = "") const
	{
		XGNode::write_to_txt(txtFile, tabs);
		fprintf_s(txtFile, "\t\t%s Interpolation: %s (%lu)\n", tabs, m_type ? "TRUE" : "FALSE", m_type);
	}

	virtual void read_keys(FILE* inFile)
	{
		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_keys.reserve(size);
		m_keys.resize(size);
		fread(m_keys.data(), sizeof(T), size, inFile);
	}

	virtual void create_keys(FILE* outFile) const
	{
		PString::push("keys", outFile);
		unsigned long size = (unsigned long)m_keys.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_keys.data(), sizeof(T), size, outFile);
	}

	virtual unsigned long read_postKey(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
	{
		unsigned long sizechange = 0;
		PString test(inFile);
		while (!strchr(test.m_pstring, '}'))
		{
			// Removes duplicate xgTime node pointers
			if (m_inputTime)
				sizechange += 22 + m_inputTime->getName().m_size;
			m_inputTime.fill(inFile, nodeList);
			PString::pull(inFile);
			test.fill(inFile);
		}
		return sizechange;
	}

	virtual void create_postKey(FILE* outFile) const
	{
		PString::push("inputTime", outFile);
		m_inputTime->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}

	virtual void write_postKey_to_txt(FILE* txtFile, const char* tabs = "") const
	{
		fprintf_s(txtFile, "\t\t%s    Input Time: %s\n", tabs, m_inputTime->getName().m_pstring);
	}

	{
	}
};



template <class T, class S>
class xgListInterpolator : public virtual xgInterpolator<std::vector<T>, S>
{
public:
	using xgInterpolator<std::vector<T>, S>::xgInterpolator;

protected:
	void read_keys(FILE* inFile)
	{
		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		this->m_keys.reserve(size);
		this->m_keys.resize(size);
		for (auto& key : this->m_keys)
		{
			fread(&size, 4, 1, inFile);
			key.reserve(size);
			key.resize(size);
			fread(key.data(), sizeof(T), size, inFile);
		}
	}

	void create_keys(FILE* outFile) const
	{
		PString::push("keys", outFile);
		unsigned long size = (unsigned long)this->m_keys.size();
		fwrite(&size, 4, 1, outFile);
		for (auto& key : this->m_keys)
		{
			size = (unsigned long)key.size();
			fwrite(&size, 4, 1, outFile);
			fwrite(key.data(), sizeof(T), size, outFile);
		}
	}
};

template <class T, class S>
class xgTimedInterpolator : public virtual xgInterpolator<T, S>
{
public:
	using xgInterpolator<T, S>::xgInterpolator;

protected:
	std::vector<float> m_times;

	void setFrame()
	{
		const size_t index = this->m_inputTime->getFrame();
		const float coefficient = this->m_inputTime->getCoefficient();
		if (index + 1 < m_times.size())
		{
			// If the value given by m_times[index + 1] is less than m_times[index],
			// it can be assumed that the interpolator had enough frames to utilize every key.
			// Therefore, the values in m_times should not have any decimals.
			if (m_times[index + 1] - m_times[index] == 1.0f || m_times[index + 1] < m_times[index])
			{
				this->m_frame = (size_t)m_times[index];
				this->m_next = (size_t)m_times[index + 1];
				this->m_coefficient = coefficient;
			}
			else
			{
				float time = m_times[index] + (m_times[index + 1] - m_times[index]) * coefficient;
				this->m_frame = (size_t)time;
				this->m_next = this->m_frame + 1;
				this->m_coefficient = time - this->m_frame;
			}
		}
		else
		{
			this->m_frame = (size_t)m_times.back();
			this->m_next = this->m_frame + 1;
			this->m_coefficient = m_times.back() - this->m_frame;
		}
	}

	void read_preKey(FILE* inFile)
	{
		xgInterpolator<T, S>::read_preKey(inFile);
		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_times.reserve(size);
		m_times.resize(size);
		fread(m_times.data(), sizeof(float), size, inFile);
	}

	void create_preKey(FILE* outFile) const
	{
		xgInterpolator<T, S>::create_preKey(outFile);
		PString::push("times", outFile);
		unsigned long size = (unsigned long)m_times.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_times.data(), sizeof(float), size, outFile);
	}

	void write_preKey_to_txt(FILE* txtFile, const char* tabs = "") const
	{
		xgInterpolator<T, S>::write_preKey_to_txt(txtFile, tabs);
		fprintf_s(txtFile, "\t\t%s    # of Times: %zu\n", tabs, m_times.size());
		for (unsigned long index = 0; index < m_times.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s     Time %lu: %g\n", tabs, index + 1, m_times[index]);
	}
};

template <class T, class S>
class xgTargetedInterpolator : public xgListInterpolator<T, S>
{
public:
	using xgListInterpolator<T, S>::xgListInterpolator;

protected:
	std::vector<unsigned long> m_targets;
	const std::vector<T> mix() const
	{
		std::vector<T> mixed = xgListInterpolator<T, S>::mix();
		std::vector<T> finalMix = mixed;
		for (unsigned long targetIndex = 0; targetIndex < m_targets.size(); ++targetIndex)
			finalMix[m_targets[targetIndex]] = mixed[targetIndex];
		return finalMix;
	}

	unsigned long read_postKey(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
	{
		PString::pull(inFile);
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_targets.reserve(size);
		m_targets.resize(size);
		fread(m_targets.data(), sizeof(unsigned long), size, inFile);
		return xgListInterpolator<T, S>::read_postKey(inFile, nodeList);
	}

	void create_postKey(FILE* outFile) const
	{
		PString::push("targets", outFile);
		unsigned long size = (unsigned long)m_targets.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_targets.data(), sizeof(float), size, outFile);
		xgListInterpolator<T, S>::create_postKey(outFile);
	}

	void write_postKey_to_txt(FILE* txtFile, const char* tabs = "") const
	{
		fprintf_s(txtFile, "\t\t%s  # of Targets: %zu\n", tabs, m_targets.size());
		for (unsigned long index = 0; index < m_targets.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s   Target %lu: %lu\n", tabs, index + 1, m_targets[index]);
		xgListInterpolator<T, S>::write_postKey_to_txt(txtFile, tabs);
	}
};
