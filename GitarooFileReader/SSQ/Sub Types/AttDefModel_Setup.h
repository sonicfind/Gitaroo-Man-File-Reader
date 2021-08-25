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
class AttDefModelSetup : public ModelSetup
{
	float m_attdef_float32;

	struct Struct64_9f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		float float_h;
		unsigned long ulong_a;
		unsigned long ulong_b_maybe;
		unsigned long ulong_c_maybe;
		unsigned long ulong_d_maybe;
		unsigned long ulong_e;
		unsigned long ulong_f_maybe;
		unsigned long ulong_g_maybe;
		unsigned long ulong_h_maybe;
	} m_attdef_64bytes;

public:
	AttDefModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile);
};
