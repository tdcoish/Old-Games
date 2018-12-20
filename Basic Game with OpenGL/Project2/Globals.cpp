#include "Globals.h"
#include <string>
//#include "Shader.h"

Uint16 kStore;
float msX=50;
float msY=50;

float edgePercent=0.0;

SDL_Window* pWin=NULL;
SDL_GLContext glContext;		//no idea what to set it to for safety.

Uint16 PG_ST=MENU;				//eventually default to menu. 

SDL_Event e;

std::string bsPth=SDL_GetBasePath();

Shader gameShader;

float gameSpeed=0.5;

//Make the collision queue store nothing. Called at Init() and each frame.
void EmptyCollisionQueue()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		collisionsQ[i][0]=-1;
		collisionsQ[i][1]=-1;
	}
}

int collisionsQ[MAX_ENTITIES][2];		//initialization.