#ifndef CUTSCENES_H_INCLUDED
#define CUTSCENES_H_INCLUDED
#include <string>
#include "IC_GlobalVars.h"
#include "Helpers.h"

bool StoreCutscenes(std::string cutScenes[64], int numScenes);

int PlayCutscene(gameState* pState, SDL_Renderer* pFrame, int* pLevel, gs dt);

#endif // CUTSCENES_H_INCLUDED
