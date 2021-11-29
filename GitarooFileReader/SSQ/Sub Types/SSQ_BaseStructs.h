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

template <class T>
struct IteratedVector
{
	std::vector<T> m_vect;
	std::vector<T>::const_iterator m_iter;
	void fill(unsigned long numElements, FILE* inFile)
	{
		m_vect.reserve(numElements);
		m_vect.resize(numElements);
		fread(m_vect.data(), sizeof(T), numElements, inFile);
	}

	inline void write(FILE* outFile) const
	{
		fwrite(m_vect.data(), sizeof(T), m_vect.size(), outFile);
	}

	void write_conditioned(FILE* outFile) const
	{
		unsigned long size = (unsigned long)m_vect.size();
		if (!size)
			size = 1;
		fwrite(&size, 4, 1, outFile);
		if (size > 1)
			fwrite(m_vect.data(), sizeof(T), size, outFile);
	}

	void jump(float time, bool loop = false)
	{
		if (m_vect.size())
		{
			if (loop)
				time = fmod(time, m_vect.back().frame);
			m_iter = std::lower_bound(m_vect.begin(), m_vect.end(), time, [](const T& a, const float b) { return a.frame < b; });
			if (m_iter != m_vect.begin())
				--m_iter;
		}
	}

	// Like jump() but it only checks odd nodes
	// Only to be used with light rotations
	void jump_skip(const float time)
	{
		if (m_vect.size())
		{
			// Binary search tree code structure copied from xutility
			long count = (long)m_vect.size();
			long first = 0;
			while (0 < count) { // divide and conquer, find half that contains answer
				long count2 = count / 2;
				if (count2 & 1)
					--count2;

				if (m_vect[first + count2].frame < time) { // try top half
					first = count2 + 2;
					count -= count2 + 2;
				}
				else {
					count = count2;
				}
			}

			if (first > 0)
				first -= 2;
			m_iter = m_vect.begin() + first;
		}
	}

	std::vector<T>::const_iterator update(float time, bool loop = false)
	{
		if (loop)
		{
			const float total = m_vect.back().frame;
			if (time >= total)
			{
				time = fmod(time, total);
				m_iter = m_vect.begin();
			}
		}

		while (m_iter + 1 != m_vect.end() && (m_iter + 1)->frame < time)
			++m_iter;
		return m_iter;
	}

	// Like update() but it skips every other node
	// Only to be used with light rotations
	std::vector<T>::const_iterator update_skip(const float time)
	{
		while (m_iter + 2 != m_vect.end() && (m_iter + 2)->frame < time)
			m_iter += 2;
		return m_iter;
	}

	inline bool empty() const
	{
		return m_vect.empty();
	}

	inline size_t size() const
	{
		return m_vect.size();
	}

	inline std::vector<T>::const_iterator begin() const
	{
		return m_vect.begin();
	}

	inline std::vector<T>::const_iterator end() const
	{
		return m_vect.end();
	}

	inline const T& front() const
	{
		return m_vect.front();
	}

	inline const T& back() const
	{
		return m_vect.back();
	}

	inline T& front()
	{
		return m_vect.front();
	}

	inline T& back()
	{
		return m_vect.back();
	}
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
		void set(int mode, int playerIndex, int padIndex);
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
