#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>    //I'm going to need fonts eventually
#include <SDL_mixer.h>  //for sounds
#include <string>       //going to need strings eventually
#include <cmath>        //for traps movement (sin)
#include <sstream>      //outputting health and other stuff
#include <ctime>        //for srand(time(x))
#include <cstdlib>      //for srand()
#include <fstream>      //for loading in plays, used for more later
#include "Timer.h"
#include "Helpers.h"
#include "IC_GlobalVars.h"
#include "Characters.h"

const float VERSION = 0.4;

extern const int FRAME_TIME;
extern const int FRAMES_PER_SECOND;

const int NUM_SPIKETRAPS = 64;   //used less than it should be

const double PI = 3.1415;

const int SHOT_INTERVAL = 300;  //in milliseconds

const int MAX_BOLTS = 128;

const int HEALTH_REGEN_PER_SEC = 1000;
const int HEALTH_DRAIN_PER_SEC = 2;     //6 seems perfect for me right now, subject to change
const int BOLT_DAMAGE = 10;
const int HUNTER_DAMAGE = 5;

const int SPIKE_UNACTIVE_TIME = 2000;           //in milliseconds, we decrease this by 1 until we hit 0

const int BOLT_SPEED_INCREASE_PER_FRAME = 100/FRAMES_PER_SECOND;  //in pixels per second

const int HUNTER_COOLDOWN_TIME = 400;       //400 ms before it can hurt again. Poorly titled.
const int NUM_HUNTERS = 3;

const int HEALTH_PACK_HEALTH = 4;       //amount the Health Packs heal for.

const int NUM_HEALTH_PACKS = 4;         //at any one time. As soon as 1 is picked up, another takes its place.

const int TIME_FOR_GAME = 60;       //x seconds

const int BURN_SPOT_TIME = 4;       //x seconds.
const int NUM_SPOTS = 16;           //x burn spots on the ground at once.


