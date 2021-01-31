#pragma once

#include <cstdint>

namespace DiaoBot {

class CronConfig {
 public:
  static const int EVERY = -1;

  CronConfig(void);
  void SetMinute(int);     // Range: 0 ~ 59
  void SetHour(int);       // Range: 0 ~ 23
  void SetDay(int);        // Range: 1 ~ 31
  void SetMonth(int);      // Range: 1 ~ 12, different from `struct tm`
  void SetDayOfWeek(int);  // Range: 1 ~ 7,  different from `struct tm`
  void ClearMinute(int);
  void ClearHour(int);
  void ClearDay(int);
  void ClearMonth(int);
  void ClearDayOfWeek(int);
  bool IsSetMinute(int) const;
  bool IsSetHour(int) const;
  bool IsSetDay(int) const;
  bool IsSetMonth(int) const;
  bool IsSetDayOfWeek(int) const;
  uint64_t GetMinute(void) const;
  uint32_t GetHour(void) const;
  uint32_t GetDay(void) const;
  uint32_t GetMonth(void) const;
  uint32_t GetDayOfWeek(void) const;

 private:
  uint64_t Minute;
  uint32_t Hour;
  uint32_t Day;
  uint32_t Month;
  uint32_t DayOfWeek;
};

}  // namespace DiaoBot