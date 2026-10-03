#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#pragma warning(push)
#pragma warning(disable: 4996)

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif
#pragma comment(lib, "winmm.lib")

enum BgmTrack
{
    BGM_NONE     = -1,
    BGM_MENU     =  0,
    BGM_STORY    =  1,
    BGM_GAMEPLAY =  2
};

#define BGM_ALIAS "bgm_track"

static const char* g_bgmNames[3] =
{
    "bgm_menu",
    "bgm_story",
    "bgm_gameplay"
};

static BgmTrack  g_currentBgm      = BGM_NONE;
static bool      g_bgmOpen         = false;
static int       g_bgmMusicVolume  = -1;   

inline bool audioFileExists(const char* path)
{
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

inline bool findAudioPath(const char* baseName, char* outPath, size_t outSize = 260)
{
    if (strstr(baseName, ".wav") || strstr(baseName, ".mp3") || strstr(baseName, ".WAV") || strstr(baseName, ".MP3"))
    {
        sprintf(outPath, "%s", baseName);
        if (audioFileExists(outPath)) return true;
        sprintf(outPath, "assets/audio/%s", baseName);
        if (audioFileExists(outPath)) return true;
        sprintf(outPath, "assets/%s", baseName);
        if (audioFileExists(outPath)) return true;
    }

    sprintf(outPath, "assets/audio/%s.mp3", baseName);
    if (audioFileExists(outPath)) return true;
    sprintf(outPath, "assets/audio/%s.wav", baseName);
    if (audioFileExists(outPath)) return true;

    sprintf(outPath, "assets/audio/sfx_%s.mp3", baseName);
    if (audioFileExists(outPath)) return true;
    sprintf(outPath, "assets/audio/sfx_%s.wav", baseName);
    if (audioFileExists(outPath)) return true;

    if (strncmp(baseName, "sfx_", 4) == 0)
    {
        sprintf(outPath, "assets/audio/%s.mp3", baseName + 4);
        if (audioFileExists(outPath)) return true;
        sprintf(outPath, "assets/audio/%s.wav", baseName + 4);
        if (audioFileExists(outPath)) return true;
    }

    sprintf(outPath, "assets/%s.mp3", baseName);
    if (audioFileExists(outPath)) return true;
    sprintf(outPath, "assets/%s.wav", baseName);
    if (audioFileExists(outPath)) return true;

    outPath[0] = '\0';
    return false;
}

inline int volumeToMci(int vol)
{
    if (vol <= 0) return 0;
    if (vol >= 10) return 1000;
    return vol * 100;
}

inline void bgmClose()
{
    if (g_bgmOpen)
    {
        mciSendStringA("stop "  BGM_ALIAS, NULL, 0, NULL);
        mciSendStringA("close " BGM_ALIAS, NULL, 0, NULL);
        g_bgmOpen = false;
    }
}

inline void bgmApplyVolume(int musicVolume)
{
    if (!g_bgmOpen) return;
    char cmd[128];
    sprintf(cmd, "setaudio " BGM_ALIAS " volume to %d", volumeToMci(musicVolume));
    mciSendStringA(cmd, NULL, 0, NULL);
    g_bgmMusicVolume = musicVolume;
}


inline void updateBgm(BgmTrack desired, int musicVolume)
{
    if (desired != g_currentBgm)
    {
        bgmClose();
        g_currentBgm = desired;

        if (desired == BGM_NONE)
            return;

        const char* name = g_bgmNames[(int)desired];
        char filePath[260];
        if (!findAudioPath(name, filePath, sizeof(filePath)))
        {
            g_bgmOpen = false;
            return;
        }

        char openCmd[512];
        sprintf(openCmd, "open \"%s\" type mpegvideo alias " BGM_ALIAS, filePath);
        MCIERROR err = mciSendStringA(openCmd, NULL, 0, NULL);
        if (err != 0)
        {
            sprintf(openCmd, "open \"%s\" alias " BGM_ALIAS, filePath);
            err = mciSendStringA(openCmd, NULL, 0, NULL);
            if (err != 0)
            {
                char errBuf[256];
                mciGetErrorStringA(err, errBuf, sizeof(errBuf));
                printf("[Audio] Failed to open BGM '%s': %s (err %d)\n", filePath, errBuf, (int)err);
                g_bgmOpen = false;
                return;
            }
        }
        g_bgmOpen = true;

        bgmApplyVolume(musicVolume);

        MCIERROR playErr = mciSendStringA("play " BGM_ALIAS " repeat", NULL, 0, NULL);
        if (playErr != 0)
        {
            mciSendStringA("play " BGM_ALIAS, NULL, 0, NULL);
        }
        printf("[Audio] Playing BGM track: %s\n", filePath);
    }
    else if (g_bgmOpen && musicVolume != g_bgmMusicVolume)
    {
        bgmApplyVolume(musicVolume);
    }
}

inline void stopBgm()
{
    bgmClose();
    g_currentBgm = BGM_NONE;
}

inline void playSfx(const char* baseName, int sfxVolume)
{
    if (sfxVolume <= 0) return;

    char filePath[260];
    if (!findAudioPath(baseName, filePath, sizeof(filePath)))
        return; 

    if (strstr(filePath, ".wav") || strstr(filePath, ".WAV"))
    {
        PlaySoundA(filePath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
    }
    else
    {
        char cmd[512];
        mciSendStringA("close sfx_ch", NULL, 0, NULL);
        sprintf(cmd, "open \"%s\" type mpegvideo alias sfx_ch", filePath);
        if (mciSendStringA(cmd, NULL, 0, NULL) == 0)
        {
            sprintf(cmd, "setaudio sfx_ch volume to %d", volumeToMci(sfxVolume));
            mciSendStringA(cmd, NULL, 0, NULL);
            mciSendStringA("play sfx_ch from 0", NULL, 0, NULL);
        }
    }
}

inline void sfxClick (int sfxVolume) { playSfx("sfx_click",  sfxVolume); }
inline void sfxHit   (int sfxVolume) { playSfx("sfx_hit",    sfxVolume); }
inline void sfxPickup(int sfxVolume) { playSfx("sfx_pickup", sfxVolume); }
inline void sfxAttack(int sfxVolume) { playSfx("attack",     sfxVolume); }

#pragma warning(pop)

#endif
