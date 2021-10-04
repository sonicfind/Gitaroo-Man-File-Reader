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
#include "IMXEntry.h"
unsigned IMXEntry::s_spriteTextureUBO;
IMXEntry::IMXEntry(FILE* inFile)
{
	fread(m_name, 1, 16, inFile);
	fread(&m_unused_1, 4, 1, inFile);
	fread(&m_unused_2, 4, 1, inFile);
	fread(m_junk, 1, 8, inFile);
}

void IMXEntry::create(FILE* outFile)
{
	fwrite(m_name, 1, 16, outFile);
	fwrite(&m_unused_1, 4, 1, outFile);
	fwrite(&m_unused_2, 4, 1, outFile);
	fwrite(m_junk, 1, 8, outFile);
}

#include "XGM/Viewer/Shaders.h"
#include <glad/glad.h>
void IMXEntry::generateSpriteBuffer(const std::vector<IMXEntry>& entries)
{
	glGenBuffers(1, &s_spriteTextureUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, s_spriteTextureUBO);
	glBufferData(GL_UNIFORM_BUFFER, 16 * entries.size(), NULL, GL_STATIC_DRAW);

	g_spriteShaders.m_base.bindUniformBlock(6, "SpriteSizes");

	glBindBufferBase(GL_UNIFORM_BUFFER, 6, s_spriteTextureUBO);
	for (size_t i = 0; i < entries.size(); ++i)
	{
		// First two values are width and height, so you can just take the base address of the IMX
		glBufferSubData(GL_UNIFORM_BUFFER, i * 16, 8, entries[i].m_imxPtr->m_data.get());
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void IMXEntry::deleteSpriteBuffer()
{
	glDeleteBuffers(1, &s_spriteTextureUBO);
}
