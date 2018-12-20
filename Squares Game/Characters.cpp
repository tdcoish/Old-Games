#include <SDL.h>
#include "Characters.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "IC_GlobalVars.h"
#include "Helpers.h"
#include <stdio.h>  //debugging
#include <iostream>

extern const double FRAME_TIME;
extern const double FRAMES_PER_SECOND;

const float PC_DIS_PER_FRAME = 30/(double)FRAMES_PER_SECOND;        //this is in units

const int NUM_SPIKETRAPS = 64;   //they should go off the screen by the end
const int SHOT_INTERVAL = 300;  //in milliseconds

const double PI = 3.14159265359;

//frequency is measured here in seconds
const double SNIPER_ANGULAR_SPEED = 2*PI/10;     //2*PI*frequency, which is 1/xHZ, in radians per second
double sniperAngle = 0;         //in radians, we use this just to start

const int SPIKETRAP_PATH_RADIUS = 50;   //in pixels
const double SPIKETRAP_ANGULAR_SPEED = 2*PI/4;      //x seconds for each rotation
double spikeTrapAngle = 0;

const int SNIPER_BULLET_SIZE = 10;              //size in pixels, width and height

const double BOMB_MAX_SPEED = 10.f;
const double BOMB_DIS_PER_FRAME = BOMB_MAX_SPEED/(double)FRAMES_PER_SECOND;
const double BOMB_ACC_PER_SEC = 0.2;     //the lower, the more looping the motion
const double BOMB_ACC_PER_FRAME = BOMB_ACC_PER_SEC/(double)FRAMES_PER_SECOND;

const double SNIPES_MAX_SPEED = 6.f;
const double SNIPES_DIS_PER_FRAME = SNIPES_MAX_SPEED/(double)FRAMES_PER_SECOND;
const double SNIPES_ACC_PER_SEC = 0.5;     //the lower, the more looping the motion
const double SNIPES_ACC_PER_FRAME = SNIPES_ACC_PER_SEC/(double)FRAMES_PER_SECOND;
const double SNIPE_FLEE_RAD = 8;
const double SNIPE_MOVE_RAD = 20;      //they move away from the PC if they're closer
const double SNIPE_MOVE_TO = 40;

///SpikeTraps methods
//The four traps start in the center of one quadrant of the screen.
SpikeTraps::SpikeTraps()
{
    //evenly spaced out on the screen, 4 per line, 4 lines
    const int HOR_SPACE = 100/5;
    const int VER_SPACE = 100/5;

    for (int i = 0; i < 64; i++){
        //There are 8 rows, we slot the sequential trap into the next slot, going back to the start after 8
        if (i%8 == 0) x[i] = -HOR_SPACE;
        if (i%8 == 1) x[i] = 0;
        if (i%8 == 2) x[i] = HOR_SPACE;
        if (i%8 == 3) x[i] = HOR_SPACE*2;
        if (i%8 == 4) x[i] = HOR_SPACE*3;
        if (i%8 == 5) x[i] = HOR_SPACE*4;
        if (i%8 == 6) x[i] = HOR_SPACE*5;
        if (i%8 == 7) x[i] = HOR_SPACE*6;

        if (i/8 == 0) y[i] = -VER_SPACE;
        if (i/8 == 1) y[i] = 0;
        if (i/8 == 2) y[i] = VER_SPACE;
        if (i/8 == 3) y[i] = VER_SPACE*2;
        if (i/8 == 4) y[i] = VER_SPACE*3;
        if (i/8 == 5) y[i] = VER_SPACE*4;
        if (i/8 == 6) y[i] = VER_SPACE*5;
        if (i/8 == 7) y[i] = VER_SPACE*6;

        timeToReactive[i] = 0;      //since they are already active
    }
}

//version 0.11. Changed movement to linear
void SpikeTraps::moveTraps()
{
    //Gonna have to rewrite spiketraps to have a center position
    //increase the angle by the angular speed, divided by the frames per second
    //double angleIncrease = SNIPER_ANGULAR_SPEED/(1000/10);
    double angleIncrease = SPIKETRAP_ANGULAR_SPEED/(double)FRAMES_PER_SECOND;
    spikeTrapAngle-=angleIncrease;

    for (int i = 0; i < NUM_SPIKETRAPS; i++){
        xPos[i] = x[i] + SPIKETRAP_PATH_RADIUS*cos(spikeTrapAngle);
        yPos[i] = y[i] + SPIKETRAP_PATH_RADIUS*sin(spikeTrapAngle);
    }
}


///PlayerCharacter methods
//The player starts in the center of the map
PlayerCharacter::PlayerCharacter(gs dt)
{
    yVel = 0;
    xVel = 0;

    clBx.x=100/2;
    clBx.y=100/4;
    trueX=clBx.x;
    trueY=clBx.y;

    clBx.w = PC_SIZE;
    clBx.h = PC_SIZE;
    smBx.w=PC_SIZE/2;
    smBx.h=PC_SIZE/2;

    rBx.w=PC_SIZE*dt.scl;
    rBx.h=PC_SIZE*dt.scl;
    rBx.x=dt.xPush+clBx.x/dt.scl;
    rBx.y=clBx.y/dt.scl;

    health = 100.f;

    down = false;
    up = false;
    left = false;
    right = false;

    hit = false;
    hitDelay = 0;

    swing = 0;
    atk.h = PC_SIZE;
    atk.w = PC_SIZE;
    rAtk.h=PC_SIZE*dt.scl;
    rAtk.w=PC_SIZE*dt.scl;

    sldUp = false;
    sldHl = 100;
    sldBx.w=PC_SIZE*2;
    sldBx.h=PC_SIZE*2;
    shld.w = sldBx.w*dt.scl;
    shld.h = sldBx.h*dt.scl;

    angle = 0;
}

