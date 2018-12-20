#include <SDL2/SDL.h>
#define GLEW_STATIC 
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <iostream>
#include <cmath>
#include <ctime>

#include "Games.h"
#include "Globals.h"
#include "Timer.h"
#include "Input.h"
#include "Shader.h"
#include "Entities.h"
#include "Level.h"
#include "Graphics.h"


struct BurnSpots{
	float xPos[16], yPos[16];
	int sinceMoved[16];
};

BurnSpots spots;

//Wastes time until next frame.
void WasteTime(Timer timer);

void Update(Timer timer);
void EdgeLogic();

void RenderEdge();

int Game()
{
	//Clear entities function necessary
	ClearEntitiesAndComponents();

	srand(time(0));
	SetUpScenario(30, rand());
	SetUpHealthPickup();

	Timer eTimer;
	Timer fpsTimer;
	eTimer.startClk();

	bool paused=false;
	bool healthRegen=false;
	while(PG_ST==GAME)
	{
		
		fpsTimer.startClk();
		

		HandleInput(eTimer);
		if(kStore&kEscape)
			PG_ST=MENU;
		if(kStore&kShift)
			paused=(!paused);
		if(kStore&kGrave)
			healthRegen=!(healthRegen);
		if(healthRegen)
			healthComps[GetPCIndex()].health=100;
			
		if(!paused){
			Update(eTimer);
		}

		//Render
		glClearColor(1.0, 1.0, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//store render commands
	
		RenderEntities();
		RenderEdge();
		RenderHUD();

		//execute render commands
		RenderAll();
		

		SDL_GL_SwapWindow(pWin);

		WasteTime(fpsTimer);
	}

	return 0;
}

void RenderEdge()
{
	//render all four "bars" of the edge
	float activeWidth=EDGE_SIZE*edgePercent/100;
	//render left, up, right, down
	//use 200 for size as hack
	RenderRectangle(0, 0, activeWidth, 100, gameShader, &sprites.blue, FLOOR);
	RenderRectangle(0, 0, 100, activeWidth, gameShader, &sprites.blue, FLOOR);
	RenderRectangle(100-activeWidth, 0, activeWidth, 100, gameShader, &sprites.blue, FLOOR);
	RenderRectangle(0, 100-activeWidth, 100, activeWidth, gameShader, &sprites.blue, FLOOR);
}

void EdgeLogic()
{
	//edge moves in if PC in territory, away if not.
	int pc=GetPCIndex();
	if(posComps[pc].x<EDGE_SIZE || posComps[pc].x>100-EDGE_SIZE || posComps[pc].y<EDGE_SIZE || posComps[pc].y>100-EDGE_SIZE){
		edgePercent+=80.0/FRAMES_PER_SECOND;
	}else{
		edgePercent-=15.0/FRAMES_PER_SECOND;
	}
	if(edgePercent>100)
		edgePercent=100;
	if(edgePercent<0)
		edgePercent=0;

	//Now if pc within edge active territory, deal damage.
	float activeSize=EDGE_SIZE*edgePercent/100;
	if(posComps[pc].x<activeSize || posComps[pc].x>100-activeSize || posComps[pc].y<activeSize || posComps[pc].y>100-activeSize)
		healthComps[pc].health-=30.0/FRAMES_PER_SECOND;
}

void Update(Timer timer)
{
	UpdateEntities(timer);
	EdgeLogic();
}

void WasteTime(Timer timer)
{
	while(timer.getTicks()<FRAME_TIME){
		static int tmWster=4;
		tmWster*=tmWster;
	}
}


