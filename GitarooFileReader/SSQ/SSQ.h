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

union Val
{
	char c[4];
	float f;
	unsigned long ul;
	long l;
};

struct IMXEntry
{
	char m_name[16] = { 0 };
	unsigned long m_unused_1;
	unsigned long m_unused_2;
	Val m_unused_3[2] = { 0 };

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
	char m_name[16] = { 0 };
	unsigned long m_isClone;
	unsigned long m_cloneID;
	unsigned long m_unused_1;
	ModelType m_type = ModelType::Normal;
	float m_length;
	float m_speed;
	float m_framerate;
	char m_junk[4];
	XGEntry(FILE* inFile);
	void create(FILE* outFile);
};

struct Frame
{
	float m_frame;
	float m_coefficient;
};

struct Position : public Frame
{
	float m_position[3];
	unsigned long m_doInterpolation;
	// Essentially m_frame * 160
	unsigned long m_otherPos;
	unsigned long ulong_c;
};

struct Rotation : public Frame
{
	float m_rotation[4];
	unsigned long m_doInterpolation;
	// Essentially m_frame * 160
	unsigned long m_otherPos;
};

class ModelSetup;
class PlayerModelSetup;
class AttDefModelSetup;
class SnakeModelSetup;

class LightSetup;

class CameraSetup
{
public:
	struct BaseGlobalValues
	{
		float m_clearColor[4];
		unsigned long m_baseAmbience[3];
		unsigned long m_useDiffuse;
		unsigned long m_vertColorDiffuse[3];
		float m_fov;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;
		unsigned long ulong_h;
	};

	struct Projection : public Frame
	{
		float m_fov;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;
		unsigned long ulong_a;
		unsigned long m_doInterpolation;
	};

	struct AmbientColor : public Frame
	{
		float m_color[3];
		unsigned long m_doInterpolation;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
		// Seems to match m_otherPos
		unsigned long ulong_c;
	};

	struct Struct64_7f : public Frame
	{
		unsigned long m_cameraRelated;
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
		unsigned long m_doInterpolation;
	};
private:

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	Val m_junk[4] = { 0 };

public:
	BaseGlobalValues m_baseGlobalValues;

private:
	std::vector<Position> m_positions;
	std::vector<Rotation> m_rotations;
	std::vector<Projection> m_projections;
	std::vector<AmbientColor> m_ambientColors;
	std::vector<LightSetup> m_lights;
	std::vector<Struct64_7f> m_64bytes_v;

public:
	void read(FILE* inFile);
	void create(FILE* outFile);
};

class LightSetup
{
	struct BaseValues
	{
		unsigned long m_isActive;
		float m_diffuse[3];
		float m_specular[3];
		float m_rotation[4];
		Val ulong_b;
		Val l_a;
		Val ulong_c;
		Val ulong_d;
		Val ulong_e;
		Val ulong_f;
		Val ulong_g;
		Val ulong_h;
		Val ulong_i;
	} m_baseValues;

	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_size;

	char m_unk[8] = { 0 };

	Val m_junk[4] = { 0 };

	std::vector<Rotation> m_rotations;

	struct LightColors : public Frame
	{
		float m_diffuse[3];
		float m_specular[3];
		unsigned long m_doInterpolation;
		unsigned long ulong_b;
		float float_a;
		unsigned long ulong_d;
	};

	std::vector<LightColors> m_colors;

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

	Val m_junk[4] = { 0 };

	struct Struct80_7f
	{
		unsigned long m_IMXindex;
		float m_initial_BottmLeft_X;
		float m_initial_BottmLeft_Y;
		float m_boxSize_X;
		float m_boxSize_Y;
		float m_worldPosition_X;
		float m_worldPosition_Y;
		float m_worldPosition_Z;
		float m_worldScale_X;
		float m_worldScale_Y;
		float m_worldScale_Z_maybe;
		float m_worldScale_W_maybe;
		float float_l;
		unsigned long ulong_b;
		float float_m;
		unsigned long ulong_c;
		unsigned long m_depthTest;
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

