/******************************************************************************************
Each tutorial simply plays and then goes back to the main menu. There is little reason that
I have included the menu state pointer, but it may be useful in the future. Actually, it
is useful if the user quits the game completely.
Irritatingly, I put Level cpyLvl in all the characters movement. That was unfortunate, although
I can just create a level manually with the right parameters (barriersize = 0).
******************************************************************************************/

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "IC_GlobalVars.h"
#include "Helpers.h"
#include "Characters.h"
#include "Tutorial State.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cmath>
#include "Timer.h"


int PlayTutorials(gameState* pState, menuState* pMenSt, tutState tut, SDL_Renderer* pFrame, gs dt)
{
    while (*pState == TUTORIAL){
        switch(tut){
            case MOVETUT: if(TutMove(pState, pMenSt, pFrame, dt) != 0) return 1; break;
            case AVOIDGM: if(AvoidGm(pState, pMenSt, pFrame, dt) != 0) return 1; break;
            case ATTACKTUT: if(AtkTut(pState, pMenSt, pFrame, dt) !=0 ) return 1; break;
            case MOVEGM: if(GmMove(pState, pMenSt, pFrame, dt) != 0) return 1; break;
        }
    }
    return 0;
}

/***********************************************************************************************
All we need to worry about is moving the PC, picking up the objects, and put the new objects
in the right place. Also, play music and render everything correctly.
One thing I might need is text files that control the positioning of the objects, since I have
to eventually do it by hand.
***********************************************************************************************/
const float OBJECT_SIZE = 4.f;
int TutMove(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt)
{
    const float OBJ_PIX=OBJECT_SIZE*dt.scl;
    MoveTut tutFile;
    if (!LoadMoveTutorial(&tutFile, tdcDirectoryPath+"\\Files\\Tutorial Files\\MoveTut2.txt")){
        std::cout << "Failed to load the movement tutorial file\n";
        return 1;
    }

    SDL_Texture* pObjects = NULL;       //the objects we will be moving to.
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\objects.png", &pObjects, pFrame)){
        std::cout << "Error loading pickups\n";
        return 1;
    }
    SDL_Texture* pPCImg = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoPC.png", &pPCImg, pFrame)){
        std::cout << "Error loading player character image\n";
        return 1;
    }
    SDL_Texture* pBlk=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        std::cout << "Error loading black image\n";
        return 1;
    }
    SDL_Texture* pMs=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoMouse.png", &pMs, pFrame)){
        std::cout << "Error loading mouse image\n";
        return 1;
    }

    TTF_Font* pFont = NULL;
    pFont = TTF_OpenFont((tdcDirectoryPath+"\\Squarish Sans CT Regular.ttf").c_str(), 36);
    if (pFont == NULL) return 1;

    SDL_Color fontColour = {0, 0, 0};

    //used for things like "move up".
    SDL_Texture* pMessage = NULL;
    SDL_Surface* pTemp = NULL;

    //Yes I'm doing this by hand. I don't want to code another text file right now.
    std::string messages[tutFile.numWaves];
    for(int i=0; i<tutFile.numWaves; i++){
        if (i == 0)messages[i] = "Move left";
        if (i == 1)messages[i] = "Move up";
        if (i == 2)messages[i] = "Move right";
        if (i == 3)messages[i] = "Move down";
        if (i>3 && i<=7){
            messages[i] = "Move diagonally by pressing two or more keys";
        }
        if (i > 7)messages[i] = "Pick up all the objects";
    }

    SDL_Texture* pControls = NULL;
    SDL_Surface* pContTemp = NULL;
    std::string controls = "Press WASD, or the arrow keys to move";
    pContTemp = TTF_RenderText_Solid(pFont, controls.c_str(), fontColour);
    pControls = SDL_CreateTextureFromSurface(pFrame, pContTemp);
    SDL_Rect ctrls;
    ctrls.x=dt.xPush+((100*dt.scl)-pContTemp->w)/2;
    ctrls.y=30*dt.scl;
    ctrls.w=pContTemp->w;
    ctrls.h=pContTemp->h;
    SDL_RenderCopy(pFrame, pControls, NULL, &ctrls);
    SDL_FreeSurface(pContTemp);

    PlayerCharacter PC(dt);
    Bolts objects(64, dt);        //lazy repurposing

    int wvNum = -1;          //used for indexing which wave we're in.

    bool nextWave = true;      //move to the next wave if true

    Timer eventTimer;
    Timer fpsTimer;
    eventTimer.startClk();
    SDL_Event e;
    srand(time(0));     //used only for wasting time.
    double timeWaster = 1;
    int pauseGame = 0;        //in ms.
    int mseX, mseY;       //pixels
    float msX, msY;         //units
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (*pState == TUTORIAL){
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
                }
            }
            PC.handleInput(&e);
        }
        pauseGame -= FRAME_TIME;
        if (pauseGame <= 0){

            ///Set up objects
            if (!nextWave){
                for (int i=0; i<tutFile.tutWave[wvNum].numObjects; i++){
                    if (objects.isAlive[i] == true){
                        nextWave = false;
                        break;
                    } else {
                        nextWave = true;
                    }
                }
            }
            if (nextWave){
                PC.trueX = 50;
                PC.trueY = 50;
                PC.clBx.x=50;
                PC.clBx.y=50;
                wvNum++;
                if (wvNum == tutFile.numWaves){
                    *pState = MENU;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    SDL_DestroyTexture(pControls);
                    SDL_DestroyTexture(pObjects);
                    return 0;       //huge memory leak cause by this as well as not freeing the mouse
                }
                for(int j=0; j<tutFile.tutWave[wvNum].numObjects; j++){
                    objects.clBx[j].x = tutFile.tutWave[wvNum].xPos[j];
                    objects.clBx[j].y = tutFile.tutWave[wvNum].yPos[j];
                    objects.clBx[j].w = OBJECT_SIZE;
                    objects.clBx[j].h = OBJECT_SIZE;
                    objects.rBx[j].x=dt.xPush+objects.clBx[j].x*dt.scl;
                    objects.rBx[j].y=objects.clBx[j].y*dt.scl;
                    objects.rBx[j].w=OBJ_PIX;
                    objects.rBx[j].h=OBJ_PIX;
                    objects.isAlive[j] = true;
                }
                nextWave = false;
                pauseGame = 100;
            }

            PC.move(0, dt);

            ///Check if player has collided with an object
            for (int i=0; i<tutFile.tutWave[wvNum].numObjects; i++){
                if (CheckCollision(PC.clBx, objects.clBx[i])){
                    objects.isAlive[i] = false;
                }
            }

            ///Render
            SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
            SDL_RenderClear(pFrame);

            //rendering PC, ugly
            static bool flipPC = true;
            double rotAngle = 0;
            //only get a new angle if we've actually moved.
            PC.angle = GetPCAngle(msX-PC.clBx.x, msY-PC.clBx.y);

            SDL_Point centerPC = {PC.rBx.w/2, PC.rBx.h/2};
            SDL_RenderCopyEx(pFrame, pPCImg, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);

            //rendering objects.
            for (int i = 0; i < tutFile.tutWave[wvNum].numObjects; i++){
                    if (objects.isAlive[i]){
                        ScaleApplyTexture(pObjects, pFrame, NULL, objects.rBx[i].x, objects.rBx[i].y, OBJECT_SIZE*dt.scl);
                    }
            }

            SDL_RenderCopy(pFrame, pControls, NULL, &ctrls);

            pTemp = TTF_RenderText_Solid(pFont, messages[wvNum].c_str(), fontColour);
            pMessage = SDL_CreateTextureFromSurface(pFrame, pTemp);
            SDL_Rect msg;
            msg.x=dt.xPush+((100*dt.scl)-pTemp->w)/2;
            msg.y=50*dt.scl;
            msg.w=pTemp->w;
            msg.h=pTemp->h;
            SDL_RenderCopy(pFrame, pMessage, NULL, &msg);
            SDL_FreeSurface(pTemp);
            SDL_DestroyTexture(pMessage);

            ScaleApplyTexture(pMs, pFrame, NULL, mseX, mseY, 1.5*dt.scl);

            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

            SDL_RenderPresent(pFrame);
        }
        if (fpsTimer.getTicks() < FRAME_TIME){
            while (fpsTimer.getTicks() < FRAME_TIME){
                timeWaster = rand();
            }
        }
    }
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_DestroyTexture(pControls);
    SDL_DestroyTexture(pObjects);
    return 0;
}

