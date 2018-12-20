#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>    //I'm going to need fonts eventually
#include <SDL_mixer.h>  //for sounds
#include <string>       //going to need strings eventually
#include <cmath>        //for traps movement (sin)
#include <sstream>      //outputting health and other stuff
#include <iostream>
#include <ctime>        //for srand(time(x))
#include <cstdlib>      //for srand()
#include <fstream>      //for loading in plays, used for more later
#include "Timer.h"
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include "Characters.h"
#include "Level IO.h"
#include "Cutscenes.h"

//The architecture here has got to be. Load in level data, then run everything

const float VERSION = 0.5;

const int NUM_SPIKETRAPS = 64;   //used less than it should be

const double PI = 3.1415;

const int SHOT_INTERVAL = 300;  //in milliseconds
const int SPRINK_SHOT_INTERVAL = 200;

const int SPIKE_UNACTIVE_TIME = 2000;           //in milliseconds, we decrease this by 1 until we hit 0

const int BOLT_SPEED_INCREASE_PER_FRAME = 100/FRAMES_PER_SECOND;  //in pixels per second, unused

const int HEALTH_REGEN_PER_SEC = 1000;          //debugging only

const int HUNTER_COOLDOWN_TIME = 400;       //400 ms before it can hurt again. Poorly titled.

const int BURN_SPOT_TIME = 4;       //x seconds.

const int PC_HIT_CHANGE = 100;       //changes visuals for this long.

const float TOT_SNP_SHT = 30;       //total health lost by taking the full sniper shot.
const float TOT_SHT_FRM = 10*TOT_SNP_SHT/FRAMES_PER_SECOND;     //cause sniper shot only lasts 1/10 a second

double FindAngle(double yVel, double xVel, bool* pFlip);

void FireSnipes(Snipes* pSnp, PlayerCharacter* pPC, int nmSnps);

void PlaySFX(int chnls[64], Mix_Chunk* pSFX);

