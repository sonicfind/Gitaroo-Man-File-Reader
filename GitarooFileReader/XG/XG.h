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
#include "Animation.h"
#include "XG_Data.h"
class XG
	: public FileType
{
	friend class XGM;
	friend class Model;

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
	XG(FILE* inFile, const std::string& directory, std::vector<IMX>& textures);
	XG(std::string filename, bool useBanner = true);
	XG(const XG&) = default;
	void create(FILE* outFile);
	bool create(std::string filename, bool trueSave = true);

	bool write_to_txt();
	bool write_to_obj(std::string newDirectory = "");
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
	// Constructs all the vertex and uniform buffers for use in the OpenGL viewer
	void initializeViewerState();
	// Deletes all the vertex and uniform buffers created for the OpenGL viewer
	void uninitializeViewerState();
	// Returns the total duration of the chosen animation in seconds 
	float getAnimationLength(size_t index);
	// Sets all vertex and bone matrix values to their defaults
	void restPose() const;
	// Updates all data to the current frame
	void animate(float frame, size_t index);
	// Draws all vertex data to the current framebuffer
	void draw(const glm::mat4 view, const bool showNormals, const bool doTransparents, const bool isAnimated = true) const;
};
