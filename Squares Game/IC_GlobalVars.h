#ifndef IC_GLOBALVARS_H_INCLUDED
#define IC_GLOBALVARS_H_INCLUDED
#include <string>
#include "SDL.h"

const double FRAME_TIME = 5;      //in microseconds.
const double FRAMES_PER_SECOND = 1000/FRAME_TIME;

//all sizes in units, not pixels
const float PC_SIZE = 2.1;
const float HUNTER_SIZE = 2.5;
const float BOLT_SIZE = 0.5;
const float SNIPER_SIZE = 2.6;
const float BURN_SIZE = 3.5;
const float HEALTH_SIZE = 1.5;
const float SNIPES_SIZE = 2.1;
const float BOMB_SIZE = 2.4;
const float SPRINKLER_SIZE = 2.4;
const float TURRET_SIZE = 2.3;
const float PILLAR_SIZE=3.5;

const double BOMB_RADIUS = 8.f;      //8 units around the bomb it will explode

const float BLT_DAM = 6;
const float HUNT_DAM = 7;
const float HEALTH = 7;     //health healed by a pack
const float DRAIN = 2;      //drain per second

const float HUNT_ANGL_INC_SEC=90;       //as measured in degrees.
const float ANGL_FM=HUNT_ANGL_INC_SEC/FRAMES_PER_SECOND;
const double MAX_HUNTER_SPEED = 12.f;             //x units per second
const double HUNTER_DIS_PER_FRAME = MAX_HUNTER_SPEED/(double)FRAMES_PER_SECOND;     //x pixels per frame
const double HUNTER_CHARGE_RADIUS = 11.f;     //x units
const int COLLISION_TIME = 500;     //500 ms of collision pathing for the hunters
const double HUNTER_CHARGE_DURATION = 700;      //x ms of charging in a straight line

const int TURRET_SHOT_INTERVAL=250;

//for some reason this has to be constant. Good programming
//practice anyway though. I mean, why shouldn't it be?
const std::string tdcDirectoryPath = SDL_GetBasePath();

enum gameState
{
    MENU, BATTLE, TUTORIAL, CTF, CUTSCENE, DEBUG, QUIT
};

enum menuState
{
    INTRO = 1, MAIN, OUTRO
};

enum menSubState
{
    TITLE = 0, SETTINGS, CREDITS, TUTORIALS
};

enum tutState
{
    MOVEGM = 0, MOVETUT, AVOIDGM, ATTACKTUT, ATTACKGM
};

enum Diff
{
    EASY=0, NORM, HARD
};

#endif // IC_GLOBALVARS_H_INCLUDED
