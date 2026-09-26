// ============================================================
//  AudioManager.h
//  Scenario-based music + SFX for Justice Avenger
// ============================================================
//
//  SUPPORTED FORMATS
//  ----------------------------------------
//  Both MP3 (.mp3) and WAV (.wav) are fully supported!
//  MCI's "mpegvideo" driver is used for BGM, allowing native
//  looping (repeat) and volume control for both MP3 and WAV.
//
//  ASSET PATHS  (place files in assets/audio/)
//  ----------------------------------------
//  BGM (auto-loops):
//    assets/audio/bgm_menu.mp3     or .wav
//    assets/audio/bgm_story.mp3    or .wav
//    assets/audio/bgm_gameplay.mp3 or .wav
//
//  SFX:
//    assets/audio/sfx_click.wav    or .mp3
//    assets/audio/sfx_hit.wav      or .mp3
//    assets/audio/sfx_pickup.wav   or .mp3
//
//  HOW VOLUME WORKS
//  ----------------------------------------
//  BGM  volume is driven by Settings::musicVolume (0-10) -> MCI 0-1000
//  SFX  volume is driven by Settings::sfxVolume   (0-10)
// ============================================================

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

// Suppress MSVC C4996 deprecated-CRT warnings for this header only
#pragma warning(push)
#pragma warning(disable: 4996)

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif
#pragma comment(lib, "winmm.lib")

// ── BGM track identifiers ────────────────────────────────────
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

// ── Internal state ───────────────────────────────────────────
static BgmTrack  g_currentBgm      = BGM_NONE;
static bool      g_bgmOpen         = false;
static int       g_bgmMusicVolume  = -1;   // cached to detect changes

// ── Helpers ──────────────────────────────────────────────────
inline bool audioFileExists(const char* path)
{
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

// Searches for <baseName>.wav first, then <baseName>.mp3 (or directly if extension provided)
inline bool findAudioPath(const char* baseName, char* outPath, size_t outSize = 260)
{
    // If user provided a path with extension already
    if (strstr(baseName, ".wav") || strstr(baseName, ".mp3"))
    {
        sprintf(outPath, "%s", baseName);
        if (audioFileExists(outPath)) return true;
        sprintf(outPath, "assets/audio/%s", baseName);
        if (audioFileExists(outPath)) return true;
    }

    // Try .wav first
    sprintf(outPath, "assets/audio/%s.wav", baseName);
    if (audioFileExists(outPath)) return true;

    // Try .mp3
    sprintf(outPath, "assets/audio/%s.mp3", baseName);
    if (audioFileExists(outPath)) return true;

    // Try direct assets/
    sprintf(outPath, "assets/%s.wav", baseName);
    if (audioFileExists(outPath)) return true;
    sprintf(outPath, "assets/%s.mp3", baseName);
    if (audioFileExists(outPath)) return true;

    outPath[0] = '\0';
    return false;
}

// Convert 0-10 volume scale to MCI 0-1000
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

// ── Public API ───────────────────────────────────────────────

// Call every game tick (or at least on state changes) to keep BGM in sync.
// Pass the current desired track and the current musicVolume (0-10).
inline void updateBgm(BgmTrack desired, int musicVolume)
{
    // Switch track if changed
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
            // Track file not added yet - silent skip
            g_bgmOpen = false;
            return;
        }

        // Open with mpegvideo device type (supports both MP3 & WAV, repeat, and volume)
        char openCmd[512];
        sprintf(openCmd, "open \"%s\" type mpegvideo alias " BGM_ALIAS, filePath);
        MCIERROR err = mciSendStringA(openCmd, NULL, 0, NULL);
        if (err != 0)
        {
            // Fallback: try opening without explicit type keyword
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

        // Apply initial volume
        bgmApplyVolume(musicVolume);

        // Play looping
        MCIERROR playErr = mciSendStringA("play " BGM_ALIAS " repeat", NULL, 0, NULL);
        if (playErr != 0)
        {
            // If repeat parameter isn't supported by this file type, play without repeat
            mciSendStringA("play " BGM_ALIAS, NULL, 0, NULL);
        }
        printf("[Audio] Playing BGM track: %s\n", filePath);
    }
    else if (g_bgmOpen && musicVolume != g_bgmMusicVolume)
    {
        // Same track, but volume changed in audio settings
        bgmApplyVolume(musicVolume);
    }
}

// Stop all BGM immediately
inline void stopBgm()
{
    bgmClose();
    g_currentBgm = BGM_NONE;
}

// Play a one-shot SFX. Gated by sfxVolume (0 = muted).
// Supports both .wav (via PlaySound) and .mp3 (via MCI).
inline void playSfx(const char* baseName, int sfxVolume)
{
    if (sfxVolume <= 0) return;

    char filePath[260];
    if (!findAudioPath(baseName, filePath, sizeof(filePath)))
        return; // sound file not added yet

    // If it's a WAV file, use PlaySoundA (fast, asynchronous, zero-latency)
    if (strstr(filePath, ".wav") || strstr(filePath, ".WAV"))
    {
        PlaySoundA(filePath, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
    }
    else
    {
        // If it's an MP3 file, play via a dedicated MCI alias
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

// Convenience wrappers (work with both .wav and .mp3 files)
inline void sfxClick (int sfxVolume) { playSfx("sfx_click",  sfxVolume); }
inline void sfxHit   (int sfxVolume) { playSfx("sfx_hit",    sfxVolume); }
inline void sfxPickup(int sfxVolume) { playSfx("sfx_pickup", sfxVolume); }

#pragma warning(pop)

#endif // AUDIO_MANAGER_H
