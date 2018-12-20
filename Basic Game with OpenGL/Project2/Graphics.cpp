/*********************
General purpose graphics. Specific graphical set up belongs in other files as of right now.
********************/

#include "Globals.h"
#include "Resources.h"
#include "Graphics.h"

RenderCommand* renderQ = new RenderCommand[MAX_RENDER_CALLS];
int activeRenderSlot=0;

//Can render circular object with or without an angle.
void RenderSprite(float xPos, float yPos, float rad, Shader shader, Sprite* pSprite, float* pAngle, Layers layer)
{
	xPos-=rad;
	yPos-=rad;
	float xSize=rad*2;
	float ySize=rad*2;
	StoreRenderCommand(xPos, yPos, xSize, ySize, shader, pSprite, pAngle, layer);
}

//Can render square or rectangular object.
void RenderSprite(float xPos, float yPos, float xSize, float ySize, Shader shader, Sprite* pSprite, float* pAngle, Layers layer)
{
	xPos-=xSize/2;
	yPos-=ySize/2;
	StoreRenderCommand(xPos, yPos, xSize, ySize, shader, pSprite, pAngle, layer);
}

//Naked rendering. Use for HUD elements.
void RenderRectangle(float xPos, float yPos, float xSize, float ySize, Shader shader, Sprite* pSprite, Layers layer)
{
	StoreRenderCommand(xPos, yPos, xSize, ySize, shader, pSprite, NULL, layer);
}

void StoreRenderCommand(float xPos, float yPos, float xSize, float ySize, Shader shader, Sprite* pSprite, float* pAngle, Layers layer)
{
	if(activeRenderSlot==MAX_RENDER_CALLS)		//prevent buffer overflow.
		return;

	int i=activeRenderSlot;		//just for readability
	renderQ[i].layer=layer;
	renderQ[i].pAngle=pAngle;
	renderQ[i].pImg=pSprite;
	renderQ[i].shader=shader;
	renderQ[i].xPos=xPos;
	renderQ[i].yPos=yPos;
	renderQ[i].xSize=xSize;
	renderQ[i].ySize=ySize;
	
	activeRenderSlot++;
}

//Iterate through renderQ, calling RenderImg()
void RenderAll()
{
	//render each layer successively.
	for(int lay=0; lay<END_LAYERS; lay++){
		for(int j=0; j<activeRenderSlot; j++){
			if(renderQ[j].layer==lay){
				RenderImg(renderQ[j]);
			}
		}
	}
	//probably should just call ResetRenderQ() here
	activeRenderSlot=0;
}

//Actually draw image
void RenderImg(RenderCommand com)
{
	UseShader(com.shader);
	com.xPos*=UNITS_TO_PIXELS;
	com.yPos*=UNITS_TO_PIXELS;
	com.xSize*=UNITS_TO_PIXELS;
	com.ySize*=UNITS_TO_PIXELS;

	glm::vec2 pos(com.xPos, com.yPos);
	glm::vec2 size(com.xSize, com.ySize);

	glm::mat4 model;
	model=glm::translate(model, glm::vec3(pos, 0.0f));

	//rotation specific code
	if(com.pAngle!=NULL){
		float angleInRadians=*com.pAngle*DEGREES_TO_RADIANS;
		model=glm::translate(model, glm::vec3(0.5f*size.x, 0.5f*size.y, 0.0f));	//move origin of rotation to center of quad
		model=glm::rotate(model, angleInRadians, glm::vec3(0.0f, 0.0f, 1.0f));		//rotate object appropriately
		model=glm::translate(model, glm::vec3(-0.5f*size.x, -0.5f*size.y, 0.0f));	//move origin back
	}

	model=glm::scale(model, glm::vec3(size, 1.0f));

	GLint modelLoc=glGetUniformLocation(com.shader.Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//projection matrix may be cause of unfortunate bugs. Watch out.
	glUniformMatrix4fv(glGetUniformLocation(com.shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform1i(glGetUniformLocation(com.shader.Program, "myTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, com.pImg->texture);
	//draw texture finally
	glBindVertexArray(com.pImg->quadVAO);		//pSprite not set in games.cpp
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}





