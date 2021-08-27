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
#include "IMX.h"
#include <glad/glad.h>
void IMX::generateTextureBuffer()
{
	m_data->generateBuffer();
}

void IMX::bindTextureBuffer() const
{
	m_data->bindBuffer();
}

void IMX::deleteTextureBuffer()
{
	m_data->deleteBuffer();
}

unsigned IMX_Data::getTextureID() const
{
	return m_textureID;
}

void IMX_Data::generateBuffer()
{
	if (!m_textureID)
	{
		// New texture buffer
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		// Now we need to convert all the texture data into each separate uncompressed 32/24bit pixel
		// and load that data into the buffer

		// The only 24bit texture configuration
		if (!hasAlpha())
		{
			m_colorData->m_image_uncompressed = new unsigned char[3ULL * m_width * m_height];
			memcpy(m_colorData->m_image_uncompressed, m_colorData->m_image, 3ULL * m_width * m_height);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_colorData->m_image_uncompressed);
		}
		// 32bit
		else
		{
			m_colorData->m_image_uncompressed = new unsigned char[4ULL * m_width * m_height];

			// 32bit uncompressed
			if (m_pixelVal1 == 4 && m_pixelVal2 == 2)
				memcpy(m_colorData->m_image_uncompressed, m_colorData->m_image, 4ULL * m_width * m_height);
			// 8bit compressed
			else if (m_pixelVal1 != 0 || m_pixelVal2 != 0)
				for (unsigned long pixel = 0; pixel < m_width * m_height; ++pixel)
					memcpy(m_colorData->m_image_uncompressed + 4ULL * pixel, m_colorData->m_palette[m_colorData->m_image[pixel]], 4);
			// 4bit compressed
			else
				for (unsigned long pixel = 0; pixel < m_width * m_height; ++pixel)
				{
					IMX_Data::Pixel4* pix = (IMX_Data::Pixel4*)&m_colorData->m_image[pixel >> 1];
					memcpy(m_colorData->m_image_uncompressed + 4ULL * pixel, m_colorData->m_palette[pixel & 1 ? (size_t)pix->pixel2 : (size_t)pix->pixel1], 4);
				}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_colorData->m_image_uncompressed);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		// Unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void IMX_Data::deleteBuffer()
{
	if (m_textureID)
	{
		if (hasAlpha())
			delete[4 * m_width * m_height] m_colorData->m_image_uncompressed;
		else
			delete[3 * m_width * m_height] m_colorData->m_image_uncompressed;
		m_colorData->m_image_uncompressed = nullptr;
		glDeleteTextures(1, &m_textureID);
		m_textureID = 0;
	}
}

void IMX_Data::bindBuffer() const
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}
