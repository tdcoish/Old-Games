#include "Games.h"
#include "Events.h"
#include "NPC.h"
#include "GLOBALS.h"
#include "SDL_TTF.h"
#include <ctime>
#include <cmath>
#include <cstdio>

int RunGames()
{
    if(!(GM_ST&MOVE_TUT||GM_ST&ATK_TUT||GM_ST&BOTH_TUT||GM_ST&ENEMY_TUT)) return -1;

    //Loads character images, gives default values.
    AllChars chrs;
    InitChars(&chrs);


    //Load Mouse
    GmMouse ms;
    SetUpMouse(&ms);

    //Load other graphics
    TextureArray txts;
    LoadGameGFX(&txts);


    Instructions instrs;
    if(GM_ST&MOVE_TUT){
        SetGameMessages(&instrs);
    }else if (GM_ST&ATK_TUT){
        SetAtkMessages(&instrs);
    }else if(GM_ST&BOTH_TUT){
        SetTutMessages(&instrs);
    }

    SDL_Texture* pClk=NULL;
    if(!LoadTexture(bsPth+"\\Img\\Gameplay\\Clock.png", &pClk)) return -1;
    tdcRct clkRct;clkRct.x=5;clkRct.y=15;clkRct.w=5;clkRct.h=5;
    tdcRct clkBr;clkBr.x=5;clkBr.y=25;clkBr.w=5;clkBr.h=50;

    tdcRct bar1;
    bar1.x=25;bar1.y=5;bar1.w=50;bar1.h=5;
    tdcRct bar2=bar1;bar2.y=15;
    tdcRct im1=bar1;im1.x-=10;im1.w=5;
    tdcRct im2=im1;im2.y=bar2.y;

    tdcRct hlBr;
    hlBr.x=25; hlBr.w=50; hlBr.y=2.5; hlBr.h=5;

    tdcRct enBar;        //used for time left for each enemy EnemyTutorial
    enBar.x=2.5; enBar.y=25; enBar.w=5; enBar.h=50;

    //Load sounds
    //LoadGameSFX();



    //Set up timers and event queuing, pause game stuff
    Timers clocks;
    clocks.events.startClk();

    SDL_Event e;
    KeyPress keysPressed;
    SetUpKeyStore(&keysPressed);
    GmMouse mouse;
    SetUpMouse(&mouse);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    //need a set up characters function, uses global state
    SetUpCharacters(chrs);

    bool reset=true;

    //have a while loop, condition: state is GAME
    while(PG_ST&GAME)
    {
        clocks.fps.startClk();

        //re-set up characters for game type/logic
        if(reset){
            chrs.pPC->bs.clBx.x=50;
            chrs.pPC->bs.clBx.y=50;
            chrs.pPC->bs.hlth=100;
            ResetTutObjs(chrs.pTutObs);
            chrs.pTrs->bs[0].isAlv=true;
            chrs.pTrs->bs[0].isSpwn=true;
            chrs.pHnts->bs[0].isAlv=true;
            chrs.pHnts->bs[0].isSpwn=true;
            chrs.pHnts->bs[0].clBx.x=30;
            chrs.pHnts->bs[0].clBx.y=30;
            //actually don't need anything for burnspots, but whatever
            for(int i=0; i<8; i++){
                chrs.pBrns->bs[i].isAlv=true;
                chrs.pBrns->tmLft[i]=100*i;
            }
            //ResetChars(&chrs, &GM_ST);
            reset=false;
        }

        //Stores input, does not handle it yet.
        HandleInput(&e, &keysPressed, &mouse);

        //Character Logic
        if(GM_ST&ENEMY_TUT){
            chrs.pPC->bs.hlth+=10/FRAMES_PER_SECOND;
            if(chrs.pPC->bs.hlth>100)chrs.pPC->bs.hlth=100;
            if(chrs.pPC->bs.hlth<0)chrs.pPC->bs.hlth=0;
        }
        //Move characters
        MoveChars(chrs, keysPressed, mouse);
        if(GM_ST&MOVE_TUT||GM_ST&ATK_TUT||GM_ST&BOTH_TUT){
            HandleTutObs(chrs.pTutObs);
        }
        if(GM_ST&ENEMY_TUT){
            TurnTurrets(chrs);
            MoveBurns(chrs.pBrns);
            TurnSpinner(chrs.pSpins, clocks.events.getTicks());
            //maybe handle Hunters here, maybe not.
        }
        //do firing
        if(GM_ST&ENEMY_TUT){
            //fire pc gun if mouse pressed down
            if(keysPressed.store&keysPressed.mouse){
                FireGun(chrs.pPC, chrs.pBlts, clocks.events.getTicks());
            }
            if(clocks.events.getTicks()-chrs.pTrs->lastFire[0]>TURRET_FIRE_RATE){
                //fire a bolt.
                FireBolt(chrs.pTrs->bs[0].clBx.x, chrs.pTrs->bs[0].clBx.y, chrs.pPC, chrs.pTrs->bs[0].angle, chrs.pBlts);
                chrs.pTrs->lastFire[0]=clocks.events.getTicks();
            }
        }


        //Check for collisions
        if(GM_ST&ENEMY_TUT){
            //right now just check if bolts have hit the player
            for(int i=0; i<128; i++){
                if(chrs.pBlts->bs[i].isAlv){
                    if(DetectCollision(chrs.pPC->bs.clBx, chrs.pBlts->bs[i].clBx)){
                        chrs.pPC->bs.hlth-=BOLT_DAMAGE;
                        chrs.pBlts->bs[i].isAlv=false;
                    }
                }
            }
            for(int i=0; i<chrs.pHnts->inf.maxNum; i++){
                if(chrs.pHnts->bs[i].isAlv){
                    if(DetectCollision(chrs.pPC->bs.clBx, chrs.pHnts->bs[i].clBx)){
                        chrs.pPC->bs.hlth-=15;
                        //make hunter bounce away.
                        //chrs.pHnts
                    }
                }
            }
            for(int i=0; i<chrs.pBrns->inf.maxNum; i++){
                if(chrs.pBrns->bs[i].isAlv&&!chrs.pBrns->warming[i]){
                    if(DetectCollision(chrs.pPC->bs.clBx, chrs.pBrns->bs[i].clBx)){
                        chrs.pPC->bs.hlth-=40/FRAMES_PER_SECOND;
                    }
                }
            }
            //later check for other things.
        }

        //Right now I'm cheating just for the tutorials.
        if(GM_ST&MOVE_TUT||GM_ST&BOTH_TUT){
            if(DetectCollision(chrs.pPC->bs.clBx, chrs.pTutObs->movs.bs.clBx)){
                chrs.pTutObs->movs.hit=true;
                ResetTutObjs(chrs.pTutObs);
            }
        }
        if(GM_ST&ATK_TUT||GM_ST&BOTH_TUT){
            if(DetectCollision(mouse.clBx, chrs.pTutObs->targets.bs.clBx)){
                if(keysPressed.store&keysPressed.mouse){
                    chrs.pTutObs->targets.hit=true;
                    ResetTutObjs(chrs.pTutObs);
                }
            }
        }


        //Handle collisions

        //render
        SDL_SetRenderDrawColor(GFX.pFrame, 255, 255, 255, 255);
        SDL_RenderClear(GFX.pFrame);

        if(GM_ST&MOVE_TUT){
            RenderObj(instrs.rBx[0], NULL, instrs.pImg[0], NULL);
            RenderObj(instrs.rBx[1], NULL, instrs.pImg[1], NULL);
            RenderObj(chrs.pTutObs->movs.bs.clBx, NULL, chrs.pTutObs->movs.inf.pImg[0], NULL);
        }else if(GM_ST&ATK_TUT){
            RenderObj(instrs.rBx[0], NULL, instrs.pImg[0], NULL);
            RenderObj(instrs.rBx[1], NULL, instrs.pImg[1], NULL);
            RenderObj(chrs.pTutObs->targets.bs.clBx, NULL, chrs.pTutObs->targets.inf.pImg[0], NULL);
        }else if(GM_ST&BOTH_TUT){
            RenderObj(instrs.rBx[0], NULL, instrs.pImg[0], NULL);
            RenderObj(instrs.rBx[1], NULL, instrs.pImg[1], NULL);
            RenderObj(instrs.rBx[2], NULL, instrs.pImg[2], NULL);
            RenderObj(chrs.pTutObs->movs.bs.clBx, NULL, chrs.pTutObs->movs.inf.pImg[0], NULL);
            RenderObj(chrs.pTutObs->targets.bs.clBx, NULL, chrs.pTutObs->targets.inf.pImg[0], NULL);
        }
        if(GM_ST&MOVE_TUT||GM_ST&ATK_TUT||GM_ST&BOTH_TUT){
            RenderObj(clkRct, NULL, pClk, NULL);
            RenderBar(clkBr, (50000-(float)clocks.events.getTicks())/50000*100);
        }
        if(GM_ST&MOVE_TUT||GM_ST&BOTH_TUT){
            RenderObj(im1, NULL, chrs.pTutObs->movs.inf.pImg[0], NULL);
            RenderBar(bar1, (float)chrs.pTutObs->movs.tmLft/5000*100);
        }
        if(GM_ST&ATK_TUT||GM_ST&BOTH_TUT){
            RenderObj(im2, NULL, chrs.pTutObs->targets.inf.pImg[0], NULL);
            RenderBar(bar2, (float)chrs.pTutObs->targets.tmLft/5000*100);
        }
        if(GM_ST&ENEMY_TUT){
            //RenderObj(chrs.pTrs->bs[0].clBx, NULL, chrs.pTrs->inf.pImg, &chrs.pTrs->bs[0].angle);
            RenderCharacters(chrs);
            RenderBar(hlBr, chrs.pPC->bs.hlth);
            RenderBar(enBar, (20000-(float)clocks.events.getTicks())/20000*100);
        }

        RenderHealthBars(chrs);
        RenderObj(chrs.pPC->bs.clBx, NULL, chrs.pPC->inf.pImg[0], &chrs.pPC->bs.angle);
        RenderAimPoints(chrs.pPC);


        //if(!(GM_ST&MOVE_TUT)) RenderObj(mouse.clBx, NULL, mouse.pImg[0], NULL, gfx);
        RenderObj(mouse.clBx, NULL, mouse.pImg[0], NULL);

        UpdateGraphics();

        WasteTime(clocks.fps);

        if(keysPressed.store&keysPressed.escape)PG_ST=MENU;
    }

    //DeleteStuffFromMemory(params)

    SDL_SetRelativeMouseMode(SDL_FALSE);

    return 0;
}

