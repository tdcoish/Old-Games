#include <SDL.h>
#include "NPC.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "GLOBALS.h"
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

const double ANGL_FM=150/FRAMES_PER_SECOND;
const double HUNTER_DIS_PER_FRAME=20/FRAMES_PER_SECOND;

///Initialization functions provide safe dummy values for all the "characters".
void InitCharBase(CharacterBase* pBs)
{
    pBs->xVel=0;
    pBs->yVel=0;
    pBs->clBx.x=0;
    pBs->clBx.y=0;
    pBs->clBx.w=0;
    pBs->clBx.h=0;
    pBs->angle=0;
    pBs->hlth=0;
    pBs->isAlv=false;
    pBs->isSpwn=false;
    pBs->imgInd=0;
}

//The player starts in the center of the map
void InitPC(PC* pPC)
{
    InitCharBase(&pPC->bs);
    pPC->bs.hlth=100;

    pPC->bs.clBx.w=PC_SIZE;
    pPC->bs.clBx.h=PC_SIZE;

    pPC->hit = false;
    pPC->hitDelay = 0;
    pPC->lastShot=0;

    pPC->inf.type=P_C;
    pPC->inf.maxNum=1;
    pPC->inf.mass=10;
    LoadTexture(bsPth+"\\Img\\Gameplay\\pc.png", &pPC->inf.pImg[0]);
}

void InitTurrets(Turrets* pTrs)
{
    for(int i=0; i<16; i++)
    {
        InitCharBase(&pTrs->bs[i]);
        pTrs->bs[i].clBx.w=TURRET_SIZE;
        pTrs->bs[i].clBx.h=TURRET_SIZE;

        pTrs->lastFire[i]=0-(i*FRAME_TIME);
    }
    //Set up EntityInfo
    pTrs->inf.type=TURRET;
    LoadTexture(bsPth+"\\Img\\Gameplay\\turret.png", &pTrs->inf.pImg[0]);
    pTrs->inf.maxNum=16;        //for now.
    pTrs->inf.mass=80;
}

void InitHunters(Hunters* pHnts)
{
    for(int i=0; i<16; i++){
        InitCharBase(&pHnts->bs[i]);
        pHnts->bs[i].clBx.w=HUNTER_SIZE;
        pHnts->bs[i].clBx.h=HUNTER_SIZE;
        //handle charging stuff
    }
    //Set up EntityInfo
    pHnts->inf.type=HUNTER;
    LoadTexture(bsPth+"\\Img\\Gameplay\\hunter.png", &pHnts->inf.pImg[0]);
    pHnts->inf.maxNum=16;
    pHnts->inf.mass=30;
}

//constructor-like. Makes sure nothing's crazy.
void InitBolts(Bolts* pBts)
{
    for(int i=0; i<128; i++){
        InitCharBase(&pBts->bs[i]);
        pBts->bs[i].clBx.w = BOLT_SIZE;
        pBts->bs[i].clBx.h = BOLT_SIZE;
    }
    pBts->inf.type=BOLT;
    pBts->inf.maxNum=128;
    pBts->inf.mass=1;
    LoadTexture(bsPth+"\\Img\\Gameplay\\bolt.png", &pBts->inf.pImg[0]);
}

void InitBurns(Burns* pBrns)
{
    for(int i=0; i<32; i++){
        InitCharBase(&pBrns->bs[i]);
        pBrns->tmLft[i]=0+i*200;
        pBrns->bs[i].isAlv=false;
        pBrns->bs[i].clBx.w=BURN_SIZE;
        pBrns->bs[i].clBx.h=BURN_SIZE;
        pBrns->warming[i]=false;
    }
    pBrns->inf.type=BURNER;
    pBrns->inf.maxNum=32;
    pBrns->inf.mass=0.1;        //doesn't matter
    LoadTexture(bsPth+"\\Img\\Gameplay\\burn0.png", &pBrns->inf.pImg[0]);
    LoadTexture(bsPth+"\\Img\\Gameplay\\burn1.png", &pBrns->inf.pImg[1]);
}

void InitSpinOrb(SpinnerOrb* pOrb, float dis)
{
    InitCharBase(&pOrb->bs);
    pOrb->bs.clBx.w=SPIN_ORB_SIZE;
    pOrb->bs.clBx.h=SPIN_ORB_SIZE;
    pOrb->dis=dis;
    pOrb->bs.isAlv=true;
}

