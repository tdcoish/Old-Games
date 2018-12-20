#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include "GLOBALS.h"

bool Init();

void ApplyTexture(SDL_Texture* pSrc, SDL_Renderer* pFrame, SDL_Rect* pSrcClp, int x, int y);

void ScaleApplyTexture(SDL_Texture* pSrc, SDL_Renderer* pFrame, SDL_Rect* pSrcClp, int x, int y, int size);

///loading a PNG and converting it to a texture. Unneccessary here
bool LoadTexture(std::string path, SDL_Texture** ppTextImg);
bool LoadTextureNoA(std::string path, SDL_Texture** ppTextImg);

///Very simple wrapper. I may make changes later.
bool LoadMusic(std::string path, Mix_Music** ppMusic);
bool LoadSFX(std::string path, Mix_Chunk** ppChunk);

bool CheckCollision(SDL_Rect A, SDL_Rect B);
bool CheckCollision(tdcRct A, tdcRct B);

double FindAngle(double xVel, double yVel);
double GetPCAngle(double xDis, double yDis);

void SetRenderBox(tdcRct ,SDL_Rect* );

void UpdateGraphics();

void RenderObj(tdcRct ,Camera* ,SDL_Texture* ,float* );

#endif      //HELPERS_H_INCLUDED hopefully included
