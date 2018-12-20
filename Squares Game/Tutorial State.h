#ifndef TUTORIAL_STATE_H_INCLUDED
#define TUTORIAL_STATE_H_INCLUDED
#include <string>

struct Wave{
    int numObjects;
    double xPos[64];
    double yPos[64];
};

struct MoveTut{
    std::string title;
    int numWaves;
    Wave tutWave[16];           //never going to have more than 16 waves
};

struct AvdWv
{
    int nmObs;          //to be picked up and returned
    float objX[8];
    float objY[8];
    int nmTrs;
    double turX[16];
    double turY[16];
    int numHnts;
    int time;
};

struct AvdGm{
    std::string title;
    int nmWvs;
    AvdWv wvs[16];
};


bool LoadMoveTutorial(MoveTut* pTut, std::string fileName);
int WriteMoveTut(MoveTut tut, std::string fileName);

bool LoadAvoidGame(AvdGm* pGm, std::string flNm);
bool WriteAvoidGame(AvdGm game, std::string flNm);

int PlayTutorials(gameState* pState, menuState* pMenSt, tutState tut, SDL_Renderer* pFrame, gs dt);

int TutMove(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt);
int GmMove(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt);

int AtkTut(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt);

int AvoidGm(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt);

#endif // TUTORIAL_STATE_H_INCLUDED