void InitSpinner(Spinner* djSpnz)
{
    for(int i=0; i<8; i++){
        for(int i=0; i<8; i++){
            InitSpinOrb(&djSpnz->arm1[i], (i+1)*7);
            InitSpinOrb(&djSpnz->arm2[i], (i+1)*7);
            InitSpinOrb(&djSpnz->arm3[i], (i+1)*7);
            InitSpinOrb(&djSpnz->arm4[i], (i+1)*7);
        }
    }
    InitCharBase(&djSpnz->bs);
    djSpnz->rotation=20;        //x degrees per second.
    djSpnz->inf.type=SPINNER;
    djSpnz->inf.maxNum=1;
    djSpnz->inf.mass=20;
    LoadTexture(bsPth+"\\Img\\Gameplay\\spinOrb.png", &djSpnz->inf.pImg[0]);
}

//Super ugly code for the tutorial objects.
void InitTutObs(TutObs* pObs)
{
    InitCharBase(&pObs->movs.bs);
    InitCharBase(&pObs->targets.bs);
    pObs->movs.bs.clBx.w=TUT_OB_SIZE;     //for now
    pObs->movs.bs.clBx.h=TUT_OB_SIZE;
    pObs->targets.bs.clBx.w=TUT_OB_SIZE;
    pObs->targets.bs.clBx.h=TUT_OB_SIZE;

    pObs->movs.inf.type=MOVE_OB;
    pObs->movs.inf.maxNum=1;
    LoadTexture(bsPth+"\\Img\\Gameplay\\movObj.png", &pObs->movs.inf.pImg[0]);

    pObs->targets.inf.type=ATK_OB;
    pObs->targets.inf.maxNum=1;
    LoadTexture(bsPth+"\\Img\\Gameplay\\atkObj.png", &pObs->targets.inf.pImg[0]);

    pObs->movs.tmLft=-1;
    pObs->targets.tmLft=-1;

    pObs->movs.inf.mass=1;
    pObs->targets.inf.mass=1;

    pObs->movs.hit=true;        //so they get reset first frame
    pObs->targets.hit=true;
}

//Reserves memory for characters, initializes them to dummy default values.
void InitChars(AllChars* pChrs)
{
    pChrs->pPC=new PC();
    pChrs->pTrs=new Turrets();
    pChrs->pHnts=new Hunters();
    pChrs->pBlts=new Bolts();
    pChrs->pTutObs=new TutObs();
    pChrs->pBrns=new Burns();
    pChrs->pSpins=new Spinner();

    InitPC(pChrs->pPC);
    InitTurrets(pChrs->pTrs);
    InitHunters(pChrs->pHnts);
    InitBolts(pChrs->pBlts);
    InitTutObs(pChrs->pTutObs);
    InitBurns(pChrs->pBrns);
    InitSpinner(pChrs->pSpins);
    //and so on for all characters.

}

///Reset functions. Called every time logic dictates.
void ResetChars(AllChars* pChrs)
{
    //reset all characters based on which states they need to be reset in.
    //or reset them differently according to different states.

    //We set up the characters according to the game state, so for now
    //eg. if(gmSt&MOV_TUT||gmSt&ATK_TUT||gmSt&BOTH_TUT) reset tutorial objects.

}

//Sets character to middle or specified location, not spawned, full health, etc.
void ResetCharBase(CharacterBase* pBs, float* pX, float* pY)
{
    if(pX!=NULL){
        pBs->clBx.x=*pX;
    }else {
        pBs->clBx.x=50;
    }
    if(pY!=NULL){
        pBs->clBx.y=*pY;
    }else {
        pBs->clBx.y=50;
    }

    pBs->angle=0;
    pBs->hlth=100;
    pBs->isAlv=true;
    pBs->xVel=0;
    pBs->yVel=0;
    pBs->isSpwn=false;
}

//Resets the PC where the program specifies. Cleans up remaining variables.
void ResetPC(PC* pPC, float xPos, float yPos)
{
    ResetCharBase(&pPC->bs, &xPos, &yPos);
    //reset individual stuff
    pPC->hit=false;
    pPC->hitDelay=0;
}

