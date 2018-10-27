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

#include "GAF/InputManager.h"

using namespace gaf;

MouseState::MouseState()
{
	memset(this, 0, sizeof(MouseState));
}

MouseState::MouseState(const MouseState & other)
{
	memcpy(this, &other, sizeof(MouseState));
}

MouseState::MouseState(MouseState && other) noexcept
{
	memcpy(this, &other, sizeof(MouseState));
	memset(&other, 0, sizeof(MouseState));
}

MouseState & MouseState::operator=(const MouseState & other)
{
	if (this != &other)
	{
		memcpy(this, &other, sizeof(MouseState));
	}
	return *this;
}

MouseState & MouseState::operator=(MouseState && other) noexcept
{
	if (this != &other)
	{
		memcpy(this, &other, sizeof(MouseState));
		memset(&other, 0, sizeof(MouseState));
	}
	return *this;
}