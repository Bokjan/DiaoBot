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

bool CronConfig::IsSetMinute(int x)
{
    return GetBit(Minute, x) == 1;
}

bool CronConfig::IsSetHour(int x)
{
    return GetBit(Hour, x) == 1;
}

bool CronConfig::IsSetDay(int x)
{
    return GetBit(Day, x) == 1;
}

bool CronConfig::IsSetMonth(int x)
{
    return GetBit(Month, x) == 1;
}

bool CronConfig::IsSetDayOfWeek(int x)
{
    return GetBit(DayOfWeek, x) == 1;
}



}