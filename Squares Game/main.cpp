/***************************************************************
Here goes. October 6, 2014. Trying to recreate the game, initially
called Infinite Circles, in SDL2. Shouldn't be too hard, since
most of the game logic is the same, it's just the rendering and
initialization that's different.
Wow. Just realized that I was modifying the original code. It will
take work to unchange that now.

Okay. So I got it up and running. I also improved the PC movement
code, as well as the hunters logic code. It's missing TTF_Fonts
and sound though, so a lot of the fun of the game is missing.
I definitely need to get that back in the game before

Got the fonts back in the game. Still haven't downloaded SDL_mixer,
but I did fix the pause bug at the start of the game. The pause
function now works completely perfect.

Just added a feature where the hunters bounced back after hitting
the player. Don't really love it. I'm surprised at how easily I
implemented it. Two lines of code to flip the velocity.

Added a visual change for when the PC is hit by anything.

I've been having a ton of issues with this horrible bug that slows
everything down. I'm convinced it has something to do with my
timers. I'm writing a high performance timer now.
Nevermind, the high performance timer was a flop. I probably
could have gotten it working correctly. What I did instead was
actually initialize SDL_Timer, in my Init() function. I don't
know why everything worked without me doing that. Hopefully that
solves this problem that keeps creeping up on me.

I think I may have zeroed in on the random slowdown problem that
keeps cropping up. I think SDL2 sets vsync by default, and then
it slows the whole program down to 60 frames per second. That's
why the faster I set the program, the slower everything happened
logically, but also why it never slowed framerate wise. I could
make a little counter that updates once a second with the framerate,
which increments every time a frame is done. I could also just
manually disable vsync.
Tried it and nope, doesn't work.
So I added a little printf that showed framerate. It's a rock solid
100 fps. When I pushed up to 5 ms per frame, it's still a rock solid
200 fps. This is extremely frustrating. It's like the logic can't
run that fast, except most of the time it does.
I did notice that, while it is calculating framerate correctly,
it's supposed to display the framerate every second. Instead, I
get obviously varied update frequencies. Maybe this is a windows
operating system thing? That wouldn't explain why the gameclock
for the time left is working properly though. Maybe SDL_Delay()
does something finicky?

Okay at this point I have this figured out. It's extremely frustrating
that I need to waste so much CPU time, but in order to not have
my program stupidly scheduled by the OS, I need to not sleep and
instead just waste CPU time.

Added rotating images.

Added SDL_mixer to the game. Turns out there never was a problem
with SDL_mixer. It's just that the audio file for the plasma shots
was long and repeating. I'm happy now. Okay so it's all working
now, although I need to remember to free all the data at the end
of the state or it leaks and bleed over. Also, that plasma bolt
sound is irritating as shit.

So it turns out that all the sounds need to be at 48000HZ in order
to play right, even the SFX. Additionally, and this probably has
to do with how I pick the channels, but when the hunters thud
into the player, the healthpack pickup sound stops.
Turns out I was right, setting the channel in Mix_PlayChannel to
-1 makes SDL_mixer take the first free channel, which doesn't
overwrite the sound that I had in x channel if I did it manually.
I can see some value in manually doing that, since I might want
to overwrite some channels if it comes to that, but not right now.

It's occured to me that I would be well served to have the bombs
using states, since there is very little crossover between the
roaming state and the exploding state.

Since the bombs move randomly, it's unfair if they can't be
dodged once their explode trigger has gone off. In other words,
the player must be able to trigger the bomb, and then not take
damage if they immediately move away from the bomb.
Also, I like the idea that one bomb exploding will trigger
another bomb if they're too close. Just like grenades in Halo.

Good news, SDL_mixer 1.2 only supported 8 channels of sound
effects. SDL_mixer 2.0 supports any number. 8 channels was simply
not going to be enough.

Snipers are the first enemy that doesn't necessarily point in
the direction that they are moving. They always look at the
player, unless they are fleeing. They should store the angle
that they're looking at and have battle state simply read that.

Snipers now track the player all across the battlefield, but
turn and flee if the player gets too close. Very cool.

Tried re-doing some of the engine. I just realized how hard it
is going to be to actually add sound and somehow keep everything
out of one file. I could maybe make a queue for each function
that then gets passed back from the function. That seems horribly
side-effect-y. Maybe just pass the mixer itself into the function?
Maybe just do sound in the function itself?

I think that maybe now is not the time to be doing a reformatting
of the code. Now is the time to add features, and to make code
that I won't mind throwing away mostly.

Finally added menu sub-states. It's probably best practice to
pass in a menu-state pointer to battle, although it does feel
a little inelegant. If I don't, it just goes to whatever menu
sub-state it was last on, which will always be MAIN.

Question: is there any real reason that I have the amount of
bolts per level stored? Am I ever going to use less than
the maximum?

There's some memory leak. Must be happening on the heap. I can't
tell if it's on me or SDL. The size, around 15MB every time I
exit from the battle state, implies that it's an image or
audio issue, I just can't find out what.

I find it ironic that, after deciding that the game just wasn't
good enough as it was, and that I needed to add a way to deal
damage, I've made a sort of CTF game mode that is actually really
fun to play. If I just added a timer it would be pretty awesome.

Added a timer, it's pretty fun now.

I just have to add an extra screen that gives the player the
option of a tutorial.
***************************************************************/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>    //I'm going to need fonts eventually, (version 0.3 health uses them)
#include <SDL_mixer.h>  //for sounds
#include <string>       //going to need strings eventually
#include <cmath>        //for traps movement (sin)
#include <sstream>      //outputting health and other stuff
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include "Timer.h"
#include "Characters.h"
#include "IC_GlobalVars.h"
#include "Cutscenes.h"
#include "Helpers.h"

