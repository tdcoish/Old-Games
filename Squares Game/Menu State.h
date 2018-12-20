#ifndef MENU_STATE_H_INCLUDED
#define MENU_STATE_H_INCLUDED

#include "IC_GlobalVars.h"
struct button
{
    bool mouseOver;
    SDL_Texture* im1;
    SDL_Texture* im2;
    SDL_Rect rnd;
    tdcRct loc;
};

void DestroyButton(button* pBt);
void RenderButton(const button bt, SDL_Renderer* pFrame);
void SetButtonRender(button* pBt, const gs dt);

int Menu(gameState* pState, menuState* pMenuState, tutState* pTutState, menSubState* pSub, SDL_Renderer* pFrame, int* pLevel, Mix_Music* pMsc, gs dt, int* nmTurs, Diff* df);

int MenuIntro(gameState* pState, menuState* pMenuState, SDL_Renderer* pFrame, gs dt);

int TtlScrn(gameState* pState, menuState* pMenuState, menSubState* pSub, SDL_Renderer* pFrame, Mix_Music* pMsc, SDL_Texture* pBkGnd, gs dt, int* nmTurs, Diff* df);
int TutScrn(gameState* pState, menuState* pMenuState, tutState* pTutState, menSubState* pSub, SDL_Renderer* pFrame, Mix_Music* pMsc, SDL_Texture* pBkGnd, gs dt);
int CredScrn(gameState* pSt, menuState* pMenSt, menSubState* pMnSbSt, SDL_Renderer* pFrame, Mix_Music* pMsc, SDL_Texture* pBk, gs dt);
int MenuMain(gameState* pState, menuState* pMenuState, menSubState* pSub, tutState* pTutState, SDL_Renderer* pFrame, int* pLevel, Mix_Music* pMsc, gs dt, int* nmTurs, Diff* df);

int MenuOutro(gameState* pState, menuState* pMenuState, SDL_Renderer* pFrame, gs dt);

#endif // MENU_STATE_H_INCLUDED
