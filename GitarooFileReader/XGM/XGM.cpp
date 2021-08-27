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
#include "Viewer/Viewer.h"
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
		m_models.emplace_back(m_filePtr, m_directory, m_textures);

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

bool XGM::exportPNGs()
{
	banner(" " + m_filename + ".XGM - Multi-Texture Export ");
	const std::string folder = m_directory + m_filename;

	// Creates a new directory with the name of the XGM as the base (if one doesn't already exist)
	// All the PNGs will be placed in this directory
	std::filesystem::create_directory(folder);

	// Unlike the single IMX version, we do not use the -v "verbose" command.
	// Doing so would've made it impossible to add the global tab string to each line created from the exe
	// +1 to the list of reasons to wanting the native python code intertwinned
	std::string cmd = "gm-imx2png -d \"" + folder + "\" ";
	for (IMX& texture : m_textures)
	{
		std::string file = folder + '\\' + texture.getName();
		texture.create(file, false);
		cmd += '\"' + file + "\" ";
	}
	system(cmd.c_str());

	for (IMX& texture : m_textures)
	{
		std::string png = texture.getName();
		png.erase(png.length() - 3, 3);
		png += "PNG";
		printf("%sExported %-16s to %s\\%s\n", g_global.tabs.c_str(), texture.getName(), m_filename.c_str(), png.c_str());
		std::remove((folder + '\\' + texture.getName()).c_str());
	}
	return true;
}

bool XGM::importPNGs()
{
	banner(" " + m_filename + ".XGM - Multi-Texture Import ");
	const std::string folder = m_directory + m_filename;

	std::vector<std::string> imxFiles;
	std::filesystem::create_directory(folder);

	std::string cmd = "gm-png2imx -d \"" + folder + "\" ";
	bool convert = false;
	bool end = false;
	for (IMX& texture : m_textures)
	{
		std::string file(texture.getName());
		file.erase(file.length() - 4, 4);

		bool found = false;
		for (const char* prefix : { "\\XGM_", "\\" })
		{
			for (const char* suffix : { "", ".i4", ".i8", ".i24", ".i32" })
			{
				if (std::filesystem::exists(folder + prefix + file + suffix + ".PNG"))
				{
					file = folder + prefix + file + suffix;
					found = true;
				}
			}
		}

		bool selected = true;
		if (!found)
		{
			std::string specials = "n";
			if (imxFiles.size())
				specials += 's';
			do
			{
				banner(" Texture: " + std::string(texture.getName()) + " ");
				printf("%sProvide the name of the .PNG file you wish to import\n", g_global.tabs.c_str());
				printf("%sType only 'N' to skip this texture", g_global.tabs.c_str());
				if (imxFiles.size())
					printf(" || Type only 'S' to skip all remaining textures\n");
				else
					printf("\n");

				printf("%sType only 'Q' to quit this process altogether", g_global.tabs.c_str());
				printf("%sInput: ", g_global.tabs.c_str());
				file = "";
				switch (stringInsertion(file, specials))
				{
				case ResultType::Quit:
					return false;
				case ResultType::SpecialCase:
					if (g_global.answer.character == 'n')
						selected = false;
					else
						end = true;
					g_global.quit = true;
					break;
				case ResultType::Success:
					if (file.find(".PNG") != std::string::npos || file.find(".png") != std::string::npos)
						file.erase(file.length() - 4, 4);

					if (std::filesystem::exists(file + ".PNG"))
						g_global.quit = true;
					else
					{
						size_t pos = file.find_last_of('\\');
						if (pos != std::string::npos)
							printf("%s\"%s\" is not a valid file of extension \".PNG\"\n", g_global.tabs.c_str(), file.substr(pos + 1).c_str());
						else
							printf("%s\"%s\" is not a valid file of extension \".PNG\"\n", g_global.tabs.c_str(), file.c_str());
					}
				}
			} while (!g_global.quit);
			g_global.quit = false;
		}

		if (selected)
		{
			convert = true;
			imxFiles.emplace_back(file + ".IMX");
			cmd += '\"' + file + ".PNG\" ";
		}
		else if (!end)
			imxFiles.emplace_back();
		else
			break;
	}

	if (convert)
	{
		system(cmd.c_str());

		for (size_t i = 0; i < imxFiles.size(); ++i)
		{
			if (imxFiles[i].length())
			{
				std::string file = imxFiles[i];
				size_t pos = file.find_last_of('\\');
				std::string shortname = file.substr(pos != std::string::npos ? pos + 1 : 0);
				printf("%sImported %-16s from %s\n", g_global.tabs.c_str(), m_textures[i].getName(), shortname.c_str());
				m_textures[i].read(file);
				std::remove(file.c_str());
				m_saved = 0;
			}
		}
		return true;
	}
	return false;
}