int GmMove(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt)
{
    SDL_Texture* pObj = NULL;       //what we'll be picking up.
    SDL_Texture* pPC = NULL;
    SDL_Texture* pBarOut = NULL;
    SDL_Texture* pBarBk = NULL;
    SDL_Texture* pBar = NULL;

    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\objects.png", &pObj, pFrame)){
        printf("Failed to load pickup objects\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\tempPC.bmp", &pPC, pFrame)){
        printf("Failed to load player character image\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\healthOut.bmp", &pBarOut, pFrame)){
        printf("Failed to load player character image\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\healthBack.bmp", &pBarBk, pFrame)){
        printf("Failed to load player character image\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\healthBar.bmp", &pBar, pFrame)){
        printf("Failed to load player character image\n");
        return -1;
    }

    PlayerCharacter PC(dt);
    Bolts objs(32, dt);
    int numObs = 10;
    for(int i=0; i<numObs; i++){
        objs.clBx[i].w = 20;
        objs.clBx[i].h = 20;
    }
    double drn = 30/FRAMES_PER_SECOND;     //health drain per frame.

    Timer gmTm;
    Timer fpsTm;
    SDL_Event e;
    gmTm.startClk();
    int mouseX = 0;
    int mouseY = 0;
    while(*pState == TUTORIAL){
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                *pMenSt = OUTRO;
                *pState = MENU;
            } else if(e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_ESCAPE){
                    *pState = MENU;
                }
            }
            SDL_GetMouseState(&mouseX, &mouseY);
            PC.handleInput(&e);
        }
        PC.move(0, dt);

        fpsTm.startClk();
        PC.health -= drn;

        //if the player has picked up an object.
        for(int i=0; i<numObs; i++){
            if(CheckCollision(PC.clBx, objs.clBx[i])){
                objs.isAlive[i] = false;
                PC.health += 10;
                if(PC.health > 100) PC.health = 100;
            }
        }
        //if we don't have enough objects (first frame or the user just took one), activate another.
        for(int i=0; i<numObs; i++){
            if(objs.isAlive[i] == false){
                objs.clBx[i].x = rand()%((100-20)/2)+100/4;
                objs.clBx[i].y = rand()%((100-20)/2)+100/4;
                objs.isAlive[i] = true;
            }
        }

        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);

        for(int i=0; i<numObs; i++){
            ScaleApplyTexture(pObj, pFrame, NULL, objs.clBx[i].x, objs.clBx[i].y, 20);
        }

        static bool flipPC = true;
        double rotAngle = 0;
        //only get a new angle if we've actually moved.
        PC.angle = GetPCAngle(mouseX-PC.clBx.x, mouseY-PC.clBx.y);
        SDL_Point centerPC = {PC.rBx.w/2, PC.rBx.h/2};
        SDL_RenderCopyEx(pFrame, pPC, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);

        //render "health"
        SDL_Rect healthBar;
        SDL_Rect healthOutline;
        healthBar.x = (100-200)/2;     //bar is 200 pixels
        healthBar.y = 40;
        healthBar.w = 200;
        healthBar.h = 25;
        healthOutline.x = healthBar.x - 5;
        healthOutline.y = healthBar.y - 5;
        healthOutline.w = healthBar.w + 10;
        healthOutline.h = healthBar.h + 10;

        SDL_RenderCopy(pFrame, pBarOut, NULL, &healthOutline);
        SDL_RenderCopy(pFrame, pBarBk, NULL, &healthBar);
        healthBar.w = 200*(PC.health/100);
        SDL_RenderCopy(pFrame, pBar, NULL, &healthBar);

        //render instructions - maybe?

        SDL_RenderPresent(pFrame);

        if(fpsTm.getTicks() < FRAME_TIME){
            while(fpsTm.getTicks() < FRAME_TIME){
                int temp = rand()*rand()*rand();
            }
        }

    }

    SDL_DestroyTexture(pPC);
    SDL_DestroyTexture(pObj);
    SDL_DestroyTexture(pBarOut);
    SDL_DestroyTexture(pBarBk);
    SDL_DestroyTexture(pBar);

    return 0;
}

