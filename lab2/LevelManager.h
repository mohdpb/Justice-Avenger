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

    switch (day)
    {
        case 1: w.smallCount = 3; w.mediumCount = 0; w.largeCount = 0; break;
        case 2: w.smallCount = 0; w.mediumCount = 2; w.largeCount = 0; break;
        case 3: w.smallCount = 0; w.mediumCount = 0; w.largeCount = 1; break;
        default: w.smallCount = 3; w.mediumCount = 0; w.largeCount = 0; break;
    }

    return w;
}

inline int totalEnemiesInWave(const DayWave &w)
{
    return w.smallCount + w.mediumCount + w.largeCount;
}

#endif
