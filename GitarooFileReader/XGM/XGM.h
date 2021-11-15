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
		size_t modelIndex = 0;

		class ModelInfo
		{
			XG* m_model;
			size_t m_animIndex;
			float m_length;
			float m_frame;

		public:

			ModelInfo(XG* model);
			~ModelInfo();
			void init();
			void update(float delta, bool loop);
			void draw(bool showNormals, bool doTransparents) const;
			void animReset();
			void nextAnim(bool animate);
			void prevAnim();
			void fullReset();
			void restPose();
			void swap(XG* model);
			void changeTitle(GLFWwindow* window, bool animate, bool loop);
		private:
			void animChanged(bool animate);
			
		};
		std::vector<ModelInfo> m_models;

		ViewerControls_XGM(const std::vector<XG*>& models, size_t index = 0)
			: ViewerControls(true)
			, modelIndex(index)
		{
			for (auto& model : models)
				m_models.push_back(model);
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

private:
	void initialize(const char* windowName);
	void uninitialize();

	void update(float current);
	void drawOpaques();
	void drawTranparents();
};
