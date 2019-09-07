#include "CronConfig.hpp"

namespace DiaoBot
{

template <typename T>
static T SetBit(T x, int pos)
{
    if (pos == CronConfig::EVERY)
        return ~0;
    return x | (1 << pos);
}

template <typename T>
static int GetBit(T x, int pos)
{
    return x & (1 << pos);
}

template <typename T>
static int ClearBit(T x, int pos)
{
    return x & (~(1 << pos));
}

CronConfig::CronConfig(void):
    Minute(0),
    Hour(0),
    Day(0),
    Month(0),
    DayOfWeek(0) { }

void CronConfig::SetMinute(int x)
{
    Minute = SetBit(Minute, x);
}

void CronConfig::SetHour(int x)
{
    Hour = SetBit(Hour, x);
}

void CronConfig::SetDay(int x)
{
    Day = SetBit(Day, x);
}

void CronConfig::SetMonth(int x)
{
    Month = SetBit(Month, x);
}

void CronConfig::SetDayOfWeek(int x)
{
    DayOfWeek = SetBit(DayOfWeek, x);
}

void CronConfig::ClearMinute(int x)
{
    Minute = ClearBit(Minute, x);
}

void CronConfig::ClearHour(int x)
{
    Hour = ClearBit(Hour, x);
}

void CronConfig::ClearDay(int x)
{
    Day = ClearBit(Day, x);
}

void CronConfig::ClearMonth(int x)
{
    Month = ClearBit(Month, x);
}

void CronConfig::ClearDayOfWeek(int x)
{
    DayOfWeek = ClearBit(DayOfWeek, x);
}

bool CronConfig::IsSetMinute(int x) const
{
    return GetBit(Minute, x) != 0;
}

bool CronConfig::IsSetHour(int x) const
{
    return GetBit(Hour, x) != 0;
}

bool CronConfig::IsSetDay(int x) const
{
    return GetBit(Day, x) != 0;
}

bool CronConfig::IsSetMonth(int x) const
{
    return GetBit(Month, x) != 0;
}

bool CronConfig::IsSetDayOfWeek(int x) const
{
    return GetBit(DayOfWeek, x) != 0;
}

uint64_t CronConfig::GetMinute(void) const
{
    return Minute;
}

uint32_t CronConfig::GetHour(void) const
{
    return Hour;
}

uint32_t CronConfig::GetDay(void) const
{
    return Day;
}

uint32_t CronConfig::GetMonth(void) const
{
    return Month;
}

uint32_t CronConfig::GetDayOfWeek(void) const
{
    return DayOfWeek;
}




}