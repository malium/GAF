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

#ifndef GAF_DAYTIME_H
#define GAF_DAYTIME_H 1

#include "GAF/GAFPrerequisites.h"

namespace gaf
{
	namespace EMonths
	{
		enum Type
		{
			January = 1,
			February,
			March,
			April,
			May,
			June,
			July,
			August,
			September,
			October,
			November,
			December
		};
	}

	namespace EWeekDays
	{
		enum Type
		{
			Monday = 1,
			Tuesday,
			Wednesday,
			Thursday,
			Friday,
			Saturday,
			Sunday
		};
	}

	const ANSICHAR* GetMonthStr(EMonths::Type month);
	const ANSICHAR* GetMonthStrShrt(EMonths::Type month);
	const ANSICHAR* GetWeekDayStr(EWeekDays::Type day);
	const ANSICHAR* GetWeekDayStrShrt(EWeekDays::Type day);

	class DayTime
	{
		uint16 m_Millisec;
		uint16 m_Seconds;
		uint16 m_Minutes;
		uint16 m_Hours;
		uint16 m_Day;
		uint16 m_Month;
		uint16 m_Year;

		void Update();
	public:
		DayTime();
		~DayTime() = default;
		DayTime(const DayTime&) = default;
		DayTime(DayTime&&)noexcept = default;
		DayTime& operator=(const DayTime&) = default;
		DayTime& operator=(DayTime&&)noexcept = default;

		DayTime(uint32 millisec, uint32 sec, uint32 min, uint32 hour, uint32 day, uint32 mon, uint16 year);
		explicit DayTime(const struct tm& rawTimeStruct);
		explicit DayTime(time_t rawTime);

		void Set(const struct tm& rawTimeStruct);
		void Set(time_t rawTime);
		void Set(uint32 millisec, uint32 sec, uint32 min, uint32 hour, uint32 day, uint32 mon, uint16 year);
		void SetMillisecs(uint32 millisecs);
		void SetSeconds(uint32 seconds);
		void SetMinutes(uint32 minutes);
		void SetHours(uint32 hours);
		void SetDay(uint32 days);
		void SetMonth(uint32 months);
		void SetYear(uint16 year);

		uint16 GetMillisec()const;
		uint8 GetSeconds()const;
		uint8 GetMinutes()const;
		uint8 GetHours()const;
		uint8 GetDays()const;
		uint8 GetMonths()const;
		uint16 GetYear()const;

		std::string ToString()const;

		bool IsLeapYear()const;

		struct tm GetRawStruct()const;
		time_t GetRawTime()const;

		void SetAsCurrent();
		
		static bool IsLeapYear(uint16 year);
		static DayTime GetCurrentDayTime();
		
		friend bool operator==(const DayTime& left, const DayTime& right);
		friend bool operator!=(const DayTime& left, const DayTime& right);
		friend bool operator<(const DayTime& left, const DayTime& right);
		friend bool operator>(const DayTime& left, const DayTime& right);
		friend bool operator<=(const DayTime& left, const DayTime& right);
		friend bool operator>=(const DayTime& left, const DayTime& right);

		void swap(DayTime& other)noexcept;
	};

	FORCEINLINE void swap(DayTime& a, DayTime& b)noexcept
	{
		a.swap(b);
	}
}

#endif /* GAF_DAYTIME_H */