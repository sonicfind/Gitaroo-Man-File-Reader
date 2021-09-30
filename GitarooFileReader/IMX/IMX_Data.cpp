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
#include "IMX_Data.h"
IMX_Data::IMX_Data() : m_colorData(std::make_shared<Image>()) {}
IMX_Data::IMX_Data(FILE* inFile) : m_colorData(std::make_shared<Image>())
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
	m_colorData->m_image = new unsigned char[m_colorData->m_imageSize];
	fread(m_colorData->m_image, 1, m_colorData->m_imageSize, inFile);
}
IMX_Data::IMX_Data(IMX_Data& imx)
	: m_width(imx.m_width), m_height(imx.m_height), m_pixelVal1(m_pixelVal1), m_pixelVal2(m_pixelVal2)
{
	m_colorData = std::make_shared<Image>(*imx.m_colorData,
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

bool IMX_Data::hasAlpha() const
{
	return m_pixelVal1 != 3 || m_pixelVal2 != 2;
}

unsigned char* IMX_Data::getSubImage(unsigned long bytes_per_pixel, float topLeft_X, float topLeft_Y, float bottomRight_X, float bottomRight_Y) const
{
	if (topLeft_X < 0 || m_width < bottomRight_X || topLeft_Y < 0 || m_height < bottomRight_Y)
		return nullptr;

	const size_t width = (size_t)roundf(bytes_per_pixel * (bottomRight_X - topLeft_X));
	const size_t height = (size_t)roundf(bottomRight_Y - topLeft_Y);
	unsigned char* subImage = new unsigned char[width * height];
	unsigned char* pos = m_colorData->m_image_uncompressed + (size_t)roundf(bytes_per_pixel * (topLeft_Y * m_width + topLeft_X));
	for (size_t i = 0; i < height; ++i, pos += size_t(bytes_per_pixel) * m_width)
		memcpy(subImage + i * width, pos, width);
	return subImage;
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
	std::copy(other.m_image, other.m_image + m_imageSize, m_image);
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
	std::copy(other.m_image, other.m_image + m_imageSize, m_image);
}

IMX_Data::Image::~Image()
{
	if (m_palette != nullptr)
		delete[m_paletteSize >> 2] m_palette;
	if (m_image != nullptr)
		delete[m_imageSize] m_image;
}
