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
#ifdef CHCBASE_EXPORTS
#define CHCBASE_API __declspec(dllexport)
#else
#define CHCBASE_API __declspec(dllimport)
#endif
#include "CHC.h"
class CHC_Editor
{
private:
	CHC* m_song;
public:
	CHC_Editor() : m_song(nullptr) {}
	CHC_Editor(CHC& song) : m_song(&song) {}
	CHC_Editor& operator=(CHC_Editor& edtr)
	{
		m_song = edtr.m_song;
	}
	//Songs
	void editSong(const bool multi = false);
	void fixNotes();
	void organizeAll();
	void PSPToPS2();
	void swapIMC();
	void audioSettings();
	void winLossSettings();
	void adjustSpeed();
	void sectionMenu();
	void playerSwapAll();
	void playOrder();
	void rearrange();
	void adjustFactors();
	bool pathTest(const size_t startIndex, const bool show = false);
	void fullPathTest();
	char testSection(const size_t sectIndex, bool** conds, bool* results, bool* reach);
	void traverseCondition(const size_t sectIndex, const size_t condIndex, bool** conds, bool* results, bool* reach);
	//SongSections
	void sectionSubMenu();
	bool reorganize(SongSection& section);
	void playerSwap(SongSection& section);
	void changeName(SongSection& section);
	void changeAudio(SongSection& section);
	void changeFrames(SongSection& section);
	void switchPhase(SongSection& section);
	void adjustTempo(SongSection& section);
	void adjustDuration(SongSection& section);
	void conditionMenu(SongSection& section);
	void conditionDelete(SongSection& section, size_t index);
	void changeConditionType(SongSection::Condition& condition);
	void changeConditionArgument(SongSection::Condition& condition);
	void changeConditionTrue(SongSection& section, SongSection::Condition& condition);
	void changeConditionFalse(SongSection& section, SongSection::Condition& condition);
};

class CHC_Main
{
private:
	CHC song;
public:
	CHC_Main(std::string filename) : song(filename) {}
	bool menu(size_t fileCount = 0);
	void saveFile(bool onExit = false);
	void writeTxt();
	bool applyChanges(const bool fix, const bool swap = false, const bool save = false);
	void edit(const bool multi = false);
	void makeTAS();
	void exportChart();
	void importChart();
	bool createColorTemplate();
};
extern "C" CHCBASE_API bool quickFix(CHC& song);
extern "C" CHCBASE_API bool loadSingleCHC(std::string& filename);
extern "C" CHCBASE_API bool loadMultiCHC(LinkedList::List<std::string>* files);
