#ifndef CHANGEABLE_GLOBALS_H_INCLUDED
#define CHANGEABLE_GLOBALS_H_INCLUDED

static Uint16 PG_ST;
static Uint16 MN_ST;
static Uint16 GM_ST;

enum Difficulty{
    EASY=0, MED, HARD
};

static Difficulty DIFF;

struct tdcRct{
    float w, h, x, y;
};

//short for graphics but who really cares?
//Now contains all the utility stuff in the game.
//As of now, all we put in here is a tdcRct, but I'll probably add some flags.
struct Camera{
    tdcRct area;
};
struct gs{
    float scl;          //scale factor between different user resolutions
    int xPush, SH, SW;  //push images past first bar, stretch image to SH
    SDL_Rect br1, br2;  //for the black bars
    SDL_Texture* pBlk;  //^
    SDL_Texture* pWte;
    SDL_Texture* pOutline;
    Camera cam;
    SDL_Renderer* pFrame;   //the renderer that everything runs into
    SDL_Window* pWindow;    //the window that everything runs into
};
static gs GFX;

#endif // CHANGEABLE_GLOBALS_H_INCLUDED