int AtkTut(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt)
{
    MoveTut tutFile;
    if (!LoadMoveTutorial(&tutFile, tdcDirectoryPath+"\\Files\\Tutorial Files\\MoveTut2.txt")){
        std::cout << "Failed to load the movement tutorial file\n";
        return 1;
    }

    Level lev;
    lev.barrierSize = 0;
    lev.numBolts = 128;

    SDL_Texture* pObjects = NULL;       //the objects we will be moving to.
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\objects.png", &pObjects, pFrame)){
        std::cout << "Error loading pickups\n";
        return 1;
    }
    SDL_Texture* pPCImg = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoPC.png", &pPCImg, pFrame)){
        std::cout << "Error loading player character image\n";
        return 1;
    }
    SDL_Texture* pSwrd = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sword.png", &pSwrd, pFrame)){
        std::cout << "Error loading sword image\n";
        return 1;
    }
    SDL_Texture* pBlk=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        std::cout << "Error loading black image\n";
        return 1;
    }

    TTF_Font* pFont = NULL;
    pFont = TTF_OpenFont((tdcDirectoryPath+"\\Squarish Sans CT Regular.ttf").c_str(), 36);
    if (pFont == NULL) return 1;

    SDL_Color fontColour = {0, 0, 0};     //used for health display

    //used for things like "move up".
    SDL_Texture* pMessage = NULL;
    SDL_Surface* pTemp = NULL;

    //Yes I'm doing this by hand. I don't want to code another text file right now.
    std::string messages[tutFile.numWaves];
    for(int i=0; i<tutFile.numWaves; i++){
        if (i == 0)messages[i] = "Move left";
        if (i == 1)messages[i] = "Move up";
        if (i == 2)messages[i] = "Move right";
        if (i == 3)messages[i] = "Move down";
        if (i>3 && i<=7){
            messages[i] = "Move diagonally by pressing two or more keys";
        }
        if (i > 7)messages[i] = "Destroy all the objects";
    }

    SDL_Texture* pControls = NULL;
    SDL_Surface* pContTemp = NULL;
    std::string controls = "Press WASD, or the arrow keys to move";
    pContTemp = TTF_RenderText_Solid(pFont, controls.c_str(), fontColour);
    pControls = SDL_CreateTextureFromSurface(pFrame, pContTemp);
    SDL_Rect ctrls;
    ctrls.x=dt.xPush+((100*dt.scl)-pContTemp->w)/2;
    ctrls.y=40*dt.scl;
    ctrls.w=pContTemp->w;
    ctrls.h=pContTemp->h;
    SDL_RenderCopy(pFrame, pControls, NULL, &ctrls);
    SDL_FreeSurface(pContTemp);

    SDL_Texture* pAtkCtrl = NULL;
    SDL_Surface* pContTempAtk = NULL;
    std::string atkCtrls = "Press Left Mouse to attack";
    pContTempAtk = TTF_RenderText_Solid(pFont, atkCtrls.c_str(), fontColour);
    pAtkCtrl = SDL_CreateTextureFromSurface(pFrame, pContTempAtk);
    SDL_Rect atkMsg;
    atkMsg.x=dt.xPush+((100*dt.scl)-pContTempAtk->w)/2;
    atkMsg.y=45*dt.scl;
    atkMsg.w=pContTempAtk->w;
    atkMsg.h=pContTempAtk->h;
    SDL_RenderCopy(pFrame, pAtkCtrl, NULL, &atkMsg);
    SDL_FreeSurface(pContTempAtk);

    PlayerCharacter PC(dt);
    Bolts objects(32, dt);        //lazy repurposing

    int wvNum = -1;             //used for indexing which wave we're in.

    bool nextWave = true;       //move to the next wave if true

    int mseX, mseY;
    float msX, msY;

    Timer eventTimer;
    Timer fpsTimer;
    eventTimer.startClk();
    SDL_Event e;
    srand(time(0));     //used only for wasting time.
    double timeWaster = 1;
    int pauseGame = 0;        //in ms.
    while (*pState == TUTORIAL){
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
                }
            }
            PC.handleInput(&e);
        }
        pauseGame -= FRAME_TIME;
        if (pauseGame <= 0){

            ///Set up objects
            if (!nextWave){
                for (int i=0; i<tutFile.tutWave[wvNum].numObjects; i++){
                    if (objects.isAlive[i] == true){
                        nextWave = false;
                        break;
                    } else {
                        nextWave = true;
                    }
                }
            }
            if (nextWave){
                PC.trueX=50;
                PC.trueY=50;
                wvNum++;
                std::cout << wvNum << "\t";
                if (wvNum == tutFile.numWaves){
                    *pState = MENU;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    SDL_DestroyTexture(pControls);
                    SDL_DestroyTexture(pObjects);
                    return 0;
                }
                for(int j=0; j<tutFile.tutWave[wvNum].numObjects; j++){
                    objects.clBx[j].x = tutFile.tutWave[wvNum].xPos[j];
                    objects.clBx[j].y = tutFile.tutWave[wvNum].yPos[j];
                    objects.clBx[j].w = OBJECT_SIZE;
                    objects.clBx[j].h = OBJECT_SIZE;
                    objects.rBx[j].x=dt.xPush+objects.clBx[j].x*dt.scl;
                    objects.rBx[j].y=objects.clBx[j].y*dt.scl;
                    objects.rBx[j].w=OBJECT_SIZE*dt.scl;
                    objects.rBx[j].h=OBJECT_SIZE*dt.scl;
                    objects.isAlive[j] = true;
                }
                nextWave = false;
                pauseGame = 100;
            }

            PC.move(0, dt);

            ///Check if the sword has hit an object
            for (int i=0; i<tutFile.tutWave[wvNum].numObjects; i++){
                if(PC.swing > 0){
                    if (CheckCollision(PC.atk, objects.clBx[i])){
                        objects.isAlive[i] = false;
                    }
                }
            }

            ///Render
            SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
            SDL_RenderClear(pFrame);

            //rendering PC, ugly
            PC.angle = GetPCAngle(msX-PC.clBx.x, msY-PC.clBx.y);

            SDL_Point centerPC = {PC.rBx.w/2, PC.rBx.h/2};
            SDL_Point centSwrd = {PC.rAtk.w/2, PC.rAtk.h/2};
            SDL_RenderCopyEx(pFrame, pPCImg, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);
            if(PC.swing > 0){
                SDL_RenderCopyEx(pFrame, pSwrd, NULL, &PC.rAtk, PC.angle, &centSwrd, SDL_FLIP_NONE);
                PC.swing -= FRAME_TIME;
            }
            //render points for help with aiming
            float xPos=PC.rBx.x+PC_SIZE/2*dt.scl;
            float yPos=PC.rBx.y+PC_SIZE/2*dt.scl;
            int gpSz=1.5*dt.scl;
            while(true){
                xPos+=sin(PC.angle/57.2957795)*gpSz;
                yPos-=cos(PC.angle/57.2957795)*gpSz;
                if(xPos<dt.xPush || xPos>dt.SW-dt.xPush)break;
                if(yPos<0 || yPos>dt.SH)break;
                ScaleApplyTexture(pBlk, pFrame, NULL, xPos, yPos, 0.5*dt.scl);
                gpSz*=2;
            }

            //rendering objects.
            for (int i = 0; i < tutFile.tutWave[wvNum].numObjects; i++){
                    if (objects.isAlive[i]){
                        ScaleApplyTexture(pObjects, pFrame, NULL, objects.rBx[i].x, objects.rBx[i].y, OBJECT_SIZE*dt.scl);
                    }
            }

            SDL_RenderCopy(pFrame, pControls, NULL, &ctrls);
            SDL_RenderCopy(pFrame, pAtkCtrl, NULL, &atkMsg);

            pTemp = TTF_RenderText_Solid(pFont, messages[wvNum].c_str(), fontColour);
            pMessage = SDL_CreateTextureFromSurface(pFrame, pTemp);
            SDL_Rect msg;
            msg.x=dt.xPush+((100*dt.scl)-pTemp->w)/2;
            msg.y=50*dt.scl;
            msg.w=pTemp->w;
            msg.h=pTemp->h;
            SDL_RenderCopy(pFrame, pMessage, NULL, &msg);
            SDL_FreeSurface(pTemp);
            SDL_DestroyTexture(pMessage);

            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);
            SDL_RenderPresent(pFrame);

        }
        if (fpsTimer.getTicks() < FRAME_TIME){
            while (fpsTimer.getTicks() < FRAME_TIME){
                timeWaster = rand();
            }
        }
    }
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_DestroyTexture(pControls);
    SDL_DestroyTexture(pObjects);
    return 0;
}

