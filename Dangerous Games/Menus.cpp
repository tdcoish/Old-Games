
#include "GLOBALS.h"
#include "Menus.h"
#include <cstdio>

/******************************************************************************
One of the two command functions. When the program is in the MENU state, it is
running this function. Menu programming is mainly dealing with changes in
variables, whether that be state or difficulty or otherwise.
All the menu screens need to load in their visuals (and sounds), as well as
doing all the logic for each screen.

There is some slightly ugly patched logic for the intro and outro screens. I will
have to include some patchy logic for the settings screen, the game select screen,
and the difficulty select screen. It should still be readable.
******************************************************************************/

int RunMenu()
{
    //Setting up the buttons loads all the buttons for us.
    Buttons btns;
    SetUpButtons(&btns);

    //Load in other media
    MenuBackgrounds mnBks;
    SetUpBackgrounds(&mnBks);

    GameTypeMessages msgs;
    SetGTMessages(&msgs);

    //No sounds for now.

    UIMouse ms;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SetUpMouse(&ms);

    SDL_Event e;


    while(PG_ST&MENU){
        //Logic
        ActivateButtons(&btns);
        UpdateMouse(&ms, e);

        //debugging for now
        while(SDL_PollEvent(&e)!=0);

        GlowButtons(&btns, ms);
        ButtonLogic(&btns);

        //Render Setup
        SDL_SetRenderDrawColor(GFX.pFrame, 255, 255, 255, 255);
        SDL_RenderClear(GFX.pFrame);

        //backgrounds first, then buttons, then mouse
        RenderBackground(&mnBks);
        RenderButtons(&btns);
        if(MN_ST&GAMESELECT)RenderGSExtras(msgs, btns);
        SDL_RenderCopy(GFX.pFrame, ms.pImg, NULL, &ms.rndBx);

        //Flip buffers, draw black bars
        UpdateGraphics();

        //Redraw frame, do logic, if Intro/outro
        IntroAndOutro(&mnBks);

        //Don't need perfect timing. Menu's don't matter.
        SDL_Delay(10);
    }

    //DeleteStuffFromMemory(params) //buttons, backgrounds, messages.

    SDL_SetRelativeMouseMode(SDL_FALSE);
    return 0;
}

///put buttons in right place, set things for bug purposes
void SetUpButtons(Buttons* pBts)
{
    for(int i=0; i<32; i++){
        pBts->active[i]=false;
        pBts->msOver[i]=false;
        pBts->pressed[i]=false;
        pBts->activePages[i]=0;
        pBts->newSt[i]=0;
    }

	//each button must be manually created.

	int ind=0;  //I don't have to write 0, 1, 2... and keeps track of total number
	pBts->activePages[ind]=TITLE;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\playBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=40;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=15;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=MENUSCREEN;
	pBts->newSt[ind]=GAMESELECT;
	ind++;

	//now do button 2
    pBts->activePages[ind]=TITLE;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\credBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=40;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=35;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=MENUSCREEN;
	pBts->newSt[ind]=CREDITS;
	ind++;

	//now do button 3.
	pBts->activePages[ind]=TITLE;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\setBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=40;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=55;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=MENUSCREEN;
	pBts->newSt[ind]=SETTINGS;
	ind++;

	//button 4
	pBts->activePages[ind]=TITLE;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\quitBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=40;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=75;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=MENUSCREEN;
	pBts->newSt[ind]=OUTRO;
	ind++;

	//button 5
	pBts->activePages[ind]=CREDITS|SETTINGS|GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\backBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=40;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=85;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=MENUSCREEN;
	pBts->newSt[ind]=TITLE;
	ind++;

	//button 8
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\pacifistBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=10;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=25;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=PACIFIST;
	ind++;

	//button 9
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\killerBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=30;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=25;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=KILL_ALL;
	ind++;

	//button 10
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\gauntletBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=50;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=25;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=GAUNTLET;
	ind++;

	//button 11
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\movTutBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=5;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=65;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=MOVE_TUT;
	ind++;

	//button 12
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\atkTutBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=22;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=65;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=ATK_TUT;
	ind++;

	//button 13
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\movAtkTutBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=38;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=65;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=BOTH_TUT;
	ind++;

	//button 14
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\finalTutBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=55;
	pBts->clBx[ind].w=15;
	pBts->clBx[ind].y=65;
	pBts->clBx[ind].h=10;
    pBts->type[ind]=STATE;
	pBts->chngType[ind]=GAMETYPE;
	pBts->newSt[ind]=ENEMY_TUT;
	ind++;

	//button 15
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\playBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=40;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=10;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=STATE;
	pBts->chngType[ind]=PROGRAM;
	pBts->newSt[ind]=GAME;
	ind++;

	//button 16
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\easyBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=75;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=30;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=DIFFICULTY;
	pBts->dif[ind]=EASY;
	ind++;

	//button 17
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\normBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=75;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].y=45;
	pBts->clBx[ind].h=10;
	pBts->type[ind]=DIFFICULTY;
	pBts->dif[ind]=MED;
	ind++;

	//button 18
	pBts->activePages[ind]=GAMESELECT;
	if(!LoadTexture(bsPth+"\\Img\\Menu\\hardBtn.png", &pBts->pImg[ind])) return;
	pBts->clBx[ind].x=75;
	pBts->clBx[ind].w=20;
	pBts->clBx[ind].h=10;
	pBts->clBx[ind].y=60;
	pBts->type[ind]=DIFFICULTY;
	pBts->dif[ind]=HARD;
	ind++;



	//When I get to the end
	for(int i=0; i<ind; i++){
	    SetRenderBox(pBts->clBx[i], &pBts->rBx[i]);
	}

	pBts->totBts=ind;
}

