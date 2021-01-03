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
#include "FileType.h"
#include "XG_Nodes.h"

class XG_Data;

class XG
	: public FileType
{
	friend class XGM;
public:
	struct Animation
	{
		float m_length = 0;
		float m_keyframe_interval = 0;
		float m_framerate = 60;
		float m_starting_keyframe = 0;
		unsigned long m_non_tempo = true;
		union
		{
			char c[4];
			float f;
			unsigned long ul;
			long l;
		} m_junk[3] = { 0 };
		Animation() = default;
		Animation(FILE* inFile);
		Animation(const Animation& anim) = default;
		bool write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
	};
private:
	char m_filepath[257] = { 0 };
	char m_name[17] = { 0 };
	unsigned long m_modelIndex;
	unsigned long m_fileSize;
	unsigned long m_unk;
public:
	bool m_fromXGM;
	std::vector<Animation> m_animations;
	std::shared_ptr<XG_Data> m_data;
	XG();
	XG(FILE* inFile, const std::string& directory);
	XG(std::string filename, bool useBanner = true);
	XG(const XG&) = default;
	XG& operator=(const XG& xg);
	void create(FILE* outFile);
	bool create(std::string filename, bool trueSave = true);

	bool write_to_txt();
	bool exportOBJ(std::string newDirectory = "");
	bool importOBJ();
	//Returns name C-string (size: 16)
	char* getName() { return m_name; }

	bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension);
	bool functionSelection(const char choice, bool isMulti);
	static void displayMultiChoices();
	static void displayMultiHelp();
	static const std::string multiChoiceString;

private:
	bool write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
};

class XG_Data
{
	friend class XG;
public:
	struct DagBase
	{
		SharedNode<XGNode> m_base;
		std::vector<DagBase> m_connected;
		DagBase();
		DagBase(XGNode* base);
		void create(FILE* outFile, bool braces = false);
	};
private:
	template <typename T>
	bool addNode(PString& type, PString& name)
	{
		if (T::compare(type))
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
	std::vector<std::shared_ptr<XGNode>> m_nodes;
	std::vector<DagBase> m_dag;
	XG_Data() = default;
	XG_Data(FILE* inFile, unsigned long& filesize);
	XG_Data(XG_Data& xg);
	void create(FILE* outFile);
};