int AvoidGm(gameState* pState, menuState* pMenSt, SDL_Renderer* pFrame, gs dt)
{
    const int TURRET_SHOT_INTERVAL = 250;
    const double boltSpeed = 22.5;

    SDL_Texture* pObjects = NULL;       //the objects we will be moving to.
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\objects.png", &pObjects, pFrame)){
        std::cout << "Error loading pickups\n";
        return 1;
    }
    SDL_Texture* pPCImg = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoPC.png", &pPCImg, pFrame)){
        std::cout << "Error loading player character image\n";
        return 1;
    }
    SDL_Texture* pSwrd = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sword.png", &pSwrd, pFrame)){
        std::cout << "Error loading sword image\n";
        return 1;
    }
    SDL_Texture* pTur = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoTurret.png", &pTur, pFrame)){
        std::cout << "Error loading turret image\n";
        return 1;
    }
    SDL_Texture* pBlt = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBolt.png", &pBlt, pFrame)){
        std::cout << "Error loading projectile image\n";
        return 1;
    }
    SDL_Texture* pHnt = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoHunter.png", &pHnt, pFrame)){
        std::cout << "Error loading hunter image\n";
        return 1;
    }
    SDL_Texture* pBase = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\base.png", &pBase, pFrame)){
        std::cout << "Error loading return base\n";
        return 1;
    }
    SDL_Texture* pMs = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Tut Images\\mouse.png", &pMs, pFrame)){
        std::cout << "Error loading mouse image\n";
        return 1;
    }
    SDL_Texture* pBlk=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        std::cout << "Error loading black image\n";
        return 1;
    }
    SDL_Texture* pWht=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityWhite.png", &pWht, pFrame)){
        std::cout << "Error loading white image\n";
        return 1;
    }

    TTF_Font* pFont = NULL;
    pFont = TTF_OpenFont((tdcDirectoryPath+"\\Squarish Sans CT Regular.ttf").c_str(), 36);
    if (pFont == NULL) return 1;

    SDL_Color fontColour = {0, 0, 0};     //used for health display

    AvdGm game;
    if(!LoadAvoidGame(&game, tdcDirectoryPath+"\\Files\\Tutorial Files\\AvdGmFl.txt")){
        printf("Failed to load game file\n");
        return -1;
    }

    const int boltDam = 10;

    PlayerCharacter PC(dt);
    Bolts objects(8, dt);         //can handle max of x objects
    Bolts blts(128, dt);              //means can handle max of x turret bolts
    Turrets turs(8, dt);                    //can handle max of x turrets
    Hunters hnts(8, dt);                    //can handle max of x hunters
    tdcRct bse;
    bse.h = 10;
    bse.w = 80;
    bse.x = 10;
    bse.y = 90;
    SDL_Rect gBse;      //graphical component
    gBse.h=bse.h*dt.scl;
    gBse.w=bse.w*dt.scl;
    gBse.x=dt.xPush+bse.x*dt.scl;
    gBse.y=bse.y*dt.scl;

    int wvNum = -1;          //used for indexing which wave we're in.
    bool nextWave = true;      //move to the next wave if true
    bool quitAll = false;

    Timer eventTimer;
    Timer fpsTimer;
    eventTimer.startClk();
    SDL_Event e;
    srand(time(0));     //used only for wasting time.
    double timeWaster = 1;
    int pauseGame = 0;        //in ms.
    bool pickedUp = false;
    int obNm = -1;
    int tmLft = 0;
    int mseX, mseY;
    float msX, msY;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (*pState == TUTORIAL){
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
                }
            }
            PC.handleInput(&e);
        }
        pauseGame -= FRAME_TIME;
        if (pauseGame <= 0){
            if (!nextWave){     //we're not currently setting up, but maybe we should be
                bool allPutBack = true;
                //when we put an object back, we "kill" it. So if any are still alive, don't spawn the next wave.
                for(int i=0; i<game.wvs[wvNum].nmObs; i++){
                    if (objects.isAlive[i]) allPutBack = false;
                }
                if(allPutBack){
                    nextWave = true;
                }
                //if they fail, they have to restart
                if (PC.health <= 0 || tmLft-eventTimer.getTicks()/1000 < 0){
                    if(PC.health <=0){
                        SDL_Delay(400);
                    }
                    nextWave = true;
                    wvNum--;
                }
            }
            if(nextWave){
                PC.trueX = 50;
                PC.trueY = 88;
                wvNum++;
                if(wvNum == game.nmWvs){
                    *pState = MENU;
                    quitAll = true;
                }
                if(!quitAll){
                    for (int i=0; i<game.wvs[wvNum].nmTrs; i++){
                        turs.xPos[i] = game.wvs[wvNum].turX[i];
                        turs.yPos[i] = game.wvs[wvNum].turY[i];
                        turs.clBx[i].x = turs.xPos[i];
                        turs.clBx[i].y = turs.yPos[i];
                        turs.rBx[i].x=dt.xPush+turs.xPos[i]*dt.scl;
                        turs.rBx[i].y=turs.yPos[i]*dt.scl;
                        turs.isAlive[i] = true;
                        turs.health[i] = 100;
                        turs.lastFire[i] = eventTimer.getTicks() + i*FRAME_TIME*8;
                    }
                    pauseGame = 100;
                    for(int i=0; i<game.wvs[wvNum].numHnts; i++){
                        hnts.spawned[i] = false;
                        hnts.prevSpawn = -1;
                        hnts.lastSpawn = -2000;
                        hnts.xPos[i] = 50;
                        hnts.yPos[i] = 50;
                        hnts.hunters[i].x = 0;
                        hnts.hunters[i].y = 0;
                        hnts.hlth[i] = 100;
                    }
                    for(int i=0; i<game.wvs[wvNum].nmObs; i++){
                        objects.xPos[i] = game.wvs[wvNum].objX[i];
                        objects.yPos[i] = game.wvs[wvNum].objY[i];
                        objects.clBx[i].x = objects.xPos[i];
                        objects.clBx[i].y = objects.yPos[i];
                        objects.clBx[i].w = OBJECT_SIZE;
                        objects.clBx[i].h = OBJECT_SIZE;
                        objects.rBx[i].x=dt.xPush+objects.xPos[i]*dt.scl;
                        objects.rBx[i].y=objects.yPos[i]*dt.scl;
                        objects.rBx[i].h=OBJECT_SIZE*dt.scl;
                        objects.rBx[i].w=OBJECT_SIZE*dt.scl;
                        objects.isAlive[i] = true;
                    }
                    for(int i=0; i<128; i++){
                        blts.isAlive[i] = false;
                    }
                    PC.health = 100;
                    tmLft = game.wvs[wvNum].time + eventTimer.getTicks()/1000;
                    pickedUp = false;
                    obNm = -1;
                    nextWave = false;
                }
            }
            if(!quitAll){
                //fire turret bolts
                for (int i=0; i<game.wvs[wvNum].nmTrs; i++){
                    if (eventTimer.getTicks()-turs.lastFire[i] > TURRET_SHOT_INTERVAL){
                        if(turs.health[i] > 0){
                            if (blts.newBolt(turs.xPos[i], turs.yPos[i], boltSpeed, 128, PC, 0, true)){
                            }
                            //turs.lastFire[i] += TURRET_SHOT_INTERVAL;
                            turs.lastFire[i]=eventTimer.getTicks();
                        }
                    }
                }
                hnts.SpawnHunters(game.wvs[wvNum].numHnts, eventTimer.getTicks());
                hnts.moveHunters(PC, game.wvs[wvNum].numHnts, 0, dt);
                for (int i = 0; i < game.wvs[wvNum].numHnts-1; i++){
                    for (int j = i+1; j < game.wvs[wvNum].numHnts; j++){      //we go through the remaining hunters
                        if(hnts.hlth[i]>0 && hnts.spawned[i] && hnts.hlth[j]>0 && hnts.spawned[j]){
                            if (CheckCollision(hnts.hunters[i], hnts.hunters[j])){
                                if (hnts.collidedTime[i] < 0 || hnts.collidedTime[j] < 0){
                                    hnts.handleCollision(i, j);
                                }
                            }
                        }
                    }
                }

                PC.move(0, dt);
                blts.moveBolts(128, dt);

                ///Check collisions
                //check if player has been hit by a bolt.
                for (int i = 0; i < 128; i++){
                    if (blts.isAlive[i]){
                        if (CheckCollision(PC.clBx, blts.clBx[i])){
                            PC.health -= boltDam;
                            blts.isAlive[i] = false;
                        }
                    }
                }
                //check against hits by hunters
                for (int i = 0; i < game.wvs[wvNum].numHnts; i++){
                    hnts.damageCooldown[i] -= FRAME_TIME;
                    if (hnts.hlth[i] > 0){
                        if (CheckCollision(PC.clBx, hnts.hunters[i])){
                            if (hnts.damageCooldown[i] < 0){
                                PC.health -= 7;
                                hnts.damageCooldown[i] = 200;
                            }
                            hnts.xVel[i] *= -1;
                            hnts.yVel[i] *= -1;
                        }
                    }
                }

                //Check if the sword has hit a turret
                for (int i=0; i<game.wvs[wvNum].nmTrs; i++){
                    if(PC.swing > 0){
                        if (CheckCollision(PC.atk, turs.clBx[i])){
                            turs.health[i] = -1;
                        }
                    }
                }

                //Check if sword has hit hunters
                for (int i=0; i<game.wvs[wvNum].numHnts; i++){
                    if(PC.swing > 0){
                        if(hnts.hlth[i] > 0){
                            if (CheckCollision(PC.atk, hnts.hunters[i])){
                                hnts.hlth[i] = -1;
                            }
                        }
                    }
                }
                for (int i=0; i<game.wvs[wvNum].nmObs; i++){
                    if (!pickedUp){     //since we can only pick up one at a time.
                        if(CheckCollision(PC.clBx, objects.clBx[i]) && objects.isAlive[i]){
                            pickedUp = true;
                            obNm = i;
                        }
                    }
                }
                //now check to see it it's been returned.
                if(pickedUp){
                    if(CheckCollision(PC.clBx, bse)){
                        pickedUp = false;
                        objects.isAlive[obNm] = false;
                        obNm = -1;
                    }
                }

                //if(PC.health <= 0) *pState = MENU;
            }

            ///Render
            SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
            SDL_RenderClear(pFrame);
            SDL_RenderCopy(pFrame, pBase, NULL, &gBse);

            //rendering PC and sword.
            PC.angle = GetPCAngle(msX-PC.clBx.x, msY-PC.clBx.y);

            SDL_Point centerPC = {PC.rBx.w/2, PC.rBx.h/2};
            SDL_Point centSwrd = {PC.rAtk.w/2, PC.rAtk.h/2};
            SDL_RenderCopyEx(pFrame, pPCImg, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);
            if(PC.swing > 0){
                SDL_RenderCopyEx(pFrame, pSwrd, NULL, &PC.rAtk, PC.angle, &centSwrd, SDL_FLIP_NONE);
                PC.swing -= FRAME_TIME;
            }
            //render points for help with aiming
            float xPos=PC.rBx.x+PC_SIZE/2*dt.scl;
            float yPos=PC.rBx.y+PC_SIZE/2*dt.scl;
            int gpSz=1.5*dt.scl;
            while(true){
                xPos+=sin(PC.angle/57.2957795)*gpSz;
                yPos-=cos(PC.angle/57.2957795)*gpSz;
                if(xPos<dt.xPush || xPos>dt.SW-dt.xPush)break;
                if(yPos<0 || yPos>dt.SH)break;
                ScaleApplyTexture(pBlk, pFrame, NULL, xPos, yPos, 0.5*dt.scl);
                gpSz*=2;
            }

            //render turrets
            for (int i=0; i<game.wvs[wvNum].nmTrs; i++){
                if(turs.health[i] > 0){
                    turs.angle[i] = FindAngle(PC.clBx.y-turs.yPos[i], PC.clBx.x-turs.xPos[i]);
                    SDL_Point cntTr = {TURRET_SIZE*dt.scl/2, TURRET_SIZE*dt.scl/2};
                    SDL_RenderCopyEx(pFrame, pTur, NULL, &turs.rBx[i], turs.angle[i], &cntTr, SDL_FLIP_NONE);
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

            //render hunters
            for (int i=0; i<game.wvs[wvNum].numHnts; i++){
                for (int i = 0; i < game.wvs[wvNum].numHnts; i++){
                    if (hnts.hlth[i]>0 && hnts.spawned[i]){
                        SDL_Point cntHnt = {HUNTER_SIZE*dt.scl/2, HUNTER_SIZE*dt.scl/2};
                        SDL_RenderCopyEx(pFrame, pHnt, NULL, &hnts.rBx[i], hnts.angle[i], &cntHnt, SDL_FLIP_NONE);
                    }
                }
            }

            //render the object(s) to pick up
            for(int i=0; i<game.wvs[wvNum].nmObs; i++){
                if(objects.isAlive[i] && obNm!=i){
                    SDL_RenderCopy(pFrame, pObjects, NULL, &objects.rBx[i]);
                }
            }

            //render health bar.
            SDL_Rect healthBar;
            SDL_Rect healthOutline;
            healthBar.x = dt.xPush+40*dt.scl;
            healthBar.y = 2*dt.scl;
            healthBar.w = 20*dt.scl;
            healthBar.h = healthBar.w/3;
            healthOutline.x = healthBar.x - 0.5*dt.scl;
            healthOutline.y = healthBar.y - 0.5*dt.scl;
            healthOutline.w = healthBar.w + 1*dt.scl;
            healthOutline.h = healthBar.h + 1*dt.scl;
            SDL_RenderCopy(pFrame, pBlk, NULL, &healthOutline);
            SDL_RenderCopy(pFrame, pWht, NULL, &healthBar);
            healthBar.w*=(PC.health/100);
            SDL_RenderCopy(pFrame, pBlk, NULL, &healthBar);

            std::string hack;
            if(pickedUp){
                hack = "Return the object to the base.";
            } else {
                hack = "Pick up the Object.";
            }
            SDL_Texture* pMessage = NULL;
            SDL_Surface* pTemp;
            pTemp = TTF_RenderText_Solid(pFont, hack.c_str(), fontColour);
            pMessage = SDL_CreateTextureFromSurface(pFrame, pTemp);
            SDL_Rect hk;
            hk.x=dt.xPush+(100*dt.scl-pTemp->w)/2;
            hk.y=40*dt.scl;
            hk.w=pTemp->w;
            hk.h=pTemp->h;
            SDL_RenderCopy(pFrame, pMessage, NULL, &hk);
            SDL_FreeSurface(pTemp);
            SDL_DestroyTexture(pMessage);

            SDL_Texture* pTimeText = NULL;
            SDL_Surface* pTime;
            std::stringstream time;
            time << "Time Left: " << tmLft-eventTimer.getTicks()/1000;
            pTime = TTF_RenderText_Solid(pFont, time.str().c_str(), fontColour);
            pTimeText = SDL_CreateTextureFromSurface(pFrame, pTime);
            SDL_Rect tm;
            tm.x=dt.xPush+(100*dt.scl-pTime->w)/2;
            tm.y=10*dt.scl;
            tm.w=pTime->w;
            tm.h=pTime->h;
            SDL_RenderCopy(pFrame, pTimeText, NULL, &tm);
            SDL_FreeSurface(pTime);
            SDL_DestroyTexture(pTimeText);

            SDL_Rect msBx;
            msBx.w = 1.0*dt.scl;
            msBx.h = 1.0*dt.scl;
            msBx.x = mseX;
            msBx.y = mseY;
            SDL_RenderCopy(pFrame, pMs, NULL, &msBx);

            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
            SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

            SDL_RenderPresent(pFrame);
        }
        if(pauseGame<=0){
            if (fpsTimer.getTicks() < FRAME_TIME){
                while (fpsTimer.getTicks() < FRAME_TIME){
                    timeWaster = rand();
                }
            }
        }
    }

    //Don't forget to delete all the images and fonts and sounds
    SDL_DestroyTexture(pObjects);
    SDL_DestroyTexture(pPCImg);
    SDL_DestroyTexture(pSwrd);
    SDL_DestroyTexture(pTur);
    SDL_DestroyTexture(pBlt);
    SDL_DestroyTexture(pHnt);
    SDL_DestroyTexture(pBase);
    SDL_DestroyTexture(pMs);
    TTF_CloseFont(pFont);

    SDL_SetRelativeMouseMode(SDL_FALSE);

    return 0;
}

