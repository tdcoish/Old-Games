#include "Globals.h"
#include "Input.h"

bool msDoubleClicked=false;
int lastMouseClick=-300;

void UpdateMouse(){
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	msX+=(float)x*PIXELS_TO_UNITS;
	msY+=(float)y*PIXELS_TO_UNITS;

	//check if mouse is out of bounds
	if(msX<0) msX=0;
	if(msX>100)msX=100;
	if(msY<0) msY=0;
	if(msY>100) msY=100;

}

//Right now simply handles keys, so is fairly shallow. 
void HandleInput(Timer eTimer)
{
	msDoubleClicked=false;		//must be made false at each frame.

	while(SDL_PollEvent(&e)!=0){
		if(e.type==SDL_KEYDOWN)
			HandleKeyDown(e.key.keysym.sym);
		else if(e.type==SDL_KEYUP)
			HandleKeyUp(e.key.keysym.sym);
		else if(e.type==SDL_MOUSEBUTTONDOWN)
			CheckMouseDoubleClicked(eTimer);
	}
	//mouse input
	UpdateMouse();
}

/********
Irrelevant right now, but allowing the user to change which keys do what is quite easy. I just need to create a bunch of variables in one struct that correspond to an SDL key.
eg:
SDL_keycode upKey=SDLK_W;		//can be changed by user
SDL_keycode leftKey=SDLK_A;		//again can be changed.
********/
void HandleKeyDown(SDL_Keycode key)
{
	switch(key){
		case SDLK_ESCAPE: kStore|=kEscape; break;
		case SDLK_w: kStore|=kUp; break;
		case SDLK_s: kStore|=kDown; break;
		case SDLK_a: kStore|=kLeft; break;
		case SDLK_d: kStore|=kRight; break;
		case SDLK_SPACE: kStore|=kSpace; break;
		case SDLK_LSHIFT: kStore|=kShift; break;
		case SDLK_BACKQUOTE: kStore|=kGrave; break;
	}
}

//Don't need to handle anything yet.
void HandleKeyUp(SDL_Keycode key)
{
	switch(key){
		case SDLK_ESCAPE: kStore=(kStore^kEscape); break;
		case SDLK_w: kStore=(kStore^kUp); break;
		case SDLK_s: kStore=(kStore^kDown); break;
		case SDLK_a: kStore=(kStore^kLeft); break;
		case SDLK_d: kStore=(kStore^kRight); break;
		case SDLK_SPACE: kStore=(kStore^kSpace); break;
		case SDLK_LSHIFT: kStore^=kShift; break;
		case SDLK_BACKQUOTE: kStore^=kGrave; break;
	}
}

void CheckMouseDoubleClicked(Timer eTimer){
	//if it was too long ago
	if(eTimer.getTicks()-lastMouseClick>300){
		lastMouseClick=eTimer.getTicks();
	}else{
		msDoubleClicked=true;
		lastMouseClick=-300;	//so user must double click again.
	}
}