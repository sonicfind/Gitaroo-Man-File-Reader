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
#include "PString\PString.h"
class XGNode;

template<typename T = XGNode>
class SharedNode;

class XGNode
{
protected:
	const PString m_nodeType;
	const PString m_name;

public:
	XGNode(const PString& type, const PString& name);
	virtual ~XGNode() = default;
	virtual unsigned long read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList) = 0;
	virtual void create(FILE* outFile) const;
	void createPrototype(FILE* outFile) const;
	void push(FILE* outFile) const;
	void write_to_simple_txt(FILE* simpleTxtFile) const;
	virtual void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	virtual const size_t getSize() const;
	const PString& getName() const;
};

template<typename T>
class SharedNode
{
	XGNode* m_node = nullptr;

public:
	SharedNode() = default;
	SharedNode(const SharedNode&) = default;
	SharedNode(XGNode* ptr) : m_node(ptr) {}
	template<typename U>
	SharedNode(const SharedNode<U>& other) : m_node(dynamic_cast<T*>(other.m_node)) {}
	SharedNode(const PString& name, const std::list<std::unique_ptr<XGNode>>& nodeList)
	{
		fill(name, nodeList);
	}

	void fill(const PString& name, const std::list<std::unique_ptr<XGNode>>& nodeList)
	{
		for (const auto& node : nodeList)
		{
			if (node->getName() == name)
			{
				m_node = node.get();
				break;
			}
		}
	}

	SharedNode& operator=(const SharedNode& other) = default;
	SharedNode& operator=(T* other)
	{
		m_node = other;
		return *this;
	}

	template<typename U>
	SharedNode& operator=(const SharedNode<U>& other)
	{
		if (T* otherNode = other.get<T>())
			m_node = otherNode;
		return *this;
	}

	template<typename U = T>
	U* get() const
	{
		return dynamic_cast<U*>(m_node);
	}

	operator bool() const
	{
		return m_node != nullptr;
	}

	T* operator->() const
	{
		return dynamic_cast<T*>(m_node);
	}

	T& operator*() const
	{
		return *dynamic_cast<T*>(m_node);
	}

	bool operator==(const SharedNode& other)
	{
		return m_node == other.m_node;
	}
};

class DagNode : public XGNode
{
public:
	using XGNode::XGNode;
};
