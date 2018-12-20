#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include <cstdio>
#include <glew.h>
#include <cmath>
//#include <string>

///Initializes SDL, SDL_image. Sets up screen window and surface
bool Init(SDL_Window** ppFrame, SDL_Renderer** ppScene, gs* pConv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Initialization failure. Error: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_Init(SDL_INIT_TIMER) < 0){
        printf("Error initializing timer. Error: %s\n", SDL_GetError());
        return false;
    }

    //Set texure filtering to linear.
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
        printf("Linear texture filtering not enabled\n");
        return false;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0")){
        printf("Failed to override vsync.\n");
        return false;
    }

    //create our window
    *ppFrame = SDL_CreateWindow("Alien Arena", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_DisplayMode md;
    SDL_GetWindowDisplayMode(*ppFrame, &md);
    pConv->scl = md.h/100;     //because the game size is 100 units
    pConv->xPush = (md.w-md.h)/2;      //so 1920-1200 /2 = 720/2 = 360.
    pConv->SH=md.h;
    pConv->SW=md.w;
    pConv->br1.x=0;
    pConv->br1.y=0;
    pConv->br1.w=pConv->xPush;
    pConv->br1.h=md.h;
    pConv->br2.x=pConv->xPush+md.h;     //because the height is the width of the actual game
    pConv->br2.y=0;
    pConv->br2.w=pConv->xPush;
    pConv->br2.h=md.h;

    if (*ppFrame == NULL){
        printf("Error: %s creating window.\n", SDL_GetError());
        return false;
    }

    //create our renderer
    *ppScene = SDL_CreateRenderer(*ppFrame, -1, SDL_RENDERER_ACCELERATED);
    if (*ppScene == NULL){
        printf("Renderer creation failure. Error: %s\n", SDL_GetError());
        return false;
    }

    //Set our renderer colour.
    SDL_SetRenderDrawColor(*ppScene, 0xFF, 0xFF, 0xFF, 0xFF);
    //SDL_RenderSetLogicalSize(*ppScene, 100, 100);

    //this is how we initialize PNG loading.
    //probably some bitwise shifting going on here.
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)){   //wtf?
        printf("SDL_image not initialized. Error: %s\n", IMG_GetError());
        return false;
    }

    if (TTF_Init()==-1){
        printf("Error initializing TTF_Fonts\n");
    }

    //44100 samples/second, 2 channels, 2048 chunk size
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        printf("SDL_mixer not initialized. Error: %s\n", Mix_GetError());
        return false;
    }

    return true;
}

///a wrapper for SDL_RenderCopy(*renderer, *texture, *srcrect, *dstrect);
void ApplyTexture(SDL_Texture* pSrc, SDL_Renderer* pFrame, SDL_Rect* pSrcClp, int x, int y)
{
    int w, h;
    SDL_QueryTexture(pSrc, NULL, NULL, &w, &h);

    SDL_Rect dstClp;
    dstClp.x = x;
    dstClp.y = y;
    dstClp.w = w;
    dstClp.h = h;

    SDL_RenderCopy(pFrame, pSrc, pSrcClp, &dstClp);
}

///Same as above, but only works for square textures. Rectangles are right out.
void ScaleApplyTexture(SDL_Texture* pSrc, SDL_Renderer* pFrame, SDL_Rect* pSrcClp, int x, int y, int size)
{
    SDL_Rect dstClp;
    dstClp.x = x;
    dstClp.y = y;
    dstClp.w = size;
    dstClp.h = size;

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
        return false;
    }
    SDL_SetColorKey(pImg, SDL_TRUE, SDL_MapRGB(pImg->format, 255, 255, 255));
    pTemp = SDL_CreateTextureFromSurface(pRend, pImg);
    if (pTemp == NULL){
        printf("Surface to texture conversion failed. Error: %s\n", SDL_GetError());
        return false;
    }
    //we again need to free the surface
    SDL_FreeSurface(pImg);

    *ppTextImg = pTemp;
    return true;
}

