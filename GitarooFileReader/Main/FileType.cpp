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
#include "pch.h"
#include "FileType.h"

FileType::FileType(const char* extension)
	: m_extension(extension) {}


FileType::FileType(std::string filename, const char* extension)
	: m_extension(extension)
	, m_saved(2)
{
	size_t pos = filename.find_last_of('\\');
	if (pos != std::string::npos)
	{
		m_directory = filename.substr(0, pos);
		m_filename = filename.substr(pos + 1);
	}
	else
		m_filename = filename;

	GlobalFunctions::banner(" Loading " + m_filename + m_extension + ' ');

	if (fopen_s(&m_filePtr, (filename + m_extension).c_str(), "rb"))
		throw "Error: " + filename + m_extension + " does not exist.";
}

bool FileType::create(std::string filename)
{
	size_t pos = filename.find_last_of('\\');
	GlobalFunctions::banner(" Saving " + filename.substr(pos != std::string::npos ? pos + 1 : 0) + m_extension + ' ');
	if (fopen_s(&m_filePtr, (filename + m_extension).c_str(), "wb"))
		// Add printf later
		return false;
	else
		return true;
}

bool FileType::write_to_txt(FILE*& txtFile)
{
	GlobalFunctions::banner(" Writing " + m_filename + '_' + m_extension.substr(1) + ".txt ");
	return !fopen_s(&txtFile, (m_directory + m_filename + '_' + m_extension.substr(1) + ".txt").c_str(), "w");
}

bool FileType::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile)
{
	write_to_txt(txtFile);
	return !fopen_s(&simpleTxtFile, (m_directory + m_filename + '_' + m_extension.substr(1) + "_SIMPLIFIED.txt").c_str(), "w")
		|| txtFile != nullptr;
}
