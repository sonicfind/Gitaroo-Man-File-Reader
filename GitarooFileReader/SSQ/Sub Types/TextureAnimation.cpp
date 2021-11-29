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
#include "TextureAnimation.h"
#include <glm/gtc/type_ptr.hpp>
TexAnim::TexAnim(FILE* inFile)
{
	char tmp[5] = { 0 };

	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "\0\0\0\0"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No '\\0\\0\\0\\0' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(m_unk1, 1, 12, inFile);
	fread(m_junk, 1, 16, inFile);
	fread(glm::value_ptr(m_offset), sizeof(glm::u32vec2), 1, inFile);
	fread(m_textureName, 1, 24, inFile);

	unsigned long numCutOuts, numTexFrames;
	fread(&numCutOuts, 4, 1, inFile);
	m_cutOuts.resize(numCutOuts);

	// Only want to read the position values
	for (auto& cut : m_cutOuts)
		fread(&cut, sizeof(float), 4, inFile);

	fread(&numTexFrames, 4, 1, inFile);
	m_textureFrames.fill(numTexFrames, inFile);
}

void TexAnim::create(FILE* outFile)
{
	fwrite("\0\0\0\0", 1, 4, outFile);

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(m_unk1, 1, 12, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(glm::value_ptr(m_offset), sizeof(glm::u32vec2), 1, outFile);
	fwrite(m_textureName, 1, 24, outFile);

	unsigned long numCutOuts = (unsigned long)m_cutOuts.size(), numTexFrames = (unsigned long)m_textureFrames.size();
	fwrite(&numCutOuts, 4, 1, outFile);

	// Only want to write the position values
	for (auto& cut : m_cutOuts)
		fwrite(&cut, sizeof(float), 4, outFile);

	fwrite(&numTexFrames, 4, 1, outFile);
	m_textureFrames.write(outFile);
}

void TexAnim::connectTexture(IMX* imx)
{
	m_imxPtr = imx;
	if (!m_imxPtr)
		GlobalFunctions::printf_tab("Uhhhhhhhh, the image (%s) wasn't loaded. Figure it out\n", m_textureName);
}

// It's imperative that the subImages are re-loaded on each viewer session
// since the actual image data held in the IMX object can be swapped out
void TexAnim::loadCuts()
{
	if (!m_imxPtr)
		return;

	// Check bytes per pixel here just in case a texture swap changes the format
	m_bytesPerPixel = m_imxPtr->m_data->hasAlpha() ? 4 : 3;
	for (auto& cut : m_cutOuts)
		cut.subImage = m_imxPtr->m_data->getSubImage(m_bytesPerPixel, cut.topLeft.x, cut.topLeft.y, cut.bottomRight.x, cut.bottomRight.y);

	m_cutOutIndex = m_textureFrames.front().cutOutIndex;
	// Load the first subImage into the texture buffer
	apply();
}

// It's imperative that the subImages are re-loaded on each viewer session
// since the actual image data held in the IMX object can be swapped out
void TexAnim::unloadCuts()
{
	if (!m_imxPtr)
		return;

	for (auto& cut : m_cutOuts)
	{
		if (cut.subImage)
		{
			const size_t size = (size_t)roundf(m_bytesPerPixel * (cut.bottomRight.x - cut.topLeft.x)) * (size_t)roundf(cut.bottomRight.y - cut.topLeft.y);
			delete[size] cut.subImage;
			cut.subImage = nullptr;
		}
	}
}

void TexAnim::jumpToFrame(const float frame)
{
	m_textureFrames.jump(frame);
}

void TexAnim::substitute(const float frame)
{
	if (!m_imxPtr)
		return;

	auto iter = m_textureFrames.update(frame, true);
	if (iter->cutOutIndex != m_cutOutIndex)
	{
		m_cutOutIndex = iter->cutOutIndex;
		apply();
	}
}

#include <glad/glad.h>
void TexAnim::apply()
{
	const auto& cut = m_cutOuts[m_cutOutIndex];
	if (cut.subImage)
	{
		m_imxPtr->m_data->bindTexture();
		glTexSubImage2D(GL_TEXTURE_2D, 0, m_offset.x, m_offset.y,
			(unsigned long)roundf(cut.bottomRight.x - cut.topLeft.x),
			(unsigned long)roundf(cut.bottomRight.y - cut.topLeft.y),
			m_imxPtr->m_data->hasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
			cut.subImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
