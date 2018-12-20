#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>    //I'm going to need fonts eventually
#include <SDL_mixer.h>  //for sounds
#include <string>       //going to need strings eventually
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include <cstdio>

extern const int FRAME_TIME;

//loading a bmp might not work.
int IntroState(gameStates* pStates, SDL_Renderer* pFrame)
{
    SDL_Texture* pBackground = NULL;
    pBackground = LoadTexture("tempIntroScreen.bmp");
    if (pBackground == NULL){
        printf("Could not load background image");
        *pStates = QUIT;
    }
    ApplyTexture(pFrame, pBackground, NULL, 0, 0);
    SDL_RenderPresent(pFrame);
    SDL_Delay(800);
    *pStates = MENU;
    return 0;
}

int OutroState(gameStates* pStates, SDL_Renderer* pFrame)
{
    SDL_Texture* pBackground = NULL;
    pBackground = LoadTexture("tempOutroScreen.bmp");
    if (pBackground == NULL){
        printf("Error loading background image");
        *pStates = QUIT;
    }
    ApplyTexture(pFrame, pBackground, NULL, 0, 0);
    SDL_RenderPresent(pFrame);

    SDL_Event eventHandler;
    while (*pStates == OUTRO){
        while (SDL_PollEvent(&eventHandler)){
            if (eventHandler.type == SDL_KEYDOWN){
                *pStates = QUIT;
            }
            if (eventHandler.type == SDL_MOUSEBUTTONDOWN){
                *pStates = QUIT;
            }
            SDL_Delay(9);       //just to maintain ~100 fps, also give the CPU a break.
        }
    }
    return 0;

}