	Val m_junk[4] = { 0 };

	struct Struct64_7f
	{
		unsigned long m_IMXEntryIndex;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		unsigned long m_transparent;
		unsigned long ulong_f;
		unsigned long ulong_g;
		unsigned long ulong_h;
		unsigned long m_transparent_2;
		unsigned long m_mipmapDepth;
		unsigned long ulong_k;
		unsigned long ulong_l;
		unsigned long ulong_m;
		unsigned long ulong_n;
		unsigned long ulong_o;
		unsigned long ulong_p;
	} m_64bytes;

	struct Struct48_8f : public Frame
	{
		float m_position[3];
		float m_worldScale_X;
		float m_worldScale_Y;
		unsigned long ulong_a;
		unsigned long ulong_b;
		unsigned long ulong_c;
		unsigned long ulong_d;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
	};

	std::vector<Struct48_8f> m_48bytes;

	struct Struct32_6f : public Frame
	{
		float float_c;
		float float_d;
		float float_e;
		float float_f;
		unsigned long ulong_a;
		unsigned long ulong_b;
	};

	std::vector<Struct32_6f> m_32Pair_1;

	struct SpriteFrame : public Frame
	{
		float m_initial_BottmLeft_X;
		float m_initial_BottmLeft_Y;
		float m_boxSize_X;
		float m_boxSize_Y;
		unsigned long ulong_a;
		// Essentially m_frame * 160
		unsigned long m_otherPos;
	};

	std::vector<SpriteFrame> m_spriteFrames;

public:
	FixedSprite(FILE* inFile);
	void create(FILE* outFile);
};

class SpritesSetup
{
	// 
	unsigned long m_headerVersion;

	char m_unk[12] = { 0 };

	Val m_junk[4] = { 0 };

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

	Val m_junk[4] = { 0 };

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

class ModelSetup
{
public:
	struct ModelAnim : public Frame
	{
		unsigned long m_animIndex;
		unsigned long m_startOverride;
		unsigned long m_noDrawing;
		unsigned long m_firstAnimofSection_maybe;
		unsigned long m_loop;
		unsigned long ulong_f;
		unsigned long m_holdLastFrame;
		unsigned long ulong_h;
		unsigned long m_unknown;
		unsigned long m_otherPos;
	};

	struct ModelScalar : public Frame
	{
		float m_scalar[3];
		unsigned long m_envMap_maybe;
		unsigned long m_doInterpolation;
		unsigned long ulong_b;
	};

	struct BaseValues
	{
		float m_basePosition[3];
		float m_baseRotation[4];
		unsigned long m_baseAnimIndex_maybe;
		unsigned long ulong_b;
		unsigned long m_depthTest;
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
	Val m_junk[4] = { 0 };
	std::vector<Position> m_positions;
	std::vector<Rotation> m_rotations;
	std::vector<ModelAnim> m_animations;
	std::vector<ModelScalar> m_scalars;
	BaseValues m_baseValues;

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
		unsigned long m_size = 0;
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
public:
	struct CutOut
	{
		float m_topLeft_X;
		float m_topLeft_Y;
		float m_bottomRight_X;
		float m_bottomRight_Y;
	};

	struct TexFrame : public Frame
	{
		unsigned long m_cutOutIndex;
		unsigned long m_unknown;
	};
private:
	// 
	unsigned long m_headerVersion;

	char m_unk1[12] = { 0 };

	Val m_junk[4] = { 0 };

	unsigned long m_offset_X;

	unsigned long m_offset_Y;

	char m_texture[24] = { 0 };

public:
	std::vector<CutOut> m_cutOuts;
	std::vector<TexFrame> m_textureFrames;

	TexAnim(FILE* inFile);
	void create(FILE* outFile);
};
