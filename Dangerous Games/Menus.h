#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED

#include "Helpers.h"
#include "GLOBALS.h"
#include "Events.h"
#include <SDL_ttf.h>


//used for button logic
enum ChangeType{
    STATE, DIFFICULTY
};

struct Buttons
{
    Uint16 activePages[32];
    bool active[32];
    bool msOver[32];
    bool pressed[32];
    tdcRct clBx[32];
    SDL_Rect rBx[32];
    SDL_Texture* pImg[32];
    ChangeType type[32];        //as of right now only values are state and difficulty
    StateTypes chngType[32];    //which substate to overwrite
    Difficulty dif[32];         //what to change the difficulty to.
    Uint16 newSt[32];       //copies over old state
    int totBts;         //so we never iterate farther than valid buttons
};

void SetUpButtons(Buttons* );
void ActivateButtons(Buttons* pBts);
void GlowButtons(Buttons* , const UIMouse );
void ButtonLogic(Buttons* );
void RenderButtons(Buttons*);
void DeleteButtons(Buttons* );

struct MenuBackgrounds
{
    SDL_Texture* pBk[8];
    Uint16 corSt[8];     //corresponding state that the background plays for.
    int totBks;
};

void SetUpBackgrounds(MenuBackgrounds* );
void RenderBackground(MenuBackgrounds* );

int RunMenu();

void IntroAndOutro(MenuBackgrounds* );

//need special logic for the game select screen. Oh well.
struct GameTypeMessages{
	std::string message[8];
	Uint16 type[8];     //set with global game states.
	SDL_Texture* pImg[8];
	tdcRct rBx[8];
};

int SetGTMessages(GameTypeMessages* );
void RenderGTMessages(GameTypeMessages );
void RenderGSExtras(GameTypeMessages, Buttons );
tdcRct GSButtonMidpoint(Buttons , ChangeType );

#endif // MENUS_H_INCLUDED
