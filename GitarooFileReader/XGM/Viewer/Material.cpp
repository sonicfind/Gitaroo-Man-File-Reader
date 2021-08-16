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
#include "Material.h"
#include <glad/glad.h>

Material::Material(const xgMaterial* materialNode, std::vector<IMX>& images)
	: m_xgMaterial(materialNode)
{
	if (m_xgMaterial->m_inputTextures.size())
	{
		PString inputString = materialNode->m_inputTextures.back()->m_imxName;
		for (int index = 0; index < inputString.m_size; ++index)
			inputString.m_pstring[index] = toupper(inputString.m_pstring[index]);

		for (auto& texture : images)
		{
			if (strcmp(texture.getName(), inputString.m_pstring) == 0)
			{
				m_texture = texture.m_data;
				m_texture->generateTextureBuffer();
				return;
			}
		}
	}
}

void Material::setShaderValues(Shader* shader)
{
	if (m_texture)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());
		// If both a texture and vertex color are applicable
		if (m_xgMaterial->m_shadingType >= 3)
			shader->setInt("shadingType", m_xgMaterial->m_shadingType + 2);
		else
			shader->setInt("shadingType", m_xgMaterial->m_shadingType);
		shader->setInt("alphaType", m_xgMaterial->m_flags & 1);
		shader->setInt("alphaMultiplier", m_xgMaterial->m_flags & ~1);
		shader->setInt("textEnv", m_xgMaterial->m_textureEnv);
	}
	else
		shader->setInt("shadingType", m_xgMaterial->m_shadingType);
	glBlendColor(m_xgMaterial->m_diffuse.red, m_xgMaterial->m_diffuse.green, m_xgMaterial->m_diffuse.blue, m_xgMaterial->m_diffuse.alpha);
	shader->setVec4("material.color", (float*)&m_xgMaterial->m_diffuse);
	shader->setVec3("material.specular", (float*)&m_xgMaterial->m_specular);
	shader->setFloat("material.shininess", m_xgMaterial->m_specular.exponent);

	shader->setInt("blendingType", m_xgMaterial->m_blendType);
}

void Material::setBlending()
{
	switch (m_xgMaterial->m_blendType)
	{
	case 0:
		if (m_xgMaterial->m_flags & 1)
		{
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
			glDisable(GL_BLEND);
		break;
	case 1:
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		if (m_texture)
		{
			if (m_xgMaterial->m_flags & 1)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			else
				glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case 2:
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;
	case 3:
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		break;
	case 4:
		if (m_xgMaterial->m_flags & 1)
		{
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
			glDisable(GL_BLEND);
		break;
	case 5:
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
	}
}
