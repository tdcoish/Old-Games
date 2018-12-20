/**************************************************************************************************************************
Most of the first level states are going to have sub-states. For Menu, it's simply the most obvious. This means that I will
need to eventually make an enum like menuStates{TITLE = 1, SETTINGS, ...}. I might also need to pass in the substate when
another part of the program throws the program back to the Menu state.
Basically, eventually it's going to be as clean as the main function.
**************************************************************************************************************************/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>    //I'm going to need fonts eventually
#include <SDL_mixer.h>  //for sounds
#include <string>       //going to need strings eventually
#include <cstdio>
#include <cmath>        //for traps movement (sin)
#include <sstream>      //outputting health and other stuff
#include <iostream>
#include "Timer.h"
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include "Menu State.h"

int Menu(gameState* pState, menuState* pMenuState, tutState* pTutState, menSubState* pSub, SDL_Renderer* pFrame, int* pLevel, Mix_Music* pMsc, gs dt, int* nmTurs, Diff* df)
{
    while (*pState == MENU){
        switch(*pMenuState){
            case INTRO: if (MenuIntro (pState, pMenuState, pFrame, dt) != 0) return 1; break;
            case MAIN: if (MenuMain(pState, pMenuState, pSub, pTutState, pFrame, pLevel, pMsc, dt, nmTurs, df) != 0) return 1; break;
            case OUTRO: if (MenuOutro(pState, pMenuState, pFrame, dt) != 0) return 1; break;
        }
    }
    return 0;
}

int MenuIntro(gameState* pState, menuState* pMenuState, SDL_Renderer* pFrame, gs dt)
{
    SDL_Texture* pBackground = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\tempIntroScreen.png", &pBackground, pFrame)){
        printf("Failed loading background image");
        *pState = QUIT;
    }
    SDL_Texture* pBlk = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        printf("Failed loading black image");
        *pState = QUIT;
    }
    SDL_Texture* pWht=NULL;
    if(!LoadTextureNoA(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityWhite.png", &pWht, pFrame)){
        printf("Failed loading white image");
        *pState=QUIT;
    }
    if (*pState == MENU && *pMenuState == INTRO){
        //clear screen.
        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);

        SDL_Rect dst;
        dst.x=dt.xPush+1;
        dst.y=0;
        dst.w=dt.SH;
        dst.h=dt.SH;
        SDL_RenderCopy(pFrame, pBackground, NULL, &dst);

        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
        SDL_RenderCopy(pFrame, pWht, NULL, &dt.br2);
        SDL_RenderPresent(pFrame);
        SDL_Delay(400);

        *pMenuState = MAIN;

        SDL_DestroyTexture(pBackground);
        return 0;
    }
    return 1;       //shouldn't get here.
}


int MenuMain(gameState* pState, menuState* pMenuState, menSubState* pSub, tutState* pTutState, SDL_Renderer* pFrame, int* pLevel, Mix_Music* pMsc, gs dt, int* nmTurs, Diff* df)
{
    SDL_Texture* pBkgrnd = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\actionShot2.png", &pBkgrnd, pFrame)){
        printf("Failed loading background.\n");
        return -1;
    }

    while (*pState == MENU && *pMenuState == MAIN){
        switch(*pSub){
            case TITLE: if (TtlScrn(pState, pMenuState, pSub, pFrame, pMsc, pBkgrnd, dt, nmTurs, df) != 0) return 1; break;
            case SETTINGS: if(true) return 1; break;
            case CREDITS: if(CredScrn(pState, pMenuState, pSub, pFrame, pMsc, pBkgrnd, dt)!=0)return 1; break;
            case TUTORIALS: if (TutScrn(pState, pMenuState, pTutState, pSub, pFrame, pMsc, pBkgrnd, dt) != 0) return 1; break;
        }
    }
    return 0;       //shouldn't get here.
}

void DestroyButton(button* pBt)
{
    SDL_DestroyTexture(pBt->im1);
    SDL_DestroyTexture(pBt->im2);
}

