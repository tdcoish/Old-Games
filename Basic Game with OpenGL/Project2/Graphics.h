#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

/**********************************
Graphics is larger than entities. 

Warning! Graphics expects units, not pixels.
**********************************/
#include "Resources.h"		//has Shader.h. could just write that include but who knows.

enum Layers{
	BACKGROUND, FLOOR, FLOOR_EFFECTS, PARTICLES, ENEMIES, PLAYER, HUD, END_LAYERS
};

struct RenderCommand{
	float xPos, yPos, xSize, ySize;
	float* pAngle;
	Sprite* pImg;
	Shader shader;
	Layers layer;
};

const int MAX_RENDER_CALLS=1024;

extern int activeRenderSlot;
extern RenderCommand* renderQ;

//Can render circular object with or without an angle.
void RenderSprite(float xPos, float yPos, float rad, Shader shader, Sprite* pSprite, float* pAngle, Layers layer);

//Can render square or rectangular object.
void RenderSprite(float xPos, float yPos, float xSize, float ySize, Shader shader, Sprite* pSprite, float* pAngle, Layers layer);

//When rendering HUD elements. Does no calculations for you.
void RenderRectangle(float xPos, float yPos, float xSize, float ySize, Shader shader, Sprite* pSprite, Layers layer);

//all above render commands are wrappers for this.
void StoreRenderCommand(float xPos, float yPos, float xSize, float ySize, Shader shader, Sprite* pSprite, float* pAngle, Layers layer);

//Iterate through renderQ, calling RenderImg()
void RenderAll();

//Actually draw image
void RenderImg(RenderCommand com);

#endif






