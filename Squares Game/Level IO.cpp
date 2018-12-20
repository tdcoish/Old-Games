/******************************************************************************
I store all the levels in text files. I then read in the right text file, and
play the game.
******************************************************************************/

#include "Level IO.h"
#include <cstdio>
#include <fstream>

void CreateDummyLevel(Level* pLevel)
{
    pLevel->levelNum = -1;
    pLevel->numHunters = -1;
    pLevel->numBolts = -1;
    pLevel->numSnipers = -1;
    pLevel->numSpots = -1;
    pLevel->huntDam = -1;
    pLevel->boltDam = -1;
    pLevel->burnDam = -1;
    pLevel->numHealth = -1;
    pLevel->healthHeal = -1;
    pLevel->drainRate = -1;
    pLevel->barrierSize = -1;
    pLevel->time = -10;
    pLevel->numBombs = -1;
    pLevel->numSnipes = 14;
    pLevel->numSprinklers = 1;
    pLevel->numSprinkBolts = -5;
    pLevel->numTurrets = -3;
    pLevel->numTurBolts = 101039;
}

int WriteLevel(Level newLevel, std::string levelFile)
{
    std::ofstream writerLevel(levelFile.c_str());
    if (!writerLevel.is_open()){
        printf("Could not write level.\n");
        return -1;
    }

    writerLevel << "Level:" << ' ' << newLevel.levelNum << '\n';
    writerLevel << "Number_of_Hunters:" << ' ' << newLevel.numHunters << '\n';
    writerLevel << "Number_of_bolts:" << ' ' << newLevel.numBolts << '\n';
    writerLevel << "Number_of_Snipers:" << ' ' << newLevel.numSnipers << '\n';
    writerLevel << "Number_of_Burn_Spots:" << ' ' << newLevel.numSpots << '\n';
    writerLevel << "Hunter_Damage:" << ' ' << newLevel.huntDam << '\n';
    writerLevel << "Bolt_Damage:" << ' ' << newLevel.boltDam << '\n';
    writerLevel << "Burn_Damage:" << ' ' << newLevel.burnDam << '\n';
    writerLevel << "Number_of_Health_Packs:" << ' ' << newLevel.numHealth<< '\n';
    writerLevel << "Health_healed_by_health_pack:" << ' ' << newLevel.healthHeal << '\n';
    writerLevel << "Health_drain_per_second:" << ' ' << newLevel.drainRate << '\n';
    writerLevel << "Barrier_dimension:" << ' ' << newLevel.barrierSize << '\n';
    writerLevel << "Time_to_survive:" << ' ' << newLevel.time << '\n';
    writerLevel << "Number_of_Bombs:" << ' ' << newLevel.numBombs << '\n';
    writerLevel << "Number_of_Snipes:" << ' ' << newLevel.numSnipes << '\n';
    writerLevel << "Number_of_Sprinklers:" << ' ' << newLevel.numSprinklers << '\n';
    writerLevel << "Number_of_Sprinkler_Bolts:" << ' ' << newLevel.numSprinkBolts << '\n';
    writerLevel << "Number_of_Turrets:" << ' ' << newLevel.numTurrets << '\n';
    writerLevel << "Number_of_Turret_Bolts:" << ' ' << newLevel.numTurBolts << '\n';

    writerLevel.close();
    return 0;
}

//had an extra .ignore line at the end. Cause me to read past the
//end of the file, got a crash.
//Also, can't just go to space, since I have spaces already.
int ReadLevel(Level* pLevel, std::string levelName)
{
    std::ifstream inLevel(levelName.c_str());
    if (!inLevel.is_open()){
        printf("Could not load level.\n");
        return -1;
    }

    inLevel.ignore(256, ' ');       //ignores everything until ' '
    inLevel >> pLevel->levelNum;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numHunters;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numBolts;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numSnipers;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numSpots;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->huntDam;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->boltDam;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->burnDam;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numHealth;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->healthHeal;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->drainRate;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->barrierSize;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->time;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numBombs;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numSnipes;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numSprinklers;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numSprinkBolts;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numTurrets;
    inLevel.ignore(256, ' ');
    inLevel >> pLevel->numTurBolts;

    inLevel.close();

    return 0;
}

