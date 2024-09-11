#pragma once
#include "core.h"

typedef struct GDF_DisplayInfo {
    u16 screen_width;
    u16 screen_height;
} GDF_DisplayInfo;

typedef enum MONTH {
    JANUARY = 1,
    FEBURARY = 2,
    MARCH = 3,
    APRIL = 4,
    MAY = 5,
    JUNE = 6,
    JULY = 7,
    AUGUST = 8,
    SEPTEMPER = 9,
    OCTOBER = 10,
    NOVEMBER = 11,
    DECEMBER = 12,
} MONTH;

typedef enum DAY {
    SUNDAY = 0,
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6,
} DAY;

typedef struct GDF_DateTime {
    u16 year;
    MONTH month;
    DAY day_of_week;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u16 milli;
} GDF_DateTime;

// This should be constantly updated. Maybe.. who knows?
bool GDF_InitInfo();
// Absolute time since progarm has ran.
f64 GDF_GetAbsoluteTime();
// Attempts to get the local system time.
void GDF_GetSystemTime(GDF_DateTime* datetime);
// Attempts to get the system time in UTC.
void GDF_GetSystemTimeUTC(GDF_DateTime* datetime);
void GDF_GetDisplayInfo(GDF_DisplayInfo* display_info);