#include "Menu State.h"
#include "Battle State.h"
#include "Tutorial State.h"
#include "CTF.h"

///Main Function
int main(int args, char* argc[])
{
    gs dt;      //stores conversion from units to pixels stuff
    SDL_Renderer* pFrame = NULL;
    SDL_Window* pWindow = NULL;

    //Init is now even more important, since it initializes the scl variable and its friends.
    if (!Init(&pWindow, &pFrame, &dt)){
        printf("Failed initialization\n");
        return -1;
    }

    //This music file gets played in all game states.
    Mix_Music* pMsc = NULL;
    if (!LoadMusic(tdcDirectoryPath+"\\Sounds\\Chopin Spring Waltz.wav", &pMsc)){
        printf("Failed to load music.\n");
        return -1;
    }
    if (Mix_PlayMusic(pMsc, -1) == -1){
        printf("Failed to play music\n");
    }

    gameState state = MENU;
    menuState menuState = INTRO;
    menSubState menSub = TITLE;      //sub-state of main menu, eg. Settings page.
    tutState tutorial = MOVETUT;

    int level = 3;

    int turs=1;       //the number of turrets we start with, changeable by the user
    Diff df=NORM;       //difficulty is also changeable.

    bool quitSDL = false;
    while (quitSDL == false)
    {
        switch(state){
            case MENU: if (Menu(&state, &menuState, &tutorial, &menSub, pFrame, &level, pMsc, dt, &turs, &df) == 1) return 1; break;
            case BATTLE: if (BattleState(&state, &menuState, pFrame, &level, dt) == 1) return 1; break;
            case TUTORIAL: if (PlayTutorials(&state, &menuState, tutorial, pFrame, dt) != 0) return 1; break;
            case CUTSCENE: if (PlayCutscene(&state, pFrame, &level, dt) != 0) return 1; break;
            case CTF: if(CTFGame(&state, &menuState, pFrame, pMsc, dt, turs, df)!=0) return 1; break;
            //case DEBUG: if (DebugState(&states, pFrame) == 1) return 1; break;
            case QUIT: quitSDL = true; break;
        }
    }

    return 0;
}





