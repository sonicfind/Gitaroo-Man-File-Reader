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
#include <glm/gtx/quaternion.hpp>
union Val
{
	char c[4];
	float f;
	unsigned long ul;
	long l;
};

struct Frame
{
	float frame;
	float coefficient;
};

struct Position : public Frame
{
	glm::vec3 position;
	unsigned long doInterpolation;
	// Essentially frame * 160
	unsigned long otherPos;
	unsigned long ulong_c;
};

struct Rotation : public Frame
{
	glm::quat rotation;
	unsigned long doInterpolation;
	// Essentially frame * 160
	unsigned long otherPos;
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

template <typename T>
auto getIter(const std::vector<T>& vect, const float time)
{
	auto iter = std::lower_bound(vect.begin(), vect.end(), time, [](const T& a, const float b) { return a.frame < b; });
	if (iter != vect.begin())
		--iter;
	return iter;
}

template <typename T>
T mix(const T a, const T b, const float coefficient)
{
	return T(a + (b - a) * coefficient);
}

void flipHand(std::vector<Rotation>& rotations);

class GameState
{
	enum class Mode
	{
		Attack_Charge,
		Guard
	};

	class Player
	{
		int playerEvent = 0;
		int eventDescriptor = 0;
		float angle = 0;
	public:
		void set(int mode, int playerIndex);
		int getEvent() const { return playerEvent; }
		int getDescriptor() const { return eventDescriptor; }
		float getAngle() const { return angle; }
	};

	Mode m_mode = Mode::Attack_Charge;
	bool m_activeModels[10];
	Player m_players[4];

public:
	void reset();
	void setGlobalStates();
	bool isModelTypeActive(int model) { return m_activeModels[model]; }
	Player getPlayerState(int player) { return m_players[player]; }
};
extern GameState g_gameState;
