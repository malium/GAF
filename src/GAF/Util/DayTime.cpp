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

#include "GAF/Util/DayTime.h"

using namespace gaf;

static const ANSICHAR* gMonthStr[] =
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

static const ANSICHAR* gMonthStrShrt[] =
{
	"JAN",
	"FEB",
	"MAR",
	"APR",
	"MAY",
	"JUN",
	"JUL",
	"AUG",
	"SEP",
	"OCT",
	"NOV",
	"DEC"
};

static const ANSICHAR* gWeekDayStr[] =
{
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday"
};

static const ANSICHAR* gWeekDayStrShrt[] =
{
	"MON",
	"TUE",
	"WED",
	"THU",
	"FRI",
	"SAT",
	"SUN"
};

const ANSICHAR* gaf::GetMonthStr(const EMonths::Type month)
{
	const auto id = static_cast<std::underlying_type<EMonths::Type>::type>(month) - 1;
	return gMonthStr[id];
}

const ANSICHAR* gaf::GetMonthStrShrt(const EMonths::Type month)
{
	const auto id = static_cast<std::underlying_type<EMonths::Type>::type>(month) - 1;
	return gMonthStrShrt[id];
}

const ANSICHAR* gaf::GetWeekDayStr(const EWeekDays::Type day)
{
	const auto id = static_cast<std::underlying_type<EWeekDays::Type>::type>(day) - 1;
	return gWeekDayStr[id];
}

const ANSICHAR* gaf::GetWeekDayStrShrt(const EWeekDays::Type day)
{
	const auto id = static_cast<std::underlying_type<EWeekDays::Type>::type>(day) - 1;
	return gWeekDayStrShrt[id];
}

bool gaf::operator==(const DayTime & left, const DayTime & right)
{
	return memcmp(&left, &right, sizeof(DayTime)) == 0;
}

bool gaf::operator!=(const DayTime & left, const DayTime & right)
{
	return !(left == right);
}

bool gaf::operator<(const DayTime & left, const DayTime & right)
{
	if (left.m_Year > right.m_Year)
		return true;
	if (left.m_Year < right.m_Year)
		return false;
	
	if (left.m_Month > right.m_Month) 
		return true;
	if (left.m_Month < right.m_Month)
		return false;
	
	if (left.m_Day > right.m_Day)
		return true; 
	if (left.m_Day < right.m_Day)
		return false;

	if (left.m_Hours > right.m_Hours)
		return true;
	if (left.m_Hours < right.m_Hours)
		return false;

	if (left.m_Minutes > right.m_Minutes)
		return true;
	if (left.m_Minutes < right.m_Minutes)
		return false;

	if (left.m_Seconds > right.m_Seconds)
		return true;
	if (left.m_Seconds < right.m_Seconds)
		return false;

	if (left.m_Millisec > right.m_Millisec)
		return true;
	if (left.m_Millisec < right.m_Millisec)
		return false;
	return true;
}

bool gaf::operator>(const DayTime & left, const DayTime & right)
{
	return right < left;
}

bool gaf::operator<=(const DayTime & left, const DayTime & right)
{
	return !(left > right);
}

bool gaf::operator>=(const DayTime & left, const DayTime & right)
{
	return !(left < right);
}

void DayTime::Update()
{
	if (m_Millisec >= 1000)
	{
		float intpart;
		modf(float(m_Millisec) / 1000.f, &intpart);
		m_Millisec -= uint32(intpart) * 1000;
		m_Seconds += uint32(intpart);
	}
	if (m_Seconds >= 60)
	{
		float intpart;
		modf(float(m_Seconds) / 60.f, &intpart);
		m_Seconds -= uint32(intpart) * 60;
		m_Minutes += uint32(intpart);
	}
	if (m_Minutes >= 60)
	{
		float intpart;
		modf(float(m_Minutes) / 60.f, &intpart);
		m_Minutes -= uint32(intpart) * 60;
		m_Hours += uint32(intpart);
	}
	if (m_Hours >= 24)
	{
		float intpart;
		modf(float(m_Hours) / 24.f, &intpart);
		m_Hours -= uint32(intpart) * 24;
		m_Day += uint32(intpart);
	}
	while (true)
	{
		if (m_Month > EMonths::December)
		{
			float intpart;
			modf(float(m_Month) / 12.f, &intpart);
			m_Month -= uint32(intpart) * 12;
			m_Year += uint32(intpart);
		}
		if (m_Day == 0)
			m_Day = 1;
		if (m_Day < 27)
			break;
		if (m_Month == EMonths::February)
		{
			const auto leap = IsLeapYear();
			if (m_Day == 28 && !leap)
			{
				m_Day -= 28;
				++m_Month;
			}
			else if (m_Day == 29 && leap)
			{
				m_Day -= 29;
				++m_Month;
			}
		}
		else if ((m_Month == EMonths::January
			|| m_Month == EMonths::March
			|| m_Month == EMonths::May
			|| m_Month == EMonths::July
			|| m_Month == EMonths::August
			|| m_Month == EMonths::October
			|| m_Month == EMonths::December) && m_Day == 31)
		{
			m_Day -= 31;
			++m_Month;
		}
		else if(m_Day == 30)/* 30-day Months */
		{
			m_Day -= 30;
			++m_Month;
		}
		else
		{
			break;
		}
	}
}