//Definitely in need of refactoring
void ResetTutObjs(TutObs* pObs){
    float newX, newY;
    int xChange, yChange;
    if(pObs->movs.hit){
        srand(time(0));
        newX=pObs->movs.bs.clBx.x;
        newY=pObs->movs.bs.clBx.y;
        //new position is within 50 units of the old position, but more than 30 units away, both x and y.
        xChange=rand()%20+10;
        yChange=rand()%20+10;   //I could do radius but why bother?
        //this way we never go off the map.
        if(newX>50){
            newX-=xChange;
        }else{
            newX+=xChange;
        }
        if(newY>50){
            newY-=yChange;
        }else{
            newY+=yChange;
        }

        //shouldn't be null, should be semi-random.
        ResetCharBase(&pObs->movs.bs, &newX, &newY);

        pObs->movs.hit=false;
        pObs->movs.tmLft=5000;       //total hack, will be changed.
    }

    if(pObs->targets.hit){
        xChange=rand()%20+30;
        yChange=rand()%20+30;

        newX=pObs->targets.bs.clBx.x;
        newY=pObs->targets.bs.clBx.y;
        if(newX>50){
            newX-=xChange;
        }else{
            newX+=xChange;
        }
        if(newY>50){
            newY-=yChange;
        }else{
            newY+=yChange;
        }

        //again, should be semi-random
        ResetCharBase(&pObs->targets.bs, &newX, &newY);

        pObs->targets.hit=false;
        pObs->targets.tmLft=5000;       //total hack, will be changed.
    }
}


///Movement functions exist for all characters that move.

void MoveChars(AllChars chrs,const KeyPress keys, GmMouse ms)
{
    //move pc, move bolts, move hunters.
    MovePC(chrs.pPC, keys, ms);
    MoveBolts(chrs.pBlts);
    MoveHunters(chrs.pPC, chrs.pHnts);
}

//We've got the input, now we just move the player.
//There is no gameplay logic here.
void MovePC(PC* pPC ,const KeyPress keys, GmMouse ms)
{
    pPC->bs.xVel=0;
    pPC->bs.yVel=0;
    //very important not to use else if here
    if(keys.store&keys.down) pPC->bs.yVel+=PC_DIS_PER_FRAME;
    if(keys.store&keys.up) pPC->bs.yVel-=PC_DIS_PER_FRAME;
    if(keys.store&keys.right)pPC->bs.xVel+=PC_DIS_PER_FRAME;
    if(keys.store&keys.left)pPC->bs.xVel-=PC_DIS_PER_FRAME;

    //we want to keep speed constant
    double maxDisSqrd = PC_DIS_PER_FRAME*PC_DIS_PER_FRAME;
    float xV=pPC->bs.xVel;  //readability
    float yV=pPC->bs.yVel;
    if ((xV*xV + yV*yV) > (maxDisSqrd)){
        if (xV == 0){
            if (yV < 0){
                yV = -1*PC_DIS_PER_FRAME;
            }else{
                yV = PC_DIS_PER_FRAME;
            }
        }else{
            xV *= sqrt(maxDisSqrd/(xV*xV + yV*yV));
            yV *= xV/pPC->bs.xVel;    //reduce yV by same amount as x, saves expensive operation.
        }
    }
    pPC->bs.xVel=xV;
    pPC->bs.yVel=yV;

    //now just move the player
    pPC->bs.clBx.x += pPC->bs.xVel;
    pPC->bs.clBx.y += pPC->bs.yVel;

    //eventually I'll need to add level width and height.
    if (pPC->bs.clBx.x < 0 || pPC->bs.clBx.x > (100-pPC->bs.clBx.w)){
        pPC->bs.clBx.x -= pPC->bs.xVel;
    }
    if (pPC->bs.clBx.y < 0 || pPC->bs.clBx.y > (100-pPC->bs.clBx.h)){
        pPC->bs.clBx.y -= pPC->bs.yVel;
    }

    //using mouse to get angle.
    float xDis=ms.clBx.x-pPC->bs.clBx.x;
    float yDis=ms.clBx.y-pPC->bs.clBx.y;
    pPC->bs.angle=FindAngle(xDis, yDis);
}