///Go through all buttons and make them active or inactive depending on the screen
void ActivateButtons(Buttons* pBts)
{
    if(PG_ST&MENU){
		for(int i=0; i<pBts->totBts; i++){
			if(pBts->activePages[i]&MN_ST){       //can be true for multiple pages
				pBts->active[i]=true;
			}else {
				pBts->active[i]=false;
			}
		}
	}
}

///go through all buttons, change msOver if mouse over, change pressed if mouse pressed
void GlowButtons(Buttons* pBts, const UIMouse ms)
{
    for(int i=0; i<pBts->totBts; i++){
        pBts->msOver[i]=false;
        pBts->pressed[i]=false;
		if(pBts->active[i]){
			if(CheckCollision(pBts->clBx[i], ms.clBx)){
				pBts->msOver[i]=true;
				if(ms.pressed){
					pBts->pressed[i]=true;
				}
			}
		}
	}
}

///Already know which button is pressed if any. Run through the list for action
void ButtonLogic(Buttons* pBts)
{
    //all this assumes that buttons only ever change state.
    for(int i=0; i<pBts->totBts; i++){
        if(pBts->pressed[i]){
            if(pBts->type[i]==STATE){       //if the button changes state.
                switch(pBts->chngType[i]){
                    case PROGRAM: PG_ST=pBts->newSt[i]; break;
                    case GAMETYPE: GM_ST=pBts->newSt[i]; break;
                    case MENUSCREEN: MN_ST=pBts->newSt[i]; break;
                }
            }else if(pBts->type[i]==DIFFICULTY){
                DIFF=pBts->dif[i];
            }
        }
    }
}

///Render active buttons. Render the button (if there is one) hovered over with black bars.
void RenderButtons(Buttons* pBts)
{
    for(int i=0; i<pBts->totBts; i++){
		if(pBts->active[i]){
		SDL_RenderCopy(GFX.pFrame, pBts->pImg[i], NULL, &pBts->rBx[i]);
			if(pBts->msOver[i]){
			    //draw subtle black bars on both sides of the button
				SDL_Rect sds;
				sds.x=pBts->rBx[i].x+0.5*GFX.scl;
				sds.y=pBts->rBx[i].y+1*GFX.scl;
				sds.w=0.5*GFX.scl;
				sds.h=pBts->rBx[i].h*0.8;
				SDL_RenderCopy(GFX.pFrame, GFX.pBlk, NULL, &sds);

				//change x value, draw the second side
				sds.x+=pBts->rBx[i].w-1.2*GFX.scl;
				SDL_RenderCopy(GFX.pFrame, GFX.pBlk, NULL, &sds);
			}
		}
	}
}

