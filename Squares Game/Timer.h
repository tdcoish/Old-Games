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

/*
//This timer is in microseconds
class HighPerfTimer
{
private:
    unsigned long long startTicks;
    unsigned long long pausedTicks;    //stores the ticks until paused
    bool started;
    bool paused;

public:
    HighPerfTimer();
    void startClk();
    void stopClk();
    void pauseClk();
    void unpauseClk();

    unsigned long long getTicks();

    bool isStarted();
    bool isPaused();
};

*/