//player can move the character in all four directions
//The bugs I'm getting here are all sorts of weird
void PlayerCharacter::handleInput(SDL_Event* pEvent)
{

    if (pEvent->type == SDL_KEYDOWN){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: up = true; break;
            case SDLK_DOWN: down = true; break;
            case SDLK_RIGHT: right = true; break;
            case SDLK_LEFT: left = true; break;

            case SDLK_w: up = true; break;
            case SDLK_s: down = true; break;
            case SDLK_d: right = true; break;
            case SDLK_a: left = true; break;
        }
    } else if (pEvent->type == SDL_KEYUP){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: up = false; break;
            case SDLK_DOWN: down = false; break;
            case SDLK_RIGHT: right = false; break;
            case SDLK_LEFT: left = false; break;

            case SDLK_w: up = false; break;
            case SDLK_s: down = false; break;
            case SDLK_d: right = false; break;
            case SDLK_a: left = false; break;
        }
    } else if (pEvent->type == SDL_MOUSEBUTTONDOWN){
        if(pEvent->button.button == SDL_BUTTON_LEFT){
            if(sldUp) sldUp = false;        //can't have the shield up when we swing.
            if(swing <= 0) swing = 100;      //because we swing for that long.
            atk.x = clBx.x;
            atk.y = clBx.y;
            atk.x+=sin(angle/57.2957795)*PC_SIZE;
            atk.y-=cos(angle/57.2957795)*PC_SIZE;       //move the box a total distance of PC_SIZE units
        }else if(pEvent->button.button == SDL_BUTTON_RIGHT){
            //handle shield
            if(swing > 0){      //only the shield OR sword can be working at one time
                swing = 0;
            }
            if(sldUp){
                sldUp = false;
            } else {
                sldUp = true;
            }
        }
    }
}

void PlayerCharacter::move(int barSize, gs dt)
{
    xVel = 0;
    yVel = 0;

    if (left){
        xVel -= PC_DIS_PER_FRAME;
    }
    if (right){
        xVel += PC_DIS_PER_FRAME;
    }
    if (up){
        yVel -= PC_DIS_PER_FRAME;      //because inverted y axis
    }
    if (down){
        yVel += PC_DIS_PER_FRAME;
    }

    //we want to keep speed constant
    double maxDisSqrd = PC_DIS_PER_FRAME*PC_DIS_PER_FRAME;
    if ((xVel*xVel + yVel*yVel) > (maxDisSqrd)){
        if (xVel == 0){
            if (yVel < 0){
                yVel = -1*PC_DIS_PER_FRAME;
            } else {
                yVel = PC_DIS_PER_FRAME;
            }
        } else {
            xVel *= sqrt(maxDisSqrd/(xVel*xVel + yVel*yVel));
            yVel *= sqrt(maxDisSqrd/(xVel*xVel + yVel*yVel));
        }
    }

    //now just move the player image
    trueX += xVel;
    trueY += yVel;

    if (trueX < 0+barSize || trueX > (100-clBx.w-barSize)){
        trueX -= xVel;
    }
    if (trueY < 0+barSize || trueY > (100-clBx.h-barSize)){
        trueY -= yVel;
    }

    clBx.x = trueX;
    clBx.y = trueY;
    rBx.x=dt.xPush+clBx.x*dt.scl;
    rBx.y=clBx.y*dt.scl;

    //move the sword as well
    atk.x = clBx.x+sin(angle/57.2957795)*PC_SIZE;
    atk.y = clBx.y-cos(angle/57.2957795)*PC_SIZE;
    rAtk.x=dt.xPush+atk.x*dt.scl;
    rAtk.y=atk.y*dt.scl;

    //move the shield as well
    sldBx.x=clBx.x-(PC_SIZE/2);      //want sld 0.5 units over on left and right
    sldBx.y=clBx.y-(PC_SIZE/2);
    shld.x = dt.xPush+sldBx.x*dt.scl;
    shld.y = sldBx.y*dt.scl;
}