///Free memory when state changes
void DeleteButtons(Buttons* pBts)
{
    for(int i=0; i<pBts->totBts; i++){
        SDL_DestroyTexture(pBts->pImg[i]);
	}
}


///Loads in background textures, assigns page
void SetUpBackgrounds(MenuBackgrounds* pBks)
{
    int ind=0;
    if(!LoadTextureNoA(bsPth+"\\Img\\Menu\\Intro.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=INTRO;
    ind++;
    if(!LoadTextureNoA(bsPth+"\\Img\\utilWte.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=TITLE;
    ind++;
    if(!LoadTextureNoA(bsPth+"\\Img\\Menu\\meShot1.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=CREDITS;
    ind++;
    if(!LoadTextureNoA(bsPth+"\\Img\\Menu\\testBack.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=SETTINGS;
    ind++;
    if(!LoadTextureNoA(bsPth+"\\Img\\utilWte.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=GAMESELECT;
    ind++;
    if(!LoadTextureNoA(bsPth+"\\Img\\utilWte.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=DIFFICULTY_;
    ind++;
    if(!LoadTextureNoA(bsPth+"\\Img\\Menu\\Outro.png", &pBks->pBk[ind])) return;
    pBks->corSt[ind]=OUTRO;
    ind++;

    pBks->totBks=ind;
}

///Renders the appropriate background
void RenderBackground(MenuBackgrounds* pBks)
{
    for(int i=0; i<pBks->totBks; i++){
        if(pBks->corSt[i]&MN_ST){
            SDL_Rect dstRct;
            dstRct.x=GFX.xPush;
            dstRct.y=0;
            dstRct.w=100*GFX.scl;
            dstRct.h=100*GFX.scl;
            SDL_RenderCopy(GFX.pFrame, pBks->pBk[i], NULL, &dstRct);
        }
    }
}

///Delays for x ms, then changes state appropriately
void IntroAndOutro(MenuBackgrounds* pBks)
{
    if(MN_ST&INTRO || MN_ST&OUTRO){
        //Render the background again over everything
        RenderBackground(pBks);
        SDL_RenderPresent(GFX.pFrame);

        //doesn't get a chance to render a frame for Outro
        if(MN_ST&INTRO){
            MN_ST=TITLE;
        }else if(MN_ST&OUTRO){
            PG_ST=QUIT;
        }
        SDL_Delay(300);
    }
}

//The messages that describe the game types.
int SetGTMessages(GameTypeMessages* pMsgs)
{
    //This font is used only for rendering the messages
    TTF_Font* pFont=NULL;
    pFont=TTF_OpenFont((bsPth+"\\Fonts\\Squarish Sans CT Regular.ttf").c_str(), 12);
    if(pFont==NULL) return 1;
    SDL_Color fontColour={0, 0, 0};

    //Our messages for now.
    for(int i=0; i<8; i++){
        pMsgs->message[i]="-------";
    }
    pMsgs->message[0]="No weapons. Survive as long as you can.";
    pMsgs->type[0]=PACIFIST;
    pMsgs->message[1]="Enemies spawn. Kill them. Don't die.";
    pMsgs->type[1]=KILL_ALL;
    pMsgs->message[2]="Get to the end of the hallway before time expires.";
    pMsgs->type[2]=GAUNTLET;
    pMsgs->message[3]="This tutorial teaches you the movement controls.";
    pMsgs->type[3]=MOVE_TUT;
    pMsgs->message[4]="This tutorial teaches you the firing controls.";
    pMsgs->type[4]=ATK_TUT;
    pMsgs->message[5]="This tutorial combines movement and firing.";
    pMsgs->type[5]=BOTH_TUT;
    pMsgs->message[6]="This tutorial teaches you the enemies in the game.";
    pMsgs->type[6]=ENEMY_TUT;

    SDL_Texture* pMsgText=NULL;
    SDL_Surface* pMsgSurf=NULL;

    for(int i=0; i<8; i++){
        pMsgSurf=TTF_RenderText_Solid(pFont, pMsgs->message[i].c_str(), fontColour);
        pMsgText=SDL_CreateTextureFromSurface(GFX.pFrame, pMsgSurf);

        //pMsgs->rBx[i].w=pMsgSurf->w;
        //pMsgs->rBx[i].h=pMsgSurf->h;
        pMsgs->rBx[i].w=60;
        pMsgs->rBx[i].h=5;
        SDL_FreeSurface(pMsgSurf);
        pMsgSurf=NULL;

        pMsgs->pImg[i]=pMsgText;
        pMsgText=NULL;
    }

    //Pick the part of the screen that we want to render to:
    for(int i=0; i<8; i++){
        pMsgs->rBx[i].x=5;
        pMsgs->rBx[i].y=45;     //could use rBx.h and rBx.w
    }

    TTF_CloseFont(pFont);
}

//We know what state is what, now we just render appropriately
void RenderGTMessages(GameTypeMessages msgs)
{
    SDL_Rect rnd;
    for(int i=0; i<8; i++){
        if(msgs.type[i]&GM_ST){
            SetRenderBox(msgs.rBx[i], &rnd);
            SDL_RenderCopy(GFX.pFrame, msgs.pImg[i], NULL, &rnd);
        }
    }
}

///Renders the messages and renders black boxes around the selected stuff
void RenderGSExtras(GameTypeMessages msgs, Buttons btns)
{
    RenderGTMessages(msgs);

    //Holy shit this is ugly code.
    //Fun fact, I was so tired when I wrote this that I wrote tdcRct instead of temp every time.
    tdcRct temp;
    //find the center of the button that is correct
    temp=GSButtonMidpoint(btns, DIFFICULTY);
    float squareSize=2.5;
    //difficulty square is rendered to the left
    temp.x-=(temp.w/2)-squareSize-1;
    //now render small square around this new centerpoint.
    temp.x-=squareSize/2;
    temp.y-=squareSize/2;
    temp.w=squareSize;
    temp.h=squareSize;

    SDL_Rect rnd;
    SetRenderBox(temp, &rnd);
    SDL_RenderCopy(GFX.pFrame, GFX.pBlk, NULL, &rnd);

    //temp gets overwritten with a new tdcRct at the btn centerpoint
    temp=GSButtonMidpoint(btns, STATE);
    squareSize=1.5;
    temp.x-=squareSize/2;
    temp.y-=squareSize/2;
    //game type square is rendered towards the center
    if(temp.y<50)temp.y+=temp.h/2+squareSize;
    if(temp.y>50)temp.y-=temp.h/2+squareSize;
    temp.w=squareSize;
    temp.h=squareSize;
    SetRenderBox(temp, &rnd);
    SDL_RenderCopy(GFX.pFrame, GFX.pBlk, NULL, &rnd);
}

///Return the midpoint of button that controls the game mode that is active.
tdcRct GSButtonMidpoint(Buttons btns, ChangeType type)
{
    tdcRct midBtn;
	for(int i=0; i<btns.totBts; i++){
		if(btns.active[i]){
		    if(type==STATE){
		        if(btns.type[i]==STATE&&btns.chngType[i]==GAMETYPE){
                    if(btns.newSt[i]&GM_ST){
                        midBtn=btns.clBx[i];
                    }
		        }
		    }else if(type==DIFFICULTY){
                if(btns.type[i]==DIFFICULTY){
                    if(btns.dif[i]==DIFF){
                        midBtn=btns.clBx[i];
                    }
                }
		    }
		}
	}
	//now we have the location of the button
	//still need to find center
	midBtn.x+=midBtn.w/2;
	midBtn.y+=midBtn.h/2;

	return midBtn;
}



































