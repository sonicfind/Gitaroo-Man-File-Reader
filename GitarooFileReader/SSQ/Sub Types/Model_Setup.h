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
		unsigned long animIndex;
		unsigned long startOverride;
		unsigned long noDrawing;
		unsigned long pollGameState;
		unsigned long loop;
		unsigned long ulong_f;
		unsigned long holdLastFrame;
		unsigned long dropShadow;
		unsigned long unknown;
		unsigned long otherPos;
	};

	struct ModelScalar : public Frame
	{
		glm::vec3 scalar;
		unsigned long envMap_maybe;
		unsigned long doInterpolation;
		unsigned long ulong_b;
	};

	struct BaseValues
	{
		glm::vec3 basePosition;
		glm::quat baseRotation;
		unsigned long baseAnimIndex_maybe;
		unsigned long ulong_b;
		unsigned long depthTest;
		unsigned long ulong_d;
		unsigned long ulong_e;
		unsigned long ulong_f;
		float float_h;
		float float_i;
		unsigned long ulong_g;
	};
	
	ModelType m_type = ModelType::Normal;
	unsigned long m_headerVersion;
	// Maybe?
	unsigned long m_controllableIndex;
	float m_bpmStartFrame;
	float m_controllableStartFrame;
	// This array could possibly be the location for the 32bit pointers to the below variables
	//
	// Not applicable in the same way for a 64bit application
	Val m_junk[4] = { 0 };
	XG* m_xg;
	glm::mat4& m_matrix;

	std::vector<Position> m_positions;
	std::vector<Rotation> m_rotations;
	std::vector<ModelAnim> m_animations;
	std::vector<ModelScalar> m_scalars;
	BaseValues m_baseValues;

public:
	ModelSetup(FILE* inFile, ModelType type, glm::mat4& mat);
	virtual void create(FILE* outFile) const;
	void bindXG(XG* xg);

	void reset();
	void setBPMFrame(const float frame);
	bool animate(const float frame);
	void updateMatrix(const float frame) const;
	std::pair<bool, ModelAnim*> getAnim(const float frame);


protected:
	virtual void animateFromGameState(const float frame) {}
};

class PlayerModelSetup : public ModelSetup
{
	struct Controllable
	{
		float angleMin;
		float angleMax;
		unsigned long descriptor;
		unsigned long eventFlag;
		unsigned long animIndex;
		// Length of time to hold onto the last frame of animation
		unsigned long holdTime;
		// 1 - forwards; 0 - backwards
		unsigned long playbackDirection;
		unsigned long interruptible;
		unsigned long useCurrentFrame_maybe;
		unsigned long randomize;
		Val junk[2];
	};

	unsigned long m_numControllables;
	std::vector<Controllable> m_controllables;

	struct Connection
	{
		unsigned long size = 0;
		std::vector<unsigned long> controllableList;
	};

	std::vector<Connection> m_connections;

	std::vector<long> m_defaults;

public:
	PlayerModelSetup(FILE* inFile, ModelType type, glm::mat4& mat);
	void create(FILE* outFile) const;

private:
	virtual void animateFromGameState(const float frame);
};

class AttDefModelSetup : public ModelSetup
{
	float m_attackSize_Z;

	struct AttackStreamValues
	{
		glm::vec3 startOffset;
		glm::vec3 targetOffset;
		char startingModel[16];
		char targetModel[16];
		// Might've been the 32bit pointers to the model matrices
		// that correspond to the two models provided here
		//
		// No longer applicable in this 64 bit application
		char junk[8];
	} m_attackValues;

	glm::mat4* m_startMatrix;
	glm::mat4* m_targetMatrix;

public:
	AttDefModelSetup(FILE* inFile, ModelType type, glm::mat4& mat);
	std::vector<std::string> getConnectedNames() const;
	void setConnectedMatrices(glm::mat4* mat_1, glm::mat4* mat_2);
	void create(FILE* outFile) const;

private:
	virtual void animateFromGameState(const float frame);
};

class SnakeModelSetup : public ModelSetup
{
public:
	SnakeModelSetup(FILE* inFile, ModelType type, glm::mat4& mat);
	void create(FILE* outFile) const;
};