int BattleState(gameStates* pStates, SDL_Surface* pFrame)
{
    //flags for debugging
    bool pauseGame = false;     //press SPACE to pause or unpause game.

    bool increaseSpeed = false;         //for right now, I'm not increasing the speed of bolts

    bool trapsDoDamage = false;
    bool boltsDoDamage = true;
    bool huntersDoDamage = true;

    bool renderBolts = true;
    bool renderTraps = false;
    bool renderPC = true;
    bool renderSniper = true;
    bool renderHunters = true;
    bool renderHealthPacks = true;
    bool renderSpots = true;

    bool healthRegen = false;
    bool healthDrain = true;

    bool huntersBounce = true;

    bool burnSpots = true;

    bool musicPlaying = false;

    ///Set up visuals
    SDL_Surface* pSpikeImg = NULL;
    SDL_Surface* pSniperImg = NULL;
    SDL_Surface* pSniperBoltImg = NULL;
    SDL_Surface* pHunterImg = NULL;
    SDL_Surface* pHealthPacksImg = NULL;
    SDL_Surface* pPCImg = NULL;
    SDL_Surface* pBurnImg = NULL;
    SDL_Surface* pPreBurn = NULL;

    TTF_Font* pFont = NULL;
    SDL_Surface* pHealthText = NULL;
    SDL_Surface* pScoreText = NULL;
    SDL_Surface* pTimeText = NULL;

    pSpikeImg = LoadAndOptimizeImage("tempSpikeTrap.bmp");
    if (pSpikeImg == NULL) return 1;

    pSniperImg = LoadAndOptimizeImage("tempSniper.bmp");
    if (pSniperImg == NULL) return 1;

    pSniperBoltImg = LoadAndOptimizeImage("tempSniperBolt.bmp");
    if (pSniperImg == NULL) return 1;

    pPCImg = LoadAndOptimizeImage("tempPC.bmp");
    if (pPCImg == NULL) return 1;

    pHunterImg = LoadAndOptimizeImage("tempHunter.bmp");
    if (pHunterImg == NULL) return 1;

    pHealthPacksImg = LoadAndOptimizeImage("tempHealthPack.bmp");
    if (pHealthPacksImg == NULL) return 1;

    pBurnImg = LoadAndOptimizeImage("tempBurnSpot.bmp");
    if (pBurnImg == NULL) return 1;

    pPreBurn = LoadAndOptimizeImage("tempPreBurn.bmp");
    if (pPreBurn == NULL) return 1;

    pFont = TTF_OpenFont("lazyFont.ttf", 36);
    if (pFont == NULL) return 1;

    SDL_Color fontColour = {192, 192, 192};     //used for health display

    /*  This is just reference for right now. I'm going to have more.
    //Loading the Sounds
    Mix_Chunk* pHighFX = NULL;
    Mix_Chunk* pMedFX = NULL;
    Mix_Chunk* pLowFX = NULL;
    Mix_Chunk* pScratchFX = NULL;

    pHighFX = Mix_LoadWAV("high.wav");
    pMedFX = Mix_LoadWAV("medium.wav");
    pLowFX = Mix_LoadWAV("low.wav");
    pScratchFX = Mix_LoadWAV("scratch.wav");

    Mix_Music* pDrumBeat = NULL;
    pDrumBeat = Mix_LoadMUS("beat.wav");
    */

    Mix_Chunk* pBoltSnd = NULL;
    pBoltSnd = Mix_LoadWAV("C:\\Users\\Timothy\\Documents\\Programming\\SDL projects\\Infinite Circles\\Infinite Circles - The Game\\Sounds\\plasmaShot.wav");
    if (pBoltSnd == NULL) return 1;

    Mix_Chunk* pTest = NULL;
    pTest = Mix_LoadWAV("C:\\Users\\Timothy\\Documents\\Programming\\SDL projects\\Infinite Circles\\Infinite Circles - The Game\\Sounds\\slice.wav");
    if (pTest == NULL) return 1;

    Mix_Music* pMsc = NULL;
    pMsc = Mix_LoadMUS("C:\\Users\\Timothy\\Documents\\Programming\\SDL projects\\Infinite Circles\\Infinite Circles - The Game\\Sounds\\song.wav");
    if (pMsc == NULL) return 1;

    //Mix_Chunk* p


    ///Set up logic.
    Timer fpsTimer;
    SDL_Event eventStr;
    Timer eventTimer;       //does not restart every frame
    eventTimer.startClk();
    int playerScore = 0;

    bool hitDelay = false;

    PlayerCharacter PC;     //basically just moves around the screen.

    SpikeTraps tdcTraps;
    SDL_Rect trapColBox;    //stores the offsets of the spike trap for collision detection.
    trapColBox.w = 20;
    trapColBox.h = 20;

    Sniper wesleySnipes;
    int lastSniperFire = 0; //minus total time from this to check for fire

    double boltSpeed = 250;         //starts off at x pixels per second

    SniperBolts tdcBolts;           //an empty array of 8 to start

    Hunters tdcHunters;             //+1 to Halo:CE
    SDL_Rect hunters[NUM_HUNTERS];
    for (int i = 0; i < NUM_HUNTERS; i++){
        hunters[i].w = 20;
        hunters[i].h = 20;
    }

    HealthPacks tdcHealthPacks;

    BurnSpots tdcSpots(NUM_SPOTS);

    srand(time(0));

    bool firstFrame = true;

    while (*pStates == BATTLE)
    {
        if (!musicPlaying){
            if (Mix_PlayMusic(pMsc, -1) == -1) return 1;
            musicPlaying = true;
        }

        fpsTimer.startClk();

        //Only event we handle right now is quitting.
        while (SDL_PollEvent(&eventStr))
        {
            if (eventStr.type == SDL_QUIT){
                *pStates = OUTRO;
            } else if (eventStr.type == SDL_KEYDOWN){
                if (eventStr.key.keysym.sym == SDLK_ESCAPE){
                    *pStates = MENU;
                } else if (eventStr.key.keysym.sym == SDLK_SPACE){
                    pauseGame = (!pauseGame);
                }
            }
            PC.handleInput(&eventStr);      //takes movement input
        }
        ///do logic

        if (pauseGame){
            eventTimer.pauseClk();
        } else {
            eventTimer.unpauseClk();
        }

        if (!pauseGame && !firstFrame){

        playerScore += 1;

        wesleySnipes.moveSniper(eventTimer.getTicks());
        //move the characters to the right place
        tdcTraps.moveTraps(eventTimer.getTicks());
        tdcBolts.moveBolts();
        tdcHunters.moveHunters(PC);
        PC.move(&tdcTraps);                     //should do this before checking for collisions

        //fire sniper shot if 200ms has passed
        if (eventTimer.getTicks()-lastSniperFire > SHOT_INTERVAL){
            if (tdcBolts.newBolt(wesleySnipes.xPos, wesleySnipes.yPos, PC, boltSpeed)){
                //if (Mix_PlayChannel(1, pBoltSnd, 0) == -1) return 1;
                if (Mix_PlayChannelTimed(-1, pTest, 0, SHOT_INTERVAL-50) == -1) return 1;
            }
            lastSniperFire+= SHOT_INTERVAL;
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
        }

        //drain health to force player to pick up health packs, has flag
        if (healthDrain){
            PC.health -= double(HEALTH_DRAIN_PER_SEC)/double(FRAMES_PER_SECOND);
        }

        //hunters check collisions with each other
        for (int i = 0; i < NUM_HUNTERS; i++){
            hunters[i].x = tdcHunters.xPos[i];
            hunters[i].y = tdcHunters.yPos[i];
        }
        for (int i = 0; i < NUM_HUNTERS-1; i++){
            for (int j = i+1; j < NUM_HUNTERS; j++){      //we go through the remaining hunters
                if (CheckCollision(hunters[i], hunters[j])){
                    if (huntersBounce){
                        if (tdcHunters.collidedTime[i] < 0 || tdcHunters.collidedTime[j] < 0){
                            tdcHunters.handleCollision(i, j);
                            //if (Mix_PlayChannel(-1, pBoltSnd, 0) == -1) return 1;
                        }
                    }
                }
            }
        }

        //If a healthpack is taken, regenerate it somewhere
        for (int i = 0; i < NUM_HEALTH_PACKS; i++){
            if (tdcHealthPacks.isActivated[i] == false){
                tdcHealthPacks.collBox[i].x = rand()%((SCREEN_WIDTH-20)/2)+SCREEN_WIDTH/4;
                tdcHealthPacks.collBox[i].y = rand()%((SCREEN_HEIGHT-20)/2)+SCREEN_HEIGHT/4;
                tdcHealthPacks.xPos[i] = tdcHealthPacks.collBox[i].x;
                tdcHealthPacks.yPos[i] = tdcHealthPacks.collBox[i].y;

                tdcHealthPacks.isActivated[i] = true;
            }
        }

        tdcSpots.Rearrange(FRAME_TIME, NUM_SPOTS);
        tdcSpots.Activate(NUM_SPOTS);

        //each enemy type has a flag indicating if they do damage or not.
        //check collisions with Player Character
        for (int i = 0; i < NUM_SPIKETRAPS; i++){
            tdcTraps.timeToReactive[i] -= 1*(1000/FRAMES_PER_SECOND);
            if (tdcTraps.timeToReactive[i] < 0) tdcTraps.timeToReactive[i] = 0;

            trapColBox.x = tdcTraps.xPos[i];
            trapColBox.y = tdcTraps.yPos[i];
            if (tdcTraps.timeToReactive[i] == 0){
                if (CheckCollision(PC.collBox, trapColBox)){
                    if (trapsDoDamage) PC.health -= 40;
                    tdcTraps.timeToReactive[i] = SPIKE_UNACTIVE_TIME;
                    hitDelay = true;
                }
            }
        }
        for (int i = 0; i < MAX_BOLTS; i++){
            if (tdcBolts.isAlive[i] == true){
                if (CheckCollision(PC.collBox, tdcBolts.collBox[i])){
                    if (boltsDoDamage) PC.health -= BOLT_DAMAGE;
                    tdcBolts.isAlive[i] = false;        //bolt can only hit once
                    hitDelay = true;
                }
            }
        }
        for (int i = 0; i < NUM_HUNTERS; i++){
            tdcHunters.damageCooldown[i] -= FRAME_TIME;
            if (CheckCollision(PC.collBox, hunters[i])){
                if (huntersDoDamage){
                    if (tdcHunters.damageCooldown[i] < 0){
                        PC.health -= HUNTER_DAMAGE;
                        tdcHunters.damageCooldown[i] = HUNTER_COOLDOWN_TIME;
                        hitDelay = true;
                    }
                }
            }
        }
        for (int i = 0; i < NUM_SPOTS; i++){
            if (CheckCollision(PC.collBox, tdcSpots.collBox[i])){
                if (burnSpots){
                    if (tdcSpots.isActive[i]) PC.health -= 1;             //this happens each frame so don't be in the spot.
                }
            }
        }
        for (int i = 0; i < NUM_HEALTH_PACKS; i++){
            if (CheckCollision(PC.collBox, tdcHealthPacks.collBox[i])){
                PC.health += HEALTH_PACK_HEALTH;
                if (PC.health > 100) PC.health = 100;
                tdcHealthPacks.isActivated[i] = false;
                hitDelay = true;
                playerScore += 1000;
            }
        }

        if (PC.health <= 0){
            *pStates = OUTRO;     //I still need to make a game over screen.
        }

        }

        //all characters have flags to render or not
        ///Render
        SDL_FillRect(pFrame, &pFrame->clip_rect, SDL_MapRGB(pFrame->format, 255,255,255));

        //render spiketraps
        if (renderTraps){
            for (int i = 0; i < NUM_SPIKETRAPS; i++){
                if (tdcTraps.timeToReactive[i] == 0){
                    ApplySurface(pSpikeImg, pFrame, NULL, tdcTraps.xPos[i], tdcTraps.yPos[i]);
                }
            }
        }

        //render sniper
        if (renderSniper){
            ApplySurface(pSniperImg, pFrame, NULL, wesleySnipes.xPos, wesleySnipes.yPos);
        }

        //render player character
        if (renderPC){
            ApplySurface(pPCImg, pFrame, NULL, PC.collBox.x, PC.collBox.y);
        }

        //render bolts
        if (renderBolts){
            for (int i = 0; i < MAX_BOLTS; i++){
                if (tdcBolts.isAlive[i] == true){
                    ApplySurface(pSniperBoltImg, pFrame, NULL, tdcBolts.xPos[i], tdcBolts.yPos[i]);
                }
            }
        }

        //render hunters
        if (renderHunters){
            for (int i = 0; i < NUM_HUNTERS; i++){
                ApplySurface(pHunterImg, pFrame, NULL, tdcHunters.xPos[i], tdcHunters.yPos[i]);
            }
        }

        //render healthpack(s)
        if (renderHealthPacks){
            for (int i = 0; i < NUM_HEALTH_PACKS; i++){
                if (tdcHealthPacks.isActivated[i] == true){
                    ApplySurface(pHealthPacksImg, pFrame, NULL, tdcHealthPacks.xPos[i], tdcHealthPacks.yPos[i]);
                }
            }
        }

        //render burn spots
        if (burnSpots){
            for (int i = 0; i < NUM_SPOTS; i++){
                if (tdcSpots.isActive[i] == true){
                    ApplySurface(pBurnImg, pFrame, NULL, tdcSpots.xPos[i], tdcSpots.yPos[i]);
                } else {
                    ApplySurface(pPreBurn, pFrame, NULL, tdcSpots.xPos[i], tdcSpots.yPos[i]);
                }
            }
        }

        //display the health in text form
        std::stringstream health;
        health << "Health: " << (int)PC.health << "/" << 100;
        pHealthText = TTF_RenderText_Solid(pFont, health.str().c_str(), fontColour);
        ApplySurface(pHealthText, pFrame, NULL, (SCREEN_WIDTH - pHealthText->w)/2, 10);
        SDL_FreeSurface(pHealthText);           //Again, no idea why we do this

        //display the score
        std::stringstream score;
        score << "Score: " << playerScore;
        pScoreText = TTF_RenderText_Solid(pFont, score.str().c_str(), fontColour);
        ApplySurface(pScoreText, pFrame, NULL, (SCREEN_WIDTH - pScoreText->w)/2, 40);
        SDL_FreeSurface(pScoreText);

        //display the time
        std::stringstream time;
        time << "Time Left: " << TIME_FOR_GAME - eventTimer.getTicks()/1000;
        pTimeText = TTF_RenderText_Solid(pFont, time.str().c_str(), fontColour);
        ApplySurface(pTimeText, pFrame, NULL, (SCREEN_WIDTH - pTimeText->w)/2, 70);
        SDL_FreeSurface(pTimeText);

        if (TIME_FOR_GAME - eventTimer.getTicks()/1000 <= 0) *pStates = OUTRO;      //game is on timer.

        if (SDL_Flip(pFrame) == -1) *pStates = QUIT;

        if (fpsTimer.getTicks() < FRAME_TIME){
            SDL_Delay(FRAME_TIME - fpsTimer.getTicks());
        }
        if (*pStates == OUTRO || *pStates == QUIT){
            SDL_Delay(1000);
        }
        if (hitDelay == true){
            SDL_Delay(0);           //make hits obvious, set to 0 right now. Sticky Friction.
            hitDelay = false;
        }
        if (firstFrame){            //gives the user 1 second after hitting play to get ready.
            firstFrame = false;
            SDL_Delay(1000);
        }
    }

}