bool XGM::exportOBJs()
{
	banner(" " + m_filename + ".XGM - Multi-Model Export ");
	const std::string folder = m_directory + m_filename;

	// Creates a new directory with the name of the XGM as the base (if one doesn't already exist)
	// All the PNGs will be placed in this directory
	std::filesystem::create_directory(folder);
	for (XG& model : m_models)
	{
		if (model.write_to_obj(folder + '\\'))
		{
			std::string obj = model.getName();
			obj.erase(obj.length() - 2, 2);
			obj += "OBJ";
			printf("%sExported %-16s to %s\\%s\n", g_global.tabs.c_str(), model.getName(), m_filename.c_str(), obj.c_str());
		}
	}
	return true;
}

bool XGM::importOBJs()
{
	banner(" " + m_filename + ".XGM - Multi-Model Import ");
	return false;
}

bool XGM::viewModel()
{
	std::vector<size_t> sectionIndexes;
	while (true)
	{
		banner(" " + m_filename + ".XGM - Model Viewer ");
		printf_tab("Type the number for each model that you wish to look at w/ spaces in-between\n");
		for (size_t index = 0; index < m_models.size(); ++index)
			printf_tab("%zu - %s\n", index, m_models[index].getName());

		if (sectionIndexes.size())
		{
			printf("%sCurrent List: ", g_global.tabs.c_str());
			for (const size_t index : sectionIndexes)
				printf("%s ", m_models[index].getName());
			putchar('\n');
		}

		printf_tab("B - Change BPM for tempo-base animations: %g\n", Animation::getTempo());
		printf_tab("A - Switch aspect ratio: %s\n", Viewer::getAspectRatioString().c_str());
		printf_tab("H - Change viewer resolution, Height: %u\n", Viewer::getScreenHeight());
		printf_tab("? - Show list of controls\n");
		switch (insertIndexValues(sectionIndexes, "bah", m_models.size(), false))
		{
		case ResultType::Help:
			printf_tab("\n");
			printf_tab("WASD - Move\n");
			printf_tab("Space/Left Shift - Ascend/Descend\n");
			printf_tab("Up/Down - Increase/Decrease movement speed\n");
			printf_tab("Mouse - Camera Aiming\n");
			printf_tab("Mouse Scroll - Increase/Decrease zoom\n");
			printf_tab("ESC - exit\n");
			printf_tab("'M' - Release/Reattach mouse control from/to the window\n");
			printf_tab("'N' - Toggle displaying vertex normal vectors\n");
			printf_tab("'O' - Switch between Animation & Pose modes\n");
			printf_tab("'L' - Toggle animation looping\n");
			printf_tab("\n");
			printf_tab("With Animation mode active:\n");
			printf_tab("\tP - Pause/Play\n");
			printf_tab("\tR (Press) - Reset current animation to frame 0\n");
			printf_tab("\tR (Hold) - Reset to first animation\n");
			printf_tab("\n");
			printf_tab("Press 'Enter' when you're done reading\n");
			printf_tab("");
			clearIn();
			testForMulti();
			break;
		case ResultType::Quit:
			return true;
		case ResultType::SpecialCase:
			testForMulti();
			switch (g_global.answer.character)
			{
			case 'b':
				if (Animation::setTempo())
					return true;
				break;
			case 'a':
				Viewer::switchAspectRatio();
				break;
			case 'h':
				if (Viewer::changeHeight())
					return true;
			}
			break;
		case ResultType::Success:
			if (sectionIndexes.size())
			{
				try
				{
					Viewer viewer(this, sectionIndexes);
					viewer.viewXG();
				}
				catch (char* str)
				{
					printf("%s", str);
				}
				sectionIndexes.clear();
			}
		}
	}
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
