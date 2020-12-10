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
#include "XGM.h"
using namespace std;
XGM::XGM() : m_saved(1) {}

XGM::XGM(std::string filename) : m_saved(2), m_name(filename + ".XGM")
{
	size_t pos = filename.find_last_of('\\');
	m_shortname = filename.substr(pos != std::string::npos ? pos + 1 : 0);
	GlobalFunctions::banner(" Loading " + m_shortname + ".XGM ");
	FILE* inFile;
	if (fopen_s(&inFile, m_name.c_str(), "rb") || inFile == nullptr)
		throw "Error: " + m_name + " could not be located.";

	unsigned long numT, numM;
	if (fread(&numT, 4, 1, inFile) != 1 || fread(&numM, 4, 1, inFile) != 1)
		throw "Error: " + m_name + " is not of sufficient size.";

	const string directory = pos != std::string::npos ? filename.substr(0, pos + 1) : "";
	for (size_t t = 0; t < numT; t++)
		m_textures.emplace_back(inFile, directory);
	for (size_t m = 0; m < numM; m++)
		m_models.emplace_back(inFile, directory);
	fclose(inFile);
}

//Create or update a XGM file
void XGM::create(string filename)
{
	{
		size_t pos = filename.find_last_of('\\');
		GlobalFunctions::banner(" Saving " + filename.substr(pos != string::npos ? pos + 1 : 0) + ' ');
	}
	FILE* outFile = nullptr;
	fopen_s(&outFile, filename.c_str(), "wb");
	if (outFile == nullptr)
		throw "Error: " + filename + " could not be created.";

	unsigned long sizes = (unsigned long)m_textures.size();
	fwrite(&sizes, 4, 1, outFile);
	sizes = (unsigned long)m_models.size();
	fwrite(&sizes, 4, 1, outFile);

	sizes = 0;
	for (IMX& imx : m_textures)
	{
		imx.create(outFile, sizes);
		fflush(outFile);
	}

	for (XG& xg : m_models)
	{
		xg.create(outFile);
		fflush(outFile);
	}
	fclose(outFile);
	m_saved = 1;
}