void MoveHunters(PC* pPC, Hunters* pHnts)
{
    //assume for now that the world is 0-100 x and y.
    for(int i=0; i<pHnts->inf.maxNum; i++){
        //handle variables unique to hunters
    }

    double wdDf, htDf, trueAngle, anglDf;
    for(int i=0; i<pHnts->inf.maxNum; i++){
        //or check health or whatever.
        if(pHnts->bs[i].isAlv&&pHnts->bs[i].isSpwn){
            //have them move towards the center of the PC.
            wdDf=pPC->bs.clBx.x+(HUNTER_SIZE-PC_SIZE)/2-pHnts->bs[i].clBx.x;
            htDf=pPC->bs.clBx.y+(HUNTER_SIZE-PC_SIZE)/2-pHnts->bs[i].clBx.y;
            trueAngle=FindAngle(wdDf, htDf);
            anglDf=pHnts->bs[i].angle-trueAngle;
            if(anglDf>0&&anglDf<=180||anglDf<=-180){
                pHnts->bs[i].angle-=ANGL_FM;
            }else{
                pHnts->bs[i].angle+=ANGL_FM;
            }
            if(pHnts->bs[i].angle<-180)pHnts->bs[i].angle+=360;
            if(pHnts->bs[i].angle>180)pHnts->bs[i].angle-=360;

            pHnts->bs[i].xVel=sin(pHnts->bs[i].angle/57.29578)*HUNTER_DIS_PER_FRAME;
            pHnts->bs[i].yVel=cos(pHnts->bs[i].angle/57.29578)*HUNTER_DIS_PER_FRAME;

            //now handle charging
            /*
            if (wdDf*wdDf + htDf*htDf < HUNTER_CHARGE_RADIUS*HUNTER_CHARGE_RADIUS && std::abs(anglDf)<10){
                charging[i] = true;
                pauseTime[i] = 70;      //pause for x ms before charge.
                xVel[i]*=2;
                yVel[i]*=2;
                chargeTime[i]=HUNTER_CHARGE_DURATION;
            }
            */
        }
        pHnts->bs[i].clBx.x+=pHnts->bs[i].xVel;
        pHnts->bs[i].clBx.y-=pHnts->bs[i].yVel; //interesting -=
    }

}

//deterministically move bolts. Kill them if they go off the edge.
void MoveBolts(Bolts* pBts)
{
    //We move the bolts according to their velocities. If they go off the edge of the screen we
    //treat them as dead, and will eventually overwrite them.
    for (int i = 0; i < 128; i++){
        if (pBts->bs[i].isAlv == true){
            pBts->bs[i].clBx.x += pBts->bs[i].xVel;
            pBts->bs[i].clBx.y += pBts->bs[i].yVel;

            if (pBts->bs[i].clBx.x > 100 || pBts->bs[i].clBx.x < 0){
                pBts->bs[i].isAlv = false;
            }
            if (pBts->bs[i].clBx.y > 100 || pBts->bs[i].clBx.y < 0){
                pBts->bs[i].isAlv = false;
            }
        }
    }
}

//Moves burners only if their tmLft is negative.
void MoveBurns(Burns* pBrns)
{
    //not sure how this is going to work, I may need a numActive variable to pass in.
    for(int i=0; i<pBrns->inf.maxNum; i++){
        pBrns->tmLft[i]-=FRAME_TIME;
        if(pBrns->tmLft[i]<0){
            pBrns->bs[i].clBx.x=rand()%90+5;    //between 5-95
            pBrns->bs[i].clBx.y=rand()%90+5;
            pBrns->tmLft[i]=BURN_TIME;
        }
        if(pBrns->tmLft[i]>BURN_TIME-BURN_WARMUP){
            pBrns->warming[i]=true;
        }else{
            pBrns->warming[i]=false;
        }
    }
}

//just updates the angle
void TurnTurrets(AllChars chrs)
{
    float xDis, yDis;
    for(int i=0; i<chrs.pTrs->inf.maxNum; i++){
        if(chrs.pTrs->bs[i].isAlv&&chrs.pTrs->bs[i].isSpwn){
            xDis=chrs.pPC->bs.clBx.x-chrs.pTrs->bs[i].clBx.x;
            yDis=chrs.pPC->bs.clBx.y-chrs.pTrs->bs[i].clBx.y;
            chrs.pTrs->bs[i].angle=FindAngle(xDis, yDis);
        }
    }

}

