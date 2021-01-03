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
#include "XGM.h"
#include <filesystem>

using namespace std;
XGM::XGM() : m_saved(1) {}

XGM::XGM(std::string filename) : m_saved(2), m_name(filename + ".XGM")
{
	size_t pos = filename.find_last_of('\\');
	m_shortname = filename.substr(pos != std::string::npos ? pos + 1 : 0);
	GlobalFunctions::banner(" Loading " + m_shortname + ".XGM ");
	FILE* inFile;
	if (fopen_s(&inFile, m_name.c_str(), "rb") || inFile == nullptr)
		throw "Error: " + m_name + " could not be located.";

	unsigned long numT, numM;
	if (fread(&numT, 4, 1, inFile) != 1 || fread(&numM, 4, 1, inFile) != 1)
		throw "Error: " + m_name + " is not of sufficient size.";

	const string directory = pos != std::string::npos ? filename.substr(0, pos + 1) : "";
	for (size_t t = 0; t < numT; t++)
		m_textures.emplace_back(inFile, directory);
	for (size_t m = 0; m < numM; m++)
		m_models.emplace_back(inFile, directory);
	fclose(inFile);
}

//Create or update a XGM file
void XGM::create(string filename)
{
	{
		size_t pos = filename.find_last_of('\\');
		GlobalFunctions::banner(" Saving " + filename.substr(pos != string::npos ? pos + 1 : 0) + ' ');
	}
	FILE* outFile = nullptr;
	fopen_s(&outFile, filename.c_str(), "wb");
	if (outFile == nullptr)
		throw "Error: " + filename + " could not be created.";

	unsigned long sizes = (unsigned long)m_textures.size();
	fwrite(&sizes, 4, 1, outFile);
	sizes = (unsigned long)m_models.size();
	fwrite(&sizes, 4, 1, outFile);

	sizes = 0;
	for (IMX& imx : m_textures)
	{
		imx.create(outFile, sizes);
		fflush(outFile);
	}

	for (XG& xg : m_models)
	{
		xg.create(outFile);
		fflush(outFile);
	}
	fclose(outFile);
	m_saved = 1;
}

bool XGM::exportPNGs()
{
	std::string dir = m_name.substr(0, m_name.length() - 4);

	// Creates a new directory with the name of the XGM as the base (if one doesn't already exist)
	// All the PNGs will be placed in this directory
	std::filesystem::create_directory(dir);
	// Unlike the single IMX version, we do not use the -v "verbose" command.
	// Doing so would've made it impossible to add the global tab string to each line created from the exe
	// +1 to the list of reasons to wanting the native python code intertwinned
	std::string cmd = "gm-imx2png -d \"" + dir + "\" ";
	for (IMX& texture : m_textures)
	{
		std::string file = dir + '\\' + texture.getName();
		texture.create(file, false);
		cmd += '\"' + file + "\" ";
	}
	system(cmd.c_str());

	for (IMX& texture : m_textures)
	{
		std::string png = texture.getName();
		png.erase(png.length() - 3, 3);
		png += "PNG";
		printf("%sExported %-16s to %s\\%s\n", g_global.tabs.c_str(), texture.getName(), m_shortname.c_str(), png.c_str());
		std::remove((dir + '\\' + texture.getName()).c_str());
	}
	return true;
}

bool XGM::importPNGs()
{
	std::string dir = m_name.substr(0, m_name.length() - 4);
	std::vector<std::string> imxFiles;
	if (!std::filesystem::exists(dir))
		std::filesystem::create_directory(dir);

	std::string cmd = "gm-png2imx -d \"" + dir + "\" ";
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
				GlobalFunctions::banner(" Texture: " + std::string(texture.getName()) + " ");
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
	std::string dir = m_name.substr(0, m_name.length() - 4);

	// Creates a new directory with the name of the XGM as the base (if one doesn't already exist)
	// All the PNGs will be placed in this directory
	std::filesystem::create_directory(dir);
	for (XG& model : m_models)
	{
		if (model.exportOBJ(dir + '\\'))
		{
			std::string obj = model.getName();
			obj.erase(obj.length() - 2, 2);
			obj += "OBJ";
			printf("%sExported %-16s to %s\\%s\n", g_global.tabs.c_str(), model.getName(), m_shortname.c_str(), obj.c_str());
		}
	}
	return true;
}

bool XGM::importOBJs()
{
	return false;
}
