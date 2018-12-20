
#include "Resources.h"
#include "Globals.h"
#include <SOIL/SOIL.h>
#include <GL/glew.h>

Sprites sprites;

void LoadAllSprites()
{
	SetUpSprite(&sprites.boltSprite, "bolt.png");
	SetUpSprite(&sprites.pcSprite, "PC.png");
	SetUpSprite(&sprites.turretSprite, "turret.png");
	SetUpSprite(&sprites.hunterSprite, "hunter.png");
	SetUpSprite(&sprites.orbSprite, "awesomeFace.png");
	SetUpSprite(&sprites.sniperSprite, "sniper.png");
	SetUpSprite(&sprites.healthPickupSprite, "HealthPickup.png");

	SetUpSprite(&sprites.cursor, "mouse.png");
	SetUpSprite(&sprites.blackSprite, "black.png");
	SetUpSprite(&sprites.whiteSprite, "white.png");
	SetUpSprite(&sprites.blue, "blue.png");
	SetUpSprite(&sprites.red, "red.png");
}

void SetUpSprite(Sprite* sprite, std::string textureName)
{
	GLuint VBO;
	GLfloat vertices[]={
		// Positions  // Texture Coords
         0.0f, 1.0f, 0.0f, 1.0f, 
         1.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f, 

		 0.0f, 1.0f, 0.0f, 1.0f, 
         1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 0.0f, 1.0f, 0.0f 
	};

	glGenVertexArrays(1, &sprite->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(sprite->quadVAO);
	//send position and Tex as one vec4
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);		//unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//Unbind VBO

	//Load texture and store texture ID.
	sprite->texture=LoadTexture("\\"+textureName);
}

GLuint LoadTexture(std::string imgName){
	imgName="img\\"+imgName;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);		//this is the texture we operate on until specified otherwise
	//set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Load image, create texture, generate mipmaps
	int wdth, hgt;
	unsigned char* pImg1=NULL;
	pImg1 = SOIL_load_image((bsPth+imgName).c_str(), &wdth, &hgt, 0, SOIL_LOAD_RGB);
	if(pImg1==NULL)
		std::cout << imgName << " did not load" << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wdth, hgt, 0, GL_RGB, GL_UNSIGNED_BYTE, pImg1);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(pImg1);
	glBindTexture(GL_TEXTURE_2D, 0);		//now operating on no textures, bug control

	return textureID;
}