//mostly just moves all the orbs around.
void TurnSpinner(Spinner* pSpinz, int time)
{
    pSpinz->rotation=sin((float)time/3000)*120;        //max rotation of x degrees
    pSpinz->bs.angle+=pSpinz->rotation/FRAMES_PER_SECOND;
    if(pSpinz->bs.angle>180)pSpinz->bs.angle-=360;
    for(int i=0; i<8; i++){
        //for efficiency and readability, maybe should have made a variable that = angle?
        double convAngle=pSpinz->bs.angle*ANGLE_TO_RAD;
        pSpinz->arm1[i].bs.clBx.x=50+sin(convAngle)*pSpinz->arm1[i].dis;
        pSpinz->arm1[i].bs.clBx.y=50-cos(convAngle)*pSpinz->arm1[i].dis;

        pSpinz->arm2[i].bs.clBx.x=50+cos(convAngle)*pSpinz->arm2[i].dis;
        pSpinz->arm2[i].bs.clBx.y=50+sin(convAngle)*pSpinz->arm2[i].dis;

        pSpinz->arm3[i].bs.clBx.x=50-sin(convAngle)*pSpinz->arm3[i].dis;
        pSpinz->arm3[i].bs.clBx.y=50+cos(convAngle)*pSpinz->arm3[i].dis;

        pSpinz->arm4[i].bs.clBx.x=50-cos(convAngle)*pSpinz->arm4[i].dis;
        pSpinz->arm4[i].bs.clBx.y=50-sin(convAngle)*pSpinz->arm4[i].dis;
    }
}

///Collision Detection functions.
void CollisionDetection(AllChars chrs)
{
    CheckAgainstType(&chrs.pPC->bs, chrs.pPC->inf, chrs.pTrs->bs, chrs.pTrs->inf);
    //and so on for all the npc's.

    CollideTutorialObjects(chrs.pPC, chrs.pTutObs);

}


//They don't need much special
void CollideTutorialObjects(PC* pPC, TutObs* pObs)
{
    if(DetectCollision(pPC->bs.clBx, pObs->movs.bs.clBx)){
        pObs->movs.hit=true;
    }
    //In reality, we have to use the weapon, not the PC for this one.
    if(DetectCollision(pPC->bs.clBx, pObs->targets.bs.clBx)){
        pObs->targets.hit=true;
    }
}

//Checks collisions against all objects of that type.
void CheckAgainstType(CharacterBase* pBs1, EntityInfo e1, CharacterBase* pBs2, EntityInfo e2)
{
    //go through all of the first type
	for(int i=0; i<e1.maxNum; i++){
		if(pBs1[i].isAlv){
            for(int j=0; j<e2.maxNum; j++){
                if(pBs2[j].isAlv){
                    if(DetectCollision(pBs1[i].clBx, pBs2[j].clBx)){
                        //This might be a great time to have an event queue. Then just make a StoreCollision function, and deal with it later.
                        DealWithCollision(&pBs1[i], e1, &pBs2[j], e2);
                    }
                }
            }
		}
	}
}

//Elegant and beautiful. Unlike the rest of this ColDet code.
//Assumes circles.
bool DetectCollision(tdcRct A,tdcRct B)
{
    tdcPoint cntA, cntB;
	cntA.x=A.x+A.w/2;
	cntA.y=A.y+A.h/2;
	cntB.x=B.x+B.w/2;
	cntB.y=B.y+B.h/2;

	//Use distance for overlapping.
	float xDis=cntA.x-cntB.x;
	float yDis=cntA.y-cntB.y;
	float totalDistanceFromCentersSqrd=xDis*xDis+yDis*yDis;

    //finds effective radius
	float angl=FindAngle(xDis, yDis);

	float radX1=A.w*sin(angl);
	float radX2=B.w*sin(angl);
	float radY1=A.w*cos(angl);
	float radY2=B.w*cos(angl);
	float effectiveRadiusSqrd=radX1*radX2+radY1*radY2;

    if(totalDistanceFromCentersSqrd<effectiveRadiusSqrd) return true;

	return false;
}

