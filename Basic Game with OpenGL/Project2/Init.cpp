#include "Globals.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "Shader.h"
#include "Entities.h"
#include "Resources.h"

void Init()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	pWin=SDL_CreateWindow("Test2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SH, SH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	//creates the context, and makes it current.
	glContext=SDL_GL_CreateContext(pWin);

	SDL_GL_SetSwapInterval(IMMEDIATE);//IMMEDIATE, VSYNC_WITH_TEAR, VSYNC

	SDL_SetRelativeMouseMode(SDL_TRUE);		//grabs the mouse for our window.
	
	glewExperimental=GL_TRUE;
	glewInit();

	glViewport(0, 0, SH, SH);

	SetShader((bsPth+"\\vertShaderGame.vs").c_str(), (bsPth+"\\fragShaderGame.frag").c_str(), &gameShader);

	ClearEntitiesAndComponents();

	LoadAllSprites();

	Timer programTimer;
	programTimer.startClk();

	//make the collision queue nothing
	EmptyCollisionQueue();

}


