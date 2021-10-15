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
#include "Sub Types/XGEntry.h"
#include "Sub Types/IMXEntry.h"
#include "Sub Types/Model_Setup.h"
#include "Sub Types/Camera_Setup.h"
#include "Sub Types/Sprites_Setup.h"
#include "Sub Types/TextureAnimation.h"
#include "Sub Types/PSetup.h"
#include "XGM/XGM.h"
#include "Viewer/Viewer.h"
class SSQ
	: public FileType
	, public Viewer
{
	unsigned long m_headerVersion;
	float m_startFrame;
	float m_endFrame;
	float m_currFrame;
	Val m_junk[4] = { 0 };
	std::vector<glm::mat4> m_modelMatrices;
	std::vector<IMXEntry> m_IMXentries;
	std::vector<XGEntry> m_XGentries;
	std::vector<std::unique_ptr<ModelSetup>> m_modelSetups;
	CameraSetup m_camera;
	SpritesSetup m_sprites;
	std::vector<TexAnim> m_texAnimations;
	PSetup m_pSetup;

	std::unique_ptr<XGM> m_xgm;

	struct ViewerControls_SSQ : public ViewerControls
	{
		bool hasFreeMovement = false;
		ViewerControls_SSQ() : ViewerControls(false) {}
	};

	bool changeStartFrame();
	bool changeEndFrame();

public:
	SSQ();
	SSQ(std::string filename, bool loadXGM = true);
	SSQ(const SSQ&) = default;
	bool loadXGM();
	bool create(std::string filename);

	bool write_to_txt() { return false; }
	bool viewSequence();

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
	void draw(const bool doTranparents);

	void setFrame(const float frame);
};
