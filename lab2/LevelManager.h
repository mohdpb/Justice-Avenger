//
//  LevelManager.h
//  Justice Avenger - the 7-day wave table
//
#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "GameCommon.h"

struct DayWave
{
    int smallCount;
    int mediumCount;
    int largeCount;
};

inline DayWave getDayWave(int day)
{
    DayWave w;

    // SHOWCASE BUILD (3 days) - matches TOTAL_DAYS = 3 in GameCommon.h
    switch (day)
    {
        case 1: w.smallCount = 3; w.mediumCount = 0; w.largeCount = 0; break;
        case 2: w.smallCount = 0; w.mediumCount = 2; w.largeCount = 0; break;
        case 3: w.smallCount = 0; w.mediumCount = 0; w.largeCount = 1; break;
        default: w.smallCount = 3; w.mediumCount = 0; w.largeCount = 0; break;
    }

    // ---- ORIGINAL 7-DAY TABLE (restore by uncommenting below, and
    // setting TOTAL_DAYS back to 7 in GameCommon.h) ----
    // switch (day)
    // {
    //     case 1: w.smallCount = 3; w.mediumCount = 0; w.largeCount = 0; break;
    //     case 2: w.smallCount = 5; w.mediumCount = 0; w.largeCount = 0; break;
    //     case 3: w.smallCount = 7; w.mediumCount = 0; w.largeCount = 0; break;
    //     case 4: w.smallCount = 3; w.mediumCount = 1; w.largeCount = 0; break;
    //     case 5: w.smallCount = 5; w.mediumCount = 2; w.largeCount = 0; break;
    //     case 6: w.smallCount = 7; w.mediumCount = 3; w.largeCount = 0; break;
    //     case 7: w.smallCount = 7; w.mediumCount = 4; w.largeCount = 1; break;
    //     default: w.smallCount = 3; w.mediumCount = 0; w.largeCount = 0; break;
    // }

    return w;
}

inline int totalEnemiesInWave(const DayWave &w)
{
    return w.smallCount + w.mediumCount + w.largeCount;
}

#endif // LEVEL_MANAGER_H
