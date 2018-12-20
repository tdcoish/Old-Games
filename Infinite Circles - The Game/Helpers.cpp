#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include <cstdio>


///I recently removed the swsurface flag and added the HWSURFACE and DOUBLEBUF flags. I see no diff
SDL_Surface* Init(const int SW, const int SH)
{
    if (SDL_INIT_EVERYTHING == -1) return NULL;
    //initialize fonts if needed

    if (TTF_Init() == -1) return NULL;

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) return NULL;

    SDL_Surface* pFrameBuffer = NULL;
    //pFrameBuffer = SDL_SetVideoMode(SW, SH, 32, SDL_SWSURFACE | SDL_RESIZABLE | SDL_FULLSCREEN);
    pFrameBuffer = SDL_SetVideoMode(SW, SH, 32, SDL_SWSURFACE | SDL_RESIZABLE);

    SDL_WM_SetCaption("Squares Game version 0.1", NULL);

    return pFrameBuffer;
}

///a wrapper for SDL_RenderCopy(*renderer, *texture, *srcrect, *dstrect);
void ApplyTexture(SDL_Renderer* pFrame, SDL_Texture* pSrc, SDL_Rect* pSrcClp, int x, int y)
{
    SDL_Rect dstClp;
    dstClp.x = x;
    dstClp.y = y;

    SDL_RenderCopy(pFrame, pSrc, pSrcClp, &dstClp);
}

///load in a PNG and convert it to a texture.
bool LoadTexture(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend)
{
    SDL_Surface* pImg = NULL;
    SDL_Texture* pTemp = NULL;
    pImg = IMG_Load(path.c_str());
    if (pImg == NULL){
        printf("PNG not loaded. IMG Error: %s\n", IMG_GetError());
        return NULL;
    }
    pTemp = SDL_CreateTextureFromSurface(pRend, pImg);
    if (pTemp == NULL){
        printf("Surface to texture conversion failed. Error: %s\n", SDL_GetError());
        return NULL;
    }
    //we again need to free the surface
    SDL_FreeSurface(pImg);

    *ppTextImg = pTemp;
    return true;
}


bool CheckCollision(SDL_Rect A, SDL_Rect B)
{
    //the sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //calculate the sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //calculate the sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    //if any of the sides from A are outside of B
    //All these calculations assume no slants or rotations
    if (bottomA <= topB)    //if the bottom of one is below the top of the other
    {
        return false;
    }
    if (topA >= bottomB) return false;
    if (leftA >= rightB) return false;
    if (rightA <= leftB) return false;

    return true;        //if none of the sides from a are outside b.
}

//This function must be called on a locked surface, using SDL_LockSurface
Uint32 getPixel(SDL_Surface* pSurface, int x, int y)
{
    int bpp = pSurface->format->BytesPerPixel;
    Uint8* pPixel = (Uint8*)pSurface->pixels + y*pSurface->pitch + x*bpp;

    switch(bpp)
    {
    case 1:
        return *pPixel;
        break;

    case 2:
        return *(Uint16*)pPixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pPixel[0] << 16 | pPixel[1] << 8 | pPixel[2];
        else
            return pPixel[0] | pPixel[1] << 8 | pPixel[2] << 16;
        break;

    case 4:
        return *(Uint32*)pPixel;
        break;
    default:
        return 0;           //shouldn't happen, avoids warnings.
    }
}

//This function must be called on a locked surface, using SDL_LockSurface
void putPixel(SDL_Surface* pSurface, int x, int y, Uint32 pixel)
{
    int bpp = pSurface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)pSurface->pixels + y * pSurface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else
        {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}




//code for creating a new square .bmp
/*
    Uint32 rMask, gMask, bMask, aMask;
    rMask = 0xFF000000;
    gMask = 0x00FF0000;
    bMask = 0x0000FF00;
    aMask = 0x000000FF;

    //size is 20x20, bit depth is 32. Final colour is gold
    pHunterImg = SDL_CreateRGBSurface(SDL_SWSURFACE, 20, 20, 32, rMask, gMask, bMask, aMask);
    SDL_FillRect(pHunterImg, &pHunterImg->clip_rect, SDL_MapRGB(pHunterImg->format, 255, 215, 0));
    SDL_SaveBMP(pHunterImg, "tempHunter.bmp");

    new and improved:
    Uint32 rMask, gMask, bMask, aMask;
    rMask = 0xFF000000;
    gMask = 0x00FF0000;
    bMask = 0x0000FF00;
    aMask = 0x000000FF;

    //255 alpha is totally opaque, 0 alpha is totally invisible
    pHealthPacksImg = SDL_CreateRGBSurface(SDL_SWSURFACE, 20, 20, 32, rMask, gMask, bMask, aMask);
    if (pHealthPacksImg == NULL) return 1;

    SDL_FillRect(pHealthPacksImg, &pHealthPacksImg->clip_rect, SDL_MapRGBA(pHealthPacksImg->format, 255, 255, 255, 255));

    for (int x = 0; x < 20; x++){
        for (int y = 0; y < 20; y++){
            //Uint32 pixel = getpixel(pHealthPacksImg, 3, 3); //returns an individual pixel
            if ((x>5 && x<15) || (y>5 && y<15)){
                Uint32 newPixel = SDL_MapRGBA(pHealthPacksImg->format, 205, 0, 0, 255);
                SDL_LockSurface(pHealthPacksImg);
                putPixel(pHealthPacksImg, x, y, newPixel);
            }
        }
    }

    SDL_SaveBMP(pHealthPacksImg, "tempHealthPack.bmp");

*/

