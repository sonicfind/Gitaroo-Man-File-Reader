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
#include "Model_Setup.h"
class PlayerModelSetup : public ModelSetup
{
	struct Struct48_2f
	{
		float float_a;
		float float_b;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long ulong_i;
		unsigned long ulong_j;
	};

	std::vector<Struct48_2f> m_player_controllable;

	struct Read4Entry
	{
		unsigned long m_size = 0;
		std::vector<unsigned long> m_vals;
	};

	std::vector<Read4Entry> m_player_read4Entry;

	std::vector<unsigned long> m_player_ulongs;

public:
	PlayerModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile);
};