bool LoadMoveTutorial(MoveTut* pTut, std::string fileName)
{
    std::ifstream inTut(fileName.c_str());
    if (!inTut.is_open()){
        std::cout << "Could not open move tutorial file.\n";
        return false;
    }

    inTut.ignore(256, ' ');
    inTut >> pTut->title;
    inTut.ignore(256, ' ');
    inTut >> pTut->numWaves;
    for (int i = 0; i < pTut->numWaves; i++){
        inTut.ignore(256, ' ');
        inTut >> pTut->tutWave[i].numObjects;
        for (int j = 0; j < pTut->tutWave[i].numObjects; j++){
            inTut.ignore(256, ' ');
            inTut >> pTut->tutWave[i].xPos[j];
            inTut.ignore(256, ',');
            inTut >> pTut->tutWave[i].yPos[j];
        }
    }
    return true;
}

int WriteMoveTut(MoveTut tut, std::string fileName)
{
    std::ofstream fileWriter(fileName.c_str());
    if (!fileWriter.is_open()){
        printf("Could not open file writer. Probably bad pathing.\n");
        return -1;
    }

    fileWriter << "Title:" << ' ' << tut.title << '\n';
    fileWriter << "Waves:" << ' ' << tut.numWaves;
    for (int i = 0; i < tut.numWaves; i++){
        fileWriter << '\n';
        fileWriter << "---------------------\n";
        fileWriter << "Number_of_Objects:" << ' ' << tut.tutWave[i].numObjects << '\n';
        fileWriter << "Pos:";
        for (int j = 0; j < tut.tutWave[i].numObjects; j++){
            fileWriter << ' ' << tut.tutWave[i].xPos[j] << ',' << tut.tutWave[i].yPos[j];
        }
    }

    fileWriter.close();
    return 0;
}

