//Christ there are a lot of hacks here.
#include "Entities.h"
#include "Resources.h"
#include "Level.h"
#include "Globals.h"

void SetUpSpritePointer(int index)
{
	if(allEntities.components[index]&PC_COMP)
		spriteComps[index].pSprite=&sprites.pcSprite;
	if(allEntities.components[index]&TURRET_COMP)
		spriteComps[index].pSprite=&sprites.turretSprite;
	if(allEntities.components[index]&EDGE_TURRET_COMP)
		spriteComps[index].pSprite=&sprites.turretSprite;
	if(allEntities.components[index]&BOLT_COMP)
		spriteComps[index].pSprite=&sprites.boltSprite;
	if(allEntities.components[index]&HUNTER_COMP)
		spriteComps[index].pSprite=&sprites.hunterSprite;
	if(allEntities.components[index]&ORB_COMP)
		spriteComps[index].pSprite=&sprites.orbSprite;
	if(allEntities.components[index]&SNIPER_COMP)
		spriteComps[index].pSprite=&sprites.sniperSprite;
	if(allEntities.components[index]&BURN_COMP)
		spriteComps[index].pSprite=&sprites.blue;		//this one gets overwritten since it uses multiple images.
}

//set position to random slot on 20x20 grid.
//Check if position is unique, if not reset.
void SetEntityPosition(int index)
{
	posComps[index].angle=0;

	bool unique=false;
	while(!unique){
		unique=true;
		posComps[index].x=(rand()%10+5)*5;
		posComps[index].y=(rand()%10+5)*5;
		for(int j=0; j<index; j++){
			if(posComps[index].x==posComps[j].x){
				if(posComps[index].y=posComps[j].y)
					unique=false;
			}
		}
	}
}

void SetHunter(int index)
{
	huntComps[index].chargeLeft=0;
	huntComps[index].cooldownCharge=0;
}

void SetOrb(int index)
{
	orbComps[index].distanceFromPC=1.5+disComps[GetPCIndex()].radius;
	orbComps[index].chargeUp=0;	//other stuff as well
}

void SetSniper(int index)
{
	snipComps[index].firing=false;
	snipComps[index].charging=false;
}

//irrelevant, since we go through the whole entity list and set up the burnspots later.
void SetBurnSpot(int index)
{
	
}

void SetEntityDamage(int index, Uint16 flags)
{
	//ugly hack. Use text files eventually.
	if(flags==BOLT_MASK){
		damComps[index].attackDamage=5;
		damComps[index].damageThis=PC_MASK;
	}
	if(flags==PC_MASK){
		damComps[index].attackDamage=100;
		damComps[index].damageThis=BOLT_MASK;		//maybe just HEALTH_COMP or NPC_BASE_MASK from now on.
	}
	if(flags==ORB_MASK){
		damComps[index].attackDamage=ORB_DAM_PER_FRAME;
		damComps[index].damageThis=NPC_BASE_MASK;
	}
	if(flags==SNIPER_MASK){
		damComps[index].attackDamage=0.5;		//per frame. Ugly right now.
		damComps[index].damageThis=PC_MASK;
	}
	if(flags==HUNTER_MASK){
		damComps[index].attackDamage=0.3;		//again per frame, ugly.
		damComps[index].damageThis=PC_MASK;
	}
	if(flags==BURN_MASK){
		damComps[index].attackDamage=50.0/FRAMES_PER_SECOND;
		damComps[index].damageThis=PC_MASK;
	}
}

void SetEntityRadius(int index)
{
	disComps[index].radius=10.0;		//debugging, should always be overwritten.

	if(allEntities.components[index]&PC_COMP)
		disComps[index].radius=PC_SIZE;
	if(allEntities.components[index]&HUNTER_COMP)
		disComps[index].radius=HUNTER_SIZE;
	if(allEntities.components[index]&TURRET_COMP)
		disComps[index].radius=TURRET_SIZE;
	if(allEntities.components[index]&EDGE_TURRET_COMP)
		disComps[index].radius=TURRET_SIZE;
	if(allEntities.components[index]&BOLT_COMP)
		disComps[index].radius=BOLT_SIZE;
	if(allEntities.components[index]&ORB_COMP)
		disComps[index].radius=ORB_SIZE;
	if(allEntities.components[index]&SNIPER_COMP)
		disComps[index].radius=PC_SIZE;
	if(allEntities.components[index]&BURN_COMP)
		disComps[index].radius=2.5;
	if(allEntities.components[index]&PICK_COMP)
		disComps[index].radius=2.0;
}

//Figure this out later.
void SetCollisionDepth(Uint32 comFlags, int dex)
{
	if(comFlags==PC_MASK)
		allEntities.collDepth[dex]=FLR|REG|PART;
	if(comFlags==TURRET_MASK)
		allEntities.collDepth[dex]=0;
}