void PlayerCharacter::move(gs dt)
{
    xVel = 0;
    yVel = 0;

    if (left){
        xVel -= PC_DIS_PER_FRAME;
    }
    if (right){
        xVel += PC_DIS_PER_FRAME;
    }
    if (up){
        yVel -= PC_DIS_PER_FRAME;      //because inverted y axis
    }
    if (down){
        yVel += PC_DIS_PER_FRAME;
    }

    //we want to keep speed constant
    double maxDisSqrd = PC_DIS_PER_FRAME*PC_DIS_PER_FRAME;
    if ((xVel*xVel + yVel*yVel) > (maxDisSqrd)){
        if (xVel == 0){
            if (yVel < 0){
                yVel = -1*PC_DIS_PER_FRAME;
            } else {
                yVel = PC_DIS_PER_FRAME;
            }
        } else {
            xVel *= sqrt(maxDisSqrd/(xVel*xVel + yVel*yVel));
            yVel *= sqrt(maxDisSqrd/(xVel*xVel + yVel*yVel));
        }
    }

    //now just move the player image
    trueX += xVel;
    trueY += yVel;

    if (trueX < 0 || trueX > (100-clBx.w)){
        trueX -= xVel;
    }
    if (trueY < 0 || trueY > (100-clBx.h)){
        trueY -= yVel;
    }

    clBx.x = trueX;
    clBx.y = trueY;
    smBx.x=trueX+PC_SIZE/4;
    smBx.y=trueY+PC_SIZE/4;
    rBx.x=dt.xPush+clBx.x*dt.scl;
    rBx.y=clBx.y*dt.scl;
}

Hunters::Hunters(int nmHnts, gs dt)
{
    //velocities are initially zero
    for (int i = 0; i < nmHnts; i++){
        xPos[i] = 0;
        yPos[i] = 0;
        xVel[i] = 0;
        yVel[i] = 0;
        chargeTime[i] = -1;      //not charging to start
        collidedTime[i] = -1;    //not collided to start
        damageCooldown[i] = -1;  //can attack to start
        pauseTime[i] = -1;
        stunTime[i] = -1;
        charging[i] = false;
        angle[i] = 0;
        flipImg[i] = false;
        hunters[i].w = HUNTER_SIZE;
        hunters[i].h = HUNTER_SIZE;
        rBx[i].w=hunters[i].w*dt.scl;
        rBx[i].h=hunters[i].h*dt.scl;
        fresh[i] = -1;
        spawned[i] = false;
        lastSpawn = -2000;              //how long ago we spawned the last hunter.
        prevSpawn = -1;
        hlth[i] = 100;
    }
}

//moves hunters according to player position,
//This is the ugliest code I've ever written
//The code needs to be rewritten where we accelerate the hunters based on where they are going relative to the
//PC, as opposed to simply relative to the PC. The later gives us poor behaviour. What's more, we can just use
//the already existing code for angles that we already have.
//This has been rewritten, and it is beautiful.
void Hunters::moveHunters(PlayerCharacter cpyPC, int nmHnts, int barSz, gs dt)
{
    for (int i = 0; i < nmHnts; i++){
        collidedTime[i] -= FRAME_TIME;
        chargeTime[i] -= FRAME_TIME;
        if(chargeTime[i]<-HUNTER_CHARGE_DURATION)charging[i]=false;
        pauseTime[i] -= FRAME_TIME;
        stunTime[i] -= FRAME_TIME;
        fresh[i] -= FRAME_TIME;
    }

    double widthDiff;
    double heightDiff;
    for (int i = 0; i < nmHnts; i++){
        if (spawned[i] && hlth[i] > 0){
            if (collidedTime[i] < 0 && !charging[i]){
                //Either the PC or the hunters have incorrect collision boxes here.
                float wdDf=cpyPC.clBx.x+(HUNTER_SIZE-PC_SIZE)/2-hunters[i].x;
                float htDf=cpyPC.clBx.y+(HUNTER_SIZE-PC_SIZE)/2-hunters[i].y;
                float trueAngle=FindAngle(htDf, wdDf);
                float anglDf=angle[i]-trueAngle;
                if(anglDf>=0&&anglDf<=180 || anglDf<=-180){
                    angle[i]-=ANGL_FM;
                }else{
                    angle[i]+=ANGL_FM;
                }
                if(angle[i]<-180)angle[i]+=360;
                if(angle[i]>180)angle[i]-=360;

                xVel[i]=sin(angle[i]/57.2957795)*HUNTER_DIS_PER_FRAME;
                yVel[i]=cos(angle[i]/57.2957795)*HUNTER_DIS_PER_FRAME;

                //first calculate distance and charge or not
                if (wdDf*wdDf + htDf*htDf < HUNTER_CHARGE_RADIUS*HUNTER_CHARGE_RADIUS && std::abs(anglDf)<10){
                    charging[i] = true;
                    pauseTime[i] = 70;      //pause for x ms before charge.
                    xVel[i]*=2;
                    yVel[i]*=2;
                    chargeTime[i]=HUNTER_CHARGE_DURATION;
                }
            }

            xPos[i] += xVel[i];
            yPos[i] -= yVel[i];
            hunters[i].x = xPos[i];
            hunters[i].y = yPos[i];
            rBx[i].x=dt.xPush+hunters[i].x*dt.scl;
            rBx[i].y=yPos[i]*dt.scl;
        }
    }
}

//we've already checked for and gotten a collision between them
void Hunters::handleCollision(int offset1, int offset2)
{
    //I have the position of both hunters, and I can do simple collision physics with them
    //need to conserve momentum and figure out the angle of collision
    //for start just swap their velocities, then later replace it with better
    double xTemp, yTemp;
    xTemp = xVel[offset1];
    yTemp = yVel[offset1];

    xVel[offset1] = yVel[offset2];
    yVel[offset1] = yVel[offset2];
    xVel[offset2] = xTemp;
    yVel[offset2] = yTemp;

    collidedTime[offset1] = COLLISION_TIME;
    collidedTime[offset2] = COLLISION_TIME;
}

