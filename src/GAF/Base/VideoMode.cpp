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

#include "GAF/WindowManager.h"
#include "..\WindowUtil.h"

using namespace gaf;

VideoMode::VideoMode()
	:m_Resolution{ 0,0 }
	, m_PixelDepth(0)
	, m_Frequency(0)
	, m_Adapter(nullptr)
{

}

VideoMode::VideoMode(const Resolution_t res, const uint8 depth, const uint16 freq, DisplayAdapter* adapter)
	:m_Resolution(res)
	, m_PixelDepth(depth)
	, m_Frequency(freq)
	, m_Adapter(adapter)
{

}

void VideoMode::SetResolution(const Resolution_t res)
{
	m_Resolution = res;
}

void VideoMode::SetDepth(const uint8 depth)
{
	m_PixelDepth = depth;
}

void VideoMode::SetFrequency(const uint16 freq)
{
	m_Frequency = freq;
}

void VideoMode::SetAdapter(DisplayAdapter* adapter)
{
	m_Adapter = adapter;
}

void VideoMode::Set(const Resolution_t res, const uint8 depth, const uint16 freq, DisplayAdapter* adapter)
{
	m_Resolution = res;
	m_PixelDepth = depth;
	m_Frequency = freq;
	m_Adapter = adapter;
}

Resolution_t VideoMode::GetResolution() const
{
	return m_Resolution;
}

uint8 VideoMode::GetPixelDepth()const
{
	return m_PixelDepth;
}

IDisplayAdapter* VideoMode::GetAdapter()const
{
	return (IDisplayAdapter*)m_Adapter;
}

uint16 VideoMode::GetFrequency() const
{
	return m_Frequency;
}

bool VideoMode::Equals(IVideoMode* other)const
{
	Assertion::WhenNullptr(other, "Trying to check if two IVideoMode are equal, "
		"but the other was nullptr.");
	return m_Resolution == other->GetResolution()
		&& m_PixelDepth == other->GetPixelDepth()
		&& m_Frequency == other->GetFrequency()
		&& m_Adapter == other->GetAdapter();
}