void RenderButton(const button bt, SDL_Renderer* pFrame){
    if(bt.mouseOver){
        SDL_RenderCopy(pFrame, bt.im2, NULL, &bt.rnd);
    }else{
        SDL_RenderCopy(pFrame, bt.im1, NULL, &bt.rnd);
    }
}

void SetButtonRender(button* pBt, const gs dt){
    pBt->rnd.x=dt.xPush+pBt->loc.x*dt.scl;
    pBt->rnd.y=pBt->loc.y*dt.scl;
    pBt->rnd.w=pBt->loc.w*dt.scl;
    pBt->rnd.h=pBt->loc.h*dt.scl;
}

int TtlScrn(gameState* pState, menuState* pMenuState, menSubState* pSub, SDL_Renderer* pFrame, Mix_Music* pMsc, SDL_Texture* pBkgrnd, gs dt, int* nmTurs, Diff* df)
{
    //load in the images, sound is already playing
    //buttons should be turned into a linked list, then have a mouse collision box that checks against them.
    button play, quit, cred, dif, tut, minus, plus, minus2, plus2;
    if(!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\playButton.png", &play.im1, pFrame)){
        printf("Failed loading first playbutton\n");
        *pState=QUIT;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\playButton2.png", &play.im2, pFrame)){
        printf("Failed loading playbutton");
        *pState = QUIT;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\quitButton.png", &quit.im1, pFrame)){
        printf("Failed loading quitButton");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\quitButton2.png", &quit.im2, pFrame)){
        printf("Failed loading quitButton");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\Credits.png", &cred.im1, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\Credits2.png", &cred.im2, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\Tutorials.png", &tut.im1, pFrame)){
        printf("Failed loading quitButton");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\Tutorials2.png", &tut.im2, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\back1.png", &minus.im1, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\back2.png", &minus.im2, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\for1.png", &plus.im1, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\for2.png", &plus.im2, pFrame)){
        printf("Failed loading tutorial button\n");
        return -1;
    }
    minus2.im1=minus.im1; minus2.im2=minus.im2;
    plus2.im1=plus.im1; plus2.im2=plus.im2;

    play.loc.x=40; play.loc.y=20; play.loc.w=20; play.loc.h=10;
    quit.loc.x=40; quit.loc.y=80; quit.loc.w=20; quit.loc.h=10;
    cred.loc.x=40; cred.loc.y=65; cred.loc.w=20; cred.loc.h=10;
    tut.loc.x=40; tut.loc.y=45; tut.loc.w=20; tut.loc.h=10;
    minus.loc.x=50; minus.loc.y=40; minus.loc.w=5; minus.loc.h=5;
    plus.loc.x=65; plus.loc.y=40; plus.loc.w=5; plus.loc.h=5;
    minus2.loc.x=50; minus2.loc.y=50; minus2.loc.w=5; minus2.loc.h=5;
    plus2.loc.x=65; plus2.loc.y=50; plus2.loc.w=5; plus2.loc.h=5;
    SetButtonRender(&play, dt);
    SetButtonRender(&quit, dt);
    SetButtonRender(&cred, dt);
    SetButtonRender(&tut, dt);
    SetButtonRender(&minus, dt);
    SetButtonRender(&minus2, dt);
    SetButtonRender(&plus, dt);
    SetButtonRender(&plus2, dt);

    SDL_Texture* pMs=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Credits\\mouse.png", &pMs, pFrame)){
        std::cout << "Error loading mouse image\n";
        return 1;
    }

    SDL_Texture* pEasy=NULL; SDL_Texture* pNorm=NULL; SDL_Texture* pHard=NULL; SDL_Texture* pDfMsg=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\easy.png", &pEasy, pFrame)){
        printf("Failed loading dif image\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\normal.png", &pNorm, pFrame)){
        printf("Failed loading dif image\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\hard.png", &pHard, pFrame)){
        printf("Failed loading dif image\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\diff.png", &pDfMsg, pFrame)){
        printf("Failed loading dif image\n");
        return -1;
    }

    SDL_Texture* pNmTrs=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Title Screen\\nmTrs.png", &pNmTrs, pFrame)){
        printf("Failed loading turret message image\n");
        return -1;
    }

    SDL_Texture* ppDgts[10];
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
    SDL_Rect dg; dg.h=8*dt.scl; dg.w=10*dt.scl; dg.x=55*dt.scl+dt.xPush; dg.y=50*dt.scl;

    SDL_Texture* pBlk=NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        printf("Failed loading black image\n");
        return -1;
    }

    //render one frame before entering the loop
    SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
    SDL_RenderClear(pFrame);

    TTF_Font* pFont = NULL;
    pFont = TTF_OpenFont((tdcDirectoryPath+"\\lazyFont.ttf").c_str(), 36);
    if (pFont == NULL){
        printf("Could not load font. Error: %s\n", TTF_GetError());
        return 1;
    }
    SDL_Color fontColour = {192, 192, 192};

    //used for rendering the integer for the level.
    int size = 3*dt.scl;

    SDL_Event eventStr;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    float msX, msY;
    int mseX, mseY;
    int difLv=*df;
    while (*pState == MENU && *pMenuState == MAIN && *pSub == TITLE){
        minus.mouseOver=false; plus.mouseOver=false; play.mouseOver=false; cred.mouseOver=false;
        tut.mouseOver=false; quit.mouseOver=false; minus2.mouseOver=false; plus2.mouseOver=false;
        SDL_GetMouseState(&mseX, &mseY);
        msX=(float)(mseX-dt.xPush)/dt.scl;
        msY=(float)mseY/dt.scl;
        //I could also create a linked list of buttons, and check for "collisions"/overlap between a mouse collbox
        if (msX > 40 && msX < 60){        //mouse x is in right spot for all buttons
            if (msY > 20 && msY < 30){    //mouse over playButton
                play.mouseOver=true;
            }else if (msY > 80 && msY < 90){  //mouse over quitButton
                quit.mouseOver=true;
            } else if (msY > 65 && msY < 75){ //mouse over credits button
                cred.mouseOver=true;
            }
        }
        if(msX>50&&msX<55&&msY>40&&msY<45){
            minus.mouseOver=true;
        }else if(msX>65&&msX<70&&msY>40&&msY<45){
            plus.mouseOver=true;
        }else if(msX>50&&msX<55&&msY>50&&msY<55){
            minus2.mouseOver=true;
        }else if(msX>65&&msX<70&&msY>50&&msY<55){
            plus2.mouseOver=true;
        }
        while (SDL_PollEvent(&eventStr)){
            if (eventStr.type == SDL_QUIT){
                *pMenuState = OUTRO;
            } else if (eventStr.type == SDL_MOUSEBUTTONDOWN){
                if(play.mouseOver)*pState=CTF;
                if(quit.mouseOver)*pMenuState=OUTRO;
                if(cred.mouseOver)*pSub=CREDITS;

                if(plus.mouseOver)difLv+=1;
                if(minus.mouseOver)difLv-=1;
                if(difLv<EASY)difLv=EASY;
                if(difLv>HARD)difLv=HARD;
                *df=(Diff)difLv;
                if(minus2.mouseOver)*nmTurs-=1;
                if(plus2.mouseOver)*nmTurs+=1;
                if(*nmTurs<1)*nmTurs=1;
                if(*nmTurs>6)*nmTurs=6;
            }
        }

        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);

        SDL_Rect bk;
        bk.x=dt.xPush;
        bk.y=0;
        bk.w=dt.SH;
        bk.h=dt.SW;
        SDL_RenderCopy(pFrame, pBkgrnd, NULL, &bk);

        SDL_Rect dfMsg; dfMsg.x=35*dt.scl+dt.xPush; dfMsg.w=15*dt.scl; dfMsg.y=37*dt.scl; dfMsg.h=10*dt.scl;
        SDL_RenderCopy(pFrame, pDfMsg, NULL, &dfMsg);
        SDL_Rect dff; dff.x=55*dt.scl+dt.xPush; dff.y=38*dt.scl; dff.w=10*dt.scl; dff.h=8*dt.scl;
        if(*df==EASY)SDL_RenderCopy(pFrame, pEasy, NULL, &dff);
        if(*df==NORM)SDL_RenderCopy(pFrame, pNorm, NULL, &dff);
        if(*df==HARD)SDL_RenderCopy(pFrame, pHard, NULL, &dff);

        RenderButton(minus, pFrame);
        RenderButton(plus, pFrame);
        RenderButton(minus2, pFrame);
        RenderButton(plus2, pFrame);
        RenderButton(play, pFrame);
        RenderButton(cred, pFrame);
        RenderButton(quit, pFrame);

        SDL_Rect trMs; trMs.x=dt.xPush+35*dt.scl; trMs.w=15*dt.scl; trMs.y=49*dt.scl; trMs.h=10*dt.scl;
        SDL_RenderCopy(pFrame, pNmTrs, NULL, &trMs);
        SDL_RenderCopy(pFrame, ppDgts[*nmTurs], NULL, &dg);

        ScaleApplyTexture(pMs, pFrame, NULL, mseX, mseY, 2*dt.scl);

        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

        SDL_RenderPresent(pFrame);
        SDL_Delay(9);
    }
    SDL_SetRelativeMouseMode(SDL_FALSE);

    DestroyButton(&minus);
    DestroyButton(&plus);
    DestroyButton(&play);
    DestroyButton(&cred);
    DestroyButton(&quit);
    DestroyButton(&tut);
    SDL_DestroyTexture(pBlk);
    TTF_CloseFont(pFont);

    if(*pMenuState != MAIN || *pState != MENU){
        SDL_DestroyTexture(pBkgrnd);
    }
    return 0;
}