void Hunters::SpawnHunters(int nmHnts, int time)
{
    //I could easily control which entrance they come from based on a simple if i%2 = 0.
    if (time-lastSpawn > 2000 && (prevSpawn+1 < nmHnts)){      //spawn every x ms.
        spawned[prevSpawn+1] = true;
        fresh[prevSpawn+1] = 200;         //they move to one spot for x ms, then do regular AI
        prevSpawn++;
        //printf("Spawned hunter\n");
        lastSpawn = time;
    }
}


///Sniper Methods
//starting the sniper in the middle just for debugging purposes
Sniper::Sniper(int barSz)
{
    interval = SHOT_INTERVAL;
    xPos = 100 - barSz-10;
    yPos = 100/2;
    health[0] = 100;
}

//This doesn't deal with the barrier just yet.
void Sniper::moveSniper(int barSz)
{
    //increase the angle by the angular speed, divided by the frames per second
    //double angleIncrease = SNIPER_ANGULAR_SPEED/(1000/10);
    double angleIncrease = SNIPER_ANGULAR_SPEED/FRAMES_PER_SECOND;
    sniperAngle-=angleIncrease;

    const static int SNIPER_PATH_RADIUS = (100/2)-barSz-10;     //in pixels

    xPos = 100/2 + SNIPER_PATH_RADIUS*cos(sniperAngle);
    yPos = 100/2 + SNIPER_PATH_RADIUS*sin(sniperAngle);
}

///HealthPacks Methods
HealthPacks::HealthPacks(int nmHlth)
{
    for (int i = 0; i < nmHlth; i++){
        xPos[i] = 100/nmHlth;
        yPos[i] = 100/nmHlth * i;
        isActivated[i] = false;         //honestly the only important variable to initialize
        clBx[i].w = HEALTH_SIZE;
        clBx[i].h = HEALTH_SIZE;
        clBx[i].x = xPos[i];
        clBx[i].y = yPos[i];
    }

}

///BurnSpots Methods
BurnSpots::BurnSpots(int nmSpts, gs dt)
{
    for(int i = 0; i < nmSpts; i++){
        xPos[i] = 100/nmSpts;
        yPos[i] = 100/nmSpts * i;
        isActive[i] = false;
        sinceDead[i] = 100*i;
        clBx[i].w = BURN_SIZE;
        clBx[i].h = BURN_SIZE;
        clBx[i].x = xPos[i];
        clBx[i].y = yPos[i];
        rBx[i].x=dt.xPush+clBx[i].x*dt.scl;
        rBx[i].y=clBx[i].y*dt.scl;
        rBx[i].w=clBx[i].w*dt.scl;
        rBx[i].h=clBx[i].h*dt.scl;
    }
}

//moves the spots if they need moving. appTime should be 10ms.
void BurnSpots::Rearrange(const int appTime, int nmSpts, int barSz, gs dt)
{
    for (int i = 0; i < nmSpts; i++){
        sinceDead[i] += appTime;
    }
    for (int i = 0; i < nmSpts; i++){
        if (sinceDead[i] > 100*nmSpts+400){     //+400 because if four or less then they'll never be activated.
            xPos[i] = 5+ rand()%10*10;
            yPos[i] = 5+ rand()%10*10;
            //should check to make sure no conflicts but whatever.
            clBx[i].x = xPos[i];
            clBx[i].y = yPos[i];
            rBx[i].x=dt.xPush+clBx[i].x*dt.scl;
            rBx[i].y=clBx[i].y*dt.scl;
            isActive[i] = false;
            sinceDead[i] = 0;
        }
    }
}

void BurnSpots::Activate(int nmSpts)
{
    for (int i = 0; i < nmSpts; i++){
        if (sinceDead[i] > 400){
            isActive[i] = true;
        }
    }
}

///Snipe Methods
Snipes::Snipes(int nmSnps, gs dt)
{
    for (int i = 0; i < nmSnps; i++){
        angle[i] = 0;
        xPos[i] = 0;
        yPos[i] = 100;
        clBx[i].w = SNIPES_SIZE;
        clBx[i].h = SNIPES_SIZE;
        clBx[i].x = xPos[i];
        clBx[i].y = yPos[i];
        rBx[i].w=SNIPES_SIZE*dt.scl;
        rBx[i].h=SNIPES_SIZE*dt.scl;
        xVel[i] = 0;
        yVel[i] = 0;
        isAlive[i] = true;
        lastShot[i] = 0;        //this means they're all going to shoot at the same time.
        fleeTime[i] = 0;
        coolDown[i] = 0;
        chrgTime[i] = 0;
        charging[i] = false;
        firing[i] = false;
        fireTime[i] = 0;
        fresh[i] = -1;
        spawned[i] = false;
        lastSpawn = -500;
    }
}

