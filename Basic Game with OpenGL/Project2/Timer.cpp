#include <SDL2/SDL.h>
#include "Timer.h"

Timer::Timer()
{
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

//Sets the timer object to started
void Timer::startClk()
{
    started = true;
    paused = false;

    startTicks = SDL_GetTicks();
}

//Stops the timer object
void Timer::stopClk()
{
    started = false;
    paused = false;
}

//Pauses the timer object
void Timer::pauseClk()
{
    if ((started == true) && (paused == false) ){
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

//Unpauses the timer object
void Timer::unpauseClk()
{
    if (paused == true){
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

//Returns the ticks since the timer started, or when it was paused
int Timer::getTicks()
{
    if (started == true){
        if (paused == true){
            return pausedTicks;
        }
        else{
            return SDL_GetTicks() - startTicks;
        }
    }
    //if the timer isn't running
    return 0;
}

//true if timer started
bool Timer::isStarted()
{
    return started;
}

//true if timer paused
bool Timer::isPaused()
{
    return paused;
}









