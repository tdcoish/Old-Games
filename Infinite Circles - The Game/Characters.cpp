#include <SDL.h>
#include "Characters.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int FRAME_TIME = 10;      //in milliseconds.
const int FRAMES_PER_SECOND = 1000/FRAME_TIME;

const int NUM_SPIKETRAPS = 64;   //they should go off the screen by the end
const int SHOT_INTERVAL = 300;  //in milliseconds

const double PI = 3.14159265359;

const int SNIPER_PATH_RADIUS = 380;     //in pixels
//frequency is measured here in seconds
const double SNIPER_ANGULAR_SPEED = 2*PI/15;     //2*PI*frequency, which is 1/xHZ, in radians per second
double sniperAngle = 0;         //in radians, we use this just to start

const int SPIKETRAP_PATH_RADIUS = 50;   //in pixels
const double SPIKETRAP_ANGULAR_SPEED = 2*PI/4;      //x seconds for each rotation
double spikeTrapAngle = 0;

const int SNIPER_BULLET_SIZE = 10;              //size in pixels, width and height

const int MAX_BOLTS = 128;

const double MAX_HUNTER_SPEED = 180;             //x pixels per second
const double HUNTER_DIS_PER_FRAME = MAX_HUNTER_SPEED/FRAMES_PER_SECOND;     //x pixels per frame
const double HUNTER_ACC_PER_SEC = 100;
const double HUNTER_ACC_PER_FRAME = HUNTER_ACC_PER_SEC/FRAMES_PER_SECOND;   //x more pixels per frame
const double HUNTER_CHARGE_RADIUS = 150;     //x pixels
const int COLLISION_TIME = 500;     //500 ms of collision pathing for the hunters
const double HUNTER_CHARGE_DURATION = 500;      //x ms of charging in a straight line

const int NUM_HUNTERS = 3;

///SpikeTraps methods
//The four traps start in the center of one quadrant of the screen.
SpikeTraps::SpikeTraps()
{
    //evenly spaced out on the screen, 4 per line, 4 lines
    const int HOR_SPACE = SCREEN_WIDTH/5;
    const int VER_SPACE = SCREEN_HEIGHT/5;

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
void SpikeTraps::moveTraps(int totalTime)
{
    //Gonna have to rewrite spiketraps to have a center position
    //increase the angle by the angular speed, divided by the frames per second
    //double angleIncrease = SNIPER_ANGULAR_SPEED/(1000/10);
    double angleIncrease = SPIKETRAP_ANGULAR_SPEED/FRAMES_PER_SECOND;
    spikeTrapAngle-=angleIncrease;

    for (int i = 0; i < NUM_SPIKETRAPS; i++){
        xPos[i] = x[i] + SPIKETRAP_PATH_RADIUS*cos(spikeTrapAngle);
        yPos[i] = y[i] + SPIKETRAP_PATH_RADIUS*sin(spikeTrapAngle);
    }
}



///PlayerCharacter methods
//The player starts in the center of the map
PlayerCharacter::PlayerCharacter()
{
    yVel = 0;
    xVel = 0;

    collBox.x = SCREEN_WIDTH/2;
    collBox.y = SCREEN_HEIGHT/2;
    trueX = collBox.x;
    trueY = collBox.y;

    collBox.w = 20;
    collBox.h = 20;

    health = 100.f;

    down = false;
    up = false;
    left = false;
    right = false;
}

//player can move the character in all four directions
//The bugs I'm getting here are all sorts of weird
void PlayerCharacter::handleInput(SDL_Event* pEvent)
{
    /*
    if (pEvent->type == SDL_KEYDOWN){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: up = true; break;     //rewrite 1 as distancePerFrame
            case SDLK_DOWN: down = true; break;
            case SDLK_RIGHT: right = true; break;
            case SDLK_LEFT: left = true; break;
        }
    } else if (pEvent->type == SDL_KEYUP){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: up = false; break;
            case SDLK_DOWN: down = false; break;
            case SDLK_RIGHT: right = false; break;
            case SDLK_LEFT: left = false; break;
        }
    }
    */


    if (pEvent->type == SDL_KEYDOWN){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: yVel -= 3; break;     //rewrite 1 as distancePerFrame
            case SDLK_DOWN: yVel += 3; break;
            case SDLK_RIGHT: xVel += 3; break;
            case SDLK_LEFT: xVel -= 3; break;
        }
    } else if (pEvent->type == SDL_KEYUP){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: yVel += 3; break;
            case SDLK_DOWN: yVel -= 3; break;
            case SDLK_RIGHT: xVel -= 3; break;
            case SDLK_LEFT: xVel += 3; break;
        }
    }


    /* Getting a weird error where I can only move up and left
    if (pEvent->type == SDL_KEYDOWN){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: yVel -= distancePerFrame; break;
            case SDLK_DOWN: yVel += distancePerFrame; break;
            case SDLK_RIGHT: xVel += distancePerFrame; break;
            case SDLK_LEFT: xVel -= distancePerFrame; break;
        }
    } else if (pEvent->type == SDL_KEYUP){
        switch (pEvent->key.keysym.sym){
            case SDLK_UP: yVel += distancePerFrame; break;
            case SDLK_DOWN: yVel -= distancePerFrame; break;
            case SDLK_RIGHT: xVel -= distancePerFrame; break;
            case SDLK_LEFT: xVel += distancePerFrame; break;
        }
    } */
}

