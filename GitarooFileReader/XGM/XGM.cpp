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
#include "Global_Functions.h"
#include "XGM.h"
#include <filesystem>
using namespace std;
using namespace GlobalFunctions;

XGM::XGM() : FileType(".XGM") {}

XGM::XGM(std::string filename, bool useBanner)
	: FileType(filename, ".XGM", useBanner)
{
	unsigned long numT, numM;
	if (fread(&numT, 4, 1, m_filePtr) != 1 || fread(&numM, 4, 1, m_filePtr) != 1)
		throw "Error: " + m_filename + " is not of sufficient size.";

	for (size_t t = 0; t < numT; t++)
		m_textures.emplace_back(m_filePtr, m_directory);

	for (size_t m = 0; m < numM; m++)
	{
		m_models.emplace_back(m_filePtr, m_directory);
		if (!m_models.back().m_saved)
			m_saved = false;
	}
	fclose(m_filePtr);
}

//Create or update a XGM file
bool XGM::create(string filename, bool trueSave)
{
	if (FileType::create(filename, true))
	{
		unsigned long sizes = (unsigned long)m_textures.size();
		fwrite(&sizes, 4, 1, m_filePtr);
		sizes = (unsigned long)m_models.size();
		fwrite(&sizes, 4, 1, m_filePtr);

		sizes = 0;
		for (IMX& imx : m_textures)
		{
			imx.create(m_filePtr, sizes);
			fflush(m_filePtr);
		}

		for (XG& xg : m_models)
		{
			xg.create(m_filePtr);
			fflush(m_filePtr);
		}

		fclose(m_filePtr);

		if (trueSave)
			m_saved = true;
		return true;
	}
	return false;
}

bool XGM::write_to_txt()
{
	FILE* txtFile, * simpleTxtFile;
	if (FileType::write_to_txt(txtFile, simpleTxtFile))
	{
		dualvfprintf_s(txtFile, simpleTxtFile, "# of Textures: %zu\n", m_textures.size());
		dualvfprintf_s(txtFile, simpleTxtFile, "  # of Models: %zu\n", m_models.size());
		dualvfprintf_s(txtFile, simpleTxtFile, "    Textures (.IMX):\n");
		for (size_t index = 0, sizes = 0; index < m_textures.size(); index++)
		{
			dualvfprintf_s(txtFile, simpleTxtFile, "\t   Texture %03zu - %s:\n", index + 1, m_textures[index].getName());
			m_textures[index].write_to_txt(txtFile, simpleTxtFile, sizes);
			fflush(txtFile);
			fflush(simpleTxtFile);
		}

		dualvfprintf_s(txtFile, simpleTxtFile, "    Models (.XG):\n");
		for (size_t index = 0; index < m_models.size(); index++)
		{
			dualvfprintf_s(txtFile, simpleTxtFile, "\t   Model %03lu - %s:\n", index + 1, m_models[index].getName(), 17);
			m_models[index].write_to_txt(txtFile, simpleTxtFile);

			// object "txtFile" already flushed
			fflush(simpleTxtFile);
		}

		fclose(txtFile);
		fclose(simpleTxtFile);
		return true;
	}
	return false;
}

bool XGM::selectTexture()
{
	while (true)
	{
		banner(" " + m_filename + ".XGM - Texture Selection ");
		switch (indexSelector(m_textures, "texture"))
		{
		case ResultType::Success:
		{
			++g_global;
			size_t index = g_global.answer.index;
			m_textures[index].menu(false, std::pair<bool, const char*>(false, ""));
			if (!m_textures[index].m_saved)
				m_saved = false;
			--g_global;
			break;
		}
		case ResultType::Quit:
			return true;
		}
	}
}

bool XGM::selectModel()
{
	while (true)
	{
		banner(" " + m_filename + ".XGM - Model Selection ");
		switch (indexSelector(m_models, "model"))
		{
		case ResultType::Success:
		{
			++g_global;
			size_t index = g_global.answer.index;
			m_models[index].menu(false, std::pair<bool, const char*>(false, ""));
			if (!m_models[index].m_saved)
				m_saved = false;
			--g_global;
			break;
		}
		case ResultType::Quit:
			return true;
		}
	}
}
