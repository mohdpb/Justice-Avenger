
#ifndef GAME_COMMON_H
#define GAME_COMMON_H


enum FighterState { IDLE, WALK, ATTACK, HURT, DEAD };


enum AppState
{
    STATE_MAIN_MENU,
    STATE_NEW_GAME_CONFIRM,   
    STATE_OPTIONS_MENU,
    STATE_AUDIO_SETTINGS,
    STATE_ABOUT_MENU,
    STATE_HIGHSCORES_MENU,
    STATE_STORY_INTRO,        
    STATE_NAME_ENTRY,         
    STATE_DAY_TRANSITION,     
    STATE_PLAYING,
    STATE_DEATH_PAUSE,        
    STATE_DAY_CLEAR_PAUSE,    
    STATE_PAUSE_MENU,         
    STATE_GAME_OVER,          
    STATE_GAME_COMPLETE       
};


#define WINDOW_W   900
#define WINDOW_H   500
#define GROUND_Y   80
#define GRAVITY    0.6


#define WORLD_W (WINDOW_W * 3)   

#define BASE_UNIT_W 45.0
#define BASE_UNIT_H 65.0


#define TICKS_PER_SECOND 60

#define ANIM_FRAME_DELAY 6   
#define ANIM_IDLE_COUNT   2
#define ANIM_WALK_COUNT   3
#define ANIM_ATTACK_COUNT 3
#define ANIM_HURT_COUNT   2
#define ANIM_DEAD_COUNT   2

#define HURT_DURATION 18  


#define TOTAL_DAYS  8
#define TOTAL_LIVES 3
#define DAY_TRANSITION_TICKS (TICKS_PER_SECOND * 1)  


#define DEATH_PAUSE_TICKS     (TICKS_PER_SECOND * 3 / 2)  
#define DAY_CLEAR_PAUSE_TICKS (TICKS_PER_SECOND * 6 / 5)  


#define STORY_SLIDE_COUNT 4


struct HitBox
{
    double x, y, w, h;
};

inline bool aabbOverlap(const HitBox &a, const HitBox &b)
{
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}


static unsigned int g_prevKeyPressed[512] = { 0 };

inline bool keyJustPressed(unsigned char key)
{
    return keyPressed[key] && !g_prevKeyPressed[key];
}


inline void updatePrevKeyState()
{
    for (int i = 0; i < 512; i++)
        g_prevKeyPressed[i] = keyPressed[i];
}


static bool g_mouseClicked = false;
static int  g_mouseClickX = 0;
static int  g_mouseClickY = 0;

#endif 