///load in a PNG and convert it to a texture. No alpha for this one.
bool LoadTextureNoA(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend)
{
    SDL_Surface* pImg = NULL;
    SDL_Texture* pTemp = NULL;
    pImg = IMG_Load(path.c_str());
    if (pImg == NULL){
        printf("PNG not loaded. IMG Error: %s\n", IMG_GetError());
        return false;
    }
    pTemp = SDL_CreateTextureFromSurface(pRend, pImg);
    if (pTemp == NULL){
        printf("Surface to texture conversion failed. Error: %s\n", SDL_GetError());
        return false;
    }
    //we again need to free the surface
    SDL_FreeSurface(pImg);

    *ppTextImg = pTemp;
    return true;
}

///Only loads BMP's
bool LoadTextureBMP(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend)
{
    SDL_Surface* pImg = NULL;
    SDL_Texture* pTemp = NULL;
    pImg = SDL_LoadBMP(path.c_str());
    if (pImg == NULL){
        printf("BMP not loaded. Error: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetColorKey(pImg, SDL_TRUE, SDL_MapRGB(pImg->format, 255, 255, 255));
    pTemp = SDL_CreateTextureFromSurface(pRend, pImg);
    if (pTemp == NULL){
        printf("Surface to texture conversion failed. Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_FreeSurface(pImg);

    *ppTextImg = pTemp;
    return true;
}

///Load in music, returns false if error
bool LoadMusic(std::string path, Mix_Music** ppMusic)
{
    Mix_Music* pTemp = NULL;
    pTemp = Mix_LoadMUS(path.c_str());
    if (pTemp == NULL) return false;

    *ppMusic = pTemp;
    return true;
}

///Load in Mix_Chunk, returns false if error
bool LoadSFX(std::string path, Mix_Chunk** ppChunk)
{
    Mix_Chunk* pTemp = NULL;
    pTemp = Mix_LoadWAV(path.c_str());
    if (pTemp == NULL) return false;

    *ppChunk = pTemp;
    return true;
}

bool CheckCollision(tdcRct A, tdcRct B)
{
    //the sides of the rectangles
    float leftA, leftB;
    float rightA, rightB;
    float topA, topB;
    float bottomA, bottomB;

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
    if (bottomA <= topB){    //if the bottom of one is below the top of the other
        return false;
    }
    if (topA >= bottomB) return false;
    if (leftA >= rightB) return false;
    if (rightA <= leftB) return false;

    return true;        //if none of the sides from a are outside b.
}

///Used only for applying the textures correctly.
double FindAngle(double yVel, double xVel)
{
    double ratio = 0;
    //we test for y, because we're always dividing x by y. TOA.
    if(yVel == 0){
        if (xVel > 0){
            return 90;
        } else if (xVel < 0){
            return -90;
        } else {
            return 0;
        }
    }

    if(yVel<0){
        yVel*=-1;
        ratio = xVel/yVel;
        yVel*=-1;
    } else if(yVel>0){
        ratio = xVel/yVel;
    }

    if(yVel>0){
        if(xVel<0){
            return -180-atanf(ratio)*57.2957795;
        } else {
            return 180-atanf(ratio)*57.2957795;
        }
    } else {
        return atanf(ratio)*57.2957795;
    }
}

///Used only for applying the textures correctly.
double GetPCAngle(double xDis, double yDis)
{
    double ratio = 0;
    //we test for y, because we're always dividing x by y. TOA.
    if(yDis == 0){
        if (xDis > 0){
            return 90;
        } else if (xDis < 0){
            return -90;
        } else {
            return 0;
        }
    }

    if (yDis < 0){      //mouse is above the player (since we inverted y)
        yDis *= -1;
        ratio = xDis/yDis;
        yDis *= -1;
    } else if (yDis > 0){
        ratio = xDis/yDis;
    }

    if(yDis > 0){
        if(xDis < 0){
            return -180-atanf(ratio)*57.2957795;
        } else {
            return 180-atanf(ratio)*57.2957795;
        }
    } else {
        return atanf(ratio)*57.2957795;         //convert from radians to degrees
    }
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


/*
    Code for checking the survival of the current wave
    if (!nextWave){     //we're not currently setting up, but maybe we should be
        bool allDead = true;
        for(int i = 0; i < game.wvs[wvNum].nmTrs; i++){
            //find if they're all dead
            if(turs.health[i] > 0){
                allDead = false;
            }
        }
        for(int i=0; i<game.wvs[wvNum].numHnts; i++){
            if (hnts.hlth[i] > 0){
                allDead = false;
            }
        }
        if(allDead){
            nextWave = true;
        }
    }
*/

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

