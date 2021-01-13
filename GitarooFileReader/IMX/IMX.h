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

struct IMX_Data
{
	struct Pixel4
	{
		unsigned char pixel1 : 4, pixel2 : 4;
	};

	unsigned long m_width = 0;
	unsigned long m_height = 0;
	unsigned long m_pixelVal1 = 0;
	unsigned long m_pixelVal2 = 0;

	struct Image
	{
		unsigned long m_paletteSize = 0;
		unsigned char(*m_palette)[4] = nullptr;
		unsigned long m_imageSize = 0;
		unsigned char* m_image = nullptr;
		Image() = default;
		Image(const Image&);
		Image(const Image&, bool);
		~Image();
	};
	std::shared_ptr<Image> m_colorData;
	IMX_Data();
	IMX_Data(FILE* inFile);
	IMX_Data(IMX_Data& imx);
	void create(FILE* outFile);
};

class IMX
{
	friend class IMX_Main;
	friend class XGM_Main;
	friend class XGM_Editor;
	char m_filepath[257] = { 0 };
	char m_name[17] = { 0 };
	unsigned long m_textureIndex;
	unsigned long m_fileSize;
	unsigned long m_non_model;
	unsigned long m_unk;
	char m_junk[12] = { 0 };
	char m_saved;
public:
	bool m_fromXGM;
	std::shared_ptr<IMX_Data> m_data;
	std::string m_directory;
	std::string m_shortname;
	IMX();
	IMX(FILE* inFile, const std::string& directory);
	IMX(std::string filename, bool useBanner = true);
	IMX(const IMX&) = default;
	IMX& operator=(IMX& imx);
	void create(FILE* outFile, unsigned long& sizes);
	void create(std::string filename, bool trueSave = true);
	void read(std::string filename);
	bool exportPNG();
	bool importPNG();
	//Returns name C-string (size: 16)
	char* getName() { return m_name; }
};
