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
#include "Global_Functions.h"
#include "IMX\IMX.h"
#include "XG\XG.h"

class XGM
{
	friend class XGM_Main;
	friend class Editor;
	friend struct IMX_PNG;
private:
	std::vector<IMX> m_textures;
	std::vector<XG> m_models;
	char m_saved;
public:
	std::string m_name;
	std::string m_shortname;
	XGM();
	XGM(std::string filename);
	XGM(const XGM&) = default;
	void create(std::string filename);
};