int WriteLev(Lev lv, std::string flName)
{
    std::ofstream wrt(flName.c_str());
    if (!wrt.is_open()){
        printf("Could not write level.\n");
        return -1;
    }

    wrt << "Level:" << ' ' << lv.title << '\n';
    wrt << "Waves:" << ' ' << lv.nmWvs << '\n';
    wrt << "---------------------" << '\n';
    for (int i=0; i<lv.nmWvs; i++){
        wrt << "Number_of_Turrets:" << ' ' << lv.wvs[i].nmTrs << '\n';
        wrt << "Turret_Pos:";
        for (int j=0; j<lv.wvs[i].nmTrs; j++){
            wrt << ' ' << lv.wvs[i].turX[j] << ',' << lv.wvs[i].turY[j];
        }
        wrt << '\n' << "Number_of_Healthpacks:" << ' ' << lv.wvs[i].nmPks << '\n';
        wrt << "Healthpack_Pos:";
        for (int j=0; j<lv.wvs[i].nmPks; j++){
            wrt << ' ' << lv.wvs[i].pkX[j] << ',' << lv.wvs[i].pkY[j];
        }
        wrt << '\n' << "Number_of_Hunters:" << ' ' << lv.wvs[i].nmHnts << '\n';
        wrt << "Number_of_Snipes:" << ' ' << lv.wvs[i].nmSnps << '\n';
        wrt << "Number_of_Burnspots:" << ' ' << lv.wvs[i].nmSpts << '\n';
        wrt << "Number_of_Bombs:" << ' ' << lv.wvs[i].nmBms << '\n';
        wrt << "Number_of_Sprinklers:" << ' ' << lv.wvs[i].nmSpnk << '\n';
        wrt << "Number_of_Snipers:" << ' ' << lv.wvs[i].nmSnprs << '\n';
        wrt << "Barrier_Size:" << ' ' << lv.wvs[i].brSz << '\n';
        wrt << "Number_of_Pillars:" << ' ' << lv.wvs[i].nmPls << '\n';
        wrt << "Pillar_Pos:";
        for(int j=0; j<lv.wvs[i].nmPls; j++){
            wrt << ' ' << lv.wvs[i].plrX[j] << ',' << lv.wvs[i].plrY[j];
        }
        wrt << '\n' << "Time_to_next_wave:" << ' ' << lv.wvs[i].time << '\n';
        wrt << "---------------------\n";
    }

    wrt.close();
    return 0;
}

int ReadLev(Lev* pLv, std::string flNm)
{
    std::ifstream inLv(flNm.c_str());
    if(!inLv.is_open()){
        printf("Could not load level\n");
        return -1;
    }

    inLv.ignore(256, ' ');
    inLv >> pLv->title;
    inLv.ignore(256, ' ');
    inLv >> pLv->nmWvs;
    for(int i=0; i<pLv->nmWvs; i++){
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmTrs;
        for(int j=0; j<pLv->wvs[i].nmTrs; j++){
            inLv.ignore(256, ' ');
            inLv >> pLv->wvs[i].turX[j];
            inLv.ignore(256, ',');
            inLv >> pLv->wvs[i].turY[j];
        }
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmPks;
        for(int j=0; j<pLv->wvs[i].nmPks; j++){
            inLv.ignore(256, ' ');
            inLv >> pLv->wvs[i].pkX[j];
            inLv.ignore(256, ',');
            inLv >> pLv->wvs[i].pkY[j];
        }
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmHnts;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmSnps;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmSpts;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmBms;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmSpnk;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmSnprs;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].brSz;
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].nmPls;
        for(int j=0; j<pLv->wvs[i].nmPls; j++){
            inLv.ignore(256, ' ');
            inLv >> pLv->wvs[i].plrX[j];
            inLv.ignore(256, ',');
            inLv >> pLv->wvs[i].plrY[j];
        }
        inLv.ignore(256, ' ');
        inLv >> pLv->wvs[i].time;
    }

    inLv.close();
}

/*      these two are equivalent
    Level* pNewTestLevel = new Level;
    CreateDummyLevel(pNewTestLevel);
    WriteLevel(*pNewTestLevel, "newTestLevel.txt");

    Level newLevel;
    CreateDummyLevel(&newLevel);
    WriteLevel(newLevel, "newTestLevel.txt");
*/

/* Load in one file, output it as another
    Level newLevel;
    ReadLevel(&newLevel, "newTestLevel.txt");
    WriteLevel(newLevel, "newTestLevel2.txt");
*/

///How to make changes to the levels
/*  Level newLevel;
    CreateDummyLevel(&newLevel);
    if (WriteLevel(newLevel, tdcDirectoryPath+"\\changedLevel.txt") == -1){
        printf("Failed to write level.\n");
        return 1;
    }

    Level newNewLevel;
    if (ReadLevel(&newNewLevel, tdcDirectoryPath+"\\changedLevel.txt") == -1){
        printf("Failed loading new level\n");
        return 1;
    }

    if (WriteLevel(newNewLevel, tdcDirectoryPath+"\\newLevel.txt") == -1){
        printf("Could not read and write level\n");
        return 1;
    } */














