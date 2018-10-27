/***********************************************************************************
* Copyright 2018 Marcos Sánchez Torrent                                            *
*                                                                                  *
* Licensed under the Apache License, Version 2.0 (the "License");                  *
* you may not use this file except in compliance with the License.                 *
* You may obtain a copy of the License at                                          *
*                                                                                  *
* http://www.apache.org/licenses/LICENSE-2.0                                       *
*                                                                                  *
* Unless required by applicable law or agreed to in writing, software              *
* distributed under the License is distributed on an "AS IS" BASIS,                *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         *
* See the License for the specific language governing permissions and              *
* limitations under the License.                                                   *
***********************************************************************************/

#pragma once

#ifndef GAF_KEYBOARD_H
#define GAF_KEYBOARD_H 1

namespace gaf
{
	namespace EKey
	{
		enum Type
		{
			BACKSPACE,
			TAB,
			CLEAR,
			ENTER,
			SHIFT,
			CTRL,
			ALT,
			PAUSE,
			CAPSLOCK,
			ESCAPE,
			SPACE,
			PAGEUP,
			PAGEDOWN,
			END,
			HOME,
			LEFT_ARROW,
			UP_ARROW,
			RIGHT_ARROW,
			DOWN_ARROW,
			SELECT,
			PRINT,
			EXECUTE,
			PRINTSCREEN,
			INS,
			DEL,
			HELP,
			N0,
			N1,
			N2,
			N3,
			N4,
			N5,
			N6,
			N7,
			N8,
			N9,
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			Q,
			R,
			S,
			T,
			U,
			V,
			W,
			X,
			Y,
			Z,
			LSYS,
			RSYS,
			APPS,
			SLEEP,
			NP0,
			NP1,
			NP2,
			NP3,
			NP4,
			NP5,
			NP6,
			NP7,
			NP8,
			NP9,
			NP_MULTIPLY,
			NP_ADD,
			NP_SEPARATOR,
			NP_SUBSTRACT,
			NP_DECIMAL,
			NP_DIVIDE,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			F13,
			F14,
			F15,
			F16,
			F17,
			F18,
			F19,
			F20,
			F21,
			F22,
			F23,
			F24,
			NP_NUMLOCK,
			SCROLL,
			LSHIFT,
			RSHIFT,
			LCTRL,
			RCTRL,
			LALT,
			RALT,
			US_SEMICOLON,
			PLUS,
			COMMA,
			MINUS,
			PERIOD,
			US_FWSLASH,
			US_TILDE,
			US_LBRACKET,
			US_BACKSLASH,
			US_RBRACKET,
			US_SINGLEQUOTE,
			OEM_8,
			US_LESSTHAN,

			NUM_KEYS
		};
	}
	const ANSICHAR* GetCharFromKey(EKey::Type key);
	
	enum KeyModifiers
	{
		CTRL,
		SHIFT,
		ALT,
	};

	struct KeyboardState
	{
		struct KeyStateInfo
		{
			InputState State[EKey::NUM_KEYS];
		}KeyInfo;

		int32 ModifierKeys;
		EKey::Type LastKeyModified;

		bool IsCTRLPressed()const;
		bool IsAltPressed()const;
		bool IsShiftPressed()const;

		void Reset();
		bool HasFocus()const;

		KeyboardState();
		~KeyboardState() = default;
		KeyboardState(const KeyboardState& other);
		KeyboardState(KeyboardState&& other)noexcept;
		KeyboardState& operator=(const KeyboardState& other);
		KeyboardState& operator=(KeyboardState&& other)noexcept;
	};
}

#endif /* GAF_KEYBOARD_H */
