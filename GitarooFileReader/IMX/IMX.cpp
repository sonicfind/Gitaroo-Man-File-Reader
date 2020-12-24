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
#include "IMX.h"
#include <filesystem>
using namespace std;

IMX::IMX() : m_textureIndex(0), m_fileSize(0), m_non_model(0), m_unk(0), m_saved(1), m_fromXGM(false) {}

IMX::IMX(FILE* inFile, const string& directory) :  m_directory(directory), m_saved(2), m_fromXGM(true)
{
	fread(m_filepath, 1, 256, inFile);
	fread(m_name, 1, 16, inFile);
	m_shortname = m_name;
	m_shortname.erase(m_shortname.length() - 4, 4);
	fread(&m_textureIndex, 4, 1, inFile);
	fread(&m_fileSize, 4, 1, inFile);
	fseek(inFile, 4, SEEK_CUR); //Skip previous file sizes value
	fread(&m_non_model, 4, 1, inFile);
	fread(&m_unk, 4, 1, inFile);
	fread(m_junk, 1, 12, inFile);
	try
	{
		m_data = make_shared<IMX_Data>(inFile);
		fseek(inFile, 8, SEEK_CUR);
	}
	catch (const char* str)
	{
		fclose(inFile);
		throw str + string(m_name) + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
	catch (string str)
	{
		fclose(inFile);
		throw str + string(m_name) + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
}

IMX::IMX(string filename, bool useBanner) : m_textureIndex(0), m_non_model(false), m_unk(false), m_saved(2), m_fromXGM(false)
{
	{
		size_t pos = filename.find_last_of('\\');
		if (pos != string::npos)
		{
			m_directory = filename.substr(0, pos + 1);
			m_shortname = filename.substr(pos + 1);
		}
		else
			m_shortname = filename;
	}
	FILE* inFile = nullptr;
	if (fopen_s(&inFile, (filename + ".IMX").c_str(), "rb"))
		throw "Error: " + filename + ".IMX could not be located.";
	if (useBanner)
		GlobalFunctions::banner(" Loading " + m_shortname + ".IMX ");
	try
	{
		m_data = make_shared<IMX_Data>(inFile);
		fclose(inFile);
		m_fileSize = m_data->m_colorData->m_imageSize + 48;
		if ((!m_data->m_pixelVal1 && !m_data->m_pixelVal2) || (m_data->m_pixelVal1 == 1 && m_data->m_pixelVal2 == 1))
			m_fileSize += m_data->m_colorData->m_paletteSize + 8;
	}
	catch (const char* str)
	{
		fclose(inFile);
		throw str + m_shortname + ".IMX [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
	catch (string str)
	{
		fclose(inFile);
		throw str + m_shortname + ".IMX [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
}

IMX& IMX::operator=(IMX& imx)
{
	if (m_data != imx.m_data)
	{
		m_shortname = imx.m_shortname;
		copy(imx.m_filepath, imx.m_filepath + 256, m_filepath);
		copy(imx.m_name, imx.m_name + 16, m_name);
		m_textureIndex = imx.m_textureIndex;
		m_fileSize = imx.m_fileSize;
		m_non_model = imx.m_non_model;
		m_unk = imx.m_unk;
		m_data = imx.m_data;
		m_saved = imx.m_saved;
	}
	return *this;
}

//Create or update a IMX file
void IMX::create(FILE* outFile, unsigned long& sizes)
{
	fwrite(m_filepath, 1, 256, outFile);
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_textureIndex, 4, 1, outFile);
	fwrite(&m_fileSize, 4, 1, outFile);
	fwrite(&sizes, 4, 1, outFile);
	sizes += m_fileSize;
	fwrite(&m_non_model, 4, 1, outFile);
	fwrite(&m_unk, 4, 1, outFile);
	fwrite(m_junk, 1, 12, outFile);
	m_data->create(outFile);
	m_saved = 1;
}

//Create or update a IMX file
void IMX::create(string filename, bool trueSave)
{
	if (trueSave)
	{
		size_t pos = filename.find_last_of('\\');
		GlobalFunctions::banner(" Saving " + filename.substr(pos != string::npos ? pos + 1 : 0) + ' ');
	}
	FILE* outFile = nullptr;
	fopen_s(&outFile, filename.c_str(), "wb");
	if (outFile == nullptr)
		throw "Error: " + filename + " could not be created.";
	m_data->create(outFile);
	fclose(outFile);

	if (trueSave)
		m_saved = 1;
}

void IMX::read(string filename)
{
	FILE* inFile = nullptr;
	if (fopen_s(&inFile, filename.c_str(), "rb"))
		throw "Error: " + filename + " does not exist.";
	try
	{
		m_data = make_shared<IMX_Data>(inFile);
		fclose(inFile);
		m_fileSize = m_data->m_colorData->m_imageSize + 48;
		if ((!m_data->m_pixelVal1 && !m_data->m_pixelVal2) || (m_data->m_pixelVal1 == 1 && m_data->m_pixelVal2 == 1))
			m_fileSize += m_data->m_colorData->m_paletteSize + 8;
	}
	catch (const char* str)
	{
		fclose(inFile);
		throw str + m_shortname + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
	catch (string str)
	{
		fclose(inFile);
		throw str + m_shortname + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
}

bool IMX::exportPNG()
{
	string file = m_directory + m_name;

	// If the IMX came from XGM file, we need to create a separate IMX file for gm-imx2png.exe to use.
	//
	// One of the places where having direct usage of the python code would make the process simpler as there would be no need
	// to create a whole new IMX file.
	if (m_fromXGM)
		create(file, false);

	// Additonally, direct usage of the python code would optimize these writes to the console
	printf("%s", g_global.tabs.c_str());
	if (m_directory.length())
		system(("gm-imx2png -d \"" + m_directory.substr(0, m_directory.length() - 1) + "\" -v \"" + file + '\"').c_str());
	else
		system(("gm-imx2png -v \"" + file + '\"').c_str());

	if (m_fromXGM)
		remove(file.c_str());
	return true;
}

bool IMX::importPNG()
{
	const string initialName = m_directory + m_shortname + ".PNG";
	string pngName;
	bool found = false;
	for (const char* prefix : { "\\XGM_", "\\" })
	{
		for (const char* suffix : { "", ".i4", ".i8", ".i24", ".i32" })
		{
			if (filesystem::exists(m_directory + prefix + m_shortname + suffix + ".PNG"))
			{
				pngName = m_directory + prefix + m_shortname + suffix + ".PNG";
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
				if (pngName.find(".PNG") == string::npos && pngName.find(".png") == string::npos)
					pngName += ".PNG";
				if (filesystem::exists(pngName))
					g_global.quit = true;
				else
				{
					size_t pos = pngName.find_last_of('\\');
					if (pos != string::npos)
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
		filesystem::copy_file(initialName, pngName.insert(pngName.length() - 4, "-Buffer"));

	size_t slash = pngName.find_last_of('\\');
	printf("%s", g_global.tabs.c_str());
	if (slash != string::npos)
		system(("gm-png2imx -d \"" + pngName.substr(0, slash) + "\" -v \"" + pngName + '\"').c_str());
	else
		system(("gm-png2imx -v \"" + pngName).c_str() + '\"');

	if (removePNG)
		remove(pngName.c_str());
	read(pngName.erase(pngName.length() - 3, 3) + "IMX");
	remove((pngName + "IMX").c_str());
	return true;
}

IMX_Data::IMX_Data() : m_colorData(make_shared<Image>()) {}
IMX_Data::IMX_Data(FILE* inFile) : m_colorData(make_shared<Image>())
{
	char test[4] = { 0 };
	fread(test, 1, 4, inFile);
	if (!strstr(test, "IMX"))
	{
		fclose(inFile);
		throw "Error: no 'IMX' tag for texture ";
	}
	fseek(inFile, 16, SEEK_CUR);
	fread(&m_width, 4, 1, inFile);
	fread(&m_height, 4, 1, inFile);
	fread(&m_pixelVal1, 4, 1, inFile);
	fread(&m_pixelVal2, 4, 1, inFile);
	if ((!m_pixelVal1 && !m_pixelVal2) || (m_pixelVal1 == 1 && m_pixelVal2 == 1))
	{
		fread(&m_colorData->m_paletteSize, 4, 1, inFile);
		m_colorData->m_palette = new unsigned char[m_colorData->m_paletteSize >> 2][4]();
		fread(m_colorData->m_palette, 4, m_colorData->m_paletteSize >> 2, inFile);
		fseek(inFile, 4, SEEK_CUR);
	}
	else if (m_pixelVal2 != 2 || (m_pixelVal1 != 4 && m_pixelVal1 != 3))
	{
		fclose(inFile);
		throw "Error: Unknown Pixel Storage values (" + to_string(m_pixelVal1) + " | " + to_string(m_pixelVal2) + ") for texture ";
	}
	fread(&m_colorData->m_imageSize, 4, 1, inFile);
	m_colorData->m_image = new unsigned char[m_colorData->m_imageSize];
	fread(m_colorData->m_image, 1, m_colorData->m_imageSize, inFile);
}
IMX_Data::IMX_Data(IMX_Data& imx)
	: m_width(imx.m_width), m_height(imx.m_height), m_pixelVal1(m_pixelVal1), m_pixelVal2(m_pixelVal2)
{
	m_colorData = make_shared<Image>(*imx.m_colorData,
											m_pixelVal1 != m_pixelVal2 || (m_pixelVal1 != 0 && m_pixelVal1 != 1));
}

void IMX_Data::create(FILE* outFile)
{
	unsigned long value = 0;
	fwrite("IMX\0", 1, 4, outFile);
	fwrite("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 1, 16, outFile);
	fwrite(&m_width, 4, 1, outFile);
	fwrite(&m_height, 4, 1, outFile);
	fwrite(&m_pixelVal1, 4, 1, outFile);
	fwrite(&m_pixelVal2, 4, 1, outFile);

	if (m_pixelVal1 == m_pixelVal2 && (m_pixelVal1 == 0 || m_pixelVal1 == 1))
	{
		fwrite(&m_colorData->m_paletteSize, 4, 1, outFile);
		fwrite(m_colorData->m_palette, 4, m_colorData->m_paletteSize >> 2, outFile);
		fwrite(&(value = 2), 1, 4, outFile);
	}

	fwrite(&m_colorData->m_imageSize, 4, 1, outFile);
	fwrite(m_colorData->m_image, 1, m_colorData->m_imageSize, outFile);
	fwrite(&(value = 3), 1, 4, outFile);
	fwrite("\0\0\0\0", 1, 4, outFile);
}

IMX_Data::Image::Image(const Image& other)
{
	if (other.m_palette)
	{
		m_paletteSize = other.m_paletteSize;
		m_palette = new unsigned char[m_paletteSize >> 2][4];
		memcpy_s(m_palette, m_paletteSize, other.m_palette, m_paletteSize);
	}
	m_imageSize = other.m_imageSize;
	copy(other.m_image, other.m_image + m_imageSize, m_image);
}

IMX_Data::Image::Image(const Image& other, bool usePalette)
{
	if (usePalette && other.m_palette)
	{
		m_paletteSize = other.m_paletteSize;
		m_palette = new unsigned char[m_paletteSize >> 2][4];
		memcpy_s(m_palette, m_paletteSize, other.m_palette, m_paletteSize);
	}
	m_imageSize = other.m_imageSize;
	copy(other.m_image, other.m_image + m_imageSize, m_image);
}

IMX_Data::Image::~Image()
{
	if (m_palette != nullptr)
		delete[m_paletteSize >> 2] m_palette;
	if (m_image != nullptr)
		delete[m_imageSize] m_image;
}
