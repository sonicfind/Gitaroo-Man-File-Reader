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
#include "XGM/XGM.h"

class IMXEntry
{
	friend class SSQ;
private:
	char m_name[16] = { 0 };
	unsigned long m_unused_1;
	unsigned long m_unused_2;
	char m_unused_3[8] = { 0 };
public:
	IMXEntry(FILE* inFile);
	void create(FILE* outFile);
};

enum class ModelType
{
	Normal,
	Player1,
	Player2,
	DuetPlayer,
	Player1AttDef = 5,
	Player2AttDef,
	DuetPlayerAttDef,
	DuetComboAttack,
	Snake
};

struct XGEntry
{
	friend class SSQ;
private:
	char m_name[16] = { 0 };

	unsigned long m_isClone;

	unsigned long m_cloneID;

	unsigned long m_unused_1;

	ModelType m_type = ModelType::Normal;

	char m_unused_2[16] = { 0 };
public:
	XGEntry(FILE* inFile);
	void create(FILE* outFile);
};

class ModelSetup;
class PlayerModelSetup;
class AttDefModelSetup;
class SnakeModelSetup;

class LightSetup;

class CameraSetup
{
	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	char m_junk[16] = { 0 };

	struct Struct64_9f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
		float float_e;
		float float_f;
		float float_g;
		float float_h;
		unsigned long ulong_h;	
	} m_64bytes;

	struct CamPosition
	{
		float m_frame;
		float m_distance;
		float m_xCoord;
		float m_yCoord;
		float m_zCoord;
		unsigned long m_isMoving;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
		unsigned long ulong_c;
	};

	std::vector<CamPosition> m_32Pair_1;

	struct CamRotation
	{
		float m_frame;
		float m_distance;
		float m_pitch;
		float m_yaw;
		float m_roll;
		float m_perspective;
		unsigned long m_isMoving;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
	};
	std::vector<CamRotation> m_32Pair_2;

	struct Struct32_6f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};

	std::vector<Struct32_6f> m_32bytes_1;

	struct Struct32_5f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
	};
	std::vector<Struct32_5f> m_32bytes_2;

	std::vector<LightSetup> m_lights;

	struct Struct64_7f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long ulong_i;
	};

	std::vector<Struct64_7f> m_64bytes_v;

public:
	void read(FILE* inFile);
	void create(FILE* outFile);
};

class LightSetup
{
	struct Struct80_7f
	{
		unsigned long ulong_a;
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		float float_h;
		float float_i;
		float float_j;
		unsigned long ulong_b;
		long l_a;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long ulong_i;
	} m_80bytes;

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	char m_junk[16] = { 0 };

	struct Struct32_6f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};

	std::vector<Struct32_6f> m_32bytes;

	struct Struct48_8f
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
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
	};

	std::vector<Struct48_8f> m_48bytes;

public:
	LightSetup(FILE* inFile);
	void create(FILE* outFile);
};

class Unk1SpriteSetup
{
	bool m_used = false;

public:
	bool used() { return m_used; }
	void read(FILE* inFile);
	void create(FILE* outFile);
};
class Unk2SpriteSetup
{
	bool m_used = false;

public:
	bool used() { return m_used; }
	void read(FILE* inFile);
	void create(FILE* outFile);
};

class FixedSprite;

class FixedSpriteSetup
{
	bool m_used = false;
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	char m_junk[16] = { 0 };

	struct Struct80_7f
	{
		unsigned long ulong_a;
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		float float_h;
		float float_i;
		float float_j;
		float float_k;
		float float_l;
		unsigned long ulong_b;
		float float_m;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		unsigned long ulong_g;
	};

	std::vector<Struct80_7f> m_80bytes;

	std::vector<FixedSprite> m_fixedSprites;

public:
	bool used() { return m_used; }
	void read(FILE* inFile);
	void create(FILE* outFile);
};