//supposedly for all the character's health.
void RenderHealthBars(AllChars chrs)
{
    tdcRct bar=chrs.pPC->bs.clBx;
    bar.y+=PC_SIZE;
    bar.h=0.4;

    RenderBar(bar, chrs.pPC->bs.hlth);

    bar.w=chrs.pTrs->bs[0].clBx.w;
    for(int i=0; i<chrs.pTrs->inf.maxNum; i++){
        bar.x=chrs.pTrs->bs[i].clBx.x;
        bar.y=chrs.pTrs->bs[i].clBx.y+TURRET_SIZE;
        RenderBar(bar, chrs.pTrs->bs[i].hlth);
    }
}

void SetUpCharacters(AllChars chrs)
{
    //This will set up the characters based on the game state
    if(GM_ST&MOVE_TUT||GM_ST&BOTH_TUT){
        //set up move objects
    }
    if(GM_ST&ATK_TUT||GM_ST&BOTH_TUT){
        //set up atk objects
    }
    if(GM_ST&ENEMY_TUT){
        //set up enemies in some way.
        //right now I'm just setting up the turrets, just one for the tutorial.
        InitTurrets(chrs.pTrs);
        chrs.pTrs->bs[0].clBx.x=50;
        chrs.pTrs->bs[0].clBx.y=40;
        chrs.pTrs->bs[0].isAlv=true;
        chrs.pTrs->bs[0].isSpwn=true;

        InitBolts(chrs.pBlts);
    }
    if(GM_ST&PACIFIST){
        //set up enemies in some way
    }
}