int BattleState(gameState* pState, menuState* pMenuState, SDL_Renderer* pFrame, int* pLevel, gs dt)
{
    //flags for debugging
    bool pauseGame = false;     //press SPACE to pause or unpause game.

    bool increaseSpeed = false;         //for right now, I'm not increasing the speed of bolts

    bool trapsDoDamage = false;
    bool boltsDoDamage = true;
    bool huntersDoDamage = true;
    bool snipesDoDamage = false;
    bool bombsDoDamage = true;

    bool renderBolts = true;
    bool renderTraps = false;
    bool renderPC = true;
    bool renderSniper = true;
    bool renderHunters = true;
    bool renderHealthPacks = true;
    bool renderBombs = true;
    bool renderSnipes = true;
    bool renderSprinkler = true;
    bool renderTurrets = true;
    bool renderPillars=true;

    bool healthRegen = true;
    bool healthDrain = false;

    bool huntersBounce = true;
    bool huntersBouncePC = true;

    bool burnSpots = true;

    ///Set up visuals
    SDL_Texture* pSpikeImg = NULL;
    SDL_Texture* pSniperImg = NULL;
    SDL_Texture* pBlt = NULL;
    SDL_Texture* pHunterImg = NULL;
    SDL_Texture* pHealthPacksImg = NULL;
    SDL_Texture* pPCImg = NULL;
    SDL_Texture* pPCHITImg = NULL;
    SDL_Texture* pBurnImg = NULL;
    SDL_Texture* pPreBurn = NULL;
    SDL_Texture* pBarrierImg = NULL;
    SDL_Texture* pBombImg = NULL;
    SDL_Texture* pBombExplode = NULL;
    SDL_Texture* pSnp0=NULL;
    SDL_Texture* pSnp1=NULL;
    SDL_Texture* pSnp2=NULL;
    SDL_Texture* pSnp3=NULL;
    SDL_Texture* pSnpCl=NULL;
    SDL_Texture* pSnpDd=NULL;
    SDL_Texture* pSnpChrg = NULL;
    SDL_Texture* pSprinkImg = NULL;
    SDL_Texture* pTurretImg = NULL;
    SDL_Texture* pBrokeTur = NULL;
    SDL_Texture* pHealthBar = NULL;
    SDL_Texture* pHealthBack = NULL;
    SDL_Texture* pHealthOutline = NULL;
    SDL_Texture* pSwrd = NULL;
    SDL_Texture* pMs = NULL;
    SDL_Texture* pSld = NULL;
    SDL_Texture* pSnpSht = NULL;
    SDL_Texture* pSldBk = NULL;
    SDL_Texture* pSldBr = NULL;
    SDL_Texture* pSldOut = NULL;
    SDL_Texture* pAim = NULL;
    SDL_Texture* pHntDead = NULL;
    SDL_Texture* pBlk=NULL;
    SDL_Texture* pWte=NULL;

    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBolt.png", &pBlt, pFrame)){
        printf("Could not load sniper bolt image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoPC.png", &pPCImg, pFrame)){
        printf("Could not load player character image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoPCHit.png", &pPCHITImg, pFrame)){
        printf("Could not load player character hit image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoHunter.png", &pHunterImg, pFrame)){
        printf("Could not load Hunter image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoHealth.png", &pHealthPacksImg, pFrame)){
        printf("Could not load health packs image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBurnAct.png", &pBurnImg, pFrame)){
        printf("Could not load burn spots image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBurnPre.png", &pPreBurn, pFrame)){
        printf("Could not load pre burn image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\barrier.bmp", &pBarrierImg, pFrame)){
        printf("Could not load barrier image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\healthBar.bmp", &pHealthBar, pFrame)){
        printf("Could not load health bar\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\healthBack.bmp", &pHealthBack, pFrame)){
        printf("Could not load health background\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\healthOut.bmp", &pHealthOutline, pFrame)){
        printf("Could not load out line of healthbar\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBomb.png", &pBombImg, pFrame)){
        printf("Could not load bomb image.\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoBombExp.png", &pBombExplode, pFrame)){
        printf("Could not load bomb exploding image.\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoSnipes0.png", &pSnp0, pFrame)){
        printf("Could not load insta-hit sniper\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoSnipes1.png", &pSnp1, pFrame)){
        printf("Could not load insta-hit sniper\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoSnipes2.png", &pSnp2, pFrame)){
        printf("Could not load insta-hit sniper\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoSnipes3.png", &pSnp3, pFrame)){
        printf("Could not load insta-hit sniper\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoCharge.png", &pSnpChrg, pFrame)){
        printf("Could not load sniper charge bars\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoSnipesCool.png", &pSnpCl, pFrame)){
        printf("Could not load insta-hit sniper cooling down\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoSnipesDead.png", &pSnpDd, pFrame)){
        printf("Could not load dead insta-hit\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoTurret.png", &pSprinkImg, pFrame)){
        printf("Could not load sprinkler weapon image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoTurret.png", &pTurretImg, pFrame)){
        printf("Could not load turret image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoTurretBroken.png", &pBrokeTur, pFrame)){
        printf("Could not load dead turret image\n");
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sword.png", &pSwrd, pFrame)){
        std::cout << "Error loading sword image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoMouse.png", &pMs, pFrame)){
        std::cout << "Error loading mouse image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sldImg.png", &pSld, pFrame)){
        std::cout << "Error loading shield image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pSnpSht, pFrame)){
        std::cout << "Error loading sniper plasma blast image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sldBk.png", &pSldBk, pFrame)){
        std::cout << "Error loading shield back image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sldBar.png", &pSldBr, pFrame)){
        std::cout << "Error loading shield bar image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\sldOut.png", &pSldOut, pFrame)){
        std::cout << "Error loading shield outline image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityCircleFilled.png", &pAim, pFrame)){
        std::cout << "Error loading aiming path outline image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\monoHunterDead.png", &pHntDead, pFrame)){
        std::cout << "Error loading dead hunter image\n";
        return 1;
    }
    if (!LoadTexture(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityBlack.png", &pBlk, pFrame)){
        std::cout << "Error loading black image\n";
        return 1;
    }
    if (!LoadTextureNoA(tdcDirectoryPath+"\\Images\\Gameplay Images\\utilityWhite.png", &pWte, pFrame)){
        std::cout << "Error loading white image\n";
        return 1;
    }


    TTF_Font* pFont = NULL;

    pFont = TTF_OpenFont((tdcDirectoryPath+"\\lazyFont.ttf").c_str(), 36);
    if (pFont == NULL) return 1;

    SDL_Color fontColour = {255, 255, 255};

    ///Load in level
    Lev lev;
    if (ReadLev(&lev, tdcDirectoryPath+"\\Files\\testFile.txt") == -1){
        printf("Failed loading new level\n");
        return 1;
    }

    ///Load in cutscenes (yes we shouldn't do this here)
    std::string cutscenes[64];
    if (!StoreCutscenes(cutscenes, 10)){
        printf("Error loading cutscenes\n");
        return 1;
    }
    for (int i = 0; i < 10; i++){
        //printf("Cutscene %i: %s\n", i, cutscenes[i].c_str());
    }

    ///Set up logic.
    Timer fpsTimer;
    SDL_Event eventStr;
    Timer eventTimer;       //does not restart every frame
    eventTimer.startClk();
    int playerScore = 0;

    bool hitDelay = false;

    SpikeTraps tdcTraps;
    SDL_Rect trapColBox;    //stores the offsets of the spike trap for collision detection.
    trapColBox.w = 20;
    trapColBox.h = 20;

    double boltSpeed = 22.5;         //starts off at x pixels per second

    PlayerCharacter PC(dt);     //basically just moves around the screen.
    Bolts blts(128, dt);
    Sprinkler sprink(dt);

    Sniper snpr(lev.wvs[0].brSz);
    int lastSniperFire = 0; //minus total time from this to check for fire
    Hunters hnts(8, dt);             //+1 to Halo:CE
    HealthPacks hlPks(8);
    BurnSpots tdcSpots(16, dt);
    Bombs bombs(32, 0, dt);
    Snipes snps(32, dt);      //we'll never need that many
    int lastSprinkFire = 0;
    Turrets turs(16, dt);
    Pillars plls;

    srand(time(0));

    bool firstFrame = true;

    //trackers
    std::string lastHit = "Unhit";      //stores which enemy hits last. Therefore tells us what killed us.
    int huntTotHits = 0;         //total hits taken from hunters
    int boltTotHits = 0;
    int burnTotDam = 0;
    int healthTotal = 0;        //total health picked up (in num_healthpacks, not health)
    int curPosStreak = 0;   //stores chain of positive streak
    int highPosStreak = 0;  //best consecutive health pickups between getting hit
    int curNegStreak = 0;
    int highNegStreak = 0;  //worst number of hits between picking up health
    int lastClean = 0;       //stores last time player was hit.
    bool hit = false;
    int highClean = 0;      //stores longest time without getting hit.

    int frameCount = 0;
    int notFullSec = 0;

    bool nextWave = true;
    int wvNum = -1;
    bool quitAll = false;
    int pauseTime = 0;
    int nwWvTm = 0;

    int mseX, mseY;
    float msX, msY;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int chnls[256];
    for(int i=0; i<256; i++){
        chnls[i] = -1;
    }

    while (*pState == BATTLE)
    {
        fpsTimer.startClk();
        pauseTime -= FRAME_TIME;
        if (!nextWave){     //we're not currently setting up, but maybe we should be
            bool allPutBack = true;
            //If any enemies are still alive, then we don't bring the next wave
            for(int i=0; i<lev.wvs[wvNum].nmHnts; i++){
                if(hnts.hlth[i] > 0) allPutBack = false;
            }
            for(int i=0; i<lev.wvs[wvNum].nmBms; i++){
                if(bombs.isAlive[i] || !bombs.spawned[i])allPutBack = false;
            }
            for(int i=0; i<lev.wvs[wvNum].nmSnprs; i++){
                if(snpr.health[i] > 0) allPutBack = false;
            }
            for(int i=0; i<lev.wvs[wvNum].nmSnps; i++){
                if(snps.isAlive[i] || !snps.spawned[i]) allPutBack = false;
            }
            for(int i=0; i<lev.wvs[wvNum].nmSpnk; i++){
                if(sprink.isAlive) allPutBack = false;
            }
            for(int i=0; i<lev.wvs[wvNum].nmTrs; i++){
                if(turs.health[i] > 0) allPutBack = false;
            }
            if(allPutBack){
                nextWave = true;
            }
            //if they fail, they have to restart
            if (PC.health <= 0){
                if(PC.health <=0){
                    SDL_Delay(400);
                }
                nextWave = true;
                wvNum--;
            }
        }
        if(nextWave){
            PC.trueX = 50;
            PC.trueY = 85;
            wvNum++;
            if(wvNum == lev.nmWvs){
                *pState = MENU;
                *pMenuState = MAIN;
                quitAll = true;
            }
            if(!quitAll){
                pauseTime = 100;
                for (int i=0; i<lev.wvs[wvNum].nmTrs; i++){
                    turs.xPos[i] = lev.wvs[wvNum].turX[i];
                    turs.yPos[i] = lev.wvs[wvNum].turY[i];
                    turs.clBx[i].x = turs.xPos[i];
                    turs.clBx[i].y = turs.yPos[i];
                    turs.rBx[i].x=dt.xPush+turs.xPos[i]*dt.scl;
                    turs.rBx[i].y=turs.yPos[i]*dt.scl;
                    turs.isAlive[i] = true;
                    turs.health[i] = 100;
                    turs.lastFire[i] = eventTimer.getTicks() + i*FRAME_TIME*8;
                }
                for(int i=0; i<lev.wvs[wvNum].nmHnts; i++){
                    hnts.spawned[i] = false;
                    hnts.prevSpawn = -1;
                    hnts.lastSpawn = eventTimer.getTicks()-2000;
                    hnts.xPos[i] = 50;
                    hnts.yPos[i] = 50;
                    hnts.xVel[i]=0;
                    hnts.yVel[i]=0;
                    hnts.hunters[i].x = dt.xPush+50*dt.scl;
                    hnts.hunters[i].y = 50*dt.scl;
                    hnts.hlth[i] = 100;
                }
                for (int i=0; i<lev.wvs[wvNum].nmPks; i++){
                    hlPks.xPos[i] = lev.wvs[wvNum].pkX[i];
                    hlPks.yPos[i] = lev.wvs[wvNum].pkY[i];
                    hlPks.clBx[i].x = hlPks.xPos[i];
                    hlPks.clBx[i].y = hlPks.yPos[i];
                    hlPks.rBx[i].x=dt.xPush+hlPks.clBx[i].x*dt.scl;
                    hlPks.rBx[i].y=hlPks.clBx[i].y*dt.scl;
                    hlPks.isAlive[i] = true;
                }
                for(int i=0; i<lev.wvs[wvNum].nmBms; i++){
                    bombs.spawned[i] = false;
                    bombs.prevSpawn = -1;
                    bombs.lastSpawn = eventTimer.getTicks()-2000;
                    bombs.xPos[i] = 50;
                    bombs.yPos[i] = 50;
                    bombs.clBx[i].x = 0;
                    bombs.clBx[i].y = 0;
                    bombs.exploding[i]=false;
                    bombs.isAlive[i]=false;
                    bombs.timeLeft[i]=40000;        //just to make it not 0
                }
                snpr.health[0] = 0;
                for(int i=0; i<lev.wvs[wvNum].nmSnprs; i++){
                    //set up circular sniper
                    snpr.health[0] = 100;
                }
                for(int i=0; i<lev.wvs[wvNum].nmSnps; i++){
                    //set up snipers
                    snps.spawned[i]=false;
                    snps.prevSpawn=-1;
                    snps.lastSpawn=-2000;
                    snps.xPos[i]=0;
                    snps.yPos[i]=0;
                    snps.clBx[i].x=0;
                    snps.clBx[i].y=0;
                    snps.isAlive[i]=false;
                }
                sprink.isAlive = false;
                for(int i=0; i<lev.wvs[wvNum].nmSpnk; i++){
                    //set up sprinkler turret
                    sprink.isAlive = true;
                    lastSprinkFire = eventTimer.getTicks();
                    sprink.rBx.x=dt.xPush+50*dt.scl-(SPRINKLER_SIZE*dt.scl)/2;
                    sprink.rBx.y=50*dt.scl-(SPRINKLER_SIZE*dt.scl)/2;
                }
                for(int i=0; i<lev.wvs[wvNum].nmSpts; i++){
                    //I don't think I actually need to set up the burn spots.
                }
                for(int i=0; i<128; i++){
                    blts.isAlive[i] = false;
                }
                for(int i=0; i<lev.wvs[wvNum].nmPls; i++){
                    PlacePillars(&plls, lev, wvNum, dt);
                }
                PC.health = 100;
                nextWave = false;
                for(int i=0; i<256; i++){
                    chnls[i] = -1;      //so we can play sounds again now.
                }
                if(eventTimer.getTicks()!=0) nwWvTm=eventTimer.getTicks()/1000;
            }
        }

        if (Mix_PlayingMusic() == 0){
            //Mix_PlayMusic(pMsc, -1);      //-1 loops forever
        }

        notFullSec += FRAME_TIME;
        frameCount++;

        if (notFullSec > 1000){
            notFullSec -= 1000;
            //printf("\nFramerate: %i\n\n", frameCount);
            frameCount = 0;
        }

        //Only event we handle right now is quitting.
        while (SDL_PollEvent(&eventStr) != 0)
        {
            if (eventStr.type == SDL_QUIT){
                *pState = MENU;
                *pMenuState = OUTRO;
            } else if (eventStr.type == SDL_KEYDOWN){
                if (eventStr.key.keysym.sym == SDLK_ESCAPE){
                    *pState = MENU;
                } else if (eventStr.key.keysym.sym == SDLK_SPACE){
                    pauseGame = (!pauseGame);
                }
            }
            bool sldCur = PC.sldUp;
            PC.handleInput(&eventStr);      //takes movement input
        }
        SDL_GetMouseState(&mseX, &mseY);
        msX=(float)(mseX-dt.xPush)/dt.scl;
        msY=(float)mseY/dt.scl;
        ///do logic

        if (pauseGame){
            eventTimer.pauseClk();
        } else {
            eventTimer.unpauseClk();
        }

        if (!pauseGame && !firstFrame && pauseTime < 0){

            playerScore += 1;

            hnts.SpawnHunters(lev.wvs[wvNum].nmHnts, eventTimer.getTicks());
            snps.SpawnSnipes(lev.wvs[wvNum].nmSnps, eventTimer.getTicks());
            bombs.SpawnBombs(lev.wvs[wvNum].nmBms, eventTimer.getTicks());

            snpr.moveSniper(lev.wvs[wvNum].brSz);
            tdcTraps.moveTraps();
            blts.moveBolts(128, dt);
            hnts.moveHunters(PC, lev.wvs[wvNum].nmHnts, lev.wvs[wvNum].brSz, dt);
            PC.move(lev.wvs[wvNum].brSz, dt);                     //should do this before checking for collisions
            snps.MoveSnipers(PC, lev.wvs[wvNum].nmSnps, lev.wvs[wvNum].brSz, dt);
            bombs.MoveBombs(lev.wvs[wvNum].nmBms, dt);
            sprink.rotateSprinkler();

            if (eventTimer.getTicks()-lastSprinkFire > SPRINK_SHOT_INTERVAL){
                if(sprink.isAlive){
                    if (blts.newBolt(sprink.xPos, sprink.yPos, boltSpeed, 128, PC, sprink.angle, false)){
                        //play music eventually.
                        //printf("Shot bolt\n");
                    }
                    lastSprinkFire+=SPRINK_SHOT_INTERVAL;
                }
            }

            for(int i = 0; i < lev.wvs[wvNum].nmTrs; i++){
                if(turs.isAlive[i]){
                    if (eventTimer.getTicks()-turs.lastFire[i] > TURRET_SHOT_INTERVAL){
                        if (blts.newBolt(turs.xPos[i], turs.yPos[i], boltSpeed, 128, PC, 0, true)){
                            //play sfx
                        }
                        turs.lastFire[i] += TURRET_SHOT_INTERVAL;
                    }
                }
            }

            //increase bolt speed for next frame
            if (increaseSpeed){
                boltSpeed += BOLT_SPEED_INCREASE_PER_FRAME;
                if (boltSpeed > 2000) boltSpeed = 600;         //x is max speed of bolt
            }

            //regenerate health if healthRegen flag is true
            if (healthRegen){
                PC.health += double(HEALTH_REGEN_PER_SEC)/double(FRAMES_PER_SECOND);         //gain x health per frame
                if (PC.health > 100) PC.health = 100;
                PC.sldHl=100;
            }

            //drain health to force player to pick up health packs, has flag
            if (healthDrain){
                PC.health -= double(2)/double(FRAMES_PER_SECOND);
            }
            if(PC.sldUp){
                PC.sldHl -= 0.1;
                if(PC.sldHl<0){
                    PC.sldHl = 0;
                    PC.sldUp = false;
                }
            } else {
                PC.sldHl += 0.1;
                if(PC.sldHl>100) PC.sldHl = 100;
            }
            /*
            //If a healthpack is taken, regenerate it somewhere
            for (int i = 0; i < lev.wvs[wvNum].nmPks; i++){
                if (hlPks.isActivated[i] == false){
                    hlPks.clBx[i].x = rand()%((100-HEALTH_SIZE)/2)+100/4;
                    hlPks.clBx[i].y = rand()%((100-20)/2)+100/4;
                    hlPks.xPos[i] = hlPks.clBx[i].x;
                    hlPks.yPos[i] = hlPks.clBx[i].y;

                    hlPks.isActivated[i] = true;
                }
            }*/

            tdcSpots.Rearrange(FRAME_TIME, lev.wvs[wvNum].nmSpts, lev.wvs[wvNum].brSz, dt);
            tdcSpots.Activate(lev.wvs[wvNum].nmSpts);

            snps.SetTimes(lev.wvs[wvNum].nmSnps);
            snps.HandleFiring(PC, lev.wvs[wvNum].nmSnps);
            //handle firing the real way here, fuck classes
            FireSnipes(&snps, &PC, lev.wvs[wvNum].nmSnps);

            //we might need to set new destinations for our bombs
            for (int i = 0; i < lev.wvs[wvNum].nmBms; i++){
                if (bombs.timeLeft[i] >= 0){             //bomb hasn't exploded yet
                    if (bombs.nextMove[i] <= 0){
                        //set new destination/velocities
                        bombs.xDest[i] = rand()%(7*(100-lev.wvs[wvNum].brSz)/8);
                        bombs.yDest[i] = rand()%(7*(100-lev.wvs[wvNum].brSz)/8);
                        bombs.nextMove[i] = rand()%1000 + 500;      //make next move 500-1500 ms later.
                    }
                    bombs.timeLeft[i] -= FRAME_TIME;
                    bombs.nextMove[i] -= FRAME_TIME;
                }
                //now reduce the amount of explosion time left
                if(bombs.exploding[i]){
                    if (bombs.explodingTime[i] >= 0){
                        bombs.explodingTime[i] -= FRAME_TIME;
                    } else {
                        bombs.isAlive[i] = false;
                    }
                }
            }

            ///Check collisions with everything except the player character or the shield.
            for(int i=0; i<lev.wvs[wvNum].nmHnts; i++){
                for (int j = i+1; j < lev.wvs[wvNum].nmHnts; j++){      //we go through the remaining hunters
                    if (CheckCollision(hnts.hunters[i], hnts.hunters[j])){
                        if (hnts.collidedTime[i] < 0 || hnts.collidedTime[j] < 0){
                            hnts.handleCollision(i, j);     //they need to set some things, so they're special
                        }
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmSnps; j++){
                    if(CheckCollision(hnts.hunters[i], snps.clBx[j])){
                        SwitchVel(&hnts.xVel[i], &hnts.yVel[i], &hnts.xPos[i], &hnts.yPos[i], &snps.xVel[j], &snps.yVel[j], &snps.xPos[j], &snps.yPos[j]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmBms; j++){
                    if(CheckCollision(hnts.hunters[i], bombs.clBx[j])){
                        SwitchVel(&hnts.xVel[i], &hnts.yVel[i], &hnts.xPos[i], &hnts.yPos[i], &bombs.xVel[j], &bombs.yVel[j], &bombs.xPos[j], &bombs.yPos[j] );
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmTrs; j++){
                    if(CheckCollision(hnts.hunters[i], turs.clBx[j])){
                        SwitchVel(&hnts.xVel[i], &hnts.yVel[i], &hnts.xPos[i], &hnts.yPos[i]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmPls; j++){
                    if(CheckCollision(hnts.hunters[i], plls.clBx[j])){
                        SwitchVel(&hnts.xVel[i], &hnts.yVel[i], &hnts.xPos[i], &hnts.yPos[i]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmSpnk; j++){
                    if(CheckCollision(hnts.hunters[i], sprink.clBx)){
                        SwitchVel(&hnts.xVel[i], &hnts.yVel[i], &hnts.xPos[i], &hnts.yPos[i]);
                    }
                }
            }
            for(int i=0; i<lev.wvs[wvNum].nmSnps; i++){
                for(int j=i+1; j<lev.wvs[wvNum].nmSnps; j++){
                    if(CheckCollision(snps.clBx[i], snps.clBx[j])){
                        SwitchVel(&snps.xVel[i], &snps.yVel[i], &snps.xPos[i], &snps.yPos[i], &snps.xVel[j], &snps.yVel[j], &snps.xPos[j], &snps.yPos[j]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmBms; j++){
                    if(CheckCollision(snps.clBx[i], bombs.clBx[j])){
                        SwitchVel(&snps.xVel[i], &snps.yVel[i], &snps.xPos[i], &snps.yPos[i], &bombs.xVel[j], &bombs.yVel[j], &bombs.xPos[j], &bombs.yPos[j]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmTrs; j++){
                    if(CheckCollision(snps.clBx[i], turs.clBx[j])){
                        SwitchVel(&snps.xVel[i], &snps.yVel[i], &snps.xPos[i], &snps.yPos[i]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmPls; j++){
                    if(CheckCollision(snps.clBx[i], plls.clBx[j])){
                        SwitchVel(&snps.xVel[i], &snps.yVel[i], &snps.xPos[i], &snps.yPos[i]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmSpnk; j++){
                    if(CheckCollision(snps.clBx[i], sprink.clBx)){
                        SwitchVel(&snps.xVel[i], &snps.yVel[i], &snps.xPos[i], &snps.yPos[i]);
                    }
                }
            }
            for(int i=0; i<lev.wvs[wvNum].nmBms; i++){
                for(int j=i+1; j<lev.wvs[wvNum].nmBms; j++){
                    if(CheckCollision(bombs.clBx[i], bombs.clBx[j])){
                        SwitchVel(&bombs.xVel[i], &bombs.yVel[i], &bombs.xPos[i], &bombs.yPos[i], &bombs.xVel[j], &bombs.yVel[j], &bombs.xPos[j], &bombs.yPos[j]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmTrs; j++){
                    if(CheckCollision(bombs.clBx[i], turs.clBx[j])){
                        SwitchVel(&bombs.xVel[i], &bombs.yVel[i], &bombs.xPos[i], &bombs.yPos[i]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmPls; j++){
                    if(CheckCollision(bombs.clBx[i], plls.clBx[j])){
                        SwitchVel(&bombs.xVel[i], &bombs.yVel[i], &bombs.xPos[i], &bombs.yPos[i]);
                    }
                }
                for(int j=0; j<lev.wvs[wvNum].nmSpnk; j++){
                    if(CheckCollision(bombs.clBx[i], sprink.clBx)){
                        SwitchVel(&bombs.xVel[i], &bombs.yVel[i], &bombs.xPos[i], &bombs.yPos[i]);
                    }
                }
            }
            //We don't need to go through the inamimate objects, eg. turrets,
            //because they don't collide with other inanimate objects

            //each enemy type has a flag indicating if they do damage or not.
            ///check collisions with Player Character
            if (healthDrain) lastHit = "Drain";

            for (int i = 0; i < 128; i++){
                if (blts.isAlive[i] == true){
                    if(PC.sldUp){
                        if(CheckCollision(PC.sldBx, blts.clBx[i])){
                            PC.sldHl-=BLT_DAM;
                            blts.isAlive[i]=false;
                        }
                    } else {
                        if(CheckCollision(PC.clBx, blts.clBx[i])){
                            if (boltsDoDamage) PC.health -= BLT_DAM;
                            blts.isAlive[i] = false;        //bolt can only hit once
                            hitDelay = true;
                            PC.hit = true;
                            PC.hitDelay = PC_HIT_CHANGE;
                            lastHit = "Bolts";
                            boltTotHits += 1;
                            curPosStreak = 0;
                            curNegStreak += 1;
                            hit = true;
                        }
                    }
                }
            }
            for (int i = 0; i < lev.wvs[wvNum].nmHnts; i++){
                hnts.damageCooldown[i] -= FRAME_TIME;
                if (hnts.damageCooldown[i]<0){
                    if(PC.sldUp){
                        if(CheckCollision(PC.sldBx, hnts.hunters[i])){
                            PC.sldHl-=HUNT_DAM;
                            if(huntersBouncePC){
                                hnts.xVel[i]*=-1;
                                hnts.yVel[i]*=-1;
                            }
                        }
                    } else {
                        if(CheckCollision(PC.clBx, hnts.hunters[i])){
                            PC.health -= HUNT_DAM;
                            hnts.damageCooldown[i] = HUNTER_COOLDOWN_TIME;
                            hitDelay = true;
                            PC.hit = true;
                            PC.hitDelay = PC_HIT_CHANGE;
                            lastHit = "Hunters";
                            huntTotHits += 1;
                            curPosStreak = 0;
                            curNegStreak += 1;
                            hit = true;
                            if (huntersBouncePC){       //they bounce back after hitting the PC
                                //flip the velocity of the hunter.
                                hnts.xVel[i] *= -1;
                                hnts.yVel[i] *= -1;
                            }
                        }
                    }
                }
            }
            for (int i = 0; i < lev.wvs[wvNum].nmSpts; i++){
                if (CheckCollision(PC.clBx, tdcSpots.clBx[i])){
                    if (burnSpots){
                        if (tdcSpots.isActive[i]){
                            PC.health -= (float)7.0*FRAMES_PER_SECOND/1000;              //this happens each frame so don't be in the spot.
                            PC.hit = true;
                            PC.hitDelay = PC_HIT_CHANGE;
                            lastHit = "Burn";
                            burnTotDam += 7.0;
                            curPosStreak = 0;
                            //curNegStreak += 1;        threw off count too much.
                            hit = true;
                        }
                    }
                }
            }
            //eventually, we need to have the shield stop the player from taking damage here.
            for (int i = 0; i < lev.wvs[wvNum].nmBms; i++){
                //if the player is in the explosion radius.
                if ((PC.trueX-bombs.xPos[i])*(PC.trueX-bombs.xPos[i]) + (PC.trueY-bombs.yPos[i])*(PC.trueY-bombs.yPos[i]) < BOMB_RADIUS*BOMB_RADIUS){
                    if(!bombs.exploding[i]){        //only damage the player once
                        //I'll probably make it so that the explosion is not immediate, and more damaging
                        //the closer to the bomb the player is.
                        if (bombsDoDamage){ //20 is just a stop gap number. DOn't want to tweak yet.
                            if(PC.sldUp){
                                PC.sldHl-=20;
                            }else{
                                PC.health-=20;
                            }
                        }
                        bombs.exploding[i] = true;
                        bombs.explodingTime[i] = 200;
                    }
                }
                //we only get here if the PC is not damaged.
                if (bombs.timeLeft[i] <= 0){     //alternatively, some bombs time runs out
                    if(bombs.isAlive[i]){
                        if (!bombs.exploding[i]) {
                            bombs.explodingTime[i] = 200;
                            bombs.exploding[i] = true;
                        }
                    }
                }
            }
            ///Check if the sword has hit anything.
            //Check if the sword has hit a turret
            for (int i=0; i<lev.wvs[wvNum].nmTrs; i++){
                if(PC.swing > 0){
                    if(turs.health[i]>0&&turs.isAlive[i]){
                        if (CheckCollision(PC.atk, turs.clBx[i])){
                            //PlaySFX(chnls, pTrDth);
                            turs.health[i] = -1;
                            turs.isAlive[i] = false;
                        }
                    }
                }
            }

            //Check if sword has hit hunters
            for (int i=0; i<lev.wvs[wvNum].nmHnts; i++){
                if(PC.swing > 0){
                    if(hnts.hlth[i] > 0){
                        if (CheckCollision(PC.atk, hnts.hunters[i])){
                            //PlaySFX(chnls, pHtScm);
                            hnts.hlth[i] = -1;
                        }
                    }
                }
            }
            for(int i=0; i<lev.wvs[wvNum].nmSnps; i++){
                if(PC.swing > 0){
                    if(snps.isAlive[i]){
                        if(CheckCollision(PC.atk, snps.clBx[i])){
                            snps.isAlive[i] = false;
                        }
                    }
                }
            }
            for(int i=0; i<lev.wvs[wvNum].nmSpnk; i++){
                if(PC.swing>0){
                    if(sprink.isAlive){
                        if(CheckCollision(PC.atk, sprink.clBx)){
                            sprink.isAlive = false;
                        }
                    }
                }
            }
            for (int i = 0; i < lev.wvs[wvNum].nmPks; i++){
                if (CheckCollision(PC.clBx, hlPks.clBx[i])){
                    PC.health += HEALTH;
                    if (PC.health > 100) PC.health = 100;
                    hlPks.isActivated[i] = false;
                    hitDelay = true;
                    //playerScore += 1000;
                    healthTotal += 1;
                    curPosStreak += 1;
                    curNegStreak = 0;
                }
            }

            if (curPosStreak > highPosStreak) highPosStreak = curPosStreak;
            if (curNegStreak > highNegStreak) highNegStreak = curNegStreak;
            if ((eventTimer.getTicks()-lastClean)/1000 > highClean){
                highClean = (eventTimer.getTicks()-lastClean)/1000;
            }
            if (hit){
                lastClean = eventTimer.getTicks();
                hit = false;
            }

            PC.hitDelay -= FRAME_TIME;
            if (PC.hitDelay <= 0) PC.hit = false;

            if (PC.health <= 0){
                *pState = MENU;     //I still need to make a game over screen.
                *pMenuState = OUTRO;
            }
        }

        //all characters have flags to render or not
        ///Render
        //again, find a way to render the whole screen white
        //SDL_FillRect(pFrame, &pFrame->clip_rect, SDL_MapRGB(pFrame->format, 255,255,255));

        SDL_SetRenderDrawColor(pFrame, 255, 255, 255, 255);
        //SDL_SetRenderDrawColor(pFrame, 0, 0, 255, 255);
        //render spiketraps
        SDL_RenderClear(pFrame);

        if (renderTraps){
            for (int i = 0; i < NUM_SPIKETRAPS; i++){
                if (tdcTraps.timeToReactive[i] == 0){
                    ApplyTexture(pSpikeImg, pFrame, NULL, tdcTraps.xPos[i], tdcTraps.yPos[i]);
                }
            }
        }

        //render sniper
        if (renderSniper){
            if(snpr.health[0] > 0){
                ScaleApplyTexture(pSniperImg, pFrame, NULL, snpr.xPos, snpr.yPos, SNIPER_SIZE);
            }
        }

        //render player character
        if (renderPC){
            //rendering PC and sword.
            PC.angle = GetPCAngle(msX-PC.clBx.x, msY-PC.clBx.y);

            SDL_Point centerPC = {PC.rBx.w/2, PC.rBx.h/2};
            SDL_Point centSwrd = {PC.rAtk.w/2, PC.rAtk.h/2};
            SDL_RenderCopyEx(pFrame, pPCImg, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);
            //SDL_RenderCopyEx(pFrame, pBlk, NULL, &PC.rBx, PC.angle, &centerPC, SDL_FLIP_NONE);
            if(PC.swing > 0){
                //render at proper size, possibly means giving sword its own attack box
                SDL_RenderCopyEx(pFrame, pSwrd, NULL, &PC.rAtk, PC.angle, &centSwrd, SDL_FLIP_NONE);
                PC.swing -= FRAME_TIME;
            }
            if(PC.sldUp){
                SDL_RenderCopy(pFrame, pSld, NULL, &PC.shld);
            }
            //render points for help with aiming
            float xPos=PC.rBx.x+PC_SIZE/2*dt.scl;
            float yPos=PC.rBx.y+PC_SIZE/2*dt.scl;
            int gpSz=5*dt.scl;
            while(true){
                xPos+=sin(PC.angle/57.2957795)*gpSz;
                yPos-=cos(PC.angle/57.2957795)*gpSz;
                if(xPos<dt.xPush || xPos>dt.SW-dt.xPush)break;
                if(yPos<0 || yPos>dt.SH)break;
                ScaleApplyTexture(pBlk, pFrame, NULL, xPos, yPos, 0.6*dt.scl);
                gpSz*=1.1;
            }
        }

        //render hunters
        if(renderHunters){
            for (int i=0; i<lev.wvs[wvNum].nmHnts; i++){
                for (int i = 0; i < lev.wvs[wvNum].nmHnts; i++){
                    SDL_Point cntHnt = {hnts.rBx[i].w/2, hnts.rBx[i].h/2};
                    if (hnts.hlth[i] > 0){
                        SDL_RenderCopyEx(pFrame, pHunterImg, NULL, &hnts.rBx[i], hnts.angle[i], &cntHnt, SDL_FLIP_NONE);
                        //SDL_RenderCopyEx(pFrame, pBlk, NULL, &hnts.rBx[i], hnts.angle[i], &cntHnt, SDL_FLIP_NONE);
                    } else {
                        SDL_RenderCopyEx(pFrame, pHntDead, NULL, &hnts.rBx[i], hnts.angle[i], &cntHnt, SDL_FLIP_NONE);
                    }
                }
            }
        }

        //render bolts
        if (renderBolts){
            for(int i=0; i<128; i++){
                if (blts.isAlive[i]){
                    blts.angle[i] = FindAngle(blts.yVel[i], blts.xVel[i]);
                    SDL_Point cntBlt = {BOLT_SIZE/2*dt.scl, BOLT_SIZE/2*dt.scl};
                    SDL_RenderCopyEx(pFrame, pBlt, NULL, &blts.rBx[i], blts.angle[i], &cntBlt, SDL_FLIP_NONE);
                }
            }
        }

        //render healthpack(s)
        if (renderHealthPacks){
            for (int i = 0; i < lev.wvs[wvNum].nmPks; i++){
                if (hlPks.isActivated[i] == true){
                    SDL_RenderCopy(pFrame, pHealthPacksImg, NULL, &hlPks.rBx[i]);
                }
            }
        }

        //render burn spots
        if (burnSpots){
            for (int i = 0; i < lev.wvs[wvNum].nmSpts; i++){
                if (tdcSpots.isActive[i] == true){
                    SDL_RenderCopy(pFrame, pBurnImg, NULL, &tdcSpots.rBx[i]);
                } else {
                    SDL_RenderCopy(pFrame, pPreBurn, NULL, &tdcSpots.rBx[i]);
                }
            }
        }

        if (renderBombs){
            for (int i = 0; i < lev.wvs[wvNum].nmBms; i++){/*
                if(i==2){
                    printf("Pos1: %i,%i\n", bombs.rBx[0].x, bombs.rBx[0].y);
                    printf("Pos2: %i,%i\n", bombs.rBx[1].x, bombs.rBx[1].y);
                    printf("Pos3: %i,%i\n", bombs.rBx[i].x, bombs.rBx[i].y);
                }*/
                if (bombs.isAlive[i]){
                    if (bombs.exploding[i]){
                        if (bombs.explodingTime[i] > 0){
                            SDL_RenderCopy(pFrame, pBombExplode, NULL, &bombs.exBx[i]);
                        }
                    } else {
                        SDL_RenderCopy(pFrame, pBombImg, NULL, &bombs.rBx[i]);
                    }
                }
            }
        }

        //eventually going to have lots of different flags for cooldown or fleeing etcetera.
        //also, snipers are a special case for their angle.
        if (renderSnipes){
            for (int i = 0; i < lev.wvs[wvNum].nmSnps; i++){
                SDL_Point cntSnp = {SNIPES_SIZE/2*dt.scl, SNIPES_SIZE/2*dt.scl};
                if (snps.isAlive[i]){
                    if (snps.fleeTime[i] > 0){    //if they're fleeing, use velocity to find angle
                        if (snps.xVel[i] != 0 || snps.yVel != 0){
                            snps.angle[i] = FindAngle(snps.yVel[i], snps.xVel[i]);
                        }
                    } else {    //they're pointing at the player.
                        snps.angle[i] = FindAngle(PC.clBx.y-snps.yPos[i], PC.clBx.x-snps.xPos[i]);
                    }

                    if(snps.coolDown[i]>0){
                        SDL_RenderCopyEx(pFrame, pSnpCl, NULL, &snps.rBx[i], snps.angle[i], &cntSnp, SDL_FLIP_NONE);
                    } else {
                        if(snps.fleeTime[i]>0 || snps.chrgTime[i]>1100){
                            SDL_RenderCopyEx(pFrame, pSnp0, NULL, &snps.rBx[i], snps.angle[i], &cntSnp, SDL_FLIP_NONE);
                        } else if(snps.chrgTime[i]>800){
                            SDL_RenderCopyEx(pFrame, pSnp1, NULL, &snps.rBx[i], snps.angle[i], &cntSnp, SDL_FLIP_NONE);
                        } else if(snps.chrgTime[i]>400){
                            SDL_RenderCopyEx(pFrame, pSnp2, NULL, &snps.rBx[i], snps.angle[i], &cntSnp, SDL_FLIP_NONE);
                        } else {
                            SDL_RenderCopyEx(pFrame, pSnp3, NULL, &snps.rBx[i], snps.angle[i], &cntSnp, SDL_FLIP_NONE);
                        }
                    }
                    //Render the shot
                    if(snps.firing[i]){
                        SDL_Point imgCnt = {3, snps.rnd[i].h/2};
                        SDL_RenderCopyEx(pFrame, pSnpSht, NULL, &snps.rnd[i], snps.angle[i], &imgCnt, SDL_FLIP_NONE);
                    }
                } else {
                    SDL_RenderCopyEx(pFrame, pSnpDd, NULL, &snps.rBx[i], snps.angle[i], &cntSnp, SDL_FLIP_NONE);
                }
            }
        }
        if (renderSprinkler && lev.wvs[wvNum].nmSpnk==1){
            SDL_Point cnt = {SPRINKLER_SIZE*dt.scl/2, SPRINKLER_SIZE*dt.scl/2};
            if (sprink.isAlive){
                if (!pauseGame){
                    sprink.rotateSprinkler();       //finds angle for us.
                }
                SDL_RenderCopyEx(pFrame, pSprinkImg, NULL, &sprink.rBx, sprink.angle*57.2957795, &cnt, SDL_FLIP_NONE);
            }else{
                SDL_RenderCopyEx(pFrame, pBrokeTur, NULL, &sprink.rBx, sprink.angle*57.2957795, &cnt, SDL_FLIP_NONE);
            }
        }
        if (renderTurrets){
            for (int i=0; i<lev.wvs[wvNum].nmTrs; i++){
                SDL_Point cntTr = {TURRET_SIZE*dt.scl/2, TURRET_SIZE*dt.scl/2};
                if (turs.isAlive[i]){
                    turs.angle[i] = FindAngle(PC.clBx.y-turs.yPos[i], PC.clBx.x-turs.xPos[i]);
                    SDL_RenderCopyEx(pFrame, pTurretImg, NULL, &turs.rBx[i], turs.angle[i], &cntTr, SDL_FLIP_NONE);
                } else {
                    SDL_RenderCopyEx(pFrame, pBrokeTur, NULL, &turs.rBx[i], turs.angle[i], &cntTr, SDL_FLIP_NONE);
                }
            }
        }
        if(renderPillars){
            for(int i=0; i<lev.wvs[wvNum].nmPls; i++){
                SDL_RenderCopy(pFrame, pSld, NULL, &plls.rBx[i]);
            }
        }

        SDL_Rect msBx;
        msBx.w = 2.5*dt.scl;
        msBx.h = 2.5*dt.scl;
        msBx.x = mseX;
        msBx.y = mseY;
        SDL_RenderCopy(pFrame, pMs, NULL, &msBx);

        //render health bar.
        SDL_Rect healthBar;
        SDL_Rect healthOutline;
        healthBar.x = dt.xPush+25*dt.scl;
        healthBar.y = 2*dt.scl;
        healthBar.w = 20*dt.scl;
        healthBar.h = healthBar.w/3;
        healthOutline.x = healthBar.x - 0.5*dt.scl;
        healthOutline.y = healthBar.y - 0.5*dt.scl;
        healthOutline.w = healthBar.w + 1*dt.scl;
        healthOutline.h = healthBar.h + 1*dt.scl;
        SDL_RenderCopy(pFrame, pBlk, NULL, &healthOutline);
        SDL_RenderCopy(pFrame, pWte, NULL, &healthBar);
        healthBar.w*=(PC.health/100);
        SDL_RenderCopy(pFrame, pBlk, NULL, &healthBar);

        //render shield bar
        SDL_Rect sldBr;
        SDL_Rect sldOut;
        sldBr.x=dt.xPush+65*dt.scl;
        sldBr.y=2*dt.scl;
        sldBr.w=20*dt.scl;
        sldBr.h=sldBr.w/3;
        sldOut.x=sldBr.x-0.5*dt.scl;
        sldOut.y=sldBr.y-0.5*dt.scl;
        sldOut.w=sldBr.w+1*dt.scl;
        sldOut.h=sldBr.h+1*dt.scl;
        SDL_RenderCopy(pFrame, pBlk, NULL, &sldOut);
        SDL_RenderCopy(pFrame, pWte, NULL, &sldBr);
        sldBr.w*=(PC.sldHl/100);
        SDL_RenderCopy(pFrame, pBlk, NULL, &sldBr);
/*
        SDL_Rect sldBar;
        SDL_Rect sldOut;
        sldBar.x = (100-20*dt.scl)/2;
        sldBar.y = 80;
        sldBar.w = 200;
        sldBar.h = 25;
        sldOut.x = sldBar.x-5;
        sldOut.y = sldBar.y-5;
        sldOut.w = sldBar.w+10;
        sldOut.h = sldBar.h+10;
        SDL_RenderCopy(pFrame, pSldOut, NULL, &sldOut);
        SDL_RenderCopy(pFrame, pSldBk, NULL, &sldBar);
        sldBar.w = 200*PC.sldHl/100;
        SDL_RenderCopy(pFrame, pSldBr, NULL, &sldBar);*/

        //display the time
        SDL_Texture* pTimeText = NULL;
        SDL_Surface* pTime;
        std::stringstream time;
        time << "Time Left: " << eventTimer.getTicks()/1000-nwWvTm;
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

    /*
        SDL_Surface* pFPS = NULL;
        SDL_Texture* pFPSText = NULL;
        std::stringstream fps;
        fps << "Frames per second: " << 1000.f/(float)fpsTimer.getTicks();
        pFPS = TTF_RenderText_Solid(pFont, fps.str().c_str(), fontColour);
        pFPSText = SDL_CreateTextureFromSurface(pFrame, pFPS);
        ApplyTexture(pFPSText, pFrame, NULL, (100 - pFPS->w)/2, 100);
        SDL_FreeSurface(pFPS);
        SDL_DestroyTexture(pFPSText);   */

        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br1);
        SDL_RenderCopy(pFrame, pBlk, NULL, &dt.br2);

        SDL_RenderPresent(pFrame);

        for(int i=0; i<64; i++){
            chnls[i] -= FRAME_TIME;
        }

        if (fpsTimer.getTicks() < (int)FRAME_TIME){
            while (fpsTimer.getTicks() < (int)FRAME_TIME){
                //SDL_Delay(1);     fucking operating system fucks this.
                //break;
                static int wasteTime = 2+rand();
            }
        } else {/*
            printf("Game running too slowly\n");        //debugging
            printf("FRAME TIME: %lf\tActual Time: %i\n", FRAME_TIME, fpsTimer.getTicks());
            printf("FPS: %lf\tActual FPS: %f\n", FRAMES_PER_SECOND, (float)(1000.f/(float)fpsTimer.getTicks()));
            //return -1;
            //printf("1000/%f == %f\n", (float)fpsTimer.getTicks(), 1000.f/(float)fpsTimer.getTicks());
            printf("fpsTimer.getTicks() = %i\n", fpsTimer.getTicks());
            printf("fpsTimer.getTicks() = %i\n", fpsTimer.getTicks());
            printf("fpsTimer.getTicks() = %i\n", fpsTimer.getTicks());
        */}

        if (*pState != BATTLE){/*
            if (PC.health <= 0) printf("\nKilled by: %s\n", lastHit.c_str());
            printf("Total amount of damage taken: %i\n",
                   (huntTotHits*HUNT_DAM + boltTotHits*BLT_DAM + burnTotDam + DRAIN*(eventTimer.getTicks()/1000)) );
            printf("Health drain damage: %i\n", DRAIN*(eventTimer.getTicks()/1000));
            printf("Hunter hits: %i, Hunter damage: %i\n", huntTotHits, huntTotHits*HUNT_DAM);
            printf("Bolt hits: %i, Bolt damage: %i\n", boltTotHits, boltTotHits*128);
            printf("Burn spot damage: %i\n", burnTotDam);
            printf("Healthpacks picked up: %i, health regenerated: %i\n", healthTotal, healthTotal*HEALTH);
            printf("Longest health pack streak: %i\n", highPosStreak);
            printf("Longest damage taking streak: %i\n", highNegStreak);
            printf("Longest time without being hit: %i seconds\n", highClean);
            */
            if (*pMenuState == OUTRO) SDL_Delay(500);
        }
        if (hitDelay == true){
            SDL_Delay(0);           //make hits obvious, set to 0 right now. Sticky Friction.
            hitDelay = false;
        }
        if (firstFrame){            //gives the user 1 second after hitting play to get ready.
            firstFrame = false;
            eventTimer.pauseClk();
            SDL_Delay(1000);
            eventTimer.unpauseClk();
        }
    }

    SDL_DestroyTexture(pSpikeImg);
    SDL_DestroyTexture(pSniperImg);
    SDL_DestroyTexture(pBlt);
    SDL_DestroyTexture(pHunterImg);
    SDL_DestroyTexture(pHealthPacksImg);
    SDL_DestroyTexture(pPCHITImg);
    SDL_DestroyTexture(pPCImg);
    SDL_DestroyTexture(pBurnImg);
    SDL_DestroyTexture(pPreBurn);
    SDL_DestroyTexture(pBarrierImg);
    SDL_DestroyTexture(pBombImg);
    SDL_DestroyTexture(pBombExplode);
    SDL_DestroyTexture(pSnp0);
    SDL_DestroyTexture(pSnpCl);
    SDL_DestroyTexture(pSprinkImg);
    SDL_DestroyTexture(pTurretImg);

    TTF_CloseFont(pFont);

    SDL_DestroyTexture(pHealthBar);
    SDL_DestroyTexture(pHealthBack);
    SDL_DestroyTexture(pHealthOutline);

    SDL_SetRelativeMouseMode(SDL_FALSE);

    return 0;
}

//This function finds the endpoint of the sniper fire, for each sniper that's firing
//It also does damage to the player character here
void FireSnipes(Snipes* pSnp, PlayerCharacter* pPC, int nmSnps)
{
    for(int i=0; i<nmSnps; i++){
        if(pSnp->isAlive[i]){
            if(pSnp->firing[i]){
                bool foundPoint = false;
                tdcRct tmp;
                tmp.w = 1;
                tmp.h = 1;
                float distX = pPC->clBx.x+PC_SIZE/2 - pSnp->xPos[i];
                float distY = pPC->clBx.y+PC_SIZE/2 - pSnp->yPos[i];
                float posX = pSnp->xPos[i];
                float posY = pSnp->yPos[i];
                float slope;
                if(distX == 0){ //straight up or straight down
                    while(posY>0 && posY<100 && !foundPoint){
                        tmp.x = posX;
                        tmp.y = posY;
                        if(CheckCollision(tmp, pPC->clBx)){
                            //reached end
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                            foundPoint = true;
                            pPC->health -= TOT_SHT_FRM;     //hack for now
                        } else {
                            if(pPC->sldHl>0 && pPC->sldUp){
                                if(CheckCollision(tmp, pPC->sldBx)){
                                    pSnp->end[i].x = posX;
                                    pSnp->end[i].y = posY;
                                    foundPoint = true;
                                    pPC->sldHl -= TOT_SHT_FRM;
                                }
                            }
                        }
                        if(distY>0){
                            posY++;
                        } else{
                            posY--;
                        }
                        if(posY<=0 || posY>=100){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                        }
                        if(foundPoint){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                        }
                    }
                }
                if(distY == 0){     //straight right or straight left
                    while(posX>0 && posX<100 && !foundPoint){
                        tmp.x = posX;
                        tmp.y = posY;
                        if(CheckCollision(tmp, pPC->clBx)){
                            //reached end
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                            foundPoint = true;
                            pPC->health -= TOT_SHT_FRM;
                        } else {
                            if(pPC->sldHl>0 && pPC->sldUp){
                                if(CheckCollision(tmp, pPC->sldBx)){
                                    pSnp->end[i].x = posX;
                                    pSnp->end[i].y = posY;
                                    foundPoint = true;
                                    pPC->sldHl -= TOT_SHT_FRM;
                                }
                            }
                        }
                        if(distX>0){
                            posX++;
                        } else{
                            posX--;
                        }
                        if(posX<=0 || posX>=100){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                            foundPoint = true;
                        }
                        if(foundPoint){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                        }
                    }
                }else{      //some sort of angle
                    slope = distY/distX;
                    while(posX>0 && posX<100 && posY>0 && posY<100 && !foundPoint){
                        tmp.x = posX;
                        tmp.y = posY;
                        if(CheckCollision(tmp, pPC->clBx)){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                            foundPoint = true;
                            pPC->health -= TOT_SHT_FRM;
                        } else {
                            if(pPC->sldHl>0 && pPC->sldUp){
                                if(CheckCollision(tmp, pPC->sldBx)){
                                    pSnp->end[i].x = posX;
                                    pSnp->end[i].y = posY;
                                    foundPoint = true;
                                    pPC->sldHl -= TOT_SHT_FRM;
                                }
                            }
                        }
                        if(std::abs(slope)>=1){ //iterate by y
                            if(distY>0) posY++;
                            if(distY<0) posY--;
                            if(distX>0) posX += 1/std::abs(slope);
                            if(distX<0) posX -= 1/std::abs(slope);
                        } else {
                            if(distX>0) posX++;
                            if(distX<0) posX--;
                            if(distY>0) posY += std::abs(slope);
                            if(distY<0) posY -= std::abs(slope);
                        }
                        if(posX<=0 || posX>=100 || posY<=0 || posY>=100){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                        }
                        if(foundPoint){
                            pSnp->end[i].x = posX;
                            pSnp->end[i].y = posY;
                        }
                    }
                }
                pSnp->cnt[i].x = pSnp->xPos[i] + (pSnp->end[i].x-pSnp->xPos[i])/2;
                pSnp->cnt[i].y = pSnp->yPos[i] + (pSnp->end[i].y-pSnp->yPos[i])/2;
                pSnp->rnd[i].w = 6;
                //height is the distance of the shot.
                pSnp->rnd[i].h = std::sqrt((pSnp->xPos[i]-pSnp->end[i].x)*(pSnp->xPos[i]-pSnp->end[i].x) + (pSnp->yPos[i]-pSnp->end[i].y)*(pSnp->yPos[i]-pSnp->end[i].y));
                pSnp->rnd[i].x = pSnp->cnt[i].x;
                //pSnp->rnd[i].y = pSnp->cnt[i].y - pSnp->rnd[i].h*std::cos(pSnp->angle[i])/2;
                pSnp->rnd[i].y = pSnp->cnt[i].y - pSnp->rnd[i].h/2;
            }
        }
    }
}

void PlaySFX(int chnls[256], Mix_Chunk* pSFX)
{
    for(int i=0; i<256; i++){
        if(chnls[i]<0){
            Mix_PlayChannelTimed(i, pSFX, 0, 300);
            chnls[i] = 300;
            return;
        }
    }
}




    /*
        //test of rotation
        const static double ANGULAR_SPEED = 2*PI/3;     //takes x seconds per cycle
        double angleInc = ANGULAR_SPEED/FRAMES_PER_SECOND;
        static double testAngle = 0;
        testAngle += angleInc;
        SDL_Point test = {healthBar.w/2, healthBar.h/2};        //center of texture
        //we need to convert from radians (above) to degrees
        SDL_RenderCopyEx(pFrame, pHealthBack, NULL, &healthBar, testAngle*57.2957795, &test, SDL_FLIP_NONE);

        //need to install TTF_Fonts for SDL2
        //display the health in text form
        SDL_Surface* pTempHealth = NULL;
        std::stringstream health;
        health << "Health: " << (int)PC.health << "/" << 100;
        pTempHealth = TTF_RenderText_Solid(pFont, health.str().c_str(), fontColour);
        pHealthText = SDL_CreateTextureFromSurface(pFrame, pTempHealth);
        ApplyTexture(pHealthText, pFrame, NULL, (100 - pTempHealth->w)/2, 10);
        SDL_FreeSurface(pTempHealth);           //Again, no idea why we do this
        SDL_DestroyTexture(pHealthText);

        //display the score
        SDL_Surface* pScore = NULL;
        std::stringstream score;
        score << "Score: " << playerScore;
        pScore = TTF_RenderText_Solid(pFont, score.str().c_str(), fontColour);
        pScoreText = SDL_CreateTextureFromSurface(pFrame, pScore);
        ApplyTexture(pScoreText, pFrame, NULL, (100 - pScore->w)/2, 40);
        SDL_FreeSurface(pScore);
        SDL_DestroyTexture(pScoreText);
    */











