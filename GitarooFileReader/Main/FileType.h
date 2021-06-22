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

	bool m_saved = false;
public:
	// Directory where the file is located
	std::string m_directory;
	// The name of the file
	std::string m_filename;
	// Extension string
	// Should go unused outside of FileType parent functions
	const std::string m_extension;

	FileType(const char* extension);
	FileType(std::string filename, const char* extension);
	FileType(const FileType&) = default;
	bool getSaveStatus() const { return m_saved; }
	virtual bool write_to_txt() = 0;
	virtual bool create(std::string filename) = 0;
	virtual void edit(const bool multi = false) = 0;

protected:
	bool write_to_txt(FILE*& txtFile);
	bool write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile);
};
