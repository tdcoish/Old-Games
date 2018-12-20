/********************************************************************************************************
Finally we're almost done.

The game cycles through:
for(each number of turrets)
    for(each number of objects)
        for(each particular arrangement of those turrets)
            capture the objects

I don't do these in for loops mostly, I just set them up using their own logic. When the user collects
the final object, we incrememnt the arrangement number by one. If the arrangement number is at its
maximum that means that we should go to the next object level. We then make the arr num 0, and make the
wav num +=1.
If the user dies, or runs out of time, I set the arr Num back by one, and set the nextArr to true. This
will bring us right back to where we left off, but might be confusing.
After all changes, the game will render the first frame, and then pause for a brief period of time. This
allows the user to collect themselves for the next outing.
********************************************************************************************************/

#include "CTF.h"
#include "Characters.h"
#include "IC_GlobalVars.h"
#include "Timer.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <string>


void SetObjectPositions(AllObjects* pObDt)
{
    pObDt->ob[0].ps[0]={50.f, 10.f};

    pObDt->ob[1].ps[0]={10.f, 5.f}; pObDt->ob[1].ps[1]={90.f, 5.f};

    pObDt->ob[2].ps[0]={10.f, 25.f}; pObDt->ob[2].ps[1]={50.f, 5.f}; pObDt->ob[2].ps[2]={90.f, 25.f};

    pObDt->ob[3].ps[0]={20, 35}; pObDt->ob[3].ps[1]={30, 15}; pObDt->ob[3].ps[2]={70, 15};
    pObDt->ob[3].ps[3]={80, 35};

    pObDt->ob[4].ps[0]={10, 55}; pObDt->ob[4].ps[1]={30, 35}; pObDt->ob[4].ps[2]={50, 15};
    pObDt->ob[4].ps[3]={70, 35}; pObDt->ob[4].ps[4]={90, 55};

    pObDt->ob[5].ps[0]={10, 45}; pObDt->ob[5].ps[1]={30, 25}; pObDt->ob[5].ps[2]={50, 5};
    pObDt->ob[5].ps[3]={70, 25}; pObDt->ob[5].ps[4]={90, 45}; pObDt->ob[5].ps[5]={50, 55};
}