void HandleTutObs(TutObs* pObs)
{
    pObs->movs.tmLft-=FRAME_TIME;
    pObs->targets.tmLft-=FRAME_TIME;
}

//Automatically renders vertically or horizontally.
void RenderBar(tdcRct outline, float percent)
{
    //render black background.
    RenderObj(outline, NULL, GFX.pBlk, NULL);

    if(outline.w>outline.h){
        float width=outline.w;
        outline.w*=0.99;
        outline.x+=(width-outline.w)/2;
        outline.y+=(width-outline.w)/2;
        outline.h-=width-outline.w;
        //render white background inside
        RenderObj(outline, NULL, GFX.pWte, NULL);

        //now render real health/time/whatever.
        outline.w*=percent/100;
        RenderObj(outline, NULL, GFX.pBlk, NULL);
    }else if(outline.w<outline.h){
        float height=outline.h;
        outline.h*=0.99;
        outline.x+=(height-outline.h)/2;
        outline.y+=(height-outline.h)/2;
        outline.w-=height-outline.h;

        RenderObj(outline, NULL, GFX.pWte, NULL);

        outline.h*=percent/100;
        RenderObj(outline, NULL, GFX.pBlk, NULL);
    }
}

void RenderAimPoints(PC* pPC)
{
    tdcRct box;
    box.w=0.4;
    box.h=0.4;
    box.x=pPC->bs.clBx.x+PC_SIZE/2;
    box.y=pPC->bs.clBx.y+PC_SIZE/2;
    float gpSz=2;
    while(true){
        box.x+=sin(pPC->bs.angle/57.2957795)*gpSz;
        box.y-=cos(pPC->bs.angle/57.2957795)*gpSz;
        if(box.x<0||box.x>100||box.y<0||box.y>100) break;
        RenderObj(box, NULL, GFX.pBlk, NULL);
        gpSz*=1.1;
    }
}

