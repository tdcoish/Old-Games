#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>    //I'm going to need fonts eventually
#include <SDL_mixer.h>  //for sounds
#include <string>       //going to need strings eventually
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include <cstdio>

extern const double FRAME_TIME;

//loading a bmp might not work.
int IntroState(gameState* pState, SDL_Renderer* pFrame)
{
    SDL_Texture* pBackground = NULL;
    if (!LoadTextureBMP(tdcDirectoryPath+"\\tempIntroScreen.bmp", &pBackground, pFrame)){
        printf("Failed loading background image");
        *pStates = QUIT;
    }
    if (*pStates == INTRO){
        //clear screen.
        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);

        ApplyTexture(pBackground, pFrame, NULL, 0, 0);
        SDL_RenderPresent(pFrame);
        SDL_Delay(800);

        *pStates = MENU;

        SDL_DestroyTexture(pBackground);
        return 0;
    }
    return 1;       //shouldn't get here.
}

int OutroState(gameState* pState, SDL_Renderer* pFrame)
{
    SDL_Texture* pBackground = NULL;
    if (!LoadTextureBMP(tdcDirectoryPath+"\\tempOutroScreen.bmp", &pBackground, pFrame)){
        printf("Failed loading background image");
        *pStates = QUIT;
    }
    if (pBackground == NULL){
        printf("Error loading background image");
        *pStates = QUIT;
    }
    if (*pStates == OUTRO){
        ApplyTexture(pBackground, pFrame, NULL, 0, 0);
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
    return 1;       //shouldn't get here.
}