//Search through entities until an inactive slot is found. Then set the component bits. Then set up the corresponding component arrays.
//Will return the index or -1 on failure. index can be ignored but is useful for some things.
//This function needs to look at a bunch of text files which specify the attackdamage and other things for each entity type.
int SetUpNewEntity(Uint32 componentFlags)
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==NO_COMP){
			allEntities.components[i]=componentFlags;
			//set collDepth.
			
			if(allEntities.components[i]&HEALTH_COMP){
				//SetUpHealthComp(i);		eventually will set up health according to componentFlags.
				healthComps[i].health=100.0f;
			}                                                                                                                                                                                              
			if(allEntities.components[i]&POS_COMP){
				SetEntityPosition(i);
			}
			if(allEntities.components[i]&VEL_COMP){
				velComps[i].xVel=0;
				velComps[i].yVel=0;
			}

			if(allEntities.components[i]&SPRITE_COMP){
				SetUpSpritePointer(i);
			}
			
			if(allEntities.components[i]&DIS_COMP){
				SetEntityRadius(i);
			}

			if(allEntities.components[i]&DAM_COMP){
				SetEntityDamage(i, componentFlags);
			}

			if(allEntities.components[i]&ORB_COMP)
				SetOrb(i);
			if(allEntities.components[i]&SNIPER_COMP)
				SetSniper(i);
			if(allEntities.components[i]&BURN_COMP)
				SetBurnSpot(i);

			return i;
		}
	}

	//Could iterate through all possible component arrays. Would then need to give those arrays a bit to hold on to.
	//Uint32 bit=1;
	//while(bit < 1<<32){
		//if bit matches array bit
		//set up value.

		//bit<<1;
	//}
	return -1;
}

void SetUpBurnSpots(int numSpots){
	//for some crazy reason, any amount greater than 6 freezes the game.
	//I've tracked it down to a conflict between SetUpScenario() and this.
	for(int i=0; i<numSpots; i++){
		SetUpNewEntity(BURN_MASK);
	}
	//ensure that they don't all reset at once
	int num=0;
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==BURN_MASK){
			burnComps[i].sinceMoved=0+num*2000/(numSpots*gameSpeed);
			num++;
		}
	}
}

void SetUpHealthPickup(){
	int index=SetUpNewEntity(PICKUP_MASK);
	pickComps[index].type=HEALTH;
	pickComps[index].sinceSpawned=0;
	pickComps[index].moveNow=true;
}

/*********************
Rules for setting up edge turrets. They always "average" on the center. If there are 3, they form a 
triangle, if 4, they form a square at all edges, if 5, pentagram, if 6, hexagon, etcetera.
Each edge turret has at least 1 coordinate that is at 100 or 0. For instance, x=50, y=0 for the top
point of the triangle.
The distance from the center point to any edge is the same. (Actually this is incorrect, imagine pentagram).
Since all turrets are going on the border, first figure out how many are going on each side. Then have
a system where we have each side have equal weight?

I'm just going to get a quick hack up and working for now.

Just figured it out. There is a total of 400 units of distance that is the perimeter. We can easily balance
all the edgeturrets by figuring out the space that should exist between them. For example, 2 turrets, means
400/2 = 200.0 units of space from the first to the last. This is doubly good, since it allows me to randomize
the location of the original turret.
*********************/
void SetUpEdgeTurrets(int num)
{
	//int trueIndex=-1;
	//for(int i=0; i<num; i++){
		//trueIndex=SetUpNewEntity(EDGE_TURRET);
		//posComps[trueIndex].x=
	//}
	float disPerEach=400.0/(float)num;
	float pos=50.0;
	float x, y;
	int slot=-1;
	for(int i=0; i<num; i++){
		slot=SetUpNewEntity(EDGE_TURRET_MASK);
		//set position
		if(pos<100){
			y=100;
			x=pos;
		}else if(pos<200){
			x=100;
			y=pos-100;
		}else if(pos<300){
			y=0;
			x=300-pos;
		}else if(pos<400){
			x=0;
			y=pos-300;
		}
		posComps[slot].x=x; posComps[slot].y=y;
		
		pos+=disPerEach;
		if(pos>400.0)pos-=400.0;		//should never be true.

		//set angle to start at.
		edgeComps[slot].middleAngle=FindAngleUsingDiff((50-x), (50-y));
		posComps[slot].angle=edgeComps[slot].middleAngle;
	}
}

void SetUpScenario(int power, int seed)
{
	ClearEntitiesAndComponents();

	SetUpNewEntity(PC_MASK);
	

	/*******************************
	Set up things manually
	********************************/
	/*
	SetUpNewEntity(TURRET_MASK);
	posComps[1].x=10;	posComps[1].y=50;
	SetUpNewEntity(TURRET_MASK);
	posComps[2].x=90; posComps[2].y=50;
	SetUpNewEntity(SNIPER_MASK);
	posComps[3].x=50; posComps[3].y=50;
	//SetUpNewEntity(BURN_MASK);
	*/
	SetUpNewEntity(HUNTER_MASK);

	SetUpEdgeTurrets(4);

	//game freezes if num higher than 6 is passed in here.
	SetUpBurnSpots(4);





	/***************************************
	Set up things procedurally/randomly
	***************************************/
	/*
	int hunter=6;
	int sniper=11;
	int turret=7;
	int burnSpot=2;
	int all=hunter+sniper+turret+burnSpot;
	srand(seed);
	int randResult;
	while(power>0){
		randResult=rand()%all;
		if(randResult<hunter){
			SetUpNewEntity(HUNTER_MASK);
			power-=hunter;
		}else if(randResult<sniper+hunter){
			SetUpNewEntity(SNIPER_MASK);
			power-=sniper;
		}else if(randResult<turret+sniper+hunter){
			SetUpNewEntity(TURRET_MASK);
			power-=turret;
		}else if(randResult<all){
			SetUpNewEntity(BURN_MASK);
			power-=burnSpot;
		}else{
			std::cout << "Error\n";
		}
	}
	*/
}


