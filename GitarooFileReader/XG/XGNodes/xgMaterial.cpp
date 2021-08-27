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
#include "xgMaterial.h"
#include <glad/glad.h>
unsigned long xgMaterial::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_blendType, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_shadingType, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_diffuse.red, 4, 1, inFile);
	fread(&m_diffuse.green, 4, 1, inFile);
	fread(&m_diffuse.blue, 4, 1, inFile);
	fread(&m_diffuse.alpha, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_specular.red, 4, 1, inFile);
	fread(&m_specular.green, 4, 1, inFile);
	fread(&m_specular.blue, 4, 1, inFile);
	fread(&m_specular.exponent, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_flags, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_textureEnv, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_uTile, 4, 1, inFile);

	PString::pull(inFile);
	fread(&m_vTile, 4, 1, inFile);

	PString test(inFile);
	int sizechange = 0;
	while (!strchr(test.m_pstring, '}'))
	{
		// Removes duplicate textures
		if (m_inputTexture)
			sizechange += 28 + m_inputTexture->getName().m_size;
		m_inputTexture.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return sizechange;
}

void xgMaterial::create(FILE* outFile, bool full) const
{
	PString::push("xgMaterial", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		PString::push("blendType", outFile);
		fwrite(&m_blendType, 4, 1, outFile);

		PString::push("shadingType", outFile);
		fwrite(&m_shadingType, 4, 1, outFile);

		PString::push("diffuse", outFile);
		fwrite(&m_diffuse.red, 4, 1, outFile);
		fwrite(&m_diffuse.green, 4, 1, outFile);
		fwrite(&m_diffuse.blue, 4, 1, outFile);
		fwrite(&m_diffuse.alpha, 4, 1, outFile);

		PString::push("specular", outFile);
		fwrite(&m_specular.red, 4, 1, outFile);
		fwrite(&m_specular.green, 4, 1, outFile);
		fwrite(&m_specular.blue, 4, 1, outFile);
		fwrite(&m_specular.exponent, 4, 1, outFile);

		PString::push("flags", outFile);
		fwrite(&m_flags, 4, 1, outFile);

		PString::push("textureEnv", outFile);
		fwrite(&m_textureEnv, 4, 1, outFile);

		PString::push("uTile", outFile);
		fwrite(&m_uTile, 4, 1, outFile);

		PString::push("vTile", outFile);
		fwrite(&m_vTile, 4, 1, outFile);

		PString::push("inputTexture", outFile);
		m_inputTexture->push(outFile);
		PString::push("outputTexture", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

void xgMaterial::write_to_txt(FILE* txtFile, const char* tabs)
{
	switch (m_blendType)
	{
	case 1:
		fprintf_s(txtFile, "\t\t\t%s    Blend Type: Additive (+Tex Alpha)\n", tabs); break;
	case 2:
		fprintf_s(txtFile, "\t\t\t%s    Blend Type: Multiplicative (Ignore Tex Alpha)\n", tabs); break;
	case 3:
		fprintf_s(txtFile, "\t\t\t%s    Blend Type: Subtractive (Ignore Tex Alpha)\n", tabs); break;
	case 4:
		fprintf_s(txtFile, "\t\t\t%s    Blend Type: Unknown\n", tabs); break;
	case 5:
		fprintf_s(txtFile, "\t\t\t%s    Blend Type: Opaque (+Tex Alpha)\n", tabs); break;
	default:
		fprintf_s(txtFile, "\t\t\t%s    Blend Type: Opaque\n", tabs);
	}

	switch (m_shadingType)
	{
	case 1:
		fprintf_s(txtFile, "\t\t\t%s  Shading Type: Shaded (No specular??)\n", tabs); break;
	case 2:
		fprintf_s(txtFile, "\t\t\t%s  Shading Type: Shaded\n", tabs); break;
	case 3:
		fprintf_s(txtFile, "\t\t\t%s  Shading Type: Unshaded, Vertex Colors\n", tabs); break;
	case 4:
		fprintf_s(txtFile, "\t\t\t%s  Shading Type: Shaded, Vertex Colors\n", tabs); break;
	default:
		fprintf_s(txtFile, "\t\t\t%s  Shading Type: Unshaded\n", tabs);
	}

	fprintf_s(txtFile, "\t\t\t%sDiffuse (RGBA): %g, %g, %g, %g\n", tabs, m_diffuse.red, m_diffuse.green, m_diffuse.blue, m_diffuse.alpha);
	fprintf_s(txtFile, "\t\t%s     Specular (RGBExp): %g, %g, %g, %g\n", tabs, m_specular.red, m_specular.green, m_specular.blue, m_specular.exponent);
	fprintf_s(txtFile, "\t\t\t\t%s Flags: %s (Possibly overriden by blend or shading)\n", tabs, m_flags & 1 ? "Use Alpha" : "Ignore Alpha");
	fprintf_s(txtFile, "\t\t%s   Texture Environment: %s\n", tabs, m_textureEnv ? "Sphere/Environment Map" : "UV Coordinates");
	fprintf_s(txtFile, "\t\t\t\t%s UTile: %lu\n", tabs, m_uTile);
	fprintf_s(txtFile, "\t\t\t\t%s VTile: %lu\n", tabs, m_vTile);
	if (m_inputTexture)
		fprintf_s(txtFile, "\t\t\t%s       Texture: %s\n", tabs, m_inputTexture->getName().m_pstring);
}
