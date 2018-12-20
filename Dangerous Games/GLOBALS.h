#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <string>
#include <cstdio>
#include "SDL.h"

const double FRAME_TIME = 5;      //in milliseconds.
const double FRAMES_PER_SECOND = 1000/FRAME_TIME;

//stupid compiler error here
//float BLT_SPD=22.5f;        //changeable, units per second.

const double ANGLE_TO_RAD=1/57.295779513082320876798;

enum EntityType{
    P_C, HUNTER, SNIPER, BOMB, TURRET, BOLT, BURNER, SPINNER, MOVE_OB, ATK_OB, TEMP
};

const int TURRET_FIRE_RATE=200;        //time between shots in milliseconds.
const int BURN_WARMUP=500;              //time it takes the burner to warm up and do damage
const int BURN_TIME=BURN_WARMUP*5;      //total time it spends burning.
const int GUN_RATE=60;     //one bolt every x milliseconds.

//all sizes in units, not pixels
const float PC_SIZE = 2.1;
const float HUNTER_SIZE = 2.5;
const float BOLT_SIZE = 0.9;
const float SNIPER_SIZE = 2.6;
const float BURN_SIZE = 3.5;
const float HEALTH_SIZE = 1.5;
const float SNIPES_SIZE = 2.1;
const float BOMB_SIZE = 2.4;
const float SPRINKLER_SIZE = 2.4;
const float TURRET_SIZE = 2.3;
const float PILLAR_SIZE=3.5;
const float TUT_OB_SIZE=5.0;
const float SPIN_ORB_SIZE=1.2;

//this will eventually not be a constant, and possibly not global.
const float BOLT_DAMAGE=15;

//Wherever the .exe is, is the folder we store here.
const std::string bsPth = SDL_GetBasePath();

//stores all relevent key presses
//uint16 KEY_PRESSES;       //maybe unsigned?

//All these state names are subject to change
extern Uint16 PG_ST;
const Uint16 MENU=1<<0;
const Uint16 GAME=1<<1;
const Uint16 QUIT=1<<2;

//Menu pages
extern Uint16 MN_ST;
const Uint16 INTRO=1<<0;
const Uint16 TITLE=1<<1;
const Uint16 CREDITS=1<<2;
const Uint16 SETTINGS=1<<3;
const Uint16 GAMESELECT=1<<4;
const Uint16 DIFFICULTY_=1<<5;
const Uint16 OUTRO=1<<6;

//Game types
extern Uint16 GM_ST;
const Uint16 MOVE_TUT=1<<0;
const Uint16 ATK_TUT=1<<1;
const Uint16 BOTH_TUT=1<<2;
const Uint16 ENEMY_TUT=1<<3;
const Uint16 PACIFIST=1<<4;
const Uint16 KILL_ALL=1<<5;
const Uint16 GAUNTLET=1<<6;

enum StateTypes{
    PROGRAM, GAMETYPE, MENUSCREEN
};

enum Difficulty{
    EASY=0, MED, HARD
};

extern Difficulty DIFF;

struct tdcPoint{
    float x, y;
};

struct tdcRct{
    float w, h, x, y;
};

//As of now, all we put in here is a tdcRct, but I'll probably add some flags.
struct Camera{
    tdcRct area;
};


//short for graphics but who really cares?
//Now contains all the utility stuff in the game.
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

extern gs GFX;


#endif // GLOBALS_H_INCLUDED
