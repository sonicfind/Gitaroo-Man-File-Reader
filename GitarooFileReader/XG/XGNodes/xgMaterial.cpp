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
unsigned int xgMaterial::s_MaterialUBO = 0;
unsigned long xgMaterial::read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
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
	unsigned long sizechange = 0;
	while (!strchr(test.m_pstring, '}'))
	{
		// Removes duplicate textures
		if (m_inputTexture)
			sizechange += 27 + m_inputTexture->getName().getSize();
		m_inputTexture.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return sizechange;
}

void xgMaterial::create(FILE* outFile) const
{
	XGNode::create(outFile);

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

	if (m_inputTexture)
	{
		PString::push("inputTexture", outFile);
		m_inputTexture->push(outFile);
		PString::push("outputTexture", outFile);
	}
	PString::push('}', outFile);
}

void xgMaterial::write_to_txt(FILE* txtFile, const char* tabs) const
{
	XGNode::write_to_txt(txtFile, tabs);

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

const size_t xgMaterial::getSize() const
{
	size_t size = XGNode::getSize()
		+ PSTRING_LEN_VAR("blendType", m_blendType)
		+ PSTRING_LEN_VAR("shadingType", m_shadingType)
		+ PSTRING_LEN_VAR("diffuse", m_diffuse)
		+ PSTRING_LEN_VAR("specular", m_specular)
		+ PSTRING_LEN_VAR("flags", m_flags)
		+ PSTRING_LEN_VAR("textureEnv", m_textureEnv)
		+ PSTRING_LEN_VAR("uTile", m_uTile)
		+ PSTRING_LEN_VAR("vTile", m_vTile);

	if (m_inputTexture)
		size += m_inputTexture->getName().getSize() + PSTRING_LEN("inputTexture") + PSTRING_LEN("outputTexture");
	return size;
}

void xgMaterial::connectTexture(std::vector<IMX>& textures)
{
	if (m_inputTexture)
		m_inputTexture->connectTexture(textures);
}

bool xgMaterial::hasTransparency() const
{
	return (m_blendType != 0 && m_blendType != 4) || m_flags & 1;
}

size_t xgMaterial::getNumMaterials() const
{
	return 1;
}

#include "XGM/Viewer/Shaders.h"
void xgMaterial::generateMaterialUniform()
{
	glGenBuffers(1, &s_MaterialUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, s_MaterialUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);

	g_shaders.m_base.bindUniformBlock(4, "Material");
	g_boneShaders.m_base.bindUniformBlock(4, "Material");

	glBindBufferBase(GL_UNIFORM_BUFFER, 4, s_MaterialUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void xgMaterial::deleteMaterialUniform()
{
	glDeleteBuffers(1, &s_MaterialUBO);
	s_MaterialUBO = 0;
}

void xgMaterial::intializeBuffers()
{
	if (m_inputTexture)
		m_inputTexture->generateTexture();
}

void xgMaterial::deleteBuffers()
{
	if (m_inputTexture)
		m_inputTexture->deleteTexture();
}

void xgMaterial::setShaderValues(const size_t index) const
{
	if (m_inputTexture)
	{
		m_inputTexture->bindTexture();
		Shader::setInt("useTexture", 1);
	}
	else
		Shader::setInt("useTexture", 0);

	glBindBuffer(GL_UNIFORM_BUFFER, s_MaterialUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &m_blendType);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	switch (m_blendType)
	{
	case 1:
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
		break;
	case 2:
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
		break;
	case 3:
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		break;
	default:
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	}
}
