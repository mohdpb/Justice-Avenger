#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "GameCommon.h"
#include <stdio.h>

struct SaveData
{
    int currentDay;
    int livesRemaining;
};

inline void setDefaultSave(SaveData &s)
{
    s.currentDay = 1;
    s.livesRemaining = TOTAL_LIVES;
}

inline bool saveFileExists()
{
    FILE *f = fopen("save.dat", "rb");
    if (f) { fclose(f); return true; }
    return false;
}


inline bool loadSave(SaveData &s)
{
    FILE *f = fopen("save.dat", "rb");
    if (!f) { setDefaultSave(s); return false; }
    fread(&s, sizeof(SaveData), 1, f);
    fclose(f);
    return true;
}

inline void writeSave(SaveData &s)
{
    FILE *f = fopen("save.dat", "wb");
    if (!f) return;
    fwrite(&s, sizeof(SaveData), 1, f);
    fclose(f);
}


inline void deleteSave()
{
    remove("save.dat");
}

#endif 
