

#ifndef ENEMY_H
#define ENEMY_H

#include "GameCommon.h"
#include <stdio.h>

enum EnemyType { ENEMY_SMALL, ENEMY_MEDIUM, ENEMY_LARGE };
enum NpcMode { NPC_PATROL, NPC_CHASE, NPC_ATTACK_MODE };

struct EnemyStats
{
	double scale;
	int    maxHealth;
	double moveSpeed;
	int    attackCooldown;
	int    attackDamage;
	int    detectRange;
	int    attackRange;
	bool   canJump;
	double jumpSpeed;
};

inline EnemyStats getEnemyStats(EnemyType type, int day)
{
	EnemyStats s;

	
	double difficulty;
	switch (day)
	{
	case 1:  difficulty = 1.0; break;
	case 2:  difficulty = 1.15; break;
	case 3:  difficulty = 1.3; break;
	case 4:  difficulty = 1.5; break;
	default: difficulty = 1.8; break;
	}
	switch (type)
	{
	case ENEMY_SMALL:
		s.scale = 2.0;
		s.maxHealth = (int)(60 * difficulty);
		s.moveSpeed = 3.0 + 0.15 * (day - 1);
		s.attackCooldown = 40 - 2 * (day - 1);
		if (s.attackCooldown < 20) s.attackCooldown = 20;
		s.attackDamage = (int)(4 * difficulty);
		s.detectRange = 260 + 10 * (day - 1);
		s.attackRange = 70;
		break;

	case ENEMY_MEDIUM:
		s.scale = 3.0;
		s.maxHealth = (int)(100 * difficulty);
		s.moveSpeed = 2.2 + 0.1 * (day - 1);
		s.attackCooldown = 55 - 2 * (day - 1);
		if (s.attackCooldown < 30) s.attackCooldown = 30;
		s.attackDamage = (int)(7 * difficulty);
		s.detectRange = 280 + 10 * (day - 1);
		s.attackRange = 85;
		break;

	case ENEMY_LARGE:
		s.scale = 4.0;
		s.maxHealth = (int)(150 * difficulty);
		s.moveSpeed = 1.5 + 0.07 * (day - 1);
		s.attackCooldown = 75 - 2 * (day - 1);
		if (s.attackCooldown < 45) s.attackCooldown = 45;
		s.attackDamage = (int)(12 * difficulty);
		s.detectRange = 300 + 10 * (day - 1);
		s.attackRange = 100;
		break;
	}
	s.canJump = (day >= 4 && type != ENEMY_LARGE);  
	s.jumpSpeed = 9.5;

	return s;
	
}


inline const char* enemyFolder(EnemyType type)
{
    switch (type)
    {
        case ENEMY_SMALL:  return "assets/enemy_small";
        case ENEMY_MEDIUM: return "assets/enemy_medium";
        case ENEMY_LARGE:  return "assets/enemy_large";
    }
    return "assets/enemy_small";
}

struct Enemy
{
    EnemyType type;
    EnemyStats stats;

    double x, y, vy;
    int facing;
    int health;
    bool alive;
    FighterState state;

    int frameIndex, frameTimer;
    int hurtTimer;
    int attackCooldown;
    bool attackLanded;

	NpcMode mode;
	int patrolDir;
	double patrolLeftBound, patrolRightBound;
	int jumpTimer;

	bool isBoss;
	bool charging;
	int chargeTimer;
	int chargeCooldownTimer;
    unsigned int idleTex[ANIM_IDLE_COUNT];
    unsigned int walkTex[ANIM_WALK_COUNT];
    unsigned int attackTex[ANIM_ATTACK_COUNT];
    unsigned int hurtTex[ANIM_HURT_COUNT];
    unsigned int deadTex[ANIM_DEAD_COUNT];
};

inline void loadEnemyAssets(Enemy &e)
{
    char fn[256];
    const char *folder = enemyFolder(e.type);

    for (int i = 0; i < ANIM_IDLE_COUNT; i++)   { sprintf(fn, "%s/idle_%d.png", folder, i);   e.idleTex[i]   = iLoadImage(fn); }
    for (int i = 0; i < ANIM_WALK_COUNT; i++)   { sprintf(fn, "%s/walk_%d.png", folder, i);   e.walkTex[i]   = iLoadImage(fn); }
    for (int i = 0; i < ANIM_ATTACK_COUNT; i++) { sprintf(fn, "%s/attack_%d.png", folder, i); e.attackTex[i] = iLoadImage(fn); }
    for (int i = 0; i < ANIM_HURT_COUNT; i++)   { sprintf(fn, "%s/hurt_%d.png", folder, i);   e.hurtTex[i]   = iLoadImage(fn); }
    for (int i = 0; i < ANIM_DEAD_COUNT; i++)   { sprintf(fn, "%s/dead_%d.png", folder, i);   e.deadTex[i]   = iLoadImage(fn); }
}