int SetGameMessages(Instructions* pMsgs)
{
    TTF_Font* pFont=NULL;
    pFont=TTF_OpenFont((bsPth+"\\Fonts\\Squarish Sans CT Regular.ttf").c_str(), 12);
    if(pFont==NULL) return 1;
    SDL_Color fontColour={0, 0, 0};

    for(int i=0; i<8; i++){
       pMsgs->msgs[i]="-------";
    }
    //going minimal now.
    pMsgs->msgs[0]="W|A|S|D";
    pMsgs->msgs[1]="ESC";
    /*
    pMsgs->msgs[0]="Press WASD to move around.";
    pMsgs->msgs[0]+="\n";
    pMsgs->msgs[0]+="Press ESCAPE to return to menu.";*/

    SDL_Texture* pMsgText=NULL;
    SDL_Surface* pMsgSurf=NULL;
    //To find the .h of the texture I could scan for '\n' and add, say, 10 every time.
    //To find the .w I could use some logic where I counted characters then minused the '\n''s
    for(int i=0; i<2; i++){
        //pMsgSurf=TTF_RenderText_Blended_Wrapped(pFont, pMsgs->msgs[i].c_str(), fontColour, 80);
        pMsgSurf=TTF_RenderText_Solid(pFont, pMsgs->msgs[i].c_str(), fontColour);
        pMsgText=SDL_CreateTextureFromSurface(GFX.pFrame, pMsgSurf);

        pMsgs->rBx[i].w=30;
        pMsgs->rBx[i].h=20;
        if(i==0){
            pMsgs->rBx[i].x=50-30/2;
            pMsgs->rBx[i].y=50-20/2;
        }else{
            pMsgs->rBx[i].x=50-30/2;
            pMsgs->rBx[i].y=50-20/2+pMsgs->rBx[i-1].h;
        }

        SDL_FreeSurface(pMsgSurf);
        pMsgSurf=NULL;

        pMsgs->pImg[i]=pMsgText;
        pMsgText=NULL;
    }

    TTF_CloseFont(pFont);

    return 0;
}

