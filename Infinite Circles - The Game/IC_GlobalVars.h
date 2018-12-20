#ifndef IC_GLOBALVARS_H_INCLUDED
#define IC_GLOBALVARS_H_INCLUDED

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int FRAME_TIME = 10;      //in milliseconds.
const int FRAMES_PER_SECOND = 1000/FRAME_TIME;

enum gameStates
{
    INTRO = 1, MENU, BATTLE, DEBUG, OUTRO, QUIT
};

#endif // IC_GLOBALVARS_H_INCLUDED
