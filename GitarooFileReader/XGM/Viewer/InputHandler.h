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
#include <GLFW/glfw3.h>

namespace InputHandling
{
	enum class KeyStatus
	{
		Unheld,
		Pressed,
		DelayToHold,
		Held,
		Heldtick,
		Released
	};

	struct KeyPair
	{
		int m_glfwValue;
		KeyStatus m_status = KeyStatus::Unheld;
		float m_pressStartTime = 0;
		bool isHeld() { return m_status == KeyStatus::Held; }
		bool isUnHeld() { return m_status == KeyStatus::Unheld; }
		bool isPressed() { return m_status == KeyStatus::Pressed; }
		bool isReleased() { return m_status == KeyStatus::Released; }
		bool isDelayed() { return m_status == KeyStatus::DelayToHold; }
		bool isTicked() { return isPressed() || m_status == KeyStatus::Heldtick; }
		bool isActive() { return isHeld() || isTicked() || isDelayed(); }
	};

	struct Keyboard
	{
		KeyPair KEY_SPACE =			{ 32 };
		KeyPair KEY_APOSTROPHE =	{ 39 };
		KeyPair KEY_COMMA =			{ 44 };
		KeyPair KEY_MINUS =			{ 45 };
		KeyPair KEY_PERIOD =		{ 46 };
		KeyPair KEY_SLASH =			{ 47 };
		KeyPair KEY_0 =				{ 48 };
		KeyPair KEY_1 =				{ 49 };
		KeyPair KEY_2 =				{ 50 };
		KeyPair KEY_3 =				{ 51 };
		KeyPair KEY_4 =				{ 52 };
		KeyPair KEY_5 =				{ 53 };
		KeyPair KEY_6 =				{ 54 };
		KeyPair KEY_7 =				{ 55 };
		KeyPair KEY_8 =				{ 56 };
		KeyPair KEY_9 =				{ 57 };
		KeyPair KEY_SEMICOLON =		{ 59 };
		KeyPair KEY_EQUAL =			{ 61 };
		KeyPair KEY_A =				{ 65 };
		KeyPair KEY_B =				{ 66 };
		KeyPair KEY_C =				{ 67 };
		KeyPair KEY_D =				{ 68 };
		KeyPair KEY_E =				{ 69 };
		KeyPair KEY_F =				{ 70 };
		KeyPair KEY_G =				{ 71 };
		KeyPair KEY_H =				{ 72 };
		KeyPair KEY_I =				{ 73 };
		KeyPair KEY_J =				{ 74 };
		KeyPair KEY_K =				{ 75 };
		KeyPair KEY_L =				{ 76 };
		KeyPair KEY_M =				{ 77 };
		KeyPair KEY_N =				{ 78 };
		KeyPair KEY_O =				{ 79 };
		KeyPair KEY_P =				{ 80 };
		KeyPair KEY_Q =				{ 81 };
		KeyPair KEY_R =				{ 82 };
		KeyPair KEY_S =				{ 83 };
		KeyPair KEY_T =				{ 84 };
		KeyPair KEY_U =				{ 85 };
		KeyPair KEY_V =				{ 86 };
		KeyPair KEY_W =				{ 87 };
		KeyPair KEY_X =				{ 88 };
		KeyPair KEY_Y =				{ 89 };
		KeyPair KEY_Z =				{ 90 };
		KeyPair KEY_LEFT_BRACKET =	{ 91 };
		KeyPair KEY_BACKSLASH =		{ 92 };
		KeyPair KEY_RIGHT_BRACKET =	{ 93 };
		KeyPair KEY_GRAVE_ACCENT =	{ 96 };
		KeyPair KEY_WORLD_1 =		{ 161 }; /* non-US #1 */
		KeyPair KEY_WORLD_2 =		{ 162 }; /* non-US #2 */