int SetAtkMessages(Instructions* pMsgs)
{
    TTF_Font* pFont=NULL;
    pFont=TTF_OpenFont((bsPth+"\\Fonts\\Squarish Sans CT Regular.ttf").c_str(), 12);
    if(pFont==NULL) return 1;
    SDL_Color fontColour={0, 0, 0};

    for(int i=0; i<8; i++){
       pMsgs->msgs[i]="-------";
    }
    pMsgs->msgs[0]="Aim with the mouse. Press LEFT_MOUSE to shoot.";
    pMsgs->msgs[1]="Press ESCAPE to return to menu.";
    /*
    pMsgs->msgs[0]="Press WASD to move around.";
    pMsgs->msgs[0]+="\n";
    pMsgs->msgs[0]+="Press ESCAPE to return to menu.";*/

    SDL_Texture* pMsgText=NULL;
    SDL_Surface* pMsgSurf=NULL;
    //To find the .h of the texture I could scan for '\n' and add, say, 10 every time.
    //To find the .w I could use some logic where I counted characters then minused the '\n''s
    for(int i=0; i<2; i++){
        //pMsgSurf=TTF_RenderText_Blended_Wrapped(pFont, pMsgs->msgs[i].c_str(), fontColour, 80);
        pMsgSurf=TTF_RenderText_Solid(pFont, pMsgs->msgs[i].c_str(), fontColour);
        pMsgText=SDL_CreateTextureFromSurface(GFX.pFrame, pMsgSurf);

        pMsgs->rBx[i].w=80;
        pMsgs->rBx[i].h=20;
        if(i==0){
            pMsgs->rBx[i].x=50-80/2;
            pMsgs->rBx[i].y=50-20/2;
        }else{
            pMsgs->rBx[i].x=50-80/2;
            pMsgs->rBx[i].y=50-20/2+pMsgs->rBx[i-1].h;
        }

        SDL_FreeSurface(pMsgSurf);
        pMsgSurf=NULL;

        pMsgs->pImg[i]=pMsgText;
        pMsgText=NULL;
    }

    TTF_CloseFont(pFont);

    return 0;
}

int SetTutMessages(Instructions* pMsgs)
{
    TTF_Font* pFont=NULL;
    pFont=TTF_OpenFont((bsPth+"\\Fonts\\Squarish Sans CT Regular.ttf").c_str(), 12);
    if(pFont==NULL) return 1;
    SDL_Color fontColour={0, 0, 0};

    for(int i=0; i<8; i++){
       pMsgs->msgs[i]="-------";
    }
    pMsgs->msgs[0]="Move with W|A|S|D";
    pMsgs->msgs[1]="Aim with MOUSE. Shoot with LMOUSE";
    pMsgs->msgs[2]="Press ESCAPE to return to menu.";
    /*
    pMsgs->msgs[0]="Press WASD to move around.";
    pMsgs->msgs[0]+="\n";
    pMsgs->msgs[0]+="Press ESCAPE to return to menu.";*/

    SDL_Texture* pMsgText=NULL;
    SDL_Surface* pMsgSurf=NULL;
    //To find the .h of the texture I could scan for '\n' and add, say, 10 every time.
    //To find the .w I could use some logic where I counted characters then minused the '\n''s
    for(int i=0; i<3; i++){
        //pMsgSurf=TTF_RenderText_Blended_Wrapped(pFont, pMsgs->msgs[i].c_str(), fontColour, 80);
        pMsgSurf=TTF_RenderText_Solid(pFont, pMsgs->msgs[i].c_str(), fontColour);
        pMsgText=SDL_CreateTextureFromSurface(GFX.pFrame, pMsgSurf);

        pMsgs->rBx[i].w=80;
        pMsgs->rBx[i].h=20;
        if(i==0){
            pMsgs->rBx[i].x=50-80/2;
            pMsgs->rBx[i].y=50-20/2;
        }else{
            pMsgs->rBx[i].x=50-80/2;
            pMsgs->rBx[i].y=50-20/2+pMsgs->rBx[i-1].h;
        }

        SDL_FreeSurface(pMsgSurf);
        pMsgSurf=NULL;

        pMsgs->pImg[i]=pMsgText;
        pMsgText=NULL;
    }

    TTF_CloseFont(pFont);

    return 0;
}


///Only loads in UI elements, background etc. Everything else is already loaded.
int LoadGameGFX(TextureArray* pTxtAr)
{
    //safety
    for(int i=0; i<32; i++){
        pTxtAr->pImg[i]=NULL;
    }

    int ind=0;
    if(!LoadTexture(bsPth+"\\Img\\Gameplay\\name.png", &pTxtAr->pImg[ind])) return 1;
    ind++;
    if(!LoadTexture(bsPth+"\\Img\\Gameplay\\name.png", &pTxtAr->pImg[ind])) return 1;
    ind++;
}

///Not implemented yet.
int LoadGameSFX();

///Prevent game from running too quickly, sleep() has problems with Operating Systems.
void WasteTime(Timer fpsTm)
{
    int tmWst;
    if(fpsTm.getTicks()<FRAME_TIME){
        while(fpsTm.getTicks()<FRAME_TIME){
            tmWst+=2;
        }
    }
}















