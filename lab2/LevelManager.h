//
//  LevelManager.h
//  Justice Avenger - the 7-day wave table
//
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
	case 1:  w.smallCount = 2; break;
	case 2:  w.smallCount = 3; break;
	case 3:  w.smallCount = 2; w.mediumCount = 2; break;
	case 4:  w.smallCount = 2; w.mediumCount = 2; w.largeCount = 1; break;
	case 5:  w.mediumCount = 2; w.largeCount = 2; break;            // day-5 boss (existing)
	case 6:  w.smallCount = 2; w.archerCount = 2; w.flyerCount = 1; break;
	case 7:  w.mediumCount = 2; w.archerCount = 1; w.flyerCount = 2; w.shieldCount = 1; break;
	case 8:  w.mediumCount = 1; w.shieldCount = 2; w.fireCount = 2; w.flyerCount = 1; break;
	case 9:  w.largeCount = 1; w.shieldCount = 1; w.fireCount = 1; w.bomberCount = 2; w.archerCount = 1; break;
	case 10: w.shieldCount = 2; w.fireCount = 1; w.bomberCount = 1; w.flyerCount = 2;
		w.hasSummonerBoss = true; break;
	default: w.smallCount = 2; break;
	}
	return w;
}

inline int totalEnemiesInWave(const DayWave &w)
{
    return w.smallCount + w.mediumCount + w.largeCount;
}

#endif // LEVEL_MANAGER_H