inline void resetEnemy(Enemy &e, EnemyType type, double startX, double patrolLeft, double patrolRight, int day = 1)
{
	e.type = type;
	e.stats = getEnemyStats(type, day);
    e.x = startX; e.y = GROUND_Y; e.vy = 0; e.facing = -1;
    e.health = e.stats.maxHealth; e.alive = true; e.state = IDLE;
    e.frameIndex = 0; e.frameTimer = 0; e.hurtTimer = 0;
    e.attackCooldown = 0; e.attackLanded = false;
	e.mode = NPC_PATROL; e.patrolDir = 1;
	e.patrolLeftBound = patrolLeft; e.patrolRightBound = patrolRight;
	e.jumpTimer = 0;
	e.isBoss = false; e.charging = false;
	e.chargeTimer = 0; e.chargeCooldownTimer = 0;
}


inline void makeBoss(Enemy &e)
{
	e.isBoss = true;
	e.stats.maxHealth = (int)(e.stats.maxHealth * 1.6);
	e.health = e.stats.maxHealth;
	e.stats.attackDamage = (int)(e.stats.attackDamage * 1.3);
}

inline void setEnemyState(Enemy &e, FighterState newState)
{
    if (e.state == newState) return;
    e.state = newState; e.frameIndex = 0; e.frameTimer = 0;
}

inline void advanceEnemyAnim(Enemy &e, int frameCount)
{
    e.frameTimer++;
    if (e.frameTimer >= ANIM_FRAME_DELAY)
    {
        e.frameTimer = 0;
        e.frameIndex++;
        if (e.frameIndex >= frameCount) e.frameIndex = 0;
    }
}

inline double enemyWidth(Enemy &e)  { return BASE_UNIT_W * e.stats.scale; }
inline double enemyHeight(Enemy &e) { return BASE_UNIT_H * e.stats.scale; }

inline HitBox getEnemyBox(Enemy &e)
{
    HitBox box;
    box.w = enemyWidth(e) * 0.65;
    box.h = enemyHeight(e) * 0.8;
    box.x = e.x - box.w / 2;
    box.y = e.y;
    return box;
}

inline void applyDamageToEnemy(Enemy &e, int amount, int knockDir)
{
	e.health -= amount;
	if (e.health <= 0) { e.health = 0; setEnemyState(e, DEAD); }
	else { setEnemyState(e, HURT); e.hurtTimer = HURT_DURATION; e.x += knockDir * 25; }
}

