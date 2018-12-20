#ifndef CHARACTERS_H_INCLUDED
#define CHARACTERS_H_INCLUDED

#include "GLOBALS.h"
#include "Helpers.h"
#include "Events.h"
//#include "SDL.h"

struct CharacterBase{
    float xVel, yVel;
    tdcRct clBx;
    float angle;
    float hlth;
    bool isAlv, isSpwn;
    int imgInd;     //which particular image we use to render the character.
};

void InitCharBase(CharacterBase* );

//I might have each character contain an int that stores the right texture to render.
struct EntityInfo{
    EntityType type;
    SDL_Texture* pImg[8];      //each character has multiple possible images.
    int maxNum;
    float mass;
};

struct PC
{
    CharacterBase bs;
    EntityInfo inf;
    tdcRct smBx;        //Wanted smaller true collision box, because character is circular. Could do per pixel, but bolts are larger than they should be.
    bool hit;
    int hitDelay;       //when less than 0, hit = false. Gets charged on hit.
    int lastShot;       //for the gun.
    //int overheat?
};

struct Turrets
{
    CharacterBase bs[16];
    EntityInfo inf;
    int lastFire[16];
};

struct Bolts
{
    CharacterBase bs[128];  //awesome
    EntityInfo inf;
};

struct Hunters{
    CharacterBase bs[16];
    EntityInfo inf;
    //charging stuff as well.
};

struct Burns{
    CharacterBase bs[32];
    EntityInfo inf;
    //switching stuff as well.
    int tmLft[32];      //time left for any particular spawn, starts at BURN_TIME, counts down from there.
    bool warming[32];       //implied from tmLft and BURN_WARMUP
};

struct SpinnerOrb{
    //need the distance from the center of the spinner orb, use trig to get this when it rotates
    float dis;
    CharacterBase bs;
    //EntityInfo inf;     //should this be here?
};

//spinner has a body, then four arms. Each arm has a bunch of orbs.
struct Spinner{
    //forget the body for now.
    CharacterBase bs;

    SpinnerOrb arm1[8];
    SpinnerOrb arm2[8];
    SpinnerOrb arm3[8];
    SpinnerOrb arm4[8];
    EntityInfo inf;
    float rotation;     //spinner rotates by this amount per frame. In degrees.
};

struct TutOb{
    CharacterBase bs;
    EntityInfo inf;
    int tmLft;
    bool hit;       //could be hit, could be picked up, doesn't really matter much.
};
struct TutObs{
    TutOb movs, targets;
};

//Useful for functions.
struct AllChars{
    PC* pPC;
    Turrets* pTrs;
    Hunters* pHnts;
    Bolts* pBlts;
    TutObs* pTutObs;
    Burns* pBrns;
    Spinner* pSpins;
};


void InitPC(PC*);
void InitTurrets(Turrets* );
void InitBolts(Bolts* );
void InitHunters(Hunters* );
void InitBurns(Burns* );
void InitSpinOrb(SpinnerOrb* ,float );
void InitSpinner(Spinner* );
void InitTutObs(TutObs* );
void InitChars(AllChars* );

void ResetChars(AllChars* , Uint16 );
void ResetCharBase(CharacterBase* , float* , float* );

void ResetPC(PC* , float , float );
void ResetTutObjs(TutObs* );

bool FireBolt(float , float , PC* , float ,Bolts* );
bool FireGun(PC* ,Bolts* ,int );

void MovePC(PC* ,const KeyPress ,GmMouse );
void MoveHunters(PC* , Hunters* );
void MoveBolts(Bolts* );
void MoveBurns(Burns* );
void MoveChars(AllChars ,const KeyPress ,GmMouse );
void TurnTurrets(AllChars chrs);
void TurnSpinner(Spinner* , int time);    //implicitly moves the orbs as well.

void CollideTutorialObjects(PC* , TutObs* );

void CollisionDetection(AllChars );
void CheckAgainstType(CharacterBase* ,EntityInfo ,CharacterBase* , EntityInfo );
bool DetectCollision(tdcRct ,tdcRct );
void DealWithCollision(CharacterBase* , EntityInfo ,CharacterBase* ,EntityInfo );

void SetBurnImgInd(Burns* pBrns);

void RenderSpinOrbs(Spinner* pSpins);
void RenderCharacters(AllChars );
void RenderEntity(CharacterBase* ,EntityInfo );


/*
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
*/


void SwitchVel(float*, float*, float*, float*);
void SwitchVel(float*, float*, float*, float*, float*, float*, float*, float*);

#endif      //CHARACTERS_H_INCLUDED hopefully included
