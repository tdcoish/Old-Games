//const int MAX_BOLTS = 8;
#include "Level IO.h"
#include "Helpers.h"

class SpikeTraps
{
    public:
        double x[64], y[64];         //stores the rotational center of the four traps
        double xPos[64], yPos[64];    //stores the actual position of the four traps
        int timeToReactive[64];         //if negative, trap is active.

        SpikeTraps();
        void moveTraps();
};

class PlayerCharacter
{
    public:
        float xVel, yVel;
        float trueX, trueY;
        tdcRct clBx;        //collision box, had to be this because I need floats
        tdcRct smBx;        //Wanted smaller true collision box, because character is circular. Could do per pixel, but bolts are larger than they should be.
        SDL_Rect rBx;       //the render box. Different from the collision box.
        double health;
        bool up, down, left, right;
        bool hit;
        int hitDelay;       //when less than 0, hit = false. Gets charged on hit.
        int swing;          //when greater than 0, we're swinging our sword.
        tdcRct atk;         //the swords collision box.
        SDL_Rect rAtk;      //render version.
        double angle;       //which way the PC is facing, also used for sword swing
        bool sldUp;         //if the shield is up
        float sldHl;          //the shield's health
        SDL_Rect shld;      //shield box for rendering
        SDL_Point sldCnt;    //center of shield
        tdcRct sldBx;       //changed so only one large square shield

        PlayerCharacter(gs dt);
        void handleInput(SDL_Event* pEvent);
        void move(int barSize, gs dt);
        void move(gs dt);
};

//Behave just like their counterparts in Halo:CE
class Hunters
{
    public:
        float xVel[8], yVel[8];
        float xPos[8], yPos[8];
        bool charging[8];
        int chargeTime[8];      //if positive, they are charging.
        int collidedTime[8];    //if positive, they have collided and move straight for a while
        int damageCooldown[8];  //if positive, they just dealt damage and won't again for a while
        int pauseTime[8];       //counts down constantly, they wait before they charge very briefly
        int stunTime[8];        //if they hit the edge of the arena, they are stunned.
        float angle[8];        //angle the hunter is pointing at.
        bool flipImg[8];        //if we flip the image or not.
        tdcRct hunters[8];    //used for collision detection mainly.
        SDL_Rect rBx[8];
        int fresh[8];           //set to x when they spawn, so they move to the right point
        bool spawned[8];        //if they've spawned or not.
        int lastSpawn;          //when the previous spawn was.
        short prevSpawn;      //which hunter was the last spawned.
        float hlth[8];

        Hunters(int nmHnts, gs dt);
        void moveHunters(PlayerCharacter playa, int nmHnts, int barSz, gs dt);     //will also make them charge if they are close
        void handleCollision(int offset1, int offset2); //bounce off the other hunter or not
        void DamagePlayer(PlayerCharacter* pPC, int nmHnts, bool doDam);
        void SpawnHunters(int nmHnts, int time);
};

//This guy just walks around in a circle and attacks the player with a bolt every so often
class Sniper
{
    //has no collision box because I will not be having the player collide with him
    public:
        float xPos, yPos;
        int interval;       //time between shots (created const global variable)
        float health[1];    //coming back to this in 2015. Why'd I make this size 1?

        Sniper(int barSz);
        void moveSniper(int barSz);
};

//I'm only envisioning 1 on screen at a time, but why not allow for more?
class HealthPacks
{
public:
    float xPos[8], yPos[8];
    bool isActivated[8];
    tdcRct clBx[8];
    SDL_Rect rBx[8];
    bool isAlive[8];

    HealthPacks(int nmPcks);
};

//There's a bunch active at once. They individually disactivate, and then are put in a new place.
class BurnSpots
{
    public:
        int xPos[64], yPos[64];             //kind of redundant
        int sinceDead[64];                //they start blue, then when active do damage.
        bool isActive[64];
        tdcRct clBx[64];
        SDL_Rect rBx[64];

        BurnSpots(int nmSpts, gs dt);
        void Rearrange(const int appTime, int nmSpts, int barSz, gs dt);
        void Activate(int nmSpts);
        void DamagePlayer(PlayerCharacter* pPC, int nmSpts, bool doDam);
};

