#ifndef GAMES_H_INCLUDED
#define GAMES_H_INCLUDED
#include "GLOBALS.h"
#include "Helpers.h"
#include "Timer.h"
#include "NPC.h"

//this is used for our non-characters, non-mouse textures.
struct TextureArray{
    SDL_Texture* pImg[32];
};

//These are text written on the screen at specific times. eg. tutorials
struct Instructions{
	std::string msgs[8];      //allocate memory for messages at run time.
	SDL_Texture* pImg[8];
	tdcRct rBx[8];
};

int SetGameMessages(Instructions* );
int SetAtkMessages(Instructions* pMsgs);
int SetTutMessages(Instructions* );

void RenderAimPoints(PC* pPC);
void RenderBar(tdcRct outline, float percent);
void HandleTutObs(TutObs* pObs);

void RenderHealthBars(AllChars chrs);

void SetUpCharacters(AllChars chrs);

int RunGames();

int LoadGameGFX(TextureArray* );

int LoadGameSFX();


void WasteTime(Timer fpsTm);

#endif // GAMES_H_INCLUDED