DayTime::DayTime()
{
	SetAsCurrent();
}

DayTime::DayTime(const uint32 millisec, const uint32 sec, const uint32 min, const uint32 hour, const uint32 day, const uint32 mon, const uint16 year)
	:m_Millisec(millisec)
	,m_Seconds(sec)
	,m_Minutes(min)
	,m_Hours(hour)
	,m_Day(day)
	,m_Month(mon)
	,m_Year(year)
{
	Update();
}

DayTime::DayTime(const tm& rawTimeStruct)
{
	Set(rawTimeStruct);
}

DayTime::DayTime(const time_t rawTime)
{
	Set(rawTime);
}

void DayTime::Set(const tm& rawTimeStruct)
{
	m_Millisec = 0;
	m_Seconds = rawTimeStruct.tm_sec;
	m_Minutes = rawTimeStruct.tm_min;
	m_Hours = rawTimeStruct.tm_hour;
	m_Day = rawTimeStruct.tm_mday;
	m_Month = rawTimeStruct.tm_mon + 1;
	m_Year = rawTimeStruct.tm_year + 1900;
}

void DayTime::Set(const time_t rawTime)
{
	Set(*localtime(&rawTime));
}

void DayTime::SetAsCurrent()
{
	*this = GetCurrentDayTime();
}

bool DayTime::IsLeapYear(const uint16 year)
{
	if ((year % 4) != 0)
		return false;
	if ((year % 100) != 0)
		return true;
	if ((year % 400) != 0)
		return false;
	return true;
}

DayTime DayTime::GetCurrentDayTime()
{
	time_t raw;
	time(&raw);
	return DayTime{ *localtime(&raw) };
}

void DayTime::swap(DayTime & other) noexcept
{
	DayTime tmp(other);
	other = *this;
	*this = tmp;
}

bool DayTime::IsLeapYear() const
{
	return IsLeapYear(m_Year);
}

tm DayTime::GetRawStruct() const
{
	tm raw;
	raw.tm_sec = m_Seconds;
	raw.tm_min = m_Minutes;
	raw.tm_hour = m_Hours;
	raw.tm_mday = m_Day;
	raw.tm_mon = m_Month - 1;
	raw.tm_year = m_Year - 1900;
	return raw;
}

time_t DayTime::GetRawTime() const
{
	auto timeinfo = GetRawStruct();
	return time_t(mktime(&timeinfo));
}

void DayTime::Set(const uint32 millisec, const uint32 sec, const uint32 min, const uint32 hour, const uint32 day, const uint32 mon, const uint16 year)
{
	m_Millisec = static_cast<uint16>(millisec);
	m_Seconds = static_cast<uint16>(sec);
	m_Minutes = static_cast<uint16>(min);
	m_Hours = static_cast<uint16>(hour);
	m_Day = static_cast<uint16>(day);
	m_Month = static_cast<uint16>(mon);
	m_Year = static_cast<uint16>(year);
	Update();
}

void DayTime::SetMillisecs(const uint32 millisecs)
{
	m_Millisec = millisecs;
	Update();
}

void DayTime::SetSeconds(const uint32 seconds)
{
	m_Seconds = seconds;
	Update();
}

void DayTime::SetMinutes(const uint32 minutes)
{
	m_Minutes = minutes;
	Update();
}

void DayTime::SetHours(const uint32 hours)
{
	m_Hours = hours;
	Update();
}

void DayTime::SetDay(const uint32 days)
{
	m_Day = days;
	Update();
}

void DayTime::SetMonth(const uint32 months)
{
	m_Month = months;
}

void DayTime::SetYear(const uint16 year)
{
	m_Year = year;
	Update();
}

uint16 DayTime::GetMillisec() const
{
	return m_Millisec;
}

uint8 DayTime::GetSeconds() const
{
	return (uint8)m_Seconds;
}

uint8 DayTime::GetMinutes() const
{
	return (uint8)m_Minutes;
}

uint8 DayTime::GetHours() const
{
	return (uint8)m_Hours;
}

uint8 DayTime::GetDays() const
{
	return (uint8)m_Day;
}

uint8 DayTime::GetMonths() const
{
	return (uint8)m_Month;
}

uint16 DayTime::GetYear() const
{
	return m_Year;
}

std::string gaf::DayTime::ToString() const
{
	ANSICHAR time[13];
	snprintf(&time[0], 13, "%02d.%02d.%02d.%03d", m_Hours, m_Minutes, m_Seconds, m_Millisec);
	time[12] = 0;
	return std::to_string(m_Day) + ' ' + GetMonthStrShrt(static_cast<EMonths::Type>(m_Month))
		+ ' ' + std::to_string(m_Year) + " - " + time;
}