//
void DealWithCollision(CharacterBase* pBs1, EntityInfo e1,CharacterBase* pBs2,EntityInfo e2)
{
    //pretend I bothered actually finding the center of these objects
	tdcPoint cntA, cntB;
	cntA.x=pBs1->clBx.x; cntA.y=pBs1->clBx.y;		//not dealing with .h or .w at all here
	cntB.x=pBs2->clBx.x; cntB.y=pBs2->clBx.y;
	cntA.x-=pBs1->clBx.w/2; cntA.y+=pBs1->clBx.h/2;
	cntB.x-=pBs2->clBx.w/2; cntB.y+=pBs2->clBx.h/2;

	//acceleration = force*mass. force = mass of pushing object*differential velocity.
	float xVelDif=pBs1->xVel-pBs2->xVel;
	float yVelDif=pBs1->yVel-pBs2->yVel;
	float totalVelDif=sqrt(xVelDif*xVelDif+yVelDif*yVelDif);

	//now I find the angle that I'm going to push one of the objects by.
	float xDis=cntB.x-cntA.x;
	float yDis=cntB.y-cntA.y;
	double angle=FindAngle(xDis, yDis);

	float acc=e1.mass/e2.mass*totalVelDif;
	//now change base2's vel by sin and cos using the angle and the force
	pBs2->yVel+=cos(angle)*acc;
	pBs2->xVel+=sin(angle)*acc;		//I might not use +, but it all works.

	//now do the second object, we already have the differential velocity
	xDis*=-1;
	yDis*=-1;
	angle=FindAngle(xDis, yDis);
	acc=e2.mass/e1.mass*totalVelDif;
	pBs1->xVel+=sin(angle)*acc;		//I could just -= and not reset the angle.
	pBs1->yVel+=cos(angle)*acc;

	//Now we need to push these objects apart from each other this frame, which requires both objects to be just a shade farther than the radius of //the other.
	float dis=sqrt((cntA.x-cntB.x)*(cntA.x-cntB.x)+(cntA.y-cntB.y)*(cntA.y-cntB.y));
	float pushDis=pBs1->clBx.w+pBs2->clBx.w-dis;		//radius of both added together, minus the distance they are already apart.
	pushDis*=1.1;		//just so they get pushed a little bit further then necessary.

	//push each object half the distance required, I might change this later, but whatever it works.
	pBs1->clBx.x-=sin(angle)*pushDis/2;
	pBs1->clBx.y-=cos(angle)*pushDis/2;
	pBs2->clBx.x+=sin(angle)*pushDis/2;
	pBs2->clBx.y+=cos(angle)*pushDis/2;
}