//Still need to properly design their movement
void Snipes::MoveSnipers(PlayerCharacter PC, int nmSnps, int barSz, gs dt)
{
    double widthDiff, heightDiff;
    bool doCalc = true;
    for (int i = 0; i < nmSnps; i++){
        if (spawned[i] && isAlive[i]){
            if (fresh[i] > 0){
                //they spawn out of the bottom left corner
                int destX = 40+5*i;
                int destY = 40+5*i;

                widthDiff = destX - xPos[i];
                heightDiff = destY - yPos[i];
                double yRatio, yRatioSqrd;
                if (widthDiff != 0){
                    yRatio = heightDiff/widthDiff;
                } else {
                    yRatio = 0;
                }
                yRatioSqrd = yRatio*yRatio;

                if (widthDiff == 0){
                    if (heightDiff > 0){
                        yVel[i] += SNIPES_ACC_PER_FRAME;
                    } else {
                        yVel[i] -= SNIPES_ACC_PER_FRAME;
                    }
                    xVel[i] += 0;       //an unneccessary step
                } else {
                    double xVelInc, yVelInc;
                    xVelInc = SNIPES_ACC_PER_FRAME/(sqrt(yRatioSqrd+1));
                    yVelInc = xVelInc*yRatio;

                    if (widthDiff > 0){         //player to the right
                        xVel[i] += xVelInc;
                        yVel[i] += yVelInc;
                    } else if (widthDiff < 0){  //player to the left
                        xVel[i] -= xVelInc;
                        yVel[i] -= yVelInc;
                    }
                }

                //if it's too fast, decrease proportionally.
                //need to use velocity increase instead of velocity
                double maxDisSqrd = SNIPES_DIS_PER_FRAME*SNIPES_DIS_PER_FRAME;
                if ((xVel[i]*xVel[i] + yVel[i]*yVel[i]) > maxDisSqrd){
                    if (xVel[i] == 0){
                        if (yVel[i] < 0){
                            yVel[i] *= -1*SNIPES_DIS_PER_FRAME;
                        } else {
                            yVel[i] = SNIPES_DIS_PER_FRAME;
                        }
                    } else{
                        xVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                        yVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                    }
                }
                fresh[i] -= FRAME_TIME;
            } else {
                //do everything else.
                //if the PC gets close, flee
                if ((xPos[i]-PC.clBx.x)*(xPos[i]-PC.clBx.x) + (yPos[i]-PC.clBx.y)*(yPos[i]-PC.clBx.y) < SNIPE_FLEE_RAD*SNIPE_FLEE_RAD){
                    fleeTime[i] = 300;      //flee for x/1000 seconds
                }
                if (isAlive[i] && (coolDown[i] > 0 || fleeTime[i] > 0)){
                    double xDest, yDest;
                    //only move away if the player is within a certain radius.
                    if ((xPos[i]-PC.clBx.x)*(xPos[i]-PC.clBx.x) + (yPos[i]-PC.clBx.y)*(yPos[i]-PC.clBx.y) < SNIPE_MOVE_RAD*SNIPE_MOVE_RAD){
                        xDest = xPos[i] - (PC.clBx.x - xPos[i]);
                        yDest = yPos[i] - (PC.clBx.y - yPos[i]);
                        doCalc = true;
                    } else if ((xPos[i]-PC.clBx.x)*(xPos[i]-PC.clBx.x) + (yPos[i]-PC.clBx.y)*(yPos[i]-PC.clBx.y) > SNIPE_MOVE_TO*SNIPE_MOVE_TO){
                        xDest = PC.clBx.x;
                        yDest = PC.clBx.y;
                        doCalc = true;
                    } else {
                        doCalc = false;
                    }

                    if (doCalc){
                        widthDiff = xDest - xPos[i];      //if positive, dest to the right
                        heightDiff = yDest - yPos[i];     //if positive, dest below

                        double yRatio, yRatioSqrd;
                        if (widthDiff != 0){
                            yRatio = heightDiff/widthDiff;  //how much y in x.
                        } else {
                            yRatio = 100000;
                        }
                        yRatioSqrd = yRatio*yRatio;

                        if (widthDiff == 0){
                            if (heightDiff > 0){
                                yVel[i] += SNIPES_ACC_PER_FRAME;
                            } else {
                                yVel[i] -= SNIPES_ACC_PER_FRAME;
                            }
                            xVel[i] += 0;       //an unneccessary step
                        } else {
                            double xVelInc, yVelInc;
                            xVelInc = SNIPES_ACC_PER_FRAME/(sqrt(yRatioSqrd+1));
                            yVelInc = xVelInc*yRatio;

                            if (widthDiff > 0){         //player to the right
                                xVel[i] += xVelInc;
                                yVel[i] += yVelInc;
                            } else if (widthDiff < 0){  //player to the left
                                xVel[i] -= xVelInc;
                                yVel[i] -= yVelInc;
                            }
                        }
                        //if it's too fast, decrease proportionally.
                        //need to use velocity increase instead of velocity
                        double maxDisSqrd = SNIPES_DIS_PER_FRAME*SNIPES_DIS_PER_FRAME;
                        if ((xVel[i]*xVel[i] + yVel[i]*yVel[i]) > maxDisSqrd){
                            if (xVel[i] == 0){
                                if (yVel[i] < 0){
                                    yVel[i] *= -1*SNIPES_DIS_PER_FRAME;
                                } else {
                                    yVel[i] = SNIPES_DIS_PER_FRAME;
                                }
                            } else{
                                xVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                                yVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                            }
                        }
                    } else {
                        xVel[i] = 0;
                        yVel[i] = 0;
                    }
                }
            }
            xPos[i] += xVel[i];
            yPos[i] += yVel[i];
            //if we go out of bounds, move back.
            if (xPos[i] < barSz)xPos[i] = barSz+SNIPES_SIZE;
            if (xPos[i] > 100-barSz) xPos[i] = 100-barSz-SNIPES_SIZE;
            if (yPos[i] < barSz)yPos[i] = barSz+SNIPES_SIZE;
            if (yPos[i] > 100-barSz)yPos[i] = 100-barSz-SNIPES_SIZE;
            clBx[i].x = xPos[i];
            clBx[i].y = yPos[i];
            rBx[i].x=xPos[i]*dt.scl+dt.xPush;
            rBx[i].y=yPos[i]*dt.scl;
        }
    }
}

