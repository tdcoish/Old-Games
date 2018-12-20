#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <SDL2/SDL.h>
#include "Timer.h"

//Keyboard and mouse bit handling.
extern Uint16 kStore;				//stores one bit for each valid key.
const Uint16 kLeft=1<<0;				//1
const Uint16 kRight=1<<1;	        //2
const Uint16 kUp=1<<2;				//4
const Uint16 kDown=1<<3;				//8
const Uint16 kEscape=1<<4;			//16
const Uint16 kShift=1<<5;	        //32
const Uint16 kSpace=1<<6;	        //64
const Uint16 mouse=1<<7;	        //128
const Uint16 wheelUp=1<<8;			//256
const Uint16 wheelDown=1<<9;	    //512
const Uint16 kGrave=1<<10;
//...and so on.

extern bool msDoubleClicked;
extern int lastMouseClick;		//or whatever max time to register double clicks. in ms

extern float msX;
extern float msY;

void HandleInput(Timer eTimer);

void HandleKeyDown(SDL_Keycode key);

void HandleKeyUp(SDL_Keycode key);


//gets called every time the left mouse button gets pressed.
void CheckMouseDoubleClicked(Timer eTimer);

//I don't handle the mouse being double clicked here, that gets changed elsewhere.
//I do need to make sure that msDoubleClicked is always made false after 1 frame. 
//I do this at the start of HandleInput();

#endif