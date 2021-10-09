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
#include "XG/XG.h"
class ModelSetup
{
protected:
	struct ModelAnim : public Frame
	{
		unsigned long m_animIndex;
		unsigned long m_startOverride;
		unsigned long m_noDrawing;
		unsigned long m_pollGameState;
		unsigned long m_loop;
		unsigned long ulong_f;
		unsigned long m_holdLastFrame;
		unsigned long m_dropShadow;
		unsigned long m_unknown;
		unsigned long m_otherPos;
	};

	struct ModelScalar : public Frame
	{
		glm::vec3 m_scalar;
		unsigned long m_envMap_maybe;
		unsigned long m_doInterpolation;
		unsigned long ulong_b;
	};

	struct BaseValues
	{
		glm::vec3 m_basePosition;
		glm::quat m_baseRotation;
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

	const char* m_name;
	
	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_controllableIndex;
	float m_bpmStartFrame;
	float m_controllableStartFrame;
	Val m_junk[4] = { 0 };
	std::vector<Position> m_positions;
	std::vector<Rotation> m_rotations;
	std::vector<ModelAnim> m_animations;
	std::vector<ModelScalar> m_scalars;
	BaseValues m_baseValues;

public:
	ModelSetup(FILE* inFile, char(&name)[16]);
	virtual void create(FILE* outFile) const;

	void reset();
	void setBPMFrame(const float frame);
	void animate(XG* xg, const float frame);
	glm::mat4 getModelMatrix(const float frame) const;
	std::pair<bool, ModelAnim*> getAnim(const float frame);


protected:
	virtual void animateFromGameState(XG* xg, const float frame) {}
};

class PlayerModelSetup : public ModelSetup
{
	struct Controllable
	{
		float m_angleMin;
		float m_angleMax;
		unsigned long m_descriptor;
		unsigned long m_eventFlag;
		unsigned long m_animIndex;
		// Length of time to hold onto the last frame of animation
		unsigned long m_holdTime;
		// 1 - forwards; 0 - backwards
		unsigned long m_playbackDirection;
		unsigned long m_interruptible;
		unsigned long m_useCurrentFrame_maybe;
		unsigned long m_randomize;
		Val m_junk[2];
	};

	unsigned long m_numControllables;
	std::vector<Controllable> m_controllables;

	struct Connection
	{
		unsigned long size = 0;
		std::vector<unsigned long> controllableList;
	};

	std::vector<Connection> m_connections;

	std::vector<long> m_endings;

public:
	PlayerModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;

private:
	virtual void animateFromGameState(XG* xg, const float frame);
};

class AttDefModelSetup : public ModelSetup
{
	float m_attackSize_Z;

	struct AttackStreamValues
	{
		glm::vec3 m_startOffset;
		glm::vec3 m_targetOffset;
		char m_startingModel[16];
		char m_targetModel[16];
		// Might've been the 32bit pointers to the model matrices
		// that correspond to the two models provided here
		//
		// No longer applicable in this 64 bit application
		char m_junk[8];
	} m_attackValues;

public:
	AttDefModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;

private:
	virtual void animateFromGameState(XG* xg, const float frame);
};

class SnakeModelSetup : public ModelSetup
{
public:
	SnakeModelSetup(FILE* inFile, char(&name)[16]);
	void create(FILE* outFile) const;
};
