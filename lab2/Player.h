
#ifndef PLAYER_H
#define PLAYER_H

#include "GameCommon.h"
#include <stdio.h>

#define PLAYER_SCALE  2.0
#define PLAYER_WIDTH  (BASE_UNIT_W * PLAYER_SCALE)   
#define PLAYER_HEIGHT (BASE_UNIT_H * PLAYER_SCALE)   
#define PLAYER_BASE_DAMAGE     30
#define PLAYER_DAY8_DAMAGE_BONUS 20
#define PLAYER_MOVE_SPEED      4.0
#define PLAYER_JUMP_SPEED      11.0
#define PLAYER_MAX_HEALTH      200
#define PLAYER_MAX_HEALTH_DAY5_FIGHT 400
#define PLAYER_MAX_HEALTH_DAY8_FIGHT 600
#define PLAYER_ATTACK_COOLDOWN 18
struct Player
{
	double x, y, vy;
	int facing;
	int health;
	int maxHealth;
	int extraDamage;         // add this line
	FighterState state;

    int frameIndex, frameTimer;
    int hurtTimer;
    int attackCooldown;
    bool onGround;
    bool attackLanded;

    int damageBonus;
    int damageBoostTimer;

    unsigned int idleTex[ANIM_IDLE_COUNT];
    unsigned int walkTex[ANIM_WALK_COUNT];
    unsigned int attackTex[ANIM_ATTACK_COUNT];
    unsigned int hurtTex[ANIM_HURT_COUNT];
    unsigned int deadTex[ANIM_DEAD_COUNT];
};

inline void loadPlayerAssets(Player &p)
{
    char fn[256];
    for (int i = 0; i < ANIM_IDLE_COUNT; i++)   { sprintf(fn, "assets/player/idle_%d.png", i);   p.idleTex[i]   = iLoadImage(fn); }
    for (int i = 0; i < ANIM_WALK_COUNT; i++)   { sprintf(fn, "assets/player/walk_%d.png", i);   p.walkTex[i]   = iLoadImage(fn); }
    for (int i = 0; i < ANIM_ATTACK_COUNT; i++) { sprintf(fn, "assets/player/attack_%d.png", i); p.attackTex[i] = iLoadImage(fn); }
    for (int i = 0; i < ANIM_HURT_COUNT; i++)   { sprintf(fn, "assets/player/hurt_%d.png", i);   p.hurtTex[i]   = iLoadImage(fn); }
    for (int i = 0; i < ANIM_DEAD_COUNT; i++)   { sprintf(fn, "assets/player/dead_%d.png", i);   p.deadTex[i]   = iLoadImage(fn); }
}

inline void resetPlayer(Player &p, double startX)
{
	p.damageBonus = 0; p.damageBoostTimer = 0; p.extraDamage = 0;
		p.x = startX; p.y = GROUND_Y; p.vy = 0; p.facing = 1;
		p.health = PLAYER_MAX_HEALTH; p.maxHealth = PLAYER_MAX_HEALTH; p.state = IDLE;
    p.frameIndex = 0; p.frameTimer = 0; p.hurtTimer = 0; p.attackCooldown = 0;
    p.onGround = true; p.attackLanded = false;
    p.damageBonus = 0; p.damageBoostTimer = 0;
}

inline void setPlayerState(Player &p, FighterState newState)
{
    if (p.state == newState) return;
    p.state = newState; p.frameIndex = 0; p.frameTimer = 0;
}

inline void advancePlayerAnim(Player &p, int frameCount)
{
    p.frameTimer++;
    if (p.frameTimer >= ANIM_FRAME_DELAY)
    {
        p.frameTimer = 0;
        p.frameIndex++;
        if (p.frameIndex >= frameCount) p.frameIndex = 0;
    }
}

inline HitBox getPlayerBox(Player &p)
{
    HitBox box;
    box.w = PLAYER_WIDTH * 0.65;
    box.h = PLAYER_HEIGHT * 0.8;
    box.x = p.x - box.w / 2;
    box.y = p.y;
    return box;
}
inline HitBox getPlayerAttackBox(Player &p)
{
	double reach = 55;
	double w = PLAYER_WIDTH * 0.5 + reach;
	double h = PLAYER_HEIGHT * 0.7;

	HitBox box;
	box.w = w;
	box.h = h;
	box.y = p.y + PLAYER_HEIGHT * 0.1;

	if (p.facing == 1)
		box.x = p.x;
	else
		box.x = p.x - w;

	return box;
}

inline int getPlayerAttackDamage(Player &p)
{
	return PLAYER_BASE_DAMAGE + p.extraDamage + (p.damageBoostTimer > 0 ? p.damageBonus : 0);
}

inline void applyPlayerDamageBoost(Player &p, int bonus, int durationTicks)
{
    p.damageBonus = bonus;
    p.damageBoostTimer = durationTicks;
}

inline void healPlayer(Player &p, int amount)
{
    p.health += amount;
    if (p.health > PLAYER_MAX_HEALTH) p.health = PLAYER_MAX_HEALTH;
}

