/***********************************************************************************
Resources for now only contains the textures.
***********************************************************************************/
#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED
#include "Entities.h"
#include <string>

//For now just hard code sprites into here. Eventually have a linked list with names or something like that
struct Sprites{
	Sprite pcSprite;
	Sprite hunterSprite;
	Sprite turretSprite;
	Sprite boltSprite;
	Sprite orbSprite;
	Sprite sniperSprite;
	Sprite healthPickupSprite;

	Sprite cursor;
	Sprite blackSprite;
	Sprite whiteSprite;
	Sprite blue;
	Sprite red;
};

GLuint LoadTexture(std::string imgName);

void SetUpSprite(Sprite* sprite, std::string textureName);

void LoadAllSprites();





#endif