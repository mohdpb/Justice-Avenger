
#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "GameCommon.h"



struct DayWave {
	int smallCount, mediumCount, largeCount;
	int archerCount, flyerCount, shieldCount;
	int fireCount, bomberCount;
	bool hasSummonerBoss;
};

inline DayWave getDayWave(int day)
{
	DayWave w = { 0, 0, 0, 0, 0, 0, 0, 0, false };
	switch (day)
	{
	case 1:  w.smallCount = 5; break;
	case 2:  w.smallCount = 8; break;
	case 3:  w.smallCount = 8; w.mediumCount = 2; break;
	case 4:  w.smallCount = 5; w.mediumCount = 3; w.largeCount = 1; break;
	case 5:  w.smallCount = 2; w.mediumCount = 3; w.largeCount = 2; break;
	case 6:  w.smallCount = 2; w.mediumCount = 2; w.largeCount = 2; w.archerCount = 2; w.flyerCount = 3; break;
	case 7:  w.smallCount = 3; w.mediumCount = 2; w.largeCount = 2; w.archerCount = 2; w.flyerCount = 3; w.shieldCount = 1; break;
	case 8:  w.smallCount = 3; w.mediumCount = 2; w.largeCount = 2; w.shieldCount = 1; w.fireCount = 1; w.flyerCount = 2; w.hasSummonerBoss = true; break;
	default: w.smallCount = 3; break;
	}
	return w;
}

inline int totalEnemiesInWave(const DayWave &w)
{
    return w.smallCount + w.mediumCount + w.largeCount;
}

#endif 