		/* Function keys */
		KeyPair KEY_ESCAPE =		{ 256 };
		KeyPair KEY_ENTER =			{ 257 };
		KeyPair KEY_TAB =			{ 258 };
		KeyPair KEY_BACKSPACE =		{ 259 };
		KeyPair KEY_INSERT =		{ 260 };
		KeyPair KEY_DELETE =		{ 261 };
		KeyPair KEY_RIGHT =			{ 262 };
		KeyPair KEY_LEFT =			{ 263 };
		KeyPair KEY_DOWN =			{ 264 };
		KeyPair KEY_UP =			{ 265 };
		KeyPair KEY_PAGE_UP =		{ 266 };
		KeyPair KEY_PAGE_DOWN =		{ 267 };
		KeyPair KEY_HOME =			{ 268 };
		KeyPair KEY_END =			{ 269 };
		KeyPair KEY_CAPS_LOCK =		{ 280 };
		KeyPair KEY_SCROLL_LOCK =	{ 281 };
		KeyPair KEY_NUKEY_LOCK =	{ 282 };
		KeyPair KEY_PRINT_SCREEN =	{ 283 };
		KeyPair KEY_PAUSE =			{ 284 };
		KeyPair KEY_F1 =			{ 290 };
		KeyPair KEY_F2 =			{ 291 };
		KeyPair KEY_F3 =			{ 292 };
		KeyPair KEY_F4 =			{ 293 };
		KeyPair KEY_F5 =			{ 294 };
		KeyPair KEY_F6 =			{ 295 };
		KeyPair KEY_F7 =			{ 296 };
		KeyPair KEY_F8 =			{ 297 };
		KeyPair KEY_F9 =			{ 298 };
		KeyPair KEY_F10 =			{ 299 };
		KeyPair KEY_F11 =			{ 300 };
		KeyPair KEY_F12 =			{ 301 };
		KeyPair KEY_F13 =			{ 302 };
		KeyPair KEY_F14 =			{ 303 };
		KeyPair KEY_F15 =			{ 304 };
		KeyPair KEY_F16 =			{ 305 };
		KeyPair KEY_F17 =			{ 306 };
		KeyPair KEY_F18 =			{ 307 };
		KeyPair KEY_F19 =			{ 308 };
		KeyPair KEY_F20 =			{ 309 };
		KeyPair KEY_F21 =			{ 310 };
		KeyPair KEY_F22 =			{ 311 };
		KeyPair KEY_F23 =			{ 312 };
		KeyPair KEY_F24 =			{ 313 };
		KeyPair KEY_F25 =			{ 314 };
		KeyPair KEY_KP_0 =			{ 320 };
		KeyPair KEY_KP_1 =			{ 321 };
		KeyPair KEY_KP_2 =			{ 322 };
		KeyPair KEY_KP_3 =			{ 323 };
		KeyPair KEY_KP_4 =			{ 324 };
		KeyPair KEY_KP_5 =			{ 325 };
		KeyPair KEY_KP_6 =			{ 326 };
		KeyPair KEY_KP_7 =			{ 327 };
		KeyPair KEY_KP_8 =			{ 328 };
		KeyPair KEY_KP_9 =			{ 329 };
		KeyPair KEY_KP_DECIMAL =	{ 330 };
		KeyPair KEY_KP_DIVIDE =		{ 331 };
		KeyPair KEY_KP_MULTIPLY =	{ 332 };
		KeyPair KEY_KP_SUBTRACT =	{ 333 };
		KeyPair KEY_KP_ADD =		{ 334 };
		KeyPair KEY_KP_ENTER =		{ 335 };
		KeyPair KEY_KP_EQUAL =		{ 336 };
		KeyPair KEY_LEFT_SHIFT =	{ 340 };
		KeyPair KEY_LEFT_CONTROL =	{ 341 };
		KeyPair KEY_LEFT_ALT =		{ 342 };
		KeyPair KEY_LEFT_SUPER =	{ 343 };
		KeyPair KEY_RIGHT_SHIFT =	{ 344 };
		KeyPair KEY_RIGHT_CONTROL =	{ 345 };
		KeyPair KEY_RIGHT_ALT =		{ 346 };
		KeyPair KEY_RIGHT_SUPER =	{ 347 };
		KeyPair KEY_MENU =			{ 348 };
	};
	extern Keyboard g_input_keyboard;

	void processKey(GLFWwindow* window, KeyPair* key, const float time);
	void processInputs(GLFWwindow* window, const float time);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void resetInputs();
}