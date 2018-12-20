/**************
Single button menu for now.

Note: Always remember to write GLEW_STATIC and include glew.h
before SDL2.
**************/
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>

#include "Globals.h"
#include "Graphics.h"
#include "Shader.h"
#include "Input.h"


int Menu()
{
	Timer eTimer;
	eTimer.startClk();
	while(PG_ST==MENU){
		HandleInput(eTimer);
		//rendering something stupid.
		glClearColor(0.9f, 0.5f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//render mouse for practice
		RenderSprite(msX, msY, 6, gameShader, &sprites.pcSprite, NULL, HUD);

		RenderAll();
		SDL_GL_SwapWindow(pWin);

		if(kStore&kSpace)
			PG_ST=GAME;
		if(kStore&kShift)
			PG_ST=QUIT;

		SDL_Delay(10);
	}

	return 0;
}






