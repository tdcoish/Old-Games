#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class Timer
{
private:
    int startTicks;
    int pausedTicks;    //stores the ticks until paused
    bool started;
    bool paused;

public:
    Timer();
    void startClk();
    void stopClk();
    void pauseClk();
    void unpauseClk();

    int getTicks();

    bool isStarted();
    bool isPaused();
};

//The game needs a clock for the frame, as well as a clock for the game.
struct Timers{
    Timer fps, events;
};



#endif