///Special logic for each character exists here
//Anything can fire a bolt
bool FireBolt(float startX, float startY, PC* playa, float angle, Bolts* pBts)
{
    //got to get BLT_SPD here working
    double TURRET_BOLT_DIS_PER_FRAME = 22.5/FRAMES_PER_SECOND;
    if(playa==NULL)TURRET_BOLT_DIS_PER_FRAME*=2.5;

    int index = 0;      //this is which slot in the array it is
    bool emptySpot = false;

    for (int i = 0; i < 128; i++){
        if (pBts->bs[i].isAlv == false){
            index = i;
            pBts->bs[i].isAlv = true;
            emptySpot = true;
            break;
        }
    }
    if (emptySpot == false) return false;          //we couldn't find an empty spot.

    //write initial starting position
    //some finicky code here that won't work if the player shoots bolts.
    float strtX=startX;
    float strtY=startY;
    float sizeFactor;
    if(playa!=NULL){
        sizeFactor=TURRET_SIZE;
    }else{
        sizeFactor=PC_SIZE;
    }
    //get to the center of the object
    strtX+=sizeFactor/2-BOLT_SIZE/2;
    strtY+=sizeFactor/2-BOLT_SIZE/2;
    //push out to the very edge of the object, plus a tiny bit extra.
    strtX+=std::sin(angle/57.2957795)*sizeFactor*1.5/2;
    strtY-=std::cos(angle/57.2957795)*sizeFactor*1.5/2;

    //the bolts starting position is where it was fired.
    pBts->bs[index].clBx.x = strtX;
    pBts->bs[index].clBx.y = strtY;

    //figure out xVel and yVel
    float destX, destY, widthDiff, heightDiff;
    if(playa!=NULL){
        destX=playa->bs.clBx.x+PC_SIZE/2;
        destY=playa->bs.clBx.y+PC_SIZE/2;
        widthDiff=destX-pBts->bs[index].clBx.x;
        heightDiff=destY-pBts->bs[index].clBx.y;
    }else{
        widthDiff=sin(angle/57.2957795);       //all that matters is the ratio
        heightDiff=-1*cos(angle/57.2957795);
    }

    //check that widthDiff is not 0 first, can't divide by zero
    if (widthDiff == 0){
        if (heightDiff > 0){
            pBts->bs[index].yVel = TURRET_BOLT_DIS_PER_FRAME;
        }else{
            pBts->bs[index].yVel = -TURRET_BOLT_DIS_PER_FRAME;
        }
        pBts->bs[index].xVel = 0;
    }else{
        double yRatio = heightDiff/widthDiff;   //how much of x y is. If y = 2, x = 3, yRatio = .67
        double yRatioSqrd = yRatio*yRatio;

        pBts->bs[index].xVel = TURRET_BOLT_DIS_PER_FRAME/(sqrt(yRatioSqrd+1));
        pBts->bs[index].yVel = pBts->bs[index].xVel*yRatio;

        if (widthDiff < 0){
            pBts->bs[index].xVel = -pBts->bs[index].xVel;
            pBts->bs[index].yVel = -pBts->bs[index].yVel;         //because it makes switches yVel as well (complicated)
        }

    pBts->bs[index].angle=FindAngle(pBts->bs[index].xVel, pBts->bs[index].yVel);
    }
    return true;
}

bool FireGun(PC* pPC, Bolts* pBts, int time)
{
    if(time-pPC->lastShot>GUN_RATE){
        float anglRnd=rand()%8-4;     //x degrees of maximum randomness.
        //printf("%f, ", pPC->bs.angle);
        //spread the angle a little bit first.
        FireBolt(pPC->bs.clBx.x, pPC->bs.clBx.y, NULL, pPC->bs.angle+anglRnd, pBts);
        pPC->lastShot=time;
    }
}

//Used for rendering the proper image.
void SetBurnImgInd(Burns* pBrns)
{
    for(int i=0; i<pBrns->inf.maxNum; i++){
        if(pBrns->tmLft[i]<(BURN_TIME-BURN_WARMUP)){
            pBrns->bs[i].imgInd=1;
        }else{
            pBrns->bs[i].imgInd=0;
        }
    }
}
///Rendering code for all the characters.
//Takes all the characters, passes them off to RenderEntity
void RenderCharacters(AllChars chrs)
{
    RenderEntity(chrs.pTrs->bs, chrs.pTrs->inf);
    //RenderEntity(p_c.bs, p_c.inf, gfx);
    RenderEntity(chrs.pBlts->bs, chrs.pBlts->inf);
    RenderEntity(chrs.pHnts->bs, chrs.pHnts->inf);
    SetBurnImgInd(chrs.pBrns);
    RenderEntity(chrs.pBrns->bs, chrs.pBrns->inf);
    //ugliness to render spinner orbs
    RenderSpinOrbs(chrs.pSpins);
    //and so on.
}

void RenderSpinOrbs(Spinner* pSpins)
{
    for(int i=0; i<8; i++){
        //check if alive, but implement later
        RenderEntity(&pSpins->arm1[i].bs, pSpins->inf);
        RenderEntity(&pSpins->arm2[i].bs, pSpins->inf);
        RenderEntity(&pSpins->arm3[i].bs, pSpins->inf);
        RenderEntity(&pSpins->arm4[i].bs, pSpins->inf);
    }
}

//Finds centerpoint, renders characters until maximum
void RenderEntity(CharacterBase* pBs,EntityInfo info)
{
    //needs special case for PC, which isn't an array
    for(int i=0; i<info.maxNum; i++){
        if(pBs[i].isAlv){
            RenderObj(pBs[i].clBx, &GFX.cam, info.pImg[pBs[i].imgInd], &pBs[i].angle);
            //Don't forget NULL when appropriate
        }
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