void SetTurretPositions(TurretPositions* pTrs)
{
    pTrs->sets[0].arNm=9;
    pTrs->sets[0].ars[0].ps[0]={50.f, 50.f};
    pTrs->sets[0].ars[1].ps[0]={50.f, 15.f};
    pTrs->sets[0].ars[2].ps[0]={50.f, 80.f};
    pTrs->sets[0].ars[3].ps[0]={10.f, 50.f};
    pTrs->sets[0].ars[4].ps[0]={90.f, 50.f};
    pTrs->sets[0].ars[5].ps[0]={90.f, 90.f};
    pTrs->sets[0].ars[6].ps[0]={10.f, 90.f};
    pTrs->sets[0].ars[7].ps[0]={10.f, 10.f};
    pTrs->sets[0].ars[8].ps[0]={90.f, 10.f};

    pTrs->sets[1].arNm=15;
    pTrs->sets[1].ars[0].ps[0]={30, 50}; pTrs->sets[1].ars[0].ps[1]={70, 50};
    pTrs->sets[1].ars[1].ps[0]={10, 50}; pTrs->sets[1].ars[1].ps[1]={90, 50};
    pTrs->sets[1].ars[2].ps[0]={50, 50}; pTrs->sets[1].ars[2].ps[1]={50, 15};
    pTrs->sets[1].ars[3].ps[0]={50, 30}; pTrs->sets[1].ars[3].ps[1]={50, 70};
    pTrs->sets[1].ars[4].ps[0]={10, 30}; pTrs->sets[1].ars[4].ps[1]={10, 70};
    pTrs->sets[1].ars[5].ps[0]={90, 30}; pTrs->sets[1].ars[5].ps[1]={90, 70};
    pTrs->sets[1].ars[6].ps[0]={20, 80}; pTrs->sets[1].ars[6].ps[1]={80, 80};
    pTrs->sets[1].ars[7].ps[0]={20, 80}; pTrs->sets[1].ars[7].ps[1]={80, 20};
    pTrs->sets[1].ars[8].ps[0]={20, 20}; pTrs->sets[1].ars[8].ps[1]={80, 80};
    pTrs->sets[1].ars[9].ps[0]={50, 50}; pTrs->sets[1].ars[9].ps[1]={10, 50};
    pTrs->sets[1].ars[10].ps[0]={50, 50}; pTrs->sets[1].ars[10].ps[1]={90, 50};
    pTrs->sets[1].ars[11].ps[0]={50, 50}; pTrs->sets[1].ars[11].ps[1]={80, 20};
    pTrs->sets[1].ars[12].ps[0]={50, 50}; pTrs->sets[1].ars[12].ps[1]={20, 20};
    pTrs->sets[1].ars[13].ps[0]={50, 50}; pTrs->sets[1].ars[13].ps[1]={20, 80};
    pTrs->sets[1].ars[14].ps[0]={50, 50}; pTrs->sets[1].ars[14].ps[1]={80, 80};

    pTrs->sets[2].arNm=7;
    pTrs->sets[2].ars[0].ps[0]={15, 50}; pTrs->sets[2].ars[0].ps[1]={50, 50}; pTrs->sets[2].ars[0].ps[2]={85, 50};
    pTrs->sets[2].ars[1].ps[0]={15, 60}; pTrs->sets[2].ars[1].ps[1]={50, 80}; pTrs->sets[2].ars[1].ps[2]={85, 60};
    pTrs->sets[2].ars[2].ps[0]={15, 10}; pTrs->sets[2].ars[2].ps[1]={50, 50}; pTrs->sets[2].ars[2].ps[2]={85, 90};
    pTrs->sets[2].ars[3].ps[0]={15, 70}; pTrs->sets[2].ars[3].ps[1]={50, 50}; pTrs->sets[2].ars[3].ps[2]={85, 30};
    pTrs->sets[2].ars[4].ps[0]={5, 10}; pTrs->sets[2].ars[4].ps[1]={70, 50}; pTrs->sets[2].ars[4].ps[2]={5, 90};
    pTrs->sets[2].ars[5].ps[0]={95, 10}; pTrs->sets[2].ars[5].ps[1]={30, 50}; pTrs->sets[2].ars[5].ps[2]={95, 90};
    pTrs->sets[2].ars[6].ps[0]={50, 10}; pTrs->sets[2].ars[6].ps[1]={50, 70}; pTrs->sets[2].ars[6].ps[2]={50, 50};

    pTrs->sets[3].arNm=13;
    pTrs->sets[3].ars[0].ps[0]={10, 50}; pTrs->sets[3].ars[0].ps[1]={40, 50};
    pTrs->sets[3].ars[0].ps[2]={60, 50}; pTrs->sets[3].ars[0].ps[3]={90, 50};
    pTrs->sets[3].ars[1].ps[0]={45, 90}; pTrs->sets[3].ars[1].ps[1]={55, 70};
    pTrs->sets[3].ars[1].ps[2]={45, 30}; pTrs->sets[3].ars[1].ps[3]={55, 10};
    pTrs->sets[3].ars[2].ps[0]={10, 10}; pTrs->sets[3].ars[2].ps[1]={90, 10};
    pTrs->sets[3].ars[2].ps[2]={10, 90}; pTrs->sets[3].ars[2].ps[3]={50, 50};
    pTrs->sets[3].ars[3].ps[0]={10, 10}; pTrs->sets[3].ars[3].ps[1]={90, 10};
    pTrs->sets[3].ars[3].ps[2]={50, 50}; pTrs->sets[3].ars[3].ps[3]={90, 90};
    pTrs->sets[3].ars[4].ps[0]={10, 10}; pTrs->sets[3].ars[4].ps[1]={50, 50};
    pTrs->sets[3].ars[4].ps[2]={10, 90}; pTrs->sets[3].ars[4].ps[3]={90, 90};
    pTrs->sets[3].ars[5].ps[0]={50, 50}; pTrs->sets[3].ars[5].ps[1]={90, 10};
    pTrs->sets[3].ars[5].ps[2]={10, 90}; pTrs->sets[3].ars[5].ps[3]={90, 90};
    pTrs->sets[3].ars[6].ps[0]={10, 70}; pTrs->sets[3].ars[6].ps[1]={90, 70};
    pTrs->sets[3].ars[6].ps[2]={40, 30}; pTrs->sets[3].ars[6].ps[3]={60, 30};
    pTrs->sets[3].ars[7].ps[0]={10, 10}; pTrs->sets[3].ars[7].ps[1]={50, 70};
    pTrs->sets[3].ars[7].ps[2]={50, 30}; pTrs->sets[3].ars[7].ps[3]={10, 90};
    pTrs->sets[3].ars[8].ps[0]={90, 10}; pTrs->sets[3].ars[8].ps[1]={50, 70};
    pTrs->sets[3].ars[8].ps[2]={50, 30}; pTrs->sets[3].ars[8].ps[3]={90, 90};
    pTrs->sets[3].ars[9].ps[0]={20, 80}; pTrs->sets[3].ars[9].ps[1]={30, 80};
    pTrs->sets[3].ars[9].ps[2]={40, 80}; pTrs->sets[3].ars[9].ps[3]={90, 10};
    pTrs->sets[3].ars[10].ps[0]={50, 80}; pTrs->sets[3].ars[10].ps[1]={70, 80};
    pTrs->sets[3].ars[10].ps[2]={90, 80}; pTrs->sets[3].ars[10].ps[3]={10, 10};
    pTrs->sets[3].ars[11].ps[0]={10, 20}; pTrs->sets[3].ars[11].ps[1]={30, 20};
    pTrs->sets[3].ars[11].ps[2]={50, 20}; pTrs->sets[3].ars[11].ps[3]={90, 90};
    pTrs->sets[3].ars[12].ps[0]={80, 20}; pTrs->sets[3].ars[12].ps[1]={70, 20};
    pTrs->sets[3].ars[12].ps[2]={60, 20}; pTrs->sets[3].ars[12].ps[3]={10, 90};

    pTrs->sets[4].arNm=9;
    pTrs->sets[4].ars[0].ps[0]={10, 50}; pTrs->sets[4].ars[0].ps[1]={30, 50}; pTrs->sets[4].ars[0].ps[2]={50, 50};
    pTrs->sets[4].ars[0].ps[3]={70, 50}; pTrs->sets[4].ars[0].ps[4]={90, 50};
    pTrs->sets[4].ars[1].ps[0]={10, 10}; pTrs->sets[4].ars[1].ps[1]={10, 90}; pTrs->sets[4].ars[1].ps[2]={50, 50};
    pTrs->sets[4].ars[1].ps[3]={90, 10}; pTrs->sets[4].ars[1].ps[4]={90, 90};
    pTrs->sets[4].ars[2].ps[0]={20, 70}; pTrs->sets[4].ars[2].ps[1]={50, 70}; pTrs->sets[4].ars[2].ps[2]={80, 70};
    pTrs->sets[4].ars[2].ps[3]={35, 20}; pTrs->sets[4].ars[2].ps[4]={65, 20};
    pTrs->sets[4].ars[3].ps[0]={10, 10}; pTrs->sets[4].ars[3].ps[1]={30, 30}; pTrs->sets[4].ars[3].ps[2]={50, 50};
    pTrs->sets[4].ars[3].ps[3]={70, 70}; pTrs->sets[4].ars[3].ps[4]={90, 90};
    pTrs->sets[4].ars[4].ps[0]={10, 90}; pTrs->sets[4].ars[4].ps[1]={30, 70}; pTrs->sets[4].ars[4].ps[2]={50, 50};
    pTrs->sets[4].ars[4].ps[3]={70, 30}; pTrs->sets[4].ars[4].ps[4]={90, 10};
    pTrs->sets[4].ars[5].ps[0]={10, 50}; pTrs->sets[4].ars[5].ps[1]={20, 20}; pTrs->sets[4].ars[5].ps[2]={50, 10};
    pTrs->sets[4].ars[5].ps[3]={80, 20}; pTrs->sets[4].ars[5].ps[4]={90, 50};
    pTrs->sets[4].ars[6].ps[0]={40, 10}; pTrs->sets[4].ars[6].ps[1]={25, 20}; pTrs->sets[4].ars[6].ps[2]={10, 50};
    pTrs->sets[4].ars[6].ps[3]={25, 80}; pTrs->sets[4].ars[6].ps[4]={40, 90};
    pTrs->sets[4].ars[7].ps[0]={60, 10}; pTrs->sets[4].ars[7].ps[1]={75, 20}; pTrs->sets[4].ars[7].ps[2]={90, 50};
    pTrs->sets[4].ars[7].ps[3]={75, 80}; pTrs->sets[4].ars[7].ps[4]={60, 90};
    pTrs->sets[4].ars[8].ps[0]={50, 10}; pTrs->sets[4].ars[8].ps[1]={50, 50}; pTrs->sets[4].ars[8].ps[2]={50, 90};
    pTrs->sets[4].ars[8].ps[3]={10, 50}; pTrs->sets[4].ars[8].ps[4]={90, 50};

    pTrs->sets[5].arNm=9;
    pTrs->sets[5].ars[0].ps[0]={10, 50}; pTrs->sets[5].ars[0].ps[1]={25, 50}; pTrs->sets[5].ars[0].ps[2]={40, 50};
    pTrs->sets[5].ars[0].ps[3]={60, 50}; pTrs->sets[5].ars[0].ps[4]={75, 50}; pTrs->sets[5].ars[0].ps[5]={90, 50};
    pTrs->sets[5].ars[1].ps[0]={50, 10}; pTrs->sets[5].ars[1].ps[1]={50, 25}; pTrs->sets[5].ars[1].ps[2]={50, 40};
    pTrs->sets[5].ars[1].ps[3]={50, 60}; pTrs->sets[5].ars[1].ps[4]={50, 75}; pTrs->sets[5].ars[1].ps[5]={50, 90};
    pTrs->sets[5].ars[2].ps[0]={10, 20}; pTrs->sets[5].ars[2].ps[1]={50, 20}; pTrs->sets[5].ars[2].ps[2]={90, 20};
    pTrs->sets[5].ars[2].ps[3]={10, 80}; pTrs->sets[5].ars[2].ps[4]={50, 80}; pTrs->sets[5].ars[2].ps[5]={90, 80};
    pTrs->sets[5].ars[3].ps[0]={10, 20}; pTrs->sets[5].ars[3].ps[1]={50, 10}; pTrs->sets[5].ars[3].ps[2]={90, 20};
    pTrs->sets[5].ars[3].ps[3]={10, 80}; pTrs->sets[5].ars[3].ps[4]={50, 90}; pTrs->sets[5].ars[3].ps[5]={90, 80};
    pTrs->sets[5].ars[4].ps[0]={20, 10}; pTrs->sets[5].ars[4].ps[1]={10, 50}; pTrs->sets[5].ars[4].ps[2]={20, 90};
    pTrs->sets[5].ars[4].ps[3]={80, 10}; pTrs->sets[5].ars[4].ps[4]={90, 50}; pTrs->sets[5].ars[4].ps[5]={80, 90};
    pTrs->sets[5].ars[5].ps[0]={20, 10}; pTrs->sets[5].ars[5].ps[1]={20, 50}; pTrs->sets[5].ars[5].ps[2]={20, 90};
    pTrs->sets[5].ars[5].ps[3]={80, 10}; pTrs->sets[5].ars[5].ps[4]={80, 50}; pTrs->sets[5].ars[5].ps[5]={80, 90};
    pTrs->sets[5].ars[6].ps[0]={10, 50}; pTrs->sets[5].ars[6].ps[1]={25, 70}; pTrs->sets[5].ars[6].ps[2]={40, 90};
    pTrs->sets[5].ars[6].ps[3]={60, 10}; pTrs->sets[5].ars[6].ps[4]={75, 30}; pTrs->sets[5].ars[6].ps[5]={90, 50};
    pTrs->sets[5].ars[7].ps[0]={10, 50}; pTrs->sets[5].ars[7].ps[1]={25, 30}; pTrs->sets[5].ars[7].ps[2]={40, 10};
    pTrs->sets[5].ars[7].ps[3]={60, 90}; pTrs->sets[5].ars[7].ps[4]={75, 70}; pTrs->sets[5].ars[7].ps[5]={90, 50};
    pTrs->sets[5].ars[8].ps[0]={30, 50}; pTrs->sets[5].ars[8].ps[1]={50, 70}; pTrs->sets[5].ars[8].ps[2]={70, 50};
    pTrs->sets[5].ars[8].ps[3]={50, 30}; pTrs->sets[5].ars[8].ps[4]={10, 10}; pTrs->sets[5].ars[8].ps[5]={90, 10};

}

