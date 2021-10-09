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
#include "IMX\IMX.h"
#include "XG\XG.h"
#include "Viewer\Viewer.h"

class XGM
	: public FileType
	, public Viewer
{
	std::vector<IMX> m_textures;
	std::vector<XG> m_models;

	unsigned int m_lightUBO;

	struct ViewerControls_XGM : public ViewerControls
	{
		bool animate = true;
		bool loop = false;

		struct ModelInfo
		{
			size_t modelIndex;
			size_t animIndex = 0;
			float length;
			float frame = 0;
			ModelInfo(size_t index, float length)
				: modelIndex(index)
				, length(length) {}
		};
		std::vector<ModelInfo> m_models;

		ViewerControls_XGM(const std::vector<size_t>& indices, const std::vector<float>& lengths)
			: ViewerControls(true)
		{
			for (size_t i = 0; i < indices.size(); ++i)
				m_models.emplace_back(indices[i], lengths[i]);
		}
	};

public:
	XGM();
	XGM(std::string filename);
	XGM(const XGM&) = default;
	bool create(std::string filename);

	bool write_to_txt();
	bool exportPNGs();
	bool importPNGs();
	bool exportOBJs();
	bool importOBJs();
	bool viewModels();
	bool selectTexture();
	bool selectModel();
	IMX* getTexture(const char* name);
	XG* getModel(const char* name);

	bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension);
	bool functionSelection(const char choice, bool isMulti);
	static void displayMultiChoices();
	static void displayMultiHelp();
	static const std::string multiChoiceString;

protected:
	void initialize(const char* windowName);
	void uninitialize();

	void update(float current);
	void draw();
};
