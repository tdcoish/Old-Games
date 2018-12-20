#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED
#include <SDL.h>
#include "Helpers.h"

///Looking back at this September 2015. Could have just used an enum?
///Wow nevermind, what an idiot. Then the bits would overlap.
struct KeyPress{
	Uint16 store;
	const static Uint16 left=1<<0;		    //1
	const static Uint16 right=1<<1;	        //2
	const static Uint16 up=1<<2;		    //4
	const static Uint16 down=1<<3;	        //8
	const static Uint16 escape=1<<4;	    //16
	const static Uint16 shift=1<<5;	        //32
	const static Uint16 space=1<<6;	        //64
	const static Uint16 mouse=1<<7;	        //128
	const static Uint16 wheelUp=1<<8;       //256
	const static Uint16 wheelDown=1<<9;	    //512
	//...and so on.
};

struct GmMouse
{
    tdcRct clBx;
    SDL_Texture* pImg[3];
    bool pressed;
};

void SetUpKeyStore(KeyPress* );
void HandleKey(SDL_Keycode , KeyPress* );
void HandleKeyUp(SDL_Keycode , KeyPress* );
void HandleInput(SDL_Event* , KeyPress* ,GmMouse* );


void UpdateMouse(GmMouse* ,const KeyPress );

void SetUpMouse(GmMouse* );

struct UIMouse
{
    tdcRct clBx;
    SDL_Rect rndBx;
    SDL_Texture* pImg;
    bool pressed, stillPressed;     //because pressing and holding ruins the functionality of the menus
};

void SetUpMouse(UIMouse* );
void UpdateMouse(UIMouse* , SDL_Event );


#endif // EVENTS_H_INCLUDED
