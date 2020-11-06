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
#include "..\..\Header\pch.h"
#include "Global_Functions.h"
#include "CHC_CloneHero.h"
#include "CH_Export.h"
#include "CH_Import.h"
using namespace std;

bool exportChart(CHC* song)
{
	CH_Exporter exporter(song);
	return exporter.exportChart();
}

bool importChart(CHC& song)
{
	CH_Importer importer(song);
	if (importer.importChart())
	{
		song = importer.getSong();
		return true;
	}
	else
		return false;
}
