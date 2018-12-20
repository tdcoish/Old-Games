//const int MAX_BOLTS = 8;

class SpikeTraps
{
public:
    double x[64], y[64];         //stores the rotational center of the four traps
    double xPos[64], yPos[64];    //stores the actual position of the four traps
    int timeToReactive[64];         //if negative, trap is active.

    SpikeTraps();
    void moveTraps(int appTime);
};

class PlayerCharacter
{
public:
    double xVel, yVel;
    double trueX, trueY;
    SDL_Rect collBox;
    double health;

    PlayerCharacter();
    void handleInput(SDL_Event* pEvent);
    void move(SpikeTraps* pSpikeTraps);

    bool up, down, left, right;
};

//Behave just like their in Halo:CE
class Hunters
{
public:
    double xVel[8], yVel[8];
    double xPos[8], yPos[8];
    bool charging[8];
    int chargeTime[8];      //if positive, they are charging.
    int collidedTime[8];    //if positive, they have collided and move straight for a while
    int damageCooldown[8];  //if positive, they just dealt damage and won't again for a while

    Hunters();
    void moveHunters(PlayerCharacter playa);     //will also make them charge if they are close
    void handleCollision(int offset1, int offset2); //bounce off the other hunter or not
};

//This guy just walks around in a circle and attacks the player with a bolt every so often
class Sniper
{
    //has no collision box because I will not be having the player collide with him
public:
    double xPos, yPos;
    int interval;       //time between shots (created const global variable)

    Sniper();
    void moveSniper(int appTime);
    void shootBall(PlayerCharacter copyPlayer);
};

//The closest thing to a particle system I've ever written.
class SniperBolts
{
public:
    double xPos[128], yPos[128];      //true position
    double xVel[128], yVel[128];      //must be normalised at creation.
    SDL_Rect collBox[128];       //checks for collision with player and screen edges
    bool isAlive[128];

    SniperBolts();
    bool newBolt(double sniperX, double sniperY, PlayerCharacter copyPlayer, double speed);
    void moveBolts();       //also kills them if they go off the screen
};

//I'm only envisioning 1 on screen at a time, but why not allow for more?
class HealthPacks
{
public:
    double xPos[8], yPos[8];
    bool isActivated[8];
    SDL_Rect collBox[8];

    HealthPacks();
};

//There's a bunch active at once. They individually disactivate, and then are put in a new place.
class BurnSpots
{
    public:
        int xPos[64], yPos[64];             //kind of redundant
        int sinceDead[64];                //they start blue, then when active do damage.
        bool isActive[64];
        SDL_Rect collBox[64];

        BurnSpots(const int NM_SPTS);
        void Rearrange(int appTime, int numActive);
        void Activate(int numActive);
};


