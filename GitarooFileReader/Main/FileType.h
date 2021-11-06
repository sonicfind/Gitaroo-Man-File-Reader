#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020-2021 Gitaroo Pals
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

//Holds all data pertaining to applicable file extensions.
class FileType
{
protected:
	// Pointer to use for reading and writing files of the class type to/from the class
	FILE* m_filePtr = nullptr;

	// Directory where the file is located
	std::string m_directory;
	// The name of the file
	std::string m_filename;
	bool m_saved = false;

public:
	// Extension string
	// Should go unused outside of FileType parent functions
	const std::string m_extension;
	static const std::string multiChoiceString;

	FileType(const char* extension, const bool saved = false);
	FileType(std::string filename, const char* extension, bool useBanner = true);
	FileType(const FileType&) = default;
	virtual ~FileType() = default;
	virtual bool create(std::string filename, bool trueSave = true);
	virtual bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension) = 0;
	virtual bool functionSelection(const char choice, bool isMulti) = 0;
	static void displayMultiChoices() {}
	static void displayMultiHelp() {}
	std::string getDirectory() { return m_directory; }
	std::string getFilename() { return m_filename; }
	bool checkSave(bool toMainMenu);
	bool fileSavePrompt();
	
	virtual bool write_to_txt() = 0;

protected:
	virtual bool write_to_txt(FILE*& txtFile);
	virtual bool write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
};
