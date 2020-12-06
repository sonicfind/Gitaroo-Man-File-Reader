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
#include "Global_Functions.h"
#include "XG_Nodes.h"

class XG_Data
{
	friend class XG_Main;
	friend class XGM_Main;
	friend class Editor;
	friend class XGM;
	friend class XG;
public:
	struct DagBase
	{
		std::shared_ptr<XGNode> m_base;
		std::vector<DagBase> m_connected;
		DagBase();
		DagBase(std::shared_ptr<XGNode> base);
		void create(FILE* outFile, bool braces = false);
	};
private:
	std::vector<std::shared_ptr<XGNode>> m_nodes;
	std::vector<DagBase> m_dag;
	template <typename T>
	bool addNode(PString& type, PString& name)
	{
		if (strstr(T::getType(), type.m_pstring))
		{
			m_nodes.push_back(std::make_shared<T>(name));
			return true;
		}
		return false;
	}
	template <typename T>
	bool cloneNode(std::shared_ptr<XGNode>& node)
	{
		if (std::dynamic_pointer_cast<T>(node))
		{
			m_nodes.push_back(node);
			return true;
		}
		return false;
	}
public:
	XG_Data() = default;
	XG_Data(FILE* inFile);
	XG_Data(XG_Data& xg);
	void create(FILE* outFile);
};

class XG
{
	friend class XG_Main;
	friend class XGM_Main;
	friend class Editor;
	friend class XGM;
public:
	struct Animation
	{
		float m_length = 0;
		float m_keyframe_interval = 0;
		float m_framerate = 60;
		float m_starting_keyframe = 0;
		unsigned long m_non_tempo = true;
		char m_junk[12] = { 0 };
		Animation() : m_length(0), m_keyframe_interval(0), m_framerate(60), m_starting_keyframe(0), m_non_tempo(true) {}
		Animation(FILE* inFile);
		Animation(const Animation& anim) = default;
	};
private:
	char m_filepath[257] = { 0 };
	char m_name[17] = { 0 };
	unsigned long m_modelIndex;
	unsigned long m_fileSize;
	unsigned long m_unk;
	std::vector<Animation> m_animations;
	std::shared_ptr<XG_Data> m_data;
	char m_saved;
public:
	bool m_fromXGM;
	std::string m_directory;
	std::string m_shortname;
	XG();
	XG(FILE* inFile, const std::string& directory);
	XG(std::string filename, bool useBanner = true);
	XG(const XG&) = default;
	XG& operator=(const XG& xg);
	void create(FILE* outFile);
	void create(std::string filename);
	//Returns name C-string (size: 16)
	char* getName() { return m_name; }
};
