#ifndef CTF_H_INCLUDED
#define CTF_H_INCLUDED
#include "IC_GlobalVars.h"
#include "Helpers.h"
/*****************************************************************************
So after about 6 months, it all comes down to today. Here is the final game
state. It's just one sided capture the flag.
*****************************************************************************/
//just easier than making a bunch of float xPos, yPos type things
struct Position{
    float x, y;
};

//Set of objects positions for a given number of objects
struct ObjectPositions{
    Position ps[6];
};
//Set of all objects positions
struct AllObjects{
    ObjectPositions ob[6];
};

//Set of turret positions
struct Arrangements{
    Position ps[8];
};
//Set of turret all turret positions for a given number of turrets
struct ArrsSet{
    int arNm;
    Arrangements ars[32];
};
//Set of all turret positions
struct TurretPositions{
    ArrsSet sets[8];
};

void SetObjectPositions(AllObjects* pDt);
void SetTurretPositions(TurretPositions* pTrs);

int CTFGame(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, Mix_Music* pMsc, gs dt, const int turs, const Diff df);

#endif // CTF_H_INCLUDED