//We just check if firing[i] > 0 in battlestate, to apply the correct image.
//We also check if a sniper is firing to take away the PC's health in battlestate.
void Snipes::HandleFiring(PlayerCharacter PC, int nmSnps)
{
    for (int i = 0; i < nmSnps; i++){
        if (fleeTime[i] < 0 && isAlive[i]){       //only if they're not fleeing.
            if (coolDown[i] < 0){
                if (!charging[i] && !firing[i]){
                    charging[i] = true;
                    chrgTime[i] = 1500;     //1.5 seconds
                    //chrgTime[i] = 50;
                }else if (chrgTime[i] <= 0 && !firing[i]){       //time to fire.
                    //Ready Fire Weapon
                    firing[i] = true;
                    fireTime[i] = 100;      //x/1000 seconds shot duration.
                    //fireTime[i] = 2000;

                    charging[i] = false;
                }
                if (fireTime[i] <= 0 && !charging[i]){ //done firing
                    firing[i] = false;
                    fireTime[i] = 0;
                    coolDown[i] = 1000;      //x/1000 seconds cooldown.
                }
            }
        } else {
            //they're either fleeing or dead.
            //they need to charge back up again from scratch.
            chrgTime[i] = 1500;
            firing[i] = false;
            charging[i] = false;
        }
    }
}

void Snipes::SetTimes(int nmSnps)
{
    for (int i = 0; i < nmSnps; i++){
        chrgTime[i] -= FRAME_TIME;
        coolDown[i] -= FRAME_TIME;
        fireTime[i] -= FRAME_TIME;
        fleeTime[i] -= FRAME_TIME;
    }
}

void Snipes::DamagePlayer(PlayerCharacter* pPC, int nmSnps, bool doDam)
{/*
    //not a collision, just doing damage
    for (int i = 0; i < curLevel.numSnipes; i++){
        if (instaS.fireTime[i] > 0){        //if it's firing, just assume it's hitting
            if (snipesDoDamage)PC.health -= 1;     //can't be bothered to do better.
        }
    }*/
}

void Snipes::SpawnSnipes(int nmSnps, int time)
{
    //I could easily control which entrance they come from based on a simple if i%2 = 0.
    if (time-lastSpawn > 600 && (prevSpawn+1 < nmSnps)){      //spawn every x ms.
        spawned[prevSpawn+1] = true;
        fresh[prevSpawn+1] = 300;         //they move to one spot for x ms, then do regular AI
        printf("Spawned Sniper\n");
        lastSpawn = time;
        isAlive[prevSpawn+1]=true;
        if(prevSpawn==-1){
            xPos[prevSpawn+1]=50;
            yPos[prevSpawn+1]=50;
        } else {
            xPos[prevSpawn+1]=xPos[prevSpawn]+5;
            yPos[prevSpawn+1]=yPos[prevSpawn]-5;
        }

        prevSpawn++;
    }
}

///Bomb Methods
Bombs::Bombs(int nmBms, int barSz, gs dt)
{
    srand(time(0));
    for (int i = 0; i < nmBms; i++){
        angle[i] = 0;
        int pieceX = (100-barSz)/nmBms;
        int pieceY = (100-barSz)/nmBms;
        xPos[i] = 100-barSz - pieceX*i;
        yPos[i] = 100-barSz - pieceY*i;
        xVel[i] = 0;
        yVel[i] = 0;
        clBx[i].w=BOMB_SIZE;
        clBx[i].h=BOMB_SIZE;
        rBx[i].w=BOMB_SIZE*dt.scl;
        rBx[i].h=BOMB_SIZE*dt.scl;
        rBx[i].x=0;
        rBx[i].y=0;
        exBx[i].h=rBx[i].h*2;
        exBx[i].w=rBx[i].w*2;
        isAlive[i] = true;
        flipImg[i] = false;
        nextMove[i] = 0;        //time for their next move.
        //bomb lasts between x and x+3 seconds.
        timeLeft[i] = rand()%10000 + 3000;
        exploding[i] = false;
        explodingTime[i] = 0;       //this gets set later anyway
        xDest[i] = 0;
        yDest[i] = 0;
    }

}

