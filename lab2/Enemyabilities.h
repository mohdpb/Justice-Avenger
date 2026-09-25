#ifndef ENEMY_ABILITIES_H
#define ENEMY_ABILITIES_H



#include "iGraphics.h"
#include "GameCommon.h"
#include "Enemy.h"
#include "Projectile.h"
#include <math.h>
#include <stdlib.h>

#define ARCHER_RANGE          460.0
#define ARCHER_COOLDOWN       95
#define FIRE_RANGE            380.0
#define FIRE_COOLDOWN         800
#define BOMBER_RANGE          430.0
#define BOMBER_COOLDOWN       150
#define RANGED_MIN_GAP        150.0   
#define FLYER_HOVER_AMP       25.0
#define FLYER_HOVER_SPEED     0.045
#define FLYER_BASE_HEIGHT     55.0
#define SHIELD_HP_BASE        45
#define BOSS_SUMMON_COOLDOWN  330
#define BOSS_VOLLEY_COOLDOWN  110
#define BOSS_MAX_SUMMONS      6

void copyEnemyTextures(Enemy &dst, Enemy &src);
extern Enemy enemyTemplates[];


inline double enemyMuzzleY(Enemy &e) { return e.y + enemyHeight(e) * 0.55; }

inline bool isRangedEnemy(Enemy &e)
{
	return e.type == ENEMY_ARCHER || e.type == ENEMY_FIRE ||
		e.type == ENEMY_BOMBER || e.type == ENEMY_SUMMONER;
}


inline void initEnemyAbilities(Enemy &e, int day)
{
	e.flyPhase = (double)(rand() % 628) / 100.0;   
	e.shieldHP = 0;
	e.shieldMax = 0;
	e.rangedCooldown = 30 + rand() % 60;           
	e.summonCooldown = BOSS_SUMMON_COOLDOWN / 2;
	e.summonsLeft = BOSS_MAX_SUMMONS;

	if (e.type == ENEMY_SHIELD)
	{
		e.shieldHP = SHIELD_HP_BASE + day * 6;
		e.shieldMax = e.shieldHP;      
	}

	if (e.type == ENEMY_FLYER)
		e.y = GROUND_Y + FLYER_BASE_HEIGHT;
}


inline void updateFlyer(Enemy &e)
{
	if (e.type != ENEMY_FLYER || !e.alive) return;

	if (e.state == DEAD)                 
	{
		e.y -= 4.0;
		if (e.y < GROUND_Y) e.y = GROUND_Y;
		return;
	}

	e.vy = 0;
	e.flyPhase += FLYER_HOVER_SPEED;
	e.y = GROUND_Y + FLYER_BASE_HEIGHT + sin(e.flyPhase) * FLYER_HOVER_AMP;
}


inline void keepRangedDistance(Enemy &e, double playerX)
{
	if (!isRangedEnemy(e) || e.type == ENEMY_SUMMONER) return;
	if (!e.alive || e.state == DEAD || e.state == HURT) return;

	double dx = playerX - e.x;
	if (fabs(dx) < RANGED_MIN_GAP)
	{
		e.x -= (dx >= 0 ? 1 : -1) * (e.stats.moveSpeed + 1.0);
		if (e.x < 40) e.x = 40;
		if (e.x > WORLD_W - 40) e.x = WORLD_W - 40;
		e.facing = (dx >= 0) ? 1 : -1;
	}
}


inline bool updateEnemyRanged(Enemy &e, double playerX)
{
	if (!e.alive || e.state == DEAD || e.state == HURT) return false;
	if (!isRangedEnemy(e)) return false;
	if (e.type == ENEMY_BOMBER) return false;

	if (e.rangedCooldown > 0) { e.rangedCooldown--; return false; }

	double dx = playerX - e.x;
	double dist = fabs(dx);
	int    face = (dx >= 0) ? 1 : -1;
	double my = enemyMuzzleY(e);

	if (e.type == ENEMY_ARCHER)
	{
		if (dist > ARCHER_RANGE || dist < 60) return false;
		e.facing = face;
		spawnArrow(e.x, my, face, e.stats.attackDamage);
		e.rangedCooldown = ARCHER_COOLDOWN;
		return true;
	}
	if (e.type == ENEMY_FIRE)
	{
		if (dist > FIRE_RANGE) return false;
		e.facing = face;
		spawnFireball(e.x, my, face, e.stats.attackDamage + 2);
		e.rangedCooldown = FIRE_COOLDOWN;
		return true;
	}
	if (e.type == ENEMY_SUMMONER)          
	{
		if (dist > 620) return false;
		e.facing = face;
		spawnArrow(e.x, my + 20, face, e.stats.attackDamage);
		spawnArrow(e.x, my, face, e.stats.attackDamage);
		spawnFireball(e.x, my - 20, face, e.stats.attackDamage);
		e.rangedCooldown = BOSS_VOLLEY_COOLDOWN;
		return true;
	}
	return false;
}

inline void updateBossSummon(Enemy &boss, Enemy list[], int &count, int maxCount, int day)
{
	if (boss.type != ENEMY_SUMMONER) return;
	if (!boss.alive || boss.state == DEAD) return;
	if (boss.summonsLeft <= 0) return;
	if (boss.summonCooldown > 0) { boss.summonCooldown--; return; }

	for (int i = 0; i < 2 && count < maxCount && boss.summonsLeft > 0; i++)
	{
		double x = boss.x + ((i % 2 == 0) ? -130 : 130);
		if (x < 60) x = 60;
		if (x > WORLD_W - 60) x = WORLD_W - 60;

		EnemyType t = (i % 2 == 0) ? ENEMY_SMALL : ENEMY_MEDIUM;
		resetEnemy(list[count], t, x, x - 70, x + 70, day);
		copyEnemyTextures(list[count], enemyTemplates[t]);
		initEnemyAbilities(list[count], day);
		count++;
		boss.summonsLeft--;
	}
	boss.summonCooldown = BOSS_SUMMON_COOLDOWN;
}


inline void updateEnemyAbilities(Enemy &e, double playerX, Enemy list[], int &count, int maxCount, int day)
{
	updateFlyer(e);
	keepRangedDistance(e, playerX);

	if (updateEnemyRanged(e, playerX))
	{
		
		if (e.state == IDLE || e.state == WALK)
		{
			e.state = ATTACK;
			e.frameIndex = 0;
			e.frameTimer = 0;
			e.attackLanded = true;
		}
	}

	updateBossSummon(e, list, count, maxCount, day);
}

#endif