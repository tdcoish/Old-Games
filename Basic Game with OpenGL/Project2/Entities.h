/*********************************************************************************
Entity system. I create a global struct that holds a giant list of all entities.
Or rather holds a large list of the components that a particular entity is made
up of. I'm not using an ID system for now, so most arrays will have huge blank 
spaces, as I access them only at the same index as the Entities struct. Eventually
I'll have IDs, which will drastically cut down on the the memory requirements, as well
as allowing me more freedom to work from within component structs, since I can hop
from ID to ID.
*********************************************************************************/
#ifndef ENTITIES_H_INCLUDED
#define ENTITIES_H_INCLUDED

#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "Games.h"
#include "Shader.h"
#include "Timer.h"

//maybe eventually include a Shader* when I have lots of different shaders.
struct Sprite{
	GLuint quadVAO;
	GLuint texture;
};

const int MAX_ENTITIES=512;

//No ID's for now.
struct HunterComponent{
	//int ID;
	int chargeLeft;
	int cooldownCharge;
	Uint8 hunterFlags;		//only hunter specific, such as charging.
};
//gotta use new().
extern HunterComponent* huntComps;

struct SniperComponent{
	int chargeStart;	//stores time they start charging.
	int fireStart;		//stores the time they started firing.
	bool firing;		//if true, we can't update the angle.
	bool charging;
};
extern SniperComponent* snipComps;

struct PCComponent{
	//int ID;
	//shield code
	//sword code
	Uint8 pcFlags;			//only pc specific, such as weapon ready.
};
extern PCComponent* pcComps;
//PC holds the orb which deals damage. Very much a placeholder.
struct OrbComponent{
	Uint8 orbFlags;
	float distanceFromPC;
	int chargeUp;
	int cooldownTime;		//these are all just placeholders.
};
extern OrbComponent* orbComps;

//will eventually become part of turrets.
struct EdgeTurretComponent{
	int lastShot;
	float middleAngle;
	bool right;		//increase angle right or left.
};
extern EdgeTurretComponent* edgeComps;

struct TurretComponent{
	//turret stuff
	Uint8 turFlags;		//maybe turrets get stun locked or something
	int lastShot;
};
extern TurretComponent* turComps;

//get moved at regular interval. Then get active.
struct BurnComponent{
	int sinceMoved;
	bool active;
};
extern BurnComponent* burnComps;

struct SpinnerComponent{
	//rate
	//what?
};

enum PICKUP_TYPE{
	HEALTH, WEAPONS, END
};
struct PickupComponent{
	PICKUP_TYPE type;
	int sinceSpawned;
	bool moveNow;
};
extern PickupComponent* pickComps;


//also has angle, since everything has that. I may put that in it's own array. Units not pixels.
struct PositionComponent{
	float x, y;
	float angle;
	int ID;
};
extern PositionComponent* posComps;

struct HealthComponent{
	float health;
	int ID;
};
extern HealthComponent* healthComps;

struct DamageComponent{
	float attackDamage;
	Uint32 damageThis;		//holds a flag for all the things, or type of things it can damage.
};
extern DamageComponent* damComps;

struct VelocityComponent{
	float xVel, yVel;
	int ID;
};
extern VelocityComponent* velComps;

struct DisplacementComponent{
	float radius;			//size of entity.
	int ID;
};
extern DisplacementComponent* disComps;

struct SpriteComponent{
	Sprite* pSprite;
	int ID;
};
extern SpriteComponent* spriteComps;

const Uint32 BCK=1<<0;
const Uint32 FLR=1<<1;
const Uint32 REG=1<<2;
const Uint32 PART=1<<3;
const Uint32 PLAY=1<<4;
struct Entities{
	Uint32 components[MAX_ENTITIES];
	Uint32 collDepth[MAX_ENTITIES];		//first "hot" data. Used for collision detection. Collide with entities at this depth.
};
extern Entities allEntities;

/*
int carry=0;
const Uint32 NO_COMP=1<<carry++;
const Uint32 HEALTH_COMP=1<<carry++;
const Uint32 POS_COMP=1<<carry++;
const Uint32 DAM_COMP=1<<carry++;
const Uint32 MOVE_COMP=1<<carry++;		//wtf am I using this for?
const Uint32 VEL_COMP=1<<carry++;
const Uint32 DIS_COMP=1<<carry++;
const Uint32 SPRITE_COMP=1<<carry++;

const Uint32 HUNTER_COMP=1<<carry++;
const Uint32 PC_COMP=1<<carry++;
const Uint32 TURRET_COMP=1<<carry++;
const Uint32 BOLT_COMP=1<<carry++;
*/

const Uint32 NO_COMP=1<<0;
const Uint32 HEALTH_COMP=1<<1;
const Uint32 POS_COMP=1<<2;
const Uint32 DAM_COMP=1<<3;
const Uint32 MOVE_COMP=1<<4;		//used for collision handling.
const Uint32 VEL_COMP=1<<5;
const Uint32 DIS_COMP=1<<6;
const Uint32 SPRITE_COMP=1<<7;

const Uint32 HUNTER_COMP=1<<8;
const Uint32 PC_COMP=1<<9;
const Uint32 TURRET_COMP=1<<10;
const Uint32 BOLT_COMP=1<<11;
const Uint32 ORB_COMP=1<<12;
const Uint32 SNIPER_COMP=1<<13;
const Uint32 BURN_COMP=1<<14;
const Uint32 PICK_COMP=1<<15;
const Uint32 EDGE_TURRET_COMP=1<<16;

const Uint32 NPC_BASE_MASK=HEALTH_COMP|POS_COMP|DAM_COMP|DIS_COMP|SPRITE_COMP;
const Uint32 HUNTER_MASK=NPC_BASE_MASK|VEL_COMP|HUNTER_COMP|MOVE_COMP;
const Uint32 TURRET_MASK=NPC_BASE_MASK|TURRET_COMP;
const Uint32 EDGE_TURRET_MASK=NPC_BASE_MASK|EDGE_TURRET_COMP;
const Uint32 PC_MASK=NPC_BASE_MASK|VEL_COMP|PC_COMP;
const Uint32 BOLT_MASK=NPC_BASE_MASK|VEL_COMP|BOLT_COMP;
const Uint32 SNIPER_MASK=NPC_BASE_MASK|SNIPER_COMP;
const Uint32 BURN_MASK=NPC_BASE_MASK|BURN_COMP^HEALTH_COMP;		//get rid of that Health comp
//const Uint32 PICKUP_MASK=PICK_COMP|POS_COMP|DIS_COMP|SPRITE_COMP;		//could make two, one health, one weapon. would be first to not have own COMP
const Uint32 PICKUP_MASK=NPC_BASE_MASK|PICK_COMP^HEALTH_COMP^DAM_COMP;

const Uint32 ORB_MASK=POS_COMP|DAM_COMP|SPRITE_COMP|ORB_COMP|DIS_COMP;

void ClearEntitiesAndComponents();

void UpdateEntities(Timer );

//this should probably be somewhere else.
float FindAngleUsingDiff(double xDif, double yDif);
float FindAngleRaw(double x1, double y1, double x2, double y2);

void RenderEntities();

//and many others.

void RenderHUD();

int GetPCIndex();




#endif