void PlayerCharacter::move(SpikeTraps* pSpikeTraps)
{
    //needs to eventually check for collisions with the spikeTraps array
    /*
    xVel = 0;
    yVel = 0;

    bool tempup, tempdown, tempright, templeft;
    tempup = up;
    tempdown = down;
    tempright = right;
    templeft = left;

    if (tempup && tempdown){
        tempup = false;
        tempdown = false;
    }

    if (templeft && tempright){
        templeft = false;
        tempright = false;
    }

    if (tempup){                    //tempDown must not be pressed
        if (templeft){
            xVel = -2.2;
            yVel = -2.2;
        } else if (tempright){
            xVel = 2.2;
            yVel = -2.2;
        } else {
            yVel = -3;
        }
    } else if (tempdown){
        if (templeft){
            xVel = -2.2;
            yVel = 2.2;
        } else if (tempright){
            xVel = 2.2;
            yVel = 2.2;
        } else {
            yVel = 3;
        }
    } else if (templeft){       //up and down aren't pressed or we wouldn't get here.
        xVel = -3;
    } else if (tempright){
        xVel = 3;
    } else {        //none are true
        xVel = 0;
        yVel = 0;
    }
    */


    //now just move the player image
    trueX += xVel;
    trueY += yVel;

    if (trueX < 0 || trueX > (SCREEN_WIDTH-collBox.w)){
        trueX -= xVel;
    }
    if (trueY < 0 || trueY > (SCREEN_HEIGHT-collBox.h)){
        trueY -= yVel;
    }

    collBox.x = trueX;
    collBox.y = trueY;
}

Hunters::Hunters()
{
    //velocities are initially zero
    for (int i = 0; i < NUM_HUNTERS; i++){
        xPos[i] = 100 * (i+1);
        yPos[i] = 100 * (i+1);
        xVel[i] = 0;
        yVel[i] = 0;
        chargeTime[i] = 0;      //not charging to start
        collidedTime[i] = 0;    //not collided to start
        damageCooldown[i] = 0;  //can attack to start
        charging[i] = false;
    }
}