//we just move the bomb directly to a spot.
void Bombs::MoveBombs(int nmBms, gs dt)
{
    double widthDiff;
    double heightDiff;
    for (int i = 0; i < nmBms; i++){
        if(isAlive[i] && !exploding[i]){            //only move alive bombs, that aren't exploding.
            widthDiff = xDest[i] - xPos[i];      //if positive, dest to the right
            heightDiff = yDest[i] - yPos[i];     //if positive, dest below

            double yRatio, yRatioSqrd;
            if (widthDiff != 0){
                yRatio = heightDiff/widthDiff;  //how much y in x.
            } else {
                yRatio = 100000;
            }
            yRatioSqrd = yRatio*yRatio;

            if (widthDiff == 0){
                if (heightDiff > 0){
                    yVel[i] += BOMB_ACC_PER_FRAME;
                } else {
                    yVel[i] -= BOMB_ACC_PER_FRAME;
                }
                xVel[i] += 0;       //an unneccessary step
            } else {
                double xVelInc, yVelInc;
                xVelInc = BOMB_ACC_PER_FRAME/(sqrt(yRatioSqrd+1));
                yVelInc = xVelInc*yRatio;

                if (widthDiff > 0){         //player to the right
                    xVel[i] += xVelInc;
                    yVel[i] += yVelInc;
                } else if (widthDiff < 0){  //player to the left
                    xVel[i] -= xVelInc;
                    yVel[i] -= yVelInc;
                }
            }
            //if it's too fast, decrease proportionally.
            //need to use velocity increase instead of velocity
            double maxDisSqrd = BOMB_DIS_PER_FRAME*BOMB_DIS_PER_FRAME;
            if ((xVel[i]*xVel[i] + yVel[i]*yVel[i]) > maxDisSqrd){
                if (xVel[i] == 0){
                    if (yVel[i] < 0){
                        yVel[i] *= -1*BOMB_DIS_PER_FRAME;
                    } else {
                        yVel[i] = BOMB_DIS_PER_FRAME;
                    }
                } else{
                    xVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                    yVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                }
            }
        xPos[i] += xVel[i];
        yPos[i] += yVel[i];
        clBx[i].x=xPos[i];
        clBx[i].y=yPos[i];
        rBx[i].x=dt.xPush+xPos[i]*dt.scl;
        rBx[i].y=yPos[i]*dt.scl;
        exBx[i].x=rBx[i].x-BOMB_SIZE*dt.scl/2;
        exBx[i].y=rBx[i].y-BOMB_SIZE*dt.scl/2;
        }
    }
}

void Bombs::SpawnBombs(int nmBmbs, int time)
{
    //I could easily control which entrance they come from based on a simple if i%2 = 0.
    if (time-lastSpawn > 2000 && (prevSpawn+1 < nmBmbs)){      //spawn every x ms.
        spawned[prevSpawn+1] = true;
        fresh[prevSpawn+1] = 300;         //they move to one spot for x ms, then do regular AI
        lastSpawn = time;
        isAlive[prevSpawn+1]=true;
        timeLeft[prevSpawn+1]=4000;

        printf("Spawned Bomb\n");
        prevSpawn++;
    }
}

///Sprinkler Methods
Sprinkler::Sprinkler(gs dt)
{
    xPos = 100/2;
    yPos = 100/2;
    interval = 150;
    angle = 0;
    isAlive = true;
    health = 100;
    flipImg = false;
    clBx.w = SPRINKLER_SIZE;
    clBx.h = SPRINKLER_SIZE;
    clBx.x = xPos;
    clBx.y = yPos;
    rBx.x=clBx.x*dt.scl+dt.xPush;
    rBx.y=clBx.y*dt.scl;
    rBx.w=SPRINKLER_SIZE*dt.scl;
    rBx.h=SPRINKLER_SIZE*dt.scl;
}

//make the sprinkler spin around and around.
void Sprinkler::rotateSprinkler()
{
    const double ANGULAR_SPEED = 2*PI/4;        //takes x seconds to rotate.
    double angleInc = ANGULAR_SPEED/FRAMES_PER_SECOND;
    angle += angleInc;
}

///Turrets Methods
Turrets::Turrets(int nmTrs, gs dt)
{
    for (int i = 0; i < nmTrs; i++){
        clBx[i].w = TURRET_SIZE;
        clBx[i].h = TURRET_SIZE;
        rBx[i].w=TURRET_SIZE*dt.scl;
        rBx[i].h=TURRET_SIZE*dt.scl;
        isAlive[i] = true;      //bit scary that I don't make them all false before this, but it shouldn't matter
        health[i] = 100;
        flipImg[i] = false;
        angle[i] = 0;
        lastFire[i] = 0 - (i*FRAME_TIME);
        xVel[i] = 0;
        yVel[i] = 0;        //why do I even have velocity for this?
    }
}