int TutScrn(gameState* pState, menuState* pMenuState, tutState* pTutState, menSubState* pSub, SDL_Renderer* pFrame, Mix_Music* pMsc, SDL_Texture* pBkGnd, gs dt)
{
    SDL_Texture* pAtkGm = NULL;
    SDL_Texture* pAtkTut = NULL;
    SDL_Texture* pMovGm = NULL;
    SDL_Texture* pMovTut = NULL;
    SDL_Texture* pAvdGm = NULL;
    SDL_Texture* pBackBt = NULL;
    SDL_Texture* pBlk = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Tutorial Screen\\AtkGm.bmp", &pAtkGm, pFrame)){
        printf("Failed to load attack game button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Tutorial Screen\\AtkTut.bmp", &pAtkTut, pFrame)){
        printf("Failed to load attack tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Tutorial Screen\\MoveGm.bmp", &pMovGm, pFrame)){
        printf("Failed to load move game button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Tutorial Screen\\MoveTut.bmp", &pMovTut, pFrame)){
        printf("Failed to load move tutorial button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Tutorial Screen\\AvdGm.bmp", &pAvdGm, pFrame)){
        printf("Failed to load avoidance game button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Tutorial Screen\\Back.png", &pBackBt, pFrame)){
        printf("Failed to load back to previous screen button\n");
        return -1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        printf("Failed to load black image\n");
        return -1;
    }

    SDL_Event e;

    float msX, msY;       //in units
    int mseX, mseY;     //in pixels

    //SDL_SetRelativeMouseMode(SDL_TRUE);

    while(*pState == MENU && *pMenuState == MAIN && *pSub == TUTORIALS){
        while(SDL_PollEvent(&e) != 0){
            //get mouse state in pixels and then convert it to units.
            SDL_GetMouseState(&mseX, &mseY);
            msX=(float)(mseX-dt.xPush)/dt.scl;
            msY=(float)mseY/dt.scl;
            if (e.type == SDL_QUIT){
                *pMenuState = OUTRO;
            } else if (e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_ESCAPE){
                    *pMenuState = OUTRO;        //although this is just debugging.
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN){
                if(msX > 40 && msX < 60){
                    if (msY > 12.5 && msY < 25){
                        //clicked attack game
                    } else if(msY > 25 && msY < 37.5){
                        //clicked attack tutorial
                        *pTutState = ATTACKTUT;
                        *pState = TUTORIAL;
                    }
                    else if(msY > 37.5 && msY < 50){
                        //clicked movement game
                        *pTutState = MOVEGM;
                        *pState = TUTORIAL;
                    }
                    else if(msY > 50 && msY < 62.5){
                        //clicked movement tutorial
                        *pTutState = MOVETUT;
                        *pState = TUTORIAL;
                    }
                    else if(msY > 62.5 && msY < 75){
                        //clicked avoidance game
                        *pTutState = AVOIDGM;
                        *pState = TUTORIAL;
                    }
                    else if(msY > 75 && msY < 87.5){
                        //clicked back button
                        *pSub = TITLE;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);

        SDL_Rect bk;
        bk.x=dt.xPush;
        bk.y=0;
        bk.w=dt.SH;
        bk.h=dt.SW;
        SDL_RenderCopy(pFrame, pBkGnd, NULL, &bk);

        SDL_Rect btn;
        btn.x=dt.xPush+40*dt.scl;
        btn.y=12.5*dt.scl;
        btn.w=20*dt.scl;
        btn.h=12.5*dt.scl;
        SDL_RenderCopy(pFrame, pAtkGm, NULL, &btn);
        btn.y+=12.5*dt.scl;
        SDL_RenderCopy(pFrame, pAtkTut, NULL, &btn);
        btn.y+=12.5*dt.scl;
        SDL_RenderCopy(pFrame, pMovGm, NULL, &btn);
        btn.y+=12.5*dt.scl;
        SDL_RenderCopy(pFrame, pMovTut, NULL, &btn);
        btn.y+=12.5*dt.scl;
        SDL_RenderCopy(pFrame, pAvdGm, NULL, &btn);
        btn.y+=12.5*dt.scl;
        SDL_RenderCopy(pFrame, pBackBt, NULL, &btn);

        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

        SDL_RenderPresent(pFrame);

        SDL_Delay(9);
    }

    SDL_DestroyTexture(pAtkGm);
    SDL_DestroyTexture(pAtkTut);
    SDL_DestroyTexture(pMovGm);
    SDL_DestroyTexture(pMovTut);
    SDL_DestroyTexture(pAvdGm);
    SDL_DestroyTexture(pBackBt);
    SDL_DestroyTexture(pBlk);

    if(*pState != MENU || *pMenuState != MAIN){
        SDL_DestroyTexture(pBkGnd);
    }

    return 0;
}

int CredScrn(gameState* pSt, menuState* pMenSt, menSubState* pMnSbSt, SDL_Renderer* pFrame, Mix_Music* pMsc, SDL_Texture* pBk, gs dt)
{
    SDL_Texture* pPic=NULL;
    SDL_Texture* pBkBt=NULL;
    SDL_Texture* pBkBt2=NULL;
    SDL_Texture* pCred=NULL;
    SDL_Texture* pMs=NULL;
    SDL_Texture* pBlk=NULL;
    if(!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Credits\\GimpMono.png", &pPic, pFrame)){
        printf("Failed loading background image\n");
        *pSt=QUIT;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Credits\\back.png", &pBkBt, pFrame)){
        printf("Failed loading back button");
        *pSt = QUIT;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Credits\\back2.png", &pBkBt2, pFrame)){
        printf("Failed loading back button2");
        *pSt = QUIT;
    }
    if(!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Credits\\credits.png", &pCred, pFrame)){
        printf("Failed loading credits\n");
        *pSt=QUIT;
    }
    if(!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\Credits\\mouse.png", &pMs, pFrame)){
        printf("Failed loading mouse image\n");
        *pSt=QUIT;
    }
    if(!LoadTexture(tdcDirectoryPath+"\\Images\\utilityBlack.png", &pBlk, pFrame)){
        printf("Failed loading black image\n");
        *pSt=QUIT;
    }

    float msX, msY;
    int mseX, mseY;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Event e;
    while(*pSt==MENU && *pMenSt==MAIN && *pMnSbSt==CREDITS){
        ///We only have to handle them pressing the back button.
        SDL_GetMouseState(&mseX, &mseY);
        msX=(float)(mseX-dt.xPush)/dt.scl;
        msY=(float)mseY/dt.scl;
        while(SDL_PollEvent(&e)!=0){
            if(e.type==SDL_MOUSEBUTTONDOWN){
                if(msX>40&&msX<60){
                    if(msY>85&&msY<95){
                        *pMnSbSt=TITLE;
                    }
                }
            }else if(e.type==SDL_KEYDOWN){
                if(e.key.keysym.sym==SDLK_ESCAPE){
                    *pMnSbSt=TITLE;
                }
            }
        }

        ///Render the background, the credits, the back button, and the mouse
        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);

        SDL_Rect bk;
        bk.x=dt.xPush;
        bk.y=0;
        bk.w=dt.SH;
        bk.h=dt.SH;
        SDL_RenderCopy(pFrame, pPic, NULL, &bk);
        SDL_RenderCopy(pFrame, pCred, NULL, &bk);

        SDL_Rect bkBt;
        bkBt.x=dt.xPush+40*dt.scl;
        bkBt.y=85*dt.scl;
        bkBt.w=20*dt.scl;
        bkBt.h=10*dt.scl;
        //if the mouse is over the button, render it with a line through it.
        if(msX>40&&msX<60 && msY>85&&msY<95){
            SDL_RenderCopy(pFrame, pBkBt2, NULL, &bkBt);
        }
        SDL_RenderCopy(pFrame, pBkBt, NULL, &bkBt);

        //mouse
        SDL_Rect msBx;
        msBx.x=mseX;
        msBx.y=mseY;
        msBx.w=2*dt.scl;
        msBx.h=2*dt.scl;
        SDL_RenderCopy(pFrame, pMs, NULL, &msBx);

        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

        SDL_RenderPresent(pFrame);
    }
    //Destroy textures

    SDL_SetRelativeMouseMode(SDL_FALSE);
    return 0;
}

int MenuOutro(gameState* pState, menuState* pMenuState, SDL_Renderer* pFrame, gs dt)
{
    SDL_Texture* pBackground = NULL;
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Menu Images\\tempOutroScreen.png", &pBackground, pFrame)){
        printf("Failed loading background image");
        *pState = QUIT;
    }
    SDL_Texture* pBlk=NULL;
    if(!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        printf("Failed loading black\n");
        *pState=QUIT;
    }
    if (pBackground == NULL){
        printf("Error loading background image");
        *pState = QUIT;
    }
    if (*pState == MENU){
        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        SDL_RenderClear(pFrame);
        SDL_Rect bk;
        bk.x=dt.xPush;
        bk.y=0;
        bk.w=dt.SH;
        bk.h=dt.SH;
        SDL_RenderCopy(pFrame, pBackground, NULL, &bk);
        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);
        SDL_RenderPresent(pFrame);

        SDL_Event eventHandler;
        while (*pState == MENU && *pMenuState == OUTRO){
            while (SDL_PollEvent(&eventHandler)){
                if (eventHandler.type == SDL_KEYDOWN){
                    *pState = QUIT;
                }
                if (eventHandler.type == SDL_MOUSEBUTTONDOWN){
                    *pState = QUIT;
                }
                SDL_Delay(9);       //just to maintain ~100 fps, also give the CPU a break.
            }
        }
        SDL_DestroyTexture(pBackground);
        return 0;
    }
    return 1;       //shouldn't get here.
}















