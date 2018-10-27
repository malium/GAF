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

#ifndef GAF_MOUSE_H
#define GAF_MOUSE_H 1

namespace gaf
{
	enum InputState : bool
	{
		UP = false,
		DOWN = true
	};

	namespace EMouseButtons
	{
		enum Type
		{
			LEFT_BUTTON,
			RIGHT_BUTTON,
			MIDDLE_BUTTON,
			MB_XBUTTON1,
			MB_XBUTTON2,
			MB_XBUTTON3,
			MB_XBUTTON4,
			MB_XBUTTON5,
			MB_XBUTTON6,
			MB_XBUTTON7,
			MB_XBUTTON8,
			MB_XBUTTON9,
			MB_XBUTTON10,
			MB_XBUTTON11,
			MB_XBUTTON12,
			MB_XBUTTON13,

			MAX_BUTTONS
		};
	}

	namespace EMouseWheels
	{
		enum Type
		{
			VERTICAL,
			HORIZONTAL,

			NUM_WHEELS
		};
	}

	namespace EMouseEvent
	{
		enum Type
		{
			POSITION_CHAGE,
			BUTTON_STATE_CHANGE,
			WHEEL_CHANGE
		};
	}

	struct MouseState
	{
		struct MouseButtonInfo
		{
			InputState State[EMouseButtons::MAX_BUTTONS];
		}ButtonInfo;

		struct MouseWheelInfo
		{
			int16 Delta[EMouseWheels::NUM_WHEELS];
		}WheelInfo;

		struct MousePositionInfo
		{
			int32 X;
			int32 Y;
		}Position, DerivedPosition, PositionDelta;

		EMouseEvent::Type MouseEvent;
		EMouseButtons::Type EventButton;

		MouseState();
		~MouseState() = default;
		MouseState(const MouseState& other);
		MouseState(MouseState&& other)noexcept;
		MouseState& operator=(const MouseState& other);
		MouseState& operator=(MouseState&& other)noexcept;
	};
}

#endif /* GAF_MOUSE_H */
