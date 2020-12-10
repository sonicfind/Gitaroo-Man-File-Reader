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
#include "IMX_Image.h"

bool ImageConverter::exportImage(IMX& image)
{
	GlobalFunctions::banner(" " + image.m_shortname + " - Texture Export ");
	m_png.exportPNG(image);
	return true;
}

bool ImageConverter::exportImages(XGM& pack)
{
	GlobalFunctions::banner(" " + pack.m_shortname + " - Multi-Texture Export ");
	m_png.exportPNG(pack);
	return true;
}

bool ImageConverter::importImage(IMX& image)
{
	GlobalFunctions::banner(" " + image.m_shortname + " - Texture Import ");
	m_png.importPNG(image);
	return true;
}

bool ImageConverter::importImages(XGM& pack)
{
	GlobalFunctions::banner(" " + pack.m_shortname + " - Multi-Texture Import ");
	m_png.importPNG(pack);
	return true;
}
