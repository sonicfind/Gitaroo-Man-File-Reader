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
using namespace std;

IMX::IMX() : m_textureIndex(0), m_fileSize(0), m_non_model(0), m_unk(0), m_saved(1), m_fromXGM(false) {}

IMX::IMX(FILE* inFile, const std::string& directory) :  m_directory(directory), m_saved(2), m_fromXGM(true)
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
		m_data = std::make_shared<IMX_Data>(inFile);
		fseek(inFile, 8, SEEK_CUR);
	}
	catch (const char* str)
	{
		fclose(inFile);
		throw str + std::string(m_name) + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
	catch (std::string str)
	{
		fclose(inFile);
		throw str + std::string(m_name) + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
}

IMX::IMX(std::string filename, bool useBanner) : m_textureIndex(0), m_non_model(false), m_unk(false), m_saved(2), m_fromXGM(false)
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
		m_data = std::make_shared<IMX_Data>(inFile);
		fclose(inFile);
		m_fileSize = m_data->m_colorData->m_imageSize + 48;
		if ((!m_data->m_pixelVal1 && !m_data->m_pixelVal2) || (m_data->m_pixelVal1 == 1 && m_data->m_pixelVal2 == 1))
			m_fileSize += m_data->m_colorData->m_paletteSize + 8;
	}
	catch (const char* str)
	{
		fclose(inFile);
		throw str + m_shortname + ".IMX [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
	catch (std::string str)
	{
		fclose(inFile);
		throw str + m_shortname + ".IMX [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
}

IMX& IMX::operator=(IMX& imx)
{
	if (m_data != imx.m_data)
	{
		m_shortname = imx.m_shortname;
		std::copy(imx.m_filepath, imx.m_filepath + 256, m_filepath);
		std::copy(imx.m_name, imx.m_name + 16, m_name);
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
void IMX::create(string filename, bool useBanner)
{
	if (useBanner)
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
	m_saved = 1;
}

void IMX::read(std::string filename)
{
	FILE* inFile = nullptr;
	if (fopen_s(&inFile, filename.c_str(), "rb"))
		throw "Error: " + filename + " does not exist.";
	try
	{
		m_data = std::make_shared<IMX_Data>(inFile);
		fclose(inFile);
		m_fileSize = m_data->m_colorData->m_imageSize + 48;
		if ((!m_data->m_pixelVal1 && !m_data->m_pixelVal2) || (m_data->m_pixelVal1 == 1 && m_data->m_pixelVal2 == 1))
			m_fileSize += m_data->m_colorData->m_paletteSize + 8;
	}
	catch (const char* str)
	{
		fclose(inFile);
		throw str + m_shortname + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
	catch (std::string str)
	{
		fclose(inFile);
		throw str + m_shortname + " [File offset: " + std::to_string(ftell(inFile) - 4) + "].";
	}
}

IMX_Data::IMX_Data() : m_colorData(make_shared<ColorData>()) {}
IMX_Data::IMX_Data(FILE* inFile) : m_colorData(make_shared<ColorData>())
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
		throw "Error: Unknown Pixel Storage values (" + std::to_string(m_pixelVal1) + " | " + std::to_string(m_pixelVal2) + ") for texture ";
	}
	fread(&m_colorData->m_imageSize, 4, 1, inFile);
	if (!m_pixelVal1 && !m_pixelVal2)
	{
		m_colorData->m_pix4 = new Pixel4[m_colorData->m_imageSize];
		fread(m_colorData->m_pix4, sizeof(Pixel4), m_colorData->m_imageSize, inFile);
	}
	else if (m_pixelVal1 == 1 && m_pixelVal2 == 1)
	{
		m_colorData->m_pix8 = new Pixel8[m_colorData->m_imageSize];
		fread(m_colorData->m_pix8, sizeof(Pixel8), m_colorData->m_imageSize, inFile);
	}
	else if (m_pixelVal1 == 3)
	{
		m_colorData->m_pix24 = new Pixel24[m_colorData->m_imageSize / 3];
		fread(m_colorData->m_pix24, sizeof(Pixel24), m_colorData->m_imageSize / 3, inFile);
	}
	else
	{
		m_colorData->m_pix32 = new Pixel32[m_colorData->m_imageSize >> 2];
		fread(m_colorData->m_pix32, sizeof(Pixel32), m_colorData->m_imageSize >> 2, inFile);
	}
}
IMX_Data::IMX_Data(IMX_Data& imx) : m_colorData(make_shared<ColorData>())
{
	m_width = imx.m_width;
	m_height = imx.m_height;
	m_pixelVal1 = imx.m_pixelVal1;
	m_pixelVal2 = imx.m_pixelVal2;
	if ((!m_pixelVal1 && !m_pixelVal2) || (m_pixelVal1 == 1 && m_pixelVal2 == 1))
	{
		m_colorData->m_paletteSize = imx.m_colorData->m_paletteSize;
		m_colorData->m_palette = new unsigned char[m_colorData->m_paletteSize >> 2][4];
		memcpy_s(m_colorData->m_palette, m_colorData->m_paletteSize, imx.m_colorData->m_palette, m_colorData->m_paletteSize);
	}
	m_colorData->m_imageSize = imx.m_colorData->m_imageSize;
	if (!m_pixelVal1 && !m_pixelVal2)
	{
		m_colorData->m_pix4 = new Pixel4[m_colorData->m_imageSize];
		std::copy(imx.m_colorData->m_pix4, imx.m_colorData->m_pix4 + m_colorData->m_imageSize, m_colorData->m_pix4);
	}
	else if (m_pixelVal1 == 1 && m_pixelVal2 == 1)
	{
		m_colorData->m_pix8 = new Pixel8[m_colorData->m_imageSize];
		std::copy(imx.m_colorData->m_pix8, imx.m_colorData->m_pix8 + m_colorData->m_imageSize, m_colorData->m_pix8);
	}
	else if (m_pixelVal1 == 3)
	{
		m_colorData->m_pix24 = new Pixel24[m_colorData->m_imageSize / 3];
		std::copy(imx.m_colorData->m_pix24, imx.m_colorData->m_pix24 + (m_colorData->m_imageSize / 3), m_colorData->m_pix24);
	}
	else
	{
		m_colorData->m_pix32 = new Pixel32[m_colorData->m_imageSize >> 2];
		std::copy(imx.m_colorData->m_pix32, imx.m_colorData->m_pix32 + (m_colorData->m_imageSize >> 2), m_colorData->m_pix32);
	}
}
IMX_Data& IMX_Data::operator=(IMX_Data& imx)
{
	if (m_colorData != imx.m_colorData)
	{
		m_width = imx.m_width;
		m_height = imx.m_height;
		m_pixelVal1 = imx.m_pixelVal1;
		m_pixelVal2 = imx.m_pixelVal2;
		m_colorData = imx.m_colorData;
	}
	return *this;
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
	if (!m_pixelVal1 && !m_pixelVal2)
	{
		fwrite(&m_colorData->m_paletteSize, 4, 1, outFile);
		fwrite(m_colorData->m_palette, 4, m_colorData->m_paletteSize >> 2, outFile);
		fwrite(&(value = 2), 1, 4, outFile);
		fwrite(&m_colorData->m_imageSize, 4, 1, outFile);
		fwrite(m_colorData->m_pix4, sizeof(Pixel4), m_colorData->m_imageSize, outFile);
	}
	else if (m_pixelVal1 == 1 && m_pixelVal2 == 1)
	{
		fwrite(&m_colorData->m_paletteSize, 4, 1, outFile);
		fwrite(m_colorData->m_palette, 4, m_colorData->m_paletteSize >> 2, outFile);
		fwrite(&(value = 2), 1, 4, outFile);
		fwrite(&m_colorData->m_imageSize, 4, 1, outFile);
		fwrite(m_colorData->m_pix8, sizeof(Pixel8), m_colorData->m_imageSize, outFile);
	}
	else if (m_pixelVal1 == 3)
	{
		fwrite(&m_colorData->m_imageSize, 4, 1, outFile);
		fwrite(m_colorData->m_pix24, sizeof(Pixel24), m_colorData->m_imageSize / 3, outFile);
	}
	else
	{
		fwrite(&m_colorData->m_imageSize, 4, 1, outFile);
		fwrite(m_colorData->m_pix32, sizeof(Pixel32), m_colorData->m_imageSize >> 2, outFile);
	}
	fwrite(&(value = 3), 1, 4, outFile);
	fwrite("\0\0\0\0", 1, 4, outFile);
}

IMX_Data::ColorData::ColorData(const ColorData& other)
{
	if (other.m_palette)
	{
		m_paletteSize = other.m_paletteSize;
		m_palette = new unsigned char[m_paletteSize >> 2][4];
		memcpy_s(m_palette, m_paletteSize, other.m_palette, m_paletteSize);
	}
	m_imageSize = other.m_imageSize;
	if (other.m_pix4 != nullptr)
	{
		m_pix4 = new Pixel4[m_imageSize];
		std::copy(other.m_pix4, other.m_pix4 + m_imageSize, m_pix4);
	}
	else if (other.m_pix8 != nullptr)
	{
		m_pix8 = new Pixel8[m_imageSize];
		std::copy(other.m_pix8, other.m_pix8 + m_imageSize, m_pix8);
	}
	else if (other.m_pix24 != nullptr)
	{
		m_pix24 = new Pixel24[m_imageSize / 3];
		std::copy(other.m_pix24, other.m_pix24 + (m_imageSize / 3), m_pix24);
	}
	else if (other.m_pix32 != nullptr)
	{
		m_pix32 = new Pixel32[m_imageSize >> 2];
		std::copy(other.m_pix32, other.m_pix32 + (m_imageSize >> 2), m_pix32);
	}
}

IMX_Data::ColorData::~ColorData()
{
	if (m_palette != nullptr)
		delete[m_paletteSize >> 2] m_palette;
	if (m_pix4 != nullptr)
		delete[m_imageSize] m_pix4;
	else if (m_pix8 != nullptr)
		delete[m_imageSize] m_pix8;
	else if (m_pix24 != nullptr)
		delete[m_imageSize / 3] m_pix24;
	else if (m_pix32 != nullptr)
		delete[m_imageSize >> 2] m_pix32;
}