bool LoadAvoidGame(AvdGm* pGm, std::string flNm)
{
    std::ifstream inGm(flNm.c_str());
    if (!inGm.is_open()){
        std::cout << "Could not open move tutorial file.\n";
        return false;
    }

    inGm.ignore(256, ' ');
    inGm >> pGm->title;
    inGm.ignore(256, ' ');
    inGm >> pGm->nmWvs;
    for (int i = 0; i < pGm->nmWvs; i++){
        inGm.ignore(256, ' ');
        inGm >> pGm->wvs[i].nmObs;
        for (int j = 0; j < pGm->wvs[i].nmObs; j++){
            inGm.ignore(256, ' ');
            inGm >> pGm->wvs[i].objX[j];
            inGm.ignore(256, ',');
            inGm >> pGm->wvs[i].objY[j];
        }
        inGm.ignore(256, ' ');
        inGm >> pGm->wvs[i].nmTrs;
        for (int j=0; j<pGm->wvs[i].nmTrs; j++){
            inGm.ignore(256, ' ');
            inGm >> pGm->wvs[i].turX[j];
            inGm.ignore(256, ',');
            inGm >> pGm->wvs[i].turY[j];
        }
        inGm.ignore(256, ' ');
        inGm >> pGm->wvs[i].numHnts;
        inGm.ignore(256, ' ');
        inGm >> pGm->wvs[i].time;
    }
    return true;
}