///Bolts methods
Bolts::Bolts(int nmBlts, gs dt)
{
    for (int i = 0; i < nmBlts; i++){
        xPos[i] = 0;
        yPos[i] = 0;
        clBx[i].w = BOLT_SIZE;
        clBx[i].h = BOLT_SIZE;
        rBx[i].w=BOLT_SIZE*dt.scl;
        rBx[i].h=BOLT_SIZE*dt.scl;
        isAlive[i] = false;     //made true upon firing
        angle[i] = 0;
        flipImg[i] = false;
        xVel[i] = 0;
        yVel[i] = 0;
    }
}

bool Bolts::newBolt(int startX, int startY, double speed, int nmBlts, PlayerCharacter PC, float angle, bool atkPC)
{
    const double TURRET_BOLT_DIS_PER_FRAME = speed/FRAMES_PER_SECOND;

    int index = 0;      //this is which slot in the array it is
    bool emptySpot = false;

    for (int i = 0; i < nmBlts; i++){
        if (isAlive[i] == false){
            index = i;
            isAlive[i] = true;
            emptySpot = true;
            break;
        }
    }
    if (emptySpot == false) return false;          //we couldn't find an empty spot.

    //write initial starting position
    float strtX=startX;
    float strtY=startY;
    strtX+=TURRET_SIZE/2;
    strtY+=TURRET_SIZE/2;
    strtX+=std::sin(angle/57.2957795)*TURRET_SIZE/2;
    strtY-=std::cos(angle/57.2957795)*TURRET_SIZE/2;

    xPos[index] = strtX;
    yPos[index] = strtY;
    clBx[index].x = xPos[index];
    clBx[index].y = yPos[index];

    //figure out xVel and yVel
    float destX, destY, widthDiff, heightDiff;
    if(atkPC){
        destX=PC.clBx.x+PC_SIZE/2;
        destY=PC.clBx.y+PC_SIZE/2;
        widthDiff=destX-xPos[index];
        heightDiff=destY-yPos[index];
    }else{
        widthDiff=sin(angle);       //all that matters is the ratio
        heightDiff=-1*cos(angle);
    }

    //check that widthDiff is not 0 first, can't divide by zero
    if (widthDiff == 0){
        if (heightDiff > 0){
            yVel[index] = TURRET_BOLT_DIS_PER_FRAME;
        } else {
            yVel[index] = -TURRET_BOLT_DIS_PER_FRAME;
        }
        xVel[index] = 0;
    } else {
        double yRatio = heightDiff/widthDiff;   //how much of x y is. If y = 2, x = 3, yRatio = .67
        double yRatioSqrd = yRatio*yRatio;

        xVel[index] = TURRET_BOLT_DIS_PER_FRAME/(sqrt(yRatioSqrd+1));
        yVel[index] = xVel[index]*yRatio;

        if (widthDiff < 0){
            xVel[index] = -xVel[index];
            yVel[index] = -yVel[index];         //because it makes switches yVel as well (complicated)
        }

    }
    return true;
}

void Bolts::moveBolts(int nmBlts, gs dt)
{
    //We move the bolts according to their velocities. If they go off the edge of the screen we
    //treat them as dead, and will eventually overwrite them.
    for (int i = 0; i < nmBlts; i++){
        if (isAlive[i] == true){
            xPos[i] += xVel[i];
            yPos[i] += yVel[i];
            clBx[i].x = xPos[i];
            clBx[i].y = yPos[i];
            rBx[i].x=dt.xPush+xPos[i]*dt.scl;
            rBx[i].y=yPos[i]*dt.scl;

            if (xPos[i] > 100 || xPos[i] < 0){
                isAlive[i] = false;
            }
            if (yPos[i] > 100 || yPos[i] < 0){
                isAlive[i] = false;
            }
        }
    }
}

//side effect, makes the boxes the right size.
void PlacePillars(Pillars* pPllrs, Lev lv, int wv, gs dt)
{
    for(int i=0; i<lv.wvs[wv].nmPls; i++){
        pPllrs->clBx[i].x=lv.wvs[wv].plrX[i];
        pPllrs->clBx[i].y=lv.wvs[wv].plrY[i];
        pPllrs->clBx[i].w=PILLAR_SIZE;
        pPllrs->clBx[i].h=PILLAR_SIZE;
        pPllrs->rBx[i].x=dt.xPush+pPllrs->clBx[i].x*dt.scl;
        pPllrs->rBx[i].y=pPllrs->clBx[i].y*dt.scl;
        pPllrs->rBx[i].w=PILLAR_SIZE*dt.scl;
        pPllrs->rBx[i].h=PILLAR_SIZE*dt.scl;
    }
}

void SwitchVel(float* xVel, float* yVel, float* xPos, float* yPos)
{
    //if we have an immovable object, then we just push the character away.
    *xVel*=-1;
    *yVel*=-1;
    *xPos+=*xVel*2;
    *yPos+=*yVel*2;
}

void SwitchVel(float* xVel, float* yVel, float* xPos, float* yPos, float* xVel2, float* yVel2, float* xPos2, float* yPos2)
{
    double xTemp, yTemp;
    xTemp = *xVel;
    yTemp = *yVel;

    *xVel=*xVel2;
    *xVel2=xTemp;
    *yVel=*yVel2;
    *yVel2=yTemp;

    *xPos+=*xVel*1;
    *yPos+=*yVel*1;
    *xPos2+=*xVel2*1;
    *yPos2+=*yVel2*1;
}











