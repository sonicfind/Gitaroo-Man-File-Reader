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
#include "Sub Types/IMXEntry.h"
#include "Sub Types/XGEntry.h"
#include "Sub Types/PlayerModel_Setup.h"
#include "Sub Types/AttDefModel_Setup.h"
#include "Sub Types/SnakeModel_Setup.h"
#include "Sub Types/Camera_Setup.h"
#include "Sub Types/Sprites_Setup.h"
#include "Sub Types/TextureAnimation.h"
#include "Sub Types/PSetup.h"
#include "XGM/XGM.h"
class SSQ
	: public FileType
{
	friend class SSQ_Main;
	friend class SSQ_Editor;
private:
	// Full filename
	std::string m_filename;
	// Short version of filename
	std::string m_shortname;
	// 
	unsigned long m_headerVersion;
	char m_unk[12] = { 0 };
	Val m_junk[4] = { 0 };
	std::vector<IMXEntry> m_IMXentries;
	std::vector<XGEntry> m_XGentries;
	std::vector<std::unique_ptr<ModelSetup>> m_modelSetups;
	CameraSetup m_camera;
	SpritesSetup m_sprites;
	std::vector<TexAnim> m_texAnimations;
	PSetup m_pSetup;

	//0 - Not saved
	//1 - Saved
	//2 - Saved at the currently pointed location
	char m_saved;

	std::unique_ptr<XGM> m_xgm;

public:
	SSQ();
	SSQ(std::string filename, bool loadXGM = true);
	SSQ(const SSQ&) = default;
	bool loadXGM();
	bool create(std::string filename, bool trueSave = true);

	bool write_to_txt() { return false; }

	bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension);
	bool functionSelection(const char choice, bool isMulti);
	static void displayMultiChoices();
	static void displayMultiHelp();
	static const std::string multiChoiceString;
};
