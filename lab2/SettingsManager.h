
#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <stdio.h>

#define VOLUME_MIN 0
#define VOLUME_MAX 10

struct Settings
{
    int sfxVolume;
    int musicVolume;
};

inline void setDefaultSettings(Settings &s)
{
    s.sfxVolume = 7;
    s.musicVolume = 7;
}

inline void loadSettings(Settings &s)
{
    FILE *f = fopen("settings.dat", "rb");
    if (!f) { setDefaultSettings(s); return; }
    fread(&s, sizeof(Settings), 1, f);
    fclose(f);
}

inline void saveSettings(Settings &s)
{
    FILE *f = fopen("settings.dat", "wb");
    if (!f) return;
    fwrite(&s, sizeof(Settings), 1, f);
    fclose(f);
}

inline void increaseSfx(Settings &s)   { if (s.sfxVolume < VOLUME_MAX) s.sfxVolume++;   saveSettings(s); }
inline void decreaseSfx(Settings &s)   { if (s.sfxVolume > VOLUME_MIN) s.sfxVolume--;   saveSettings(s); }
inline void increaseMusic(Settings &s) { if (s.musicVolume < VOLUME_MAX) s.musicVolume++; saveSettings(s); }
inline void decreaseMusic(Settings &s) { if (s.musicVolume > VOLUME_MIN) s.musicVolume--; saveSettings(s); }

#endif 