inline bool updateEnemy(Enemy &e, double playerX)
{
    bool checkHitThisTick = false;
    if (!e.alive) return false;

    if (e.attackCooldown > 0) e.attackCooldown--;
	
	if (e.chargeTimer > 0) e.chargeTimer--;
	if (e.chargeCooldownTimer > 0) e.chargeCooldownTimer--;
	if (e.jumpTimer > 0) e.jumpTimer--;
    if (e.state == DEAD)
    {
        advanceEnemyAnim(e, ANIM_DEAD_COUNT);
        if (e.frameIndex == ANIM_DEAD_COUNT - 1 && e.frameTimer == 0) e.alive = false;
        return false;
    }

    if (e.state == HURT)
    {
        e.hurtTimer--;
        advanceEnemyAnim(e, ANIM_HURT_COUNT);
        if (e.hurtTimer <= 0) setEnemyState(e, IDLE);
    }
    else if (e.state == ATTACK)
    {
        advanceEnemyAnim(e, ANIM_ATTACK_COUNT);
        if (!e.attackLanded && e.frameIndex == ANIM_ATTACK_COUNT - 1)
        {
            checkHitThisTick = true;
            e.attackLanded = true;
        }
        if (e.frameIndex == 0 && e.frameTimer == 0 && e.attackLanded)
        {
            setEnemyState(e, IDLE);
            e.attackLanded = false;
            e.attackCooldown = e.stats.attackCooldown;
        }
    }
    else
    {
        double dist = e.x - playerX;
        double absDist = dist < 0 ? -dist : dist;

        if (absDist < e.stats.attackRange && e.attackCooldown == 0) e.mode = NPC_ATTACK_MODE;
        else if (absDist < e.stats.detectRange) e.mode = NPC_CHASE;
        else e.mode = NPC_PATROL;

        bool moving = false;

        if (e.mode == NPC_ATTACK_MODE)
        {
            setEnemyState(e, ATTACK);
            e.attackLanded = false;
            e.facing = (playerX < e.x) ? -1 : 1;
        }
		
		else if (e.mode == NPC_CHASE)
		{
			double speed = e.stats.moveSpeed;

			if (e.isBoss)
			{
				if (!e.charging && e.chargeCooldownTimer <= 0)
				{
					e.charging = true;
					e.chargeTimer = 45;
				}
				if (e.charging)
				{
					speed = e.stats.moveSpeed * 3.0;
					if (e.chargeTimer <= 0)
					{
						e.charging = false;
						e.chargeCooldownTimer = 180;
					}
				}
			}

			if (playerX < e.x) { e.x -= speed; e.facing = -1; }
			else                { e.x += speed; e.facing = 1; }
			moving = true;

			if (e.stats.canJump && e.y <= GROUND_Y && e.jumpTimer <= 0)
			{
				e.vy = e.stats.jumpSpeed;
				e.x += (e.facing == 1 ? 40 : -40);
				e.jumpTimer = 100;
			}
		}
        else
        {
            e.x += e.stats.moveSpeed * 0.4 * e.patrolDir;
            e.facing = e.patrolDir;
            moving = true;
            if (e.x > e.patrolRightBound) e.patrolDir = -1;
            if (e.x < e.patrolLeftBound)  e.patrolDir = 1;
        }

        if (e.state != ATTACK) setEnemyState(e, moving ? WALK : IDLE);
        if (e.state == WALK) advanceEnemyAnim(e, ANIM_WALK_COUNT);
        if (e.state == IDLE) advanceEnemyAnim(e, ANIM_IDLE_COUNT);
    }

    e.y += e.vy;
    e.vy -= GRAVITY;
    if (e.y <= GROUND_Y) { e.y = GROUND_Y; e.vy = 0; }

   
    if (e.x < 40) e.x = 40;
    if (e.x > WORLD_W - 40) e.x = WORLD_W - 40;

    return checkHitThisTick;
}


inline void drawEnemy(Enemy &e, double cameraX)
{
	unsigned int *texArr = NULL;
	int count = 0;
	

    switch (e.state)
    {
        case IDLE:   texArr = e.idleTex;   count = ANIM_IDLE_COUNT;   break;
        case WALK:   texArr = e.walkTex;   count = ANIM_WALK_COUNT;   break;
        case ATTACK: texArr = e.attackTex; count = ANIM_ATTACK_COUNT; break;
        case HURT:   texArr = e.hurtTex;   count = ANIM_HURT_COUNT;   break;
        case DEAD:   texArr = e.deadTex;   count = ANIM_DEAD_COUNT;   break;
    }

    int idx = e.frameIndex;
    if (count > 0 && idx >= count) idx = count - 1;

    double w = enemyWidth(e);
    double h = enemyHeight(e);
    double screenX = e.x - cameraX;

    if (texArr != NULL && count > 0 && texArr[idx] != 0)
    {
        if (e.facing == 1)
        {
            iShowImage(screenX - w / 2, e.y, (int)w, (int)h, texArr[idx]);
        }
        else
        {
            glPushMatrix();
            glTranslatef((float)screenX, 0.0f, 0.0f);
            glScalef(-1.0f, 1.0f, 1.0f);
            glTranslatef((float)-screenX, 0.0f, 0.0f);
            iShowImage(screenX - w / 2, e.y, (int)w, (int)h, texArr[idx]);
            glPopMatrix();
        }
    }
    else
    {
		if (e.isBoss)                     iSetColor(e.charging ? 255 : 90, 0, e.charging ? 0 : 0);
		else if (e.type == ENEMY_SMALL)   iSetColor(220, 120, 60);
		else if (e.type == ENEMY_MEDIUM)  iSetColor(200, 60, 60);
		else                               iSetColor(140, 20, 20);                             
        iFilledRectangle(screenX - w / 2, e.y, w, h);
    }

	if (e.alive)
	{
		double barW = w;
		double barX = screenX - barW / 2;
		double barY = e.y + h + 6;
		double pct = (double)e.health / (double)e.stats.maxHealth;
		if (pct < 0) pct = 0;

		iSetColor(40, 40, 40);
		iFilledRectangle(barX, barY, barW, 8);
		iSetColor(230, 60, 60);
		iFilledRectangle(barX, barY, barW * pct, 8);
		iSetColor(255, 255, 255);
		iRectangle(barX, barY, barW, 8);
	}
}

#endif 