bool WriteAvoidGame(AvdGm game, std::string flNm)
{
    std::ofstream flWrt(flNm.c_str());
    if (!flWrt.is_open()){
        printf("Could not open file writer. Probably bad pathing.\n");
        return false;
    }

    flWrt << "Title:" << ' ' << game.title << '\n';
    flWrt << "Waves:" << ' ' << game.nmWvs;
    for (int i = 0; i < game.nmWvs; i++){
        flWrt << '\n';
        flWrt << "---------------------\n";
        flWrt << "Number_of_Pickups:" << ' ' << game.wvs[i].nmObs << '\n';
        flWrt << "Pickup_Pos:";
        for (int j = 0; j < game.wvs[i].nmObs; j++){
            flWrt << ' ' << game.wvs[i].objX[j] << ',' << game.wvs[i].objY[j];
        }
        flWrt << '\n';
        flWrt << "Number_of_Turrets:" << ' ' << game.wvs[i].nmTrs << '\n';
        flWrt << "Turret_Pos:";
        for(int j=0; j<game.wvs[i].nmTrs; j++){
            flWrt << ' ' << game.wvs[i].turX[j] << ',' << game.wvs[i].turY[j];
        }
        flWrt << '\n';
        flWrt << "Number_of_Hunters:" << ' ' << game.wvs[i].numHnts;
        flWrt << '\n';
        flWrt << "Time:" << ' ' << game.wvs[i].time;
    }
    flWrt << "\n---------------------";      //just cause I like it to look good

    flWrt.close();
    return true;
}

























