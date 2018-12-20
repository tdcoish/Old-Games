#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED


//Question: Why the fuck does including this line make everything compile correctly?
//Does SDL2 have a standard Uint16 type? This is somewhat plausible but mostly ridiculous.
#include <SDL2/SDL.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Resources.h"
#include "Timer.h"

//Window
extern SDL_Window* pWin;
//GlContext
extern SDL_GLContext glContext;

//resolution -- All screens are square. Does not accept lower.
const float SH=600;		//will be made not const later.
const float UNITS_TO_PIXELS=SH/100;		//will be made extern along with SH later.
const float PIXELS_TO_UNITS=1/UNITS_TO_PIXELS;

//projection matrix	--possibly make non-const
const glm::mat4 projection = glm::ortho(0.0f, SH, SH, 0.0f, -1.0f, 1.0f);

//Wherever the .exe is, is the folder we store here.
extern std::string bsPth;

extern float msX;
extern float msY;

//framerate handling.
const double FRAMES_PER_SECOND=100.f;
const double FRAME_TIME=1000.f/FRAMES_PER_SECOND;

//Game states
extern Uint16 PG_ST;
const Uint16 MENU=1<<0;
const Uint16 GAME=1<<1;
const Uint16 QUIT=1<<2;

//The event queue really ought to be global.
extern SDL_Event e;

//Controls VSYNC
const int VSYNC_WITH_TEAR=-1;
const int IMMEDIATE=0;
const int VSYNC=1;

//multiply anything in degrees to get corresponding radians.
const double DEGREES_TO_RADIANS=0.0174532925;
//multiply anything in radians to get corresponding degrees. //could technically just divide using above
const double RADIANS_TO_DEGREES=57.2957795;

const float PC_MAX_SPEED_PER_FRAME=70.0/FRAMES_PER_SECOND;

//the amount that the orb damages each enemy per frame.
const float ORB_DAM_PER_FRAME=80/FRAMES_PER_SECOND;

//hunter max turn per frame
const double ANGL_FM=120.0/FRAMES_PER_SECOND;		//x degrees per second.
//hunter speed per frame
const double HUNTER_DIS_PER_FRAME=40.0/FRAMES_PER_SECOND;	//x units per second.

//hack. eventually have multiple shaders. Run them similarly to sprites below.
extern Shader gameShader;

//hack. 
extern Sprites sprites;

//all sizes in units
const float PC_SIZE=2.5;				
const float HUNTER_SIZE=3.0;
const float TURRET_SIZE=1.8;
const float BOLT_SIZE=1.0;
const float ORB_SIZE=0.6;

const float BOLT_DIS_PER_FRAME=10.0/FRAMES_PER_SECOND;		//x units per second. Somewhat slow

const float EDGE_SIZE=10.0;		//size of damaging border.
extern float edgePercent;

void EmptyCollisionQueue();
extern int collisionsQ[MAX_ENTITIES][2];		//stores 2 indices for each collision
//The collision queue can theoretically hold MAX_ENTITIES! amount of collisions, but there's no way I'm making an array that large.

extern Timer programTimer;

extern float gameSpeed;

#endif