//moves hunters according to player posiition,
//This is the ugliest code I've ever written
//It's a nightmare to understand and it's not indented properly
void Hunters::moveHunters(PlayerCharacter copyPlayer)
{
    for (int i = 0; i < NUM_HUNTERS; i++){
        collidedTime[i] -= FRAME_TIME;
        chargeTime[i] -= FRAME_TIME;
    }

    //the destination if the player for both hunters
    int destX = copyPlayer.collBox.x;
    int destY = copyPlayer.collBox.y;

    double widthDiff;
    double heightDiff;
    //I see the error of my ways here, we don't want max acceleration in both directions
    for (int i = 0; i < NUM_HUNTERS; i++){
        if (collidedTime[i] < 0 && chargeTime[i] < 0){
            charging[i] = false;        //since it only gets here if the charge is up.

            widthDiff = destX - xPos[i];      //if positive, player to the right
            heightDiff = destY - yPos[i];     //if positive, player below

            //first calculate distance and charge or not
            if (widthDiff*widthDiff + heightDiff*heightDiff < HUNTER_CHARGE_RADIUS*HUNTER_CHARGE_RADIUS){
                if (chargeTime[i] < -HUNTER_CHARGE_DURATION){     //not already charging
                    chargeTime[i] = HUNTER_CHARGE_DURATION;
                    charging[i] = true;

                    //charging doubles the hunters speed
                    if (widthDiff == 0){
                        if (heightDiff > 0){
                            yVel[i] = HUNTER_DIS_PER_FRAME*2;       //or HUNTER_DIS_PER_FRAME
                        } else {
                            yVel[i] = -HUNTER_DIS_PER_FRAME*2;
                        }
                    } else {
                        double yRatio = heightDiff/widthDiff;
                        double yRatioSqrd = yRatio*yRatio;

                        xVel[i] = HUNTER_DIS_PER_FRAME*2/(sqrt(yRatioSqrd+1));
                        yVel[i] = xVel[i]*yRatio;

                        if (widthDiff < 0){
                            xVel[i] = -xVel[i];
                            yVel[i] = -yVel[i];
                        }
                    }
                }
            }

            if (charging[i] == false){
                double yRatio, yRatioSqrd;
                if (widthDiff != 0) yRatio = heightDiff/widthDiff;  //how much y in x.
                yRatioSqrd = yRatio*yRatio;

                if (widthDiff == 0){
                    if (heightDiff > 0){
                        yVel[i] += HUNTER_ACC_PER_FRAME;
                    } else {
                        yVel[i] -= HUNTER_ACC_PER_FRAME;
                    }
                    xVel[i] += 0;       //an unneccessary step
                } else {
                    double xVelInc, yVelInc;
                    xVelInc = HUNTER_ACC_PER_FRAME/(sqrt(yRatioSqrd+1));
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
                double maxDisSqrd = HUNTER_DIS_PER_FRAME*HUNTER_DIS_PER_FRAME;
                if ((xVel[i]*xVel[i] + yVel[i]*yVel[i]) > maxDisSqrd){
                    if (xVel[i] == 0){
                        if (yVel[i] < 0){
                            yVel[i] *= -1*HUNTER_DIS_PER_FRAME;
                        } else {
                            yVel[i] = HUNTER_DIS_PER_FRAME;
                        }
                    } else if (xVel[i] < 0){
                        xVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                        //xVel[i] = -1*sqrt(maxDisSqrd/(yRatioSqrd+1));
                        yVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                    } else {
                        xVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                        //xVel[i] = sqrt(maxDisSqrd/(yRatioSqrd+1));
                        yVel[i] *= sqrt(maxDisSqrd/(xVel[i]*xVel[i] + yVel[i]*yVel[i]));
                    }
                }
            }
        }
        xPos[i] += xVel[i];
        yPos[i] += yVel[i];
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

///Sniper Methods
//starting the sniper in the middle just for debugging purposes
Sniper::Sniper()
{
    interval = SHOT_INTERVAL;
    xPos = SCREEN_WIDTH/2;
    yPos = SCREEN_HEIGHT/2;
}

//This is eventually going to be a circle written trigonometrically.
void Sniper::moveSniper(int appTime)
{
    //increase the angle by the angular speed, divided by the frames per second
    //double angleIncrease = SNIPER_ANGULAR_SPEED/(1000/10);
    double angleIncrease = SNIPER_ANGULAR_SPEED/FRAMES_PER_SECOND;
    sniperAngle-=angleIncrease;

    xPos = SCREEN_WIDTH/2 + SNIPER_PATH_RADIUS*cos(sniperAngle);
    yPos = SCREEN_HEIGHT/2 + SNIPER_PATH_RADIUS*sin(sniperAngle);
}

//The sniper fires every 200 ms and in the direction of the player
//turns out I went a different direction architecturally
void Sniper::shootBall(PlayerCharacter copyPlayer)
{
    //create a ball and fire it in the direction of the player at a certain speed

}

///SniperBullet Methods
//positional and velocital data will be overwritten when the slot goes live
SniperBolts::SniperBolts()
{
    for (int i = 0; i < MAX_BOLTS; i++){
        xPos[i] = 400;
        yPos[i] = 400;
        xVel[i] = 20;
        yVel[i] = 20;
        collBox[i].w = 10;
        collBox[i].h = 10;
        collBox[i].x = xPos[i];
        collBox[i].y = yPos[i];
        isAlive[i] = false;
    }
}

//Creates a new sniper bolt, figures out proper position(easy) and velocity(hard)
bool SniperBolts::newBolt(double sniperX, double sniperY, PlayerCharacter copyPlayer, double speed)
{
    const double SNIPER_BULLET_DIS_PER_FRAME = speed/FRAMES_PER_SECOND;

    int index = 0;      //this is which slot in the array it is
    bool emptySpot = false;

    for (int i = 0; i < MAX_BOLTS; i++){
        if (isAlive[i] == false){
            index = i;
            isAlive[i] = true;
            emptySpot = true;
            break;
        }
    }
    if (emptySpot == false) return false;          //we couldn't find an empty spot.

    //write initial starting position
    xPos[index] = sniperX;
    yPos[index] = sniperY;
    collBox[index].x = xPos[index];
    collBox[index].y = yPos[index];

    //figure out xVel and yVel
    float playerX = copyPlayer.collBox.x;
    float playerY = copyPlayer.collBox.y;

    double widthDiff = playerX - xPos[index];      //if positive, player to the right
    double heightDiff = playerY - yPos[index];     //if positive, player below

    //check that widthDiff is not 0 first, can't divide by zero
    if (widthDiff == 0){
        if (heightDiff > 0){
            yVel[index] = SNIPER_BULLET_DIS_PER_FRAME;
        } else {
            yVel[index] = -SNIPER_BULLET_DIS_PER_FRAME;
        }
        xVel[index] = 0;
    } else {
        double yRatio = heightDiff/widthDiff;   //how much of x y is. If y = 2, x = 3, yRatio = .67
        double yRatioSqrd = yRatio*yRatio;

        xVel[index] = SNIPER_BULLET_DIS_PER_FRAME/(sqrt(yRatioSqrd+1));
        yVel[index] = xVel[index]*yRatio;

        if (widthDiff < 0){
            xVel[index] = -xVel[index];
            yVel[index] = -yVel[index];         //because it makes switches yVel as well (complicated)
        }

    }
    return true;
}

void SniperBolts::moveBolts()
{
    //We move the bolts according to their velocities. If they go off the edge of the screen we
    //treat them as dead, and will eventually overwrite them.

    for (int i = 0; i < MAX_BOLTS; i++){
        if (isAlive[i] == true){
            xPos[i] += xVel[i];
            yPos[i] += yVel[i];
            collBox[i].x = xPos[i];
            collBox[i].y = yPos[i];

            if (xPos[i] > SCREEN_WIDTH || xPos[i] < 0){
                isAlive[i] = false;
            }
            if (yPos[i] > SCREEN_HEIGHT || yPos[i] < 0){
                isAlive[i] = false;
            }
        }
    }
}

///HealthPacks Methods
HealthPacks::HealthPacks()
{
    for (int i = 0; i < 8; i++){
        xPos[i] = SCREEN_WIDTH/8;
        yPos[i] = SCREEN_HEIGHT/8 * i;
        isActivated[i] = false;         //honestly the only important variable to initialize
        collBox[i].w = 20;
        collBox[i].h = 20;
        collBox[i].x = xPos[i];
        collBox[i].y = yPos[i];
    }

}

///BurnSpots Methods
BurnSpots::BurnSpots(const int NM_SPTS)
{
    for(int i = 0; i < NM_SPTS; i++){
        xPos[i] = SCREEN_WIDTH/NM_SPTS;
        yPos[i] = SCREEN_HEIGHT/NM_SPTS * i;
        isActive[i] = false;
        sinceDead[i] = 100*i;
        collBox[i].w = 20;
        collBox[i].h = 20;
        collBox[i].x = xPos[i];
        collBox[i].y = yPos[i];
    }
}

//moves the spots if they need moving. appTime should be 10ms.
void BurnSpots::Rearrange(int appTime, int numActive)
{
    for (int i = 0; i < numActive; i++){
        sinceDead[i] += appTime;
    }
    for (int i = 0; i < numActive; i++){
        if (sinceDead[i] > 100*numActive){
            xPos[i] = (((rand()%800)%32)*25);
            yPos[i] = (((rand()%800)%32)*25);
            //should check to make sure no conflicts but whatever.
            collBox[i].x = xPos[i];
            collBox[i].y = yPos[i];
            isActive[i] = false;
            sinceDead[i] = 0;
        }
    }
}

void BurnSpots::Activate(int numActive)
{
    for (int i = 0; i < numActive; i++){
        if (sinceDead[i] > 400){
            isActive[i] = true;
        }
    }

}