int CTFGame(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, Mix_Music* pMsc, gs dt, const int turs, const Diff df)
{
    const float OBJECT_SIZE=4.3;
    float boltSpeed=22.5f;
    if(df==EASY)boltSpeed*=0.5;
    if(df==NORM)boltSpeed*=0.75;
    int bltDamTemp=50;        //half the players health.
    if(df==EASY)bltDamTemp*=0.5;
    if(df==NORM)bltDamTemp*=0.75;
    const float bltDam=bltDamTemp;

    int shotIntervalTemp=TURRET_SHOT_INTERVAL;
    if(df==EASY)shotIntervalTemp*=2;
    if(df==NORM)shotIntervalTemp*=1.5;
    const int shotInterval=shotIntervalTemp;

    float tmExt=1;
    if(df==EASY)tmExt*=4;
    if(df==NORM)tmExt*=2;


    SDL_Texture* pObjects = NULL;       //the objects we will be moving to.
    SDL_Texture* pPCImg = NULL;
    SDL_Texture* pTur = NULL;
    SDL_Texture* pBlt = NULL;
    SDL_Texture* pHnt = NULL;
    SDL_Texture* pBase = NULL;
    SDL_Texture* pMs = NULL;
    SDL_Texture* pBlk=NULL;
    SDL_Texture* pWht=NULL;

    SDL_Texture* pMsg1=NULL;
    SDL_Texture* pMsg2=NULL;
    SDL_Texture* pTm=NULL;
    SDL_Texture* ppDgts[10];

    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\objects.png", &pObjects, pFrame)){
        std::cout << "Error loading pickups\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoPC.png", &pPCImg, pFrame)){
        std::cout << "Error loading player character image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoTurret.png", &pTur, pFrame)){
        std::cout << "Error loading turret image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBolt.png", &pBlt, pFrame)){
        std::cout << "Error loading projectile image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoHunter.png", &pHnt, pFrame)){
        std::cout << "Error loading hunter image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\base.png", &pBase, pFrame)){
        std::cout << "Error loading return base\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\mouse.png", &pMs, pFrame)){
        std::cout << "Error loading mouse image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        std::cout << "Error loading black image\n";
        return 1;
    }
    if (!LoadTextureNoA(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityWhite.png", &pWht, pFrame)){
        std::cout << "Error loading white image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\Text Messages\\pickup.png", &pMsg1, pFrame)){
        std::cout << "Error loading text\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\Text Messages\\return.png", &pMsg2, pFrame)){
        std::cout << "Error loading text\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\Text Messages\\TimeLeft.png", &pTm, pFrame)){
        std::cout << "Error loading text\n";
        return 1;
    }
    //more convenient way of loading each digit.
    for(int i=0; i<10; i++){
        std::stringstream tmp;
        std::string dir;
        tmp<<tdcDirectoryPath+"\\Images\\Gameplay Images\\Numbers\\"<<i<<".png";
        dir=tmp.str();
        tmp.clear();
        if(!LoadTexture(dir, &ppDgts[i], pFrame)){
            std::cout << "Error loading number image\n";
            return 1;
        }
    }
    //Rects used to render the messages
    SDL_Rect msg;
    msg.x=30*dt.scl+dt.xPush;
    msg.y=48*dt.scl;
    msg.w=40*dt.scl;
    msg.h=5*dt.scl;
    SDL_Rect tm;
    tm.x=40*dt.scl+dt.xPush;
    tm.y=10*dt.scl;
    tm.w=15*dt.scl;        //because it should be slightly off.
    tm.h=5*dt.scl;


    TTF_Font* pFont = NULL;
    pFont = TTF_OpenFont((tdcDirectoryPath+"\\Squarish Sans CT Regular.ttf").c_str(), 36);
    //pFont = TTF_OpenFont("C:\\WINDOWS\\Fonts\\ARIAL.TTF", 36);
    if (pFont == NULL) return 1;

    SDL_Color fontColour = {0, 0, 0};     //used for health display

    AllObjects obPs;
    SetObjectPositions(&obPs);

    TurretPositions trPs;
    SetTurretPositions(&trPs);

    PlayerCharacter PC(dt);
    Bolts obs(6, dt);
    for(int i=0; i<6; i++){
        obs.clBx[i].w=OBJECT_SIZE;
        obs.clBx[i].h=OBJECT_SIZE;
        obs.rBx[i].w=OBJECT_SIZE*dt.scl;
        obs.rBx[i].h=OBJECT_SIZE*dt.scl;
    }
    Turrets trs(16, dt);
    Bolts blts(128, dt);              //means can handle max of x turret bolts
    tdcRct bse;
    bse.h = 5;
    bse.w = 90;
    bse.x = 5;
    bse.y = 95;
    SDL_Rect gBse;      //graphical component, consider way of making this neater. Possibly it's own struct
    gBse.h=bse.h*dt.scl;
    gBse.w=bse.w*dt.scl;
    gBse.x=dt.xPush+bse.x*dt.scl;
    gBse.y=bse.y*dt.scl;

    bool nextTrNm=true;            //if we move to a higher number of turrets.
    int nmTrs=turs-1;                    //the number of turrets we're currently on.
    int flgs = -1;                   //used for indexing which object set we're in. Always 1 less than the amount of flags.
    bool nextWave = false;          //move to the next object set if true
    int trAr=-1;                   //the arrangemen, or set we're currently on. Set here to immediately fail.
    bool nextArr=false;              //move to the next set/arrangement if true.

    bool quitAll = false;

    bool pcRegen=true;

    Timer eventTimer;
    srand(time(0));             //used only for wasting time. Ensures framerate.
    double timeWaster = 1;      //used only for wasting time. Ensures framerate.
    Timer fpsTimer;
    eventTimer.startClk();
    SDL_Event e;
    int pauseGame = 0;        //in ms.
    bool pauseNext=false;
    bool pauseGameKey=false;
    bool pickedUp = false;
    int obNm = -1;
    int tmLft = 0;
    int mseX, mseY;
    float msX, msY;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Mix_Volume(-1, 20);

    while (*pState == CTF){
        fpsTimer.startClk();
        SDL_GetMouseState(&mseX, &mseY);
        msX=(float)(mseX-dt.xPush)/dt.scl;
        msY=(float)mseY/dt.scl;
        while (SDL_PollEvent(&e) != 0){
            if (e.type == SDL_QUIT){
                *pState = MENU;
                *pMenSt = OUTRO;
            } else if (e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_ESCAPE){
                    *pState = MENU;
                }else if(e.key.keysym.sym==SDLK_SPACE){
                    pauseGameKey=(!pauseGameKey);
                }
            }
            PC.handleInput(&e);
        }
        pauseGame -= FRAME_TIME;        //I want to show the first frame of the new level, and then pause
        if(pauseGame<=0 && !pauseGameKey){
            if(pauseNext){
                pauseNext=false;
                pauseGame=300;
            }
            while(nextTrNm || nextWave || nextArr){
                if(nextTrNm){
                    nmTrs++;
                    if(nmTrs>6){    //something like this is necessary
                        *pState=MENU;
                        nmTrs=6;
                        flgs=4;     //just safety
                        quitAll=true;
                        break;
                    }
                    flgs=-1;
                    nextWave=true;
                    nextArr=true;
                    nextTrNm=false;
                }
                if(nextWave){
                    flgs++;
                    if(flgs>=6){    //because if we're on flgs=6, then we have 7 flags. offsets
                        nextTrNm=true;
                    }else{
                        nextWave = false;
                    }
                }
                if(nextArr){
                    PC.trueX=50;
                    PC.trueY=92;
                    trAr+=4;
                    //if we've done all the arrangements, then move to the next numObs, go to the first arrangement
                    if(trAr>=trPs.sets[nmTrs-1].arNm){
                        trAr-=1;
                        while(trAr>=0){
                            trAr-=4;
                        }
                        nextWave=true;
                    }else{
                        //set up the flags
                        for(int i=0; i<flgs+1; i++){
                            obs.xPos[i]=obPs.ob[flgs].ps[i].x;
                            obs.yPos[i]=obPs.ob[flgs].ps[i].y;
                            obs.clBx[i].x=obs.xPos[i];
                            obs.clBx[i].y=obs.yPos[i];
                            obs.rBx[i].x=dt.xPush+obs.xPos[i]*dt.scl;
                            obs.rBx[i].y=obs.yPos[i]*dt.scl;
                            obs.isAlive[i]=true;
                        }
                        for(int i=0; i<nmTrs; i++){
                            trs.clBx[i].x=trPs.sets[nmTrs-1].ars[trAr].ps[i].x;
                            trs.clBx[i].y=trPs.sets[nmTrs-1].ars[trAr].ps[i].y;
                            trs.rBx[i].x=dt.xPush+trs.clBx[i].x*dt.scl;
                            trs.rBx[i].y=trs.clBx[i].y*dt.scl;
                            trs.isAlive[i]=true;        //possibly unnecessary.
                            trs.health[i]=100;          //possibly unneccesary.
                            trs.lastFire[i]=eventTimer.getTicks()+i*FRAME_TIME*8;
                        }
                        for(int i=0; i<128; i++){
                            blts.isAlive[i] = false;
                        }
                        PC.health = 100;
                        tmLft=6*1000*tmExt*(flgs+2)+eventTimer.getTicks();
                        pickedUp = false;
                        obNm = -1;
                        nextArr=false;

                        pauseNext=true;
                    }
                }
            }
            if(!quitAll){
                bool allPutBack = true;
                //when we put an object back, we "kill" it. So if any are still alive, don't spawn the next wave.
                for(int i=0; i<flgs+1; i++){
                    if (obs.isAlive[i]) allPutBack = false;
                }
                if(allPutBack){
                    nextArr = true;
                }
                //if they fail, they have to restart
                if (PC.health <= 0 || tmLft-eventTimer.getTicks() < 0){
                    nextArr = true;
                    trAr-=4;
                }
                //fire turret bolts
                for (int i=0; i<nmTrs; i++){
                    if (eventTimer.getTicks()-trs.lastFire[i] > shotInterval){
                        if(trs.health[i] > 0){
                            if (blts.newBolt(trs.clBx[i].x, trs.clBx[i].y, boltSpeed, 128, PC, trs.angle[i], true)){
                                //play damage sound
                            }
                            trs.lastFire[i]=eventTimer.getTicks();
                        }
                    }
                }

                PC.move(dt);
                if(pcRegen){
                    PC.health+=15.f/FRAMES_PER_SECOND;   //recovers 5 health per second.
                    if(PC.health>100)PC.health=100;
                }
                blts.moveBolts(128, dt);

                ///Check collisions
                //check if player has been hit by a bolt.
                for (int i = 0; i < 128; i++){
                    if (blts.isAlive[i]){
                        if (CheckCollision(PC.smBx, blts.clBx[i])){
                            PC.health -= bltDam;
                            blts.isAlive[i] = false;
                        }
                    }
                }

                //Check to see if we collided/pickedUp an object/flag
                for (int i=0; i<flgs+1; i++){
                    if (!pickedUp){     //since we can only pick up one at a time.
                        if(CheckCollision(PC.clBx, obs.clBx[i]) && obs.isAlive[i]){
                            pickedUp = true;
                            obNm = i;
                        }
                    }
                }
                //now check to see it it's been returned.
                if(pickedUp){
                    if(CheckCollision(PC.clBx, bse)){
                        pickedUp = false;
                        obs.isAlive[obNm] = false;
                        obNm = -1;
                    }
                }
            }

            ///Render
            SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
            SDL_RenderClear(pFrame);
            //Text gets rendered first
            if(pickedUp){
                SDL_RenderCopy(pFrame, pMsg2, NULL, &msg);
            } else {
                SDL_RenderCopy(pFrame, pMsg1, NULL, &msg);
            }
            SDL_RenderCopy(pFrame, pTm, NULL, &tm);
            int dig1=(tmLft-eventTimer.getTicks())/1000/10;
            int dig2=(tmLft-eventTimer.getTicks())/1000%10;
            ScaleApplyTexture(ppDgts[dig1], pFrame, NULL, dt.xPush+55*dt.scl, 10*dt.scl, 5*dt.scl);
            ScaleApplyTexture(ppDgts[dig2], pFrame, NULL, dt.xPush+58*dt.scl, 10*dt.scl, 5*dt.scl);

            SDL_RenderCopy(pFrame, pBase, NULL, &gBse);

            //rendering PC and sword.
            PC.angle = GetPCAngle(msX-PC.clBx.x, msY-PC.clBx.y);

            SDL_Point centerPC = {PC.rBx.w/2, PC.rBx.h/2};
            SDL_RenderCopyEx(pFrame, pPCImg, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);

            //render points for help with aiming, although I no longer have any sword
            float xPos=PC.rBx.x+PC_SIZE/2*dt.scl;
            float yPos=PC.rBx.y+PC_SIZE/2*dt.scl;
            int gpSz=1.5*dt.scl;
            while(true){
                xPos+=sin(PC.angle/57.2957795)*gpSz;
                yPos-=cos(PC.angle/57.2957795)*gpSz;
                if(xPos<dt.xPush || xPos>dt.SW-dt.xPush)break;
                if(yPos<0 || yPos>dt.SH)break;
                ScaleApplyTexture(pBlk, pFrame, NULL, xPos, yPos, 0.3*dt.scl);
                gpSz*=2;
            }

            //render turrets
            for (int i=0; i<nmTrs; i++){
                SDL_Point cntTr={TURRET_SIZE*dt.scl/2, TURRET_SIZE*dt.scl/2};
                if(trs.health[i] > 0){
                    trs.angle[i] = FindAngle(PC.clBx.y-trs.clBx[i].y, PC.clBx.x-trs.clBx[i].x);
                    SDL_RenderCopyEx(pFrame, pTur, NULL, &trs.rBx[i], trs.angle[i], &cntTr, SDL_FLIP_NONE);
                }
            }

            //render bolts
            for(int i=0; i<128; i++){
                if (blts.isAlive[i]){
                    blts.angle[i] = FindAngle(blts.yVel[i], blts.xVel[i]);
                    SDL_Point cntBlt = {BOLT_SIZE*dt.scl/2, BOLT_SIZE*dt.scl/2};
                    SDL_RenderCopyEx(pFrame, pBlt, NULL, &blts.rBx[i], blts.angle[i], &cntBlt, SDL_FLIP_NONE);
                }
            }

            //render the object(s) to pick up
            for(int i=0; i<flgs+1; i++){
                if(obs.isAlive[i] && obNm!=i){
                    SDL_RenderCopy(pFrame, pObjects, NULL, &obs.rBx[i]);
                }
            }

            //render health bar.
            //can also be moved away to clean up this code
            SDL_Rect healthBar;
            SDL_Rect healthOutline;
            healthBar.x = dt.xPush+10*dt.scl;
            healthBar.y = 2*dt.scl;
            healthBar.w = 80*dt.scl;
            healthBar.h = 5*dt.scl;
            healthOutline.x = healthBar.x - 0.5*dt.scl;
            healthOutline.y = healthBar.y - 0.5*dt.scl;
            healthOutline.w = healthBar.w + 1*dt.scl;
            healthOutline.h = healthBar.h + 1*dt.scl;
            SDL_RenderCopy(pFrame, pBlk, NULL, &healthOutline);
            SDL_RenderCopy(pFrame, pWht, NULL, &healthBar);
            healthBar.w*=(PC.health/100);
            SDL_RenderCopy(pFrame, pBlk, NULL, &healthBar);

            SDL_Rect msBx; msBx.w = 1.0*dt.scl; msBx.h = 1.0*dt.scl; msBx.x = mseX; msBx.y = mseY;
            SDL_RenderCopy(pFrame, pMs, NULL, &msBx);

            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

            SDL_RenderPresent(pFrame);
        }else{
            tmLft+=FRAME_TIME;      //don't want time ticking down while paused
        }
        if (fpsTimer.getTicks() < FRAME_TIME){
            while (fpsTimer.getTicks() < FRAME_TIME){
                timeWaster = rand();
            }
        }
    }

    //Don't forget to delete all the images and fonts and sounds
    SDL_DestroyTexture(pObjects);
    SDL_DestroyTexture(pPCImg);
    SDL_DestroyTexture(pTur);
    SDL_DestroyTexture(pBlt);
    SDL_DestroyTexture(pHnt);
    SDL_DestroyTexture(pBase);
    SDL_DestroyTexture(pMs);
    SDL_DestroyTexture(pBlk);
    SDL_DestroyTexture(pWht);
    TTF_CloseFont(pFont);

    SDL_SetRelativeMouseMode(SDL_FALSE);

    return 0;

}























































