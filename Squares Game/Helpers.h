#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

//short for graphics but who really cares?
struct gs{
    float scl;
    int xPush, SH, SW;
    SDL_Rect br1, br2;
};

struct tdcRct{
    float w, h, x, y;
};

bool Init(SDL_Window** ppFrame, SDL_Renderer** ppScene, gs* pDt);

void ApplyTexture(SDL_Texture* pSrc, SDL_Renderer* pFrame, SDL_Rect* pSrcClp, int x, int y);

void ScaleApplyTexture(SDL_Texture* pSrc, SDL_Renderer* pFrame, SDL_Rect* pSrcClp, int x, int y, int size);

///loading a PNG and converting it to a texture. Unneccessary here
bool LoadTexture(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend);
bool LoadTextureNoA(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend);
bool LoadTextureBMP(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend);

///Very simple wrapper. I may make changes later.
bool LoadMusic(std::string path, Mix_Music** ppMusic);
bool LoadSFX(std::string path, Mix_Chunk** ppChunk);

bool CheckCollision(SDL_Rect A, SDL_Rect B);
bool CheckCollision(tdcRct A, tdcRct B);

double FindAngle(double yVel, double xVel);
double GetPCAngle(double xDis, double yDis);

Uint32 getPixel(SDL_Surface* pSurface, int x, int y);

void putPixel(SDL_Surface* pSurface, int x, int y, Uint32 pixel);

#endif      //HELPERS_H_INCLUDED hopefully included