class FixedSprite
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	char m_junk[16] = { 0 };

	struct Struct64_7f
	{
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
		unsigned long ulong_k;
		unsigned long ulong_l;
		unsigned long ulong_m;
		unsigned long ulong_n;
		unsigned long ulong_o;
		unsigned long ulong_p;
	} m_64bytes;

	struct Struct48_8f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
	};

	std::vector<Struct48_8f> m_48bytes;

	struct Struct32_6f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};

	std::vector<Struct32_6f> m_32Pair_1;

	std::vector<Struct32_6f> m_32Pair_2;

public:
	FixedSprite(FILE* inFile);
	void create(FILE* outFile);
};

class SpritesSetup
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	char m_junk[16] = { 0 };

	unsigned long m_unused;

	FixedSpriteSetup m_fixedSpriteSetup;

	Unk1SpriteSetup m_unk1SpriteSetup;

	Unk2SpriteSetup m_unk2SpriteSetup;

public:
	void read(FILE* inFile);
	void create(FILE* outFile);
};

class TexAnim;

class PSetup
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	char m_junk[16] = { 0 };

	unsigned long m_numMystery;

public:
	void read(FILE* inFile);
	void create(FILE* outFile);
};


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

	char m_junk[16] = { 0 };

	std::vector<IMXEntry> m_IMXentries;

	std::vector<XGEntry> m_XGentries;

	std::vector<std::shared_ptr<ModelSetup>> m_modelSetups;

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
	SSQ(std::string filename, bool useBanner = true);
	SSQ(const SSQ&) = default;
	bool create(std::string filename, bool trueSave = true);

	bool write_to_txt() { return false; }

	bool menu(bool nextFile, const std::pair<bool, const char*> nextExtension);
	bool functionSelection(const char choice, bool isMulti);
	static void displayMultiChoices();
	static void displayMultiHelp();
	static const std::string multiChoiceString;
};

class ModelSetup
{
	struct Struct32_5f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
	};
	struct Struct32_6f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};
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
	struct Struct64_9f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		float float_g;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		float float_h;
		float float_i;
		unsigned long ulong_g;
	};
protected:
	char* m_name;

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	char m_junk[16] = { 0 };

	struct ModelPosition
	{
		float m_frame;
		float m_distance;
		float m_xCoord;
		float m_yCoord;
		float m_zCoord;
		unsigned long m_isMoving;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
		unsigned long ulong_c;
	};

	std::vector<ModelPosition> m_32Pair_1;

	struct ModelRotation
	{
		float m_frame;
		float m_distance;
		float m_pitch;
		float m_yaw;
		float m_roll;
		float m_perspective;
		unsigned long m_isMoving;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
	};

	std::vector<ModelRotation> m_32Pair_2;

	std::vector<Struct48_2f> m_48bytes;

	std::vector<Struct32_5f> m_32bytes_Opt;

	Struct64_9f m_64bytes_Opt;

public:
	ModelSetup(FILE* inFile, char (&name)[16]);
	virtual void create(FILE* outFile);
};

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
		unsigned long m_size;
		std::vector<unsigned long> m_vals;
	};

	std::vector<Read4Entry> m_player_read4Entry;

	std::vector<unsigned long> m_player_ulongs;

public:
	PlayerModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile);
};

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

class SnakeModelSetup : public ModelSetup
{
public:
	SnakeModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile);
};

class TexAnim
{
	// 
	unsigned long m_headerVersion;

	char m_unk1[12] = { 0 };

	char m_junk[16] = { 0 };

	unsigned long m_unk2;

	unsigned long m_unk3;

	char m_texture[24] = { 0 };

	struct Struct16_4f
	{
		float float_a;
		float float_b;
		float float_c;
		float float_d;
	};

	std::vector<Struct16_4f> m_16Pair_1;

	struct Struct16_2f
	{
		float float_a;
		float float_b;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};

	std::vector<Struct16_2f> m_16Pair_2;

public:
	TexAnim(FILE* inFile);
	void create(FILE* outFile);
};
