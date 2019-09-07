#pragma once

#include <cstdint>

namespace DiaoBot
{

class CronConfig
{
public:
    static const int EVERY = -1;

    CronConfig(void);
    void SetMinute(int);    // Range: 0 ~ 59
    void SetHour(int);      // Range: 0 ~ 23
    void SetDay(int);       // Range: 1 ~ 31
    void SetMonth(int);     // Range: 1 ~ 12
    void SetDayOfWeek(int); // Range: 1 ~ 7
    void ClearMinute(int);
    void ClearHour(int);
    void ClearDay(int);
    void ClearMonth(int);
    void ClearDayOfWeek(int);
    bool IsSetMinute(int);
    bool IsSetHour(int);
    bool IsSetDay(int);
    bool IsSetMonth(int);
    bool IsSetDayOfWeek(int);
    
private:
    uint64_t Minute;
    uint32_t Hour;
    uint32_t Day;
    uint32_t Month;
    uint32_t DayOfWeek;
};

}