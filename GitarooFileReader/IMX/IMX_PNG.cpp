#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020 Gitaroo Pals; 2019, 2020 boringhexi (for use of the python exe's)
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
#include <filesystem>
#include "IMX_PNG.h"

bool IMX_PNG::exportPNG(IMX& image)
{
	std::string file = image.m_directory + image.m_shortname + ".IMX";

	// If the IMX came from XGM file, we need to create a separate IMX file for gm-imx2png.exe to use.
	//
	// One of the places where having direct usage of the python code would make the process simpler as there would be no need
	// to create a whole new IMX file.
	if (image.m_fromXGM)
		image.create(file, false);
	
	// Additonally, direct usage of the python code would optimize these writes to the console
	printf("%s", g_global.tabs.c_str());
	if (image.m_directory.length())
		system(("gm-imx2png -d " + image.m_directory + " -v " + file).c_str());
	else
		system(("gm-imx2png -v " + file).c_str());

	if (image.m_fromXGM)
		std::remove(file.c_str());
	return true;
}

bool IMX_PNG::exportPNG(XGM& pack)
{
	std::string dir = pack.m_name.substr(0, pack.m_name.length() - 4);

	// Creates a new directory with the name of the XGM as the base (if one doesn't already exist)
	// All the PNGs will be placed in this directory
	std::filesystem::create_directory(dir);
	// Unlike the single IMX version, we do not use the -v "verbose" command.
	// Doing so would've made it impossible to add the global tab string to each line created from the exe
	// +1 to the list of reasons to wanting the native python code intertwinned
	std::string cmd = "gm-imx2png -d \"" + dir + "\" ";
	for (IMX& texture : pack.m_textures)
	{
		std::string file = dir + "\\XGM_" + texture.m_name;
		texture.create(file, false);
		cmd += '\"' + file + "\" ";
	}
	system(cmd.c_str());

	for (IMX& texture : pack.m_textures)
	{
		std::string png = "XGM_";
		png += texture.m_name;
		png.erase(png.length() - 3, 3);
		png += "PNG";
		printf("%sExported %-16s to %s\\%s\n", g_global.tabs.c_str(), texture.m_name, pack.m_shortname.c_str(), png.c_str());
		std::remove((dir + "\\XGM_" + texture.m_name).c_str());
	}
	return true;
}

bool IMX_PNG::importPNG(IMX& image)
{
	const std::string initialName = image.m_directory + image.m_shortname + ".PNG";
	std::string pngName;
	bool found = false;
	for (const char* prefix : s_prefixes)
	{
		for (const char* suffix : s_suffixes)
		{
			if (std::filesystem::exists(image.m_directory + prefix + image.m_shortname + suffix + ".PNG"))
			{
				pngName = image.m_directory + prefix + image.m_shortname + suffix + ".PNG";
				found = true;
			}
		}
	}
	
	if (!found)
	{
		do
		{
			printf("%sProvide the name of the .PNG file you wish to import (Or 'Q' to exit): ", g_global.tabs.c_str());
			pngName.clear();
			switch (GlobalFunctions::stringInsertion(pngName))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::Success:
				if (pngName.find(".PNG") == std::string::npos && pngName.find(".png") == std::string::npos)
					pngName += ".PNG";
				if (std::filesystem::exists(pngName))
					g_global.quit = true;
				else
				{
					size_t pos = pngName.find_last_of('\\');
					if (pos != std::string::npos)
						printf("%s\"%s\" is not a valid file of extension \".PNG\"\n", g_global.tabs.c_str(), pngName.substr(pos + 1).c_str());
					else
						printf("%s\"%s\" is not a valid file of extension \".PNG\"\n", g_global.tabs.c_str(), pngName.c_str());
				}
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}

	bool removePNG = initialName.compare(pngName) == 0;
	// As to NOT overwrite an already in-use IMX file
	//
	// +1 to the list of reasons to integrate the python code
	//
	// Having to deal with file overwrites instead of being able to provide and recieve
	// texture data directly through code is a pain.
	if (removePNG)
		std::filesystem::copy_file(initialName, pngName.insert(pngName.length() - 4, "-Buffer"));

	size_t slash = pngName.find_last_of('\\');
	printf("%s", g_global.tabs.c_str());
	if (slash != std::string::npos)
		system(("gm-png2imx -d \"" + pngName.substr(0, slash) + "\" -v \"" + pngName + '\"').c_str());
	else
		system(("gm-png2imx -v \"" + pngName).c_str() + '\"');

	if (removePNG)
		std::remove(pngName.c_str());
	image.read(pngName.erase(pngName.length() - 3, 3) + "IMX");
	std::remove((pngName + "IMX").c_str());
	return true;
}

bool IMX_PNG::importPNG(XGM& pack)
{
	std::string dir = pack.m_name.substr(0, pack.m_name.length() - 4);
	std::vector<std::string> imxFiles;
	if (!std::filesystem::exists(dir))
		std::filesystem::create_directory(dir);

	std::string cmd = "gm-png2imx -d \"" + dir + "\" ";
	bool convert = false;
	bool end = false;
	for (IMX& texture : pack.m_textures)
	{
		std::string file(texture.m_name);
		file.erase(file.length() - 4, 4);

		bool found = false;
		for (const char* prefix : s_prefixes)
		{
			for (const char* suffix : s_suffixes)
			{
				if (std::filesystem::exists(dir + prefix + file + suffix + ".PNG"))
				{
					file = dir + prefix + file + suffix;
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
				GlobalFunctions::banner(" Texture: " + std::string(texture.m_name) + " ");
				printf("%sProvide the name of the .PNG file you wish to import\n", g_global.tabs.c_str());
				printf("%sType only 'N' to skip this texture", g_global.tabs.c_str());
				if (imxFiles.size())
					printf(" || Type only 'S' to skip all remaining textures\n");
				else
					printf("\n");
				printf("%sType only 'Q' to quit this process altogether", g_global.tabs.c_str());
				printf("%sInput: ", g_global.tabs.c_str());
				file = "";
				switch (GlobalFunctions::stringInsertion(file, specials))
				{
				case GlobalFunctions::ResultType::Quit:
					return false;
				case GlobalFunctions::ResultType::SpecialCase:
					if (g_global.answer.character == 'n')
						selected = false;
					else
						end = true;
					g_global.quit = true;
					break;
				case GlobalFunctions::ResultType::Success:
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
				printf("%sImported %-16s from %s\n", g_global.tabs.c_str(), pack.m_textures[i].m_name, shortname.c_str());
				pack.m_textures[i].read(file);
				std::remove(file.c_str());
				pack.m_saved = 0;
			}
		}
		return true;
	}
	return false;
}
