#include "GLOBALS.h"
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Helpers.h"
#include <cstdio>
#include <cmath>


///Initializes SDL, SDL_image. Sets up screen window and surface
///Completely initializes our graphics struct
bool Init()
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

    GFX.pWindow=NULL;
    GFX.pFrame=NULL;

    //create our window
    GFX.pWindow=SDL_CreateWindow("Dangerous Games 0.2", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);


    SDL_DisplayMode md;
    SDL_GetWindowDisplayMode(GFX.pWindow, &md);
    GFX.scl = md.h/100;     //because the game size is 100 units
    GFX.xPush = (md.w-md.h)/2;      //so 1920-1200 /2 = 720/2 = 360.
    GFX.SH=md.h;
    GFX.SW=md.w;
    GFX.br1.x=0;
    GFX.br1.y=0;
    GFX.br1.w=GFX.xPush;
    GFX.br1.h=md.h;
    GFX.br2.x=GFX.xPush+md.h;     //because the height is the width of the actual game
    GFX.br2.y=0;
    GFX.br2.w=GFX.xPush;
    GFX.br2.h=md.h;

    if (GFX.pWindow == NULL){
        printf("Error: %s creating window.\n", SDL_GetError());
        return false;
    }

    //create our renderer
    GFX.pFrame = SDL_CreateRenderer(GFX.pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (GFX.pFrame == NULL){
        printf("Renderer creation failure. Error: %s\n", SDL_GetError());
        return false;
    }

    //Set our renderer colour.
    SDL_SetRenderDrawColor(GFX.pFrame, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(GFX.pFrame);

    //this is how we initialize PNG loading.
    //probably some bitwise shifting going on here.
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)){   //wtf?
        printf("SDL_image not initialized. Error: %s\n", IMG_GetError());
        return false;
    }

    //we always want that very useful pure black image. Might do the same for white.
    if(!LoadTexture(bsPth+"\\Img\\utilBlk.png", &GFX.pBlk)) return -1;
    //Same for the useful white image.
    if(!LoadTextureNoA(bsPth+"\\Img\\utilWte.png", &GFX.pWte)) return -1;
    if(!LoadTexture(bsPth+"\\Img\\outline.png", &GFX.pOutline)) return -1;

    //Can't believe I have to use this crap again.
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
bool LoadTexture(std::string path, SDL_Texture** ppTextImg)
{
    SDL_Surface* pImg = NULL;
    SDL_Texture* pTemp = NULL;
    pImg = IMG_Load(path.c_str());
    if (pImg == NULL){
        printf("PNG not loaded. IMG Error: %s\n", IMG_GetError());
        return false;
    }
    SDL_SetColorKey(pImg, SDL_TRUE, SDL_MapRGB(pImg->format, 255, 255, 255));
    pTemp = SDL_CreateTextureFromSurface(GFX.pFrame, pImg);
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
bool LoadTextureNoA(std::string path, SDL_Texture** ppTextImg)
{
    SDL_Surface* pImg = NULL;
    SDL_Texture* pTemp = NULL;
    pImg = IMG_Load(path.c_str());
    if (pImg == NULL){
        printf("PNG not loaded. IMG Error: %s\n", IMG_GetError());
        return false;
    }
    pTemp = SDL_CreateTextureFromSurface(GFX.pFrame, pImg);
    if (pTemp == NULL){
        printf("Surface to texture conversion failed. Error: %s\n", SDL_GetError());
        return false;
    }
    //we again need to free the surface
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
double FindAngle(double xVel, double yVel)
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

///Using clBx sets the values of the render box
void SetRenderBox(tdcRct clBx, SDL_Rect* pRBx)
{
    pRBx->x=clBx.x*GFX.scl+GFX.xPush;
    pRBx->y=clBx.y*GFX.scl;
    pRBx->w=clBx.w*GFX.scl;
    pRBx->h=clBx.h*GFX.scl;
}

///Draws the black bars on the sides and flips the buffers.
void UpdateGraphics()
{
    SDL_RenderCopy(GFX.pFrame, GFX.pBlk, NULL, &GFX.br1);
    SDL_RenderCopy(GFX.pFrame, GFX.pBlk, NULL, &GFX.br2);
    SDL_RenderPresent(GFX.pFrame);
}


///Takes position, camera to transform with, image to render, angle to render
///at, and the renderer
///If the image is static, pass NULL for the float pointer.
//Should make Camera a pointer like angle, makes things more robust and usable elsewhere.
void RenderObj(tdcRct square,Camera* pCam,SDL_Texture* pImg,float* pAngle)
{
    //transform coordinates to camera. Cam will be null if UI element.
    if(pCam!=NULL){
        square.x-=pCam->area.x;
        square.y-=pCam->area.y;
    }

    SDL_Rect temp;
    SetRenderBox(square, &temp);

    if(pAngle!=NULL){
        SDL_Point cnt;
        cnt.x=square.w*GFX.scl/2;
        cnt.y=square.h*GFX.scl/2;

        SDL_RenderCopyEx(GFX.pFrame, pImg, NULL, &temp, *pAngle, &cnt, SDL_FLIP_NONE);
    }else {
        SDL_RenderCopy(GFX.pFrame, pImg, NULL, &temp);
    }
}
/* To call this function
    RenderObj(pBlts->clBx[i], camera, pBlts->pImg, pBlts->angle[i], gfx);
    or
    RenderObj(pStatic->clBx[i], camera, pBlts->pImg, NULL, gfx);
*/






