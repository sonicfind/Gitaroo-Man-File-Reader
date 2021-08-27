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
#include "IMX_Data.h"
class IMX
	: public FileType
{
	friend class XGM;

	char m_filepath[257] = { 0 };
	char m_name[17] = { 0 };
	unsigned long m_textureIndex;
	unsigned long m_fileSize;
	unsigned long m_non_model;
	unsigned long m_unk;
	char m_junk[12] = { 0 };

public:
	bool m_fromXGM;
	std::unique_ptr<IMX_Data> m_data;

	IMX(FILE* inFile, const std::string& directory);
	IMX(std::string filename, bool useBanner = true);
	void create(FILE* outFile, unsigned long& sizes);
	bool create(std::string filename, bool trueSave = true);

	bool write_to_txt();
	void read(std::string filename);
	bool exportPNG();
	bool importPNG();
	//Returns name C-string (size: 16)
	char* getName() { return m_name; }

	bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension);
	bool functionSelection(const char choice, bool isMulti);
	static void displayMultiChoices();
	static void displayMultiHelp();
	static const std::string multiChoiceString;

private:
	void write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, size_t& sizes);

public:
	void generateTextureBuffer();
	void deleteTextureBuffer();
};