//Has a telegraphed windup, but then it does unavoidable damage.
class Snipes
{
    public:
        float xPos[64], yPos[64];
        float xVel[64], yVel[64];
        tdcRct clBx[64];
        SDL_Rect rBx[64];
        bool isAlive[64];
        double angle[64];
        int lastShot[64];       //time since last shot.
        int fleeTime[64];       //they flee when the player gets close
        int coolDown[64];       //they need to cooldown before charging up.
        int chrgTime[64];       //time to charge a shot, gets set and counts down.
        bool charging[64];      //charging up a shot
        bool firing[64];        //actually firing a shot.
        int fireTime[64];       //length of the shot itself.
        int fresh[64];          //set to x when they spawn, so they move to the right point
        bool spawned[64];       //if they've spawned or not.
        int lastSpawn;          //when the previous spawn was.
        short prevSpawn;        //which hunter was the last spawned.
        SDL_Point end[64];      //the spot where the sniper hits something or goes out of bounds
        SDL_Point cnt[64];      //the center of the shot, used for rendering.
        SDL_Rect rnd[64];       //the rect used for rendering

        Snipes(int nmSnps, gs dt);
        void MoveSnipers(PlayerCharacter PC, int nmSnps, int barSz, gs dt);
        void HandleFiring(PlayerCharacter PC, int nmSnps);
        void SetTimes(int nmSnps);
        void DamagePlayer(PlayerCharacter* pPC, int nmSnps, bool doDam);
        void SpawnSnipes(int nmSnps, int time);
};

//Randomly putters about, explodes after time, or if PC gets too close.
//we check if it's too close in the battlefield, although we could do that
//here probably better.
class Bombs
{
    public:
        float xPos[8], yPos[8];
        float xDest[8], yDest[8];
        float xVel[8], yVel[8];
        bool isAlive[8];
        double angle[8];
        bool flipImg[8];
        int timeLeft[8];       //time before exploding
        int nextMove[8];       //time before they move again.
        bool exploding[8];       //they explode for a while, then dissapear.
        int explodingTime[8];      //gets set when they explode, counts down.
        int fresh[8];           //set to x when they spawn, so they move to the right point
        bool spawned[8];        //if they've spawned or not.
        int lastSpawn;          //when the previous spawn was.
        short prevSpawn;      //which hunter was the last spawned.
        float hlth[8];
        tdcRct clBx[8];
        SDL_Rect rBx[8];
        SDL_Rect exBx[8];

        Bombs(int nmBms, int barSz, gs dt);
        void MoveBombs(int nmBms, gs dt);
        void DamagePlayer(PlayerCharacter* pPC, int nmBms, bool doDam);
        void SpawnBombs(int nmBms, int time);
};

//only one sprinkler, but make multiple snipers, and get rid of the track.
class Sprinkler
{
    public:
        float xPos, yPos;
        int interval;       //time between shots (created const global variable)
        tdcRct clBx;
        SDL_Rect rBx;
        double angle;
        bool isAlive;
        int health;
        bool flipImg;

        Sprinkler(gs dt);
        void rotateSprinkler();
};

class Turrets
{
    public:
        double xPos[16], yPos[16];
        double xVel[16], yVel[16];
        tdcRct clBx[16];
        SDL_Rect rBx[16];
        bool isAlive[16];
        int health[16];
        bool flipImg[16];
        double angle[16];       //used to follow the player. I'll eventually implement a max turning speed.
        int lastFire[16];

        Turrets(int nmTurs, gs dt);
};

//Finally combined all the damn bolt classes into this.
class Bolts
{
    public:
        float xPos[128], yPos[128];      //true position
        float xVel[128], yVel[128];      //must be normalised at creation.
        tdcRct clBx[128];       //checks for collision with player and screen edges
        SDL_Rect rBx[128];
        bool isAlive[128];
        double angle[128];        //angle the hunter is pointing at.
        bool flipImg[128];        //if we flip the image or not.

        Bolts(int nmBlts, gs dt);
        bool newBolt(int startX, int startY, double speed, int nmBlts, PlayerCharacter PC, float angle, bool atkPC);
        void moveBolts(int nmBlts, gs dt);       //also kills them if they go off the screen
        //void DamagePlayer(PlayerCharacter* pPC, Level cpyLvl, bool doDam);
};

struct Pillars
{
    tdcRct clBx[32];
    SDL_Rect rBx[32];
};

void PlacePillars(Pillars* pPllrs, Lev lv, int wv, gs dt);

void SwitchVel(float*, float*, float*, float*);
void SwitchVel(float*, float*, float*, float*, float*, float*, float*, float*);


