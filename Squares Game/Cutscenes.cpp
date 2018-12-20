/****************************************************************************
This file contains all the relevant code for loading and writing cutscene
files, which are simple text files, and handling the data, in the Cutscenes
class.

Best part is I don't even need to create any of the cutscenes in code, I can
just use a simple text editor to do that.

Worst part is that I need to somehow load in all the different cutscenes by
hand. I might just make a bunch of cutscenes called "cutscene0" cutscene1,
and so on. That has the downside of not being informative about the content,
but the upside of telling me the exact order that the cutscenes are going to
play in.

Turns out MinGW doesn't support std::to_string, which is a huge pain in the
ass.

I have a problem. I'm only reading until the first space, and storing that
in the string.

Finally got it to work.
****************************************************************************/
#include "Cutscenes.h"
#include <cstdio>           //debugging
#include <fstream>
#include <sstream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "IC_GlobalVars.h"
#include "Helpers.h"

//I could pass by hand an int that tells us how many cutscenes to load
//or I could check if we even can load a certain cutscene, and then
//break if not.
bool StoreCutscenes(std::string cutScenes[64], int numScenes)
{
    //the final argument to itoa is the base, should always be 10
    //also, itoa is a lifesaver
    std::string path;
    std::stringstream tempFile;
    char buffer[32];        //could make it 2 really.
    for (int i = 0; i < numScenes; i++){
        tempFile.str("");       //clear ss for each new file
        itoa(i, buffer, 10);
        std::string num(buffer);
        path = tdcDirectoryPath+"\\Cutscenes\\cutscene"+num+".txt";
        std::ifstream inScene(path.c_str());
        if (!inScene.is_open()){
            printf("Could not load cutscene %i\n", i);
            return false;
        }
        tempFile << inScene.rdbuf();
        cutScenes[i] = tempFile.str();
    }
    return true;
}

//so we load in all the cutscenes, and play the right one.
int PlayCutscene(gameState* pState, SDL_Renderer* pFrame, int* pLevel, gs dt)
{
    std::string cutScenes[64];
    if (!StoreCutscenes(cutScenes, 10)){
        printf("Failed loading cutscenes\n");
        return 1;
    }
    //Draw the screen black, then paste a texture on that has the
    //text from the cutscene written to it.
    TTF_Font* pFont = NULL;
    pFont = TTF_OpenFont((tdcDirectoryPath+"\\lazyFont.ttf").c_str(), 36);
    if (pFont == NULL) return 1;
    SDL_Color fontColour = {0, 192, 192};     //used for all cutscenes

    SDL_SetRenderDrawColor(pFrame, 0, 0, 0, 255);       //pure black background.
    SDL_RenderClear(pFrame);

    SDL_Texture* pSceneText;
    SDL_Surface* pSceneSoft;
    pSceneSoft = TTF_RenderText_Solid(pFont, cutScenes[*pLevel].c_str(), fontColour);
    printf(cutScenes[*pLevel].c_str());
    pSceneText = SDL_CreateTextureFromSurface(pFrame, pSceneSoft);
    //ScaleApplyTexture(pSceneText, pFrame, NULL, 0, 0, SCREEN_WIDTH);
    ApplyTexture(pSceneText, pFrame, NULL, 0, 0);
    SDL_RenderPresent(pFrame);

    SDL_Event eventStr;
    while (*pState == CUTSCENE){
        while (SDL_PollEvent(&eventStr)){
            if (eventStr.type == SDL_MOUSEBUTTONDOWN){
                *pState = MENU;
            } else if (eventStr.type == SDL_KEYDOWN){
                *pState = MENU;
            } else if (eventStr.type == SDL_QUIT){
                *pState = QUIT;
            }
        }
        SDL_Delay(10);
    }

    SDL_FreeSurface(pSceneSoft);
    SDL_DestroyTexture(pSceneText);

    return 0;
}


