inline void setPlayerMaxHealthForFight(Player &p, int newMax)
{
	p.health = newMax;
	p.maxHealth = newMax;
}
inline void setPlayerHurt(Player &p)
{
    setPlayerState(p, HURT);
    p.hurtTimer = HURT_DURATION;
}

inline void applyDamageToPlayer(Player &p, int amount, int knockDir)
{
	p.health -= amount;
	if (p.health <= 0) { p.health = 0; setPlayerState(p, DEAD); }
	else { setPlayerHurt(p); p.x += knockDir * 20; }
}

inline bool updatePlayer(Player &p)
{
    bool checkHitThisTick = false;

    if (p.attackCooldown > 0) p.attackCooldown--;
    if (p.damageBoostTimer > 0) p.damageBoostTimer--;

    if (p.state == DEAD)
    {
        advancePlayerAnim(p, ANIM_DEAD_COUNT);
        return false;
    }

    if (p.state == HURT)
    {
        p.hurtTimer--;
        advancePlayerAnim(p, ANIM_HURT_COUNT);
        if (p.hurtTimer <= 0) setPlayerState(p, IDLE);
    }
    else if (p.state == ATTACK)
    {
        advancePlayerAnim(p, ANIM_ATTACK_COUNT);
        if (!p.attackLanded && p.frameIndex == ANIM_ATTACK_COUNT - 1)
        {
            checkHitThisTick = true;
            p.attackLanded = true;
        }
        if (p.frameIndex == 0 && p.frameTimer == 0 && p.attackLanded)
        {
            setPlayerState(p, IDLE);
            p.attackLanded = false;
            p.attackCooldown = PLAYER_ATTACK_COOLDOWN;
        }
    }
    else
    {
        bool moving = false;
        if (isKeyPressed('a') || isKeyPressed('A')) { p.x -= PLAYER_MOVE_SPEED; p.facing = -1; moving = true; }
        if (isKeyPressed('d') || isKeyPressed('D')) { p.x += PLAYER_MOVE_SPEED; p.facing = 1;  moving = true; }
        if ((isKeyPressed('w') || isKeyPressed('W')) && p.onGround) { p.vy = PLAYER_JUMP_SPEED; p.onGround = false; }
        if ((isKeyPressed('j') || isKeyPressed('J')) && p.attackCooldown == 0)
        {
            setPlayerState(p, ATTACK);
            p.attackLanded = false;
        }

        if (p.state != ATTACK) setPlayerState(p, moving ? WALK : IDLE);
        if (p.state == WALK) advancePlayerAnim(p, ANIM_WALK_COUNT);
        if (p.state == IDLE) advancePlayerAnim(p, ANIM_IDLE_COUNT);
    }

    p.y += p.vy;
    p.vy -= GRAVITY;
    if (p.y <= GROUND_Y) { p.y = GROUND_Y; p.vy = 0; p.onGround = true; }

    // World bounds are now WORLD_W (the full scrollable level), not just
    // the screen width - see GameCommon.h.
    if (p.x < 40) p.x = 40;
    if (p.x > WORLD_W - 40) p.x = WORLD_W - 40;

    return checkHitThisTick;
}


inline void drawPlayer(Player &p, double cameraX)
{
    unsigned int *texArr = NULL;
    int count = 0;

    switch (p.state)
    {
        case IDLE:   texArr = p.idleTex;   count = ANIM_IDLE_COUNT;   break;
        case WALK:   texArr = p.walkTex;   count = ANIM_WALK_COUNT;   break;
        case ATTACK: texArr = p.attackTex; count = ANIM_ATTACK_COUNT; break;
        case HURT:   texArr = p.hurtTex;   count = ANIM_HURT_COUNT;   break;
        case DEAD:   texArr = p.deadTex;   count = ANIM_DEAD_COUNT;   break;
    }

    int idx = p.frameIndex;
    if (count > 0 && idx >= count) idx = count - 1;

    double screenX = p.x - cameraX;

    if (texArr != NULL && count > 0 && texArr[idx] != 0)
    {
        if (p.facing == 1)
        {
            iShowImage(screenX - PLAYER_WIDTH / 2, p.y, (int)PLAYER_WIDTH, (int)PLAYER_HEIGHT, texArr[idx]);
        }
        else
        {
            glPushMatrix();
            glTranslatef((float)screenX, 0.0f, 0.0f);
            glScalef(-1.0f, 1.0f, 1.0f);
            glTranslatef((float)-screenX, 0.0f, 0.0f);
            iShowImage(screenX - PLAYER_WIDTH / 2, p.y, (int)PLAYER_WIDTH, (int)PLAYER_HEIGHT, texArr[idx]);
            glPopMatrix();
        }
    }
    else
    {
        if (p.damageBoostTimer > 0) iSetColor(30, 80, 200);
        else iSetColor(60, 120, 230);
        iFilledRectangle(screenX - PLAYER_WIDTH / 2, p.y, PLAYER_WIDTH, PLAYER_HEIGHT);
    }
}

#endif 
