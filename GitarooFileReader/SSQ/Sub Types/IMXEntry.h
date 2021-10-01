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
#include "SSQ_BaseStructs.h"
#include "IMX/IMX.h"
struct IMXEntry
{
	char m_name[16] = { 0 };
	unsigned long m_unused_1;
	unsigned long m_unused_2;
	char m_junk[8];

	IMX* m_imxPtr = nullptr;

	IMXEntry(FILE* inFile);
	void create(FILE* outFile);

private:
	static unsigned s_spriteTextureUBO;

public:
	static void generateSpriteBuffer(const std::vector<IMXEntry>& entries);
	static void deleteSpriteBuffer();
};
