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
#include "XG_Nodes.h"
XGNode::XGNode(const PString& type, const PString& name)
	: m_nodeType(type)
	, m_name(name) {}

void XGNode::push(FILE* outFile) const
{
	m_name.push(outFile);
}

void XGNode::write_to_simple_txt(FILE* simpleTxtFile) const
{
	fprintf_s(simpleTxtFile, "%-22s: %s\n", m_nodeType.m_pstring, m_name.m_pstring);
}

void XGNode::write_to_txt(FILE* txtFile, const char* tabs) const
{
	fprintf_s(txtFile, "%s: %s\n", m_nodeType.m_pstring, m_name.m_pstring);
}

void XGNode::create(FILE* outFile) const
{
	m_nodeType.push(outFile);
	m_name.push(outFile);
}

void XGNode::createPrototype(FILE* outFile) const
{
	XGNode::create(outFile);
	PString::push(";", outFile);
}

const PString& XGNode::getName() const
{
	return m_name;
}
