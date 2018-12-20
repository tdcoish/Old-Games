#ifndef LEVEL_IO_H_INCLUDED
#define LEVEL_IO_H_INCLUDED
#include <string>

struct Level
{
    int levelNum;
    int numHunters, numBolts, numSnipers, numSpots, numBombs, numSnipes, numSprinklers, numSprinkBolts, numTurrets, numTurBolts;
    int huntDam, boltDam, burnDam, bombDam, snipeDam;
    int numHealth;
    int healthHeal;     //amount each health pack heals for.
    int drainRate;      //rate that health gets drained per second.
    int barrierSize;
    int time;           //amount of time the player has to survive for.
};

struct LvWv
{
    int nmPks;
    float pkX[16];
    float pkY[16];
    int nmTrs;
    float turX[16];
    float turY[16];
    int nmHnts, nmSnps, nmSpts, nmBms, nmSpnk, nmSnprs, brSz, nmPls;
    float plrX[16];
    float plrY[16];
    int time;
};

struct Lev
{
    std::string title;
    int nmWvs;
    LvWv wvs[16];
};

void CreateDummyLevel(Level* pLevel);
int WriteLevel(Level newLevel, std::string levelFile);
int ReadLevel(Level* pLevel, std::string levelName);

int WriteLev(Lev lv, std::string flName);
int ReadLev(Lev* pLv, std::string flNm);

#endif // LEVEL_IO_H_INCLUDED
