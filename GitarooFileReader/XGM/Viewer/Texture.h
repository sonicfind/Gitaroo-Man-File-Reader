#pragma once
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
#include "IMX/IMX.h"
struct Texture
{
	char m_name[16];
	unsigned int m_textureID = 0;
	unsigned long m_width = 0;
	unsigned long m_height = 0;
	bool m_hasAlpha = true;
	unsigned char* m_pixelData = nullptr;
	Texture(IMX& imx);
	~Texture();
};

extern std::list<Texture> g_textures;
