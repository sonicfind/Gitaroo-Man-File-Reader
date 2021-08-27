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
#include "xgTexture.h"
unsigned long xgTexture::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	m_imxName.fill(inFile);
	PString::pull(inFile);
	fread(&m_mipmap_depth, 4, 1, inFile);
	PString::pull(inFile);
	return 0;
}
void xgTexture::create(FILE* outFile, bool full) const
{
	PString::push("xgTexture", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("url", outFile);
		m_imxName.push(outFile);
		PString::push("mipmap_depth", outFile);
		fwrite(&m_mipmap_depth, 4, 1, outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}
void xgTexture::write_to_txt(FILE* txtFile, const char* tabs)
{
	fprintf_s(txtFile, "\t\t\t%s    Texture: %s\n", tabs, m_imxName.m_pstring);
	fprintf_s(txtFile, "\t\t%s      Mip map depth: %lu\n", tabs, m_mipmap_depth);
}

void xgTexture::connectTexture(std::vector<IMX>& textures)
{
	PString compare = m_imxName;
	for (int index = 0; index < compare.m_size; ++index)
		compare.m_pstring[index] = toupper(compare.m_pstring[index]);

	for (auto& texture : textures)
		if (strcmp(texture.getName(), compare.m_pstring) == 0)
		{
			m_imxPtr = &texture;
			return;
		}
}

void xgTexture::generateTextureBuffer()
{
	m_imxPtr->generateTextureBuffer();
}

void xgTexture::bindTextureBuffer() const
{
	m_imxPtr->bindTextureBuffer();
}

void xgTexture::deleteTextureBuffer()
{
	m_imxPtr->deleteTextureBuffer();
}
