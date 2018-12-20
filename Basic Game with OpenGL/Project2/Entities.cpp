/******************************************************************
-Collision detection
-Collision resolution
-Setting up of all Entities
-All functions relating to entities
	-Movement
	-Take damage
	-Render
	-etcetera
******************************************************************/

#include "Entities.h"
#include "Globals.h"
#include "Shader.h"
#include "Resources.h"
#include "Timer.h"
#include "Graphics.h"
#include "Level.h"
#include "Input.h"

//Shader gameShader;
/*
void WrapSetShader(Shader* pShader){
	SetShader(
}
SetShader((bsPth+"\\vertShaderGame.vs").c_str(), (bsPth+"\\fragShaderGame.frag").c_str(), &gameShader);
*/

HunterComponent* huntComps = new HunterComponent[MAX_ENTITIES];
PCComponent* pcComps = new PCComponent[MAX_ENTITIES];
OrbComponent* orbComps = new OrbComponent[MAX_ENTITIES];
TurretComponent* turComps = new TurretComponent[MAX_ENTITIES];
EdgeTurretComponent* edgeComps=new EdgeTurretComponent[MAX_ENTITIES];
SniperComponent* snipComps = new SniperComponent[MAX_ENTITIES];
BurnComponent* burnComps = new BurnComponent[MAX_ENTITIES];
PositionComponent* posComps = new PositionComponent[MAX_ENTITIES];
HealthComponent* healthComps = new HealthComponent[MAX_ENTITIES];
PickupComponent* pickComps=new PickupComponent[MAX_ENTITIES];
DamageComponent* damComps = new DamageComponent[MAX_ENTITIES];
VelocityComponent* velComps=new VelocityComponent[MAX_ENTITIES];
DisplacementComponent* disComps = new DisplacementComponent[MAX_ENTITIES];
SpriteComponent* spriteComps = new SpriteComponent[MAX_ENTITIES];

Entities allEntities;

void ClearEntitiesAndComponents(){
	//clear entities
	for(int i=0; i<MAX_ENTITIES; i++){
		allEntities.components[i]=NO_COMP;
	}
	//clear components
	for(int i=0; i<MAX_ENTITIES; i++){
		//make every component 0 or null or something appropriate
		spriteComps[i].pSprite=NULL;		//only really important one
		disComps[i].radius=0;
		posComps[i].angle=0;
		turComps[i].lastShot=0;
		snipComps[i].fireStart=0;
		snipComps[i].firing=false;
		burnComps[i].sinceMoved=0;
		healthComps[i].health=0;
		damComps[i].attackDamage=0;
		damComps[i].damageThis=NO_COMP;
		orbComps[i].distanceFromPC=0.5;
		pickComps[i].sinceSpawned=0;
		pickComps[i].type=HEALTH;
	}
}

void CullEntity(int index)
{
	allEntities.components[index]=NO_COMP;
	//extra stuff for safety but this works pretty well.
}

void CullBolts()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&BOLT_COMP){
			//cull if off screen. They never come back
			if(posComps[i].x>=100 || posComps[i].x<=0)
				CullEntity(i);
			if(posComps[i].y>=100 || posComps[i].y<=0)
				CullEntity(i);
		}
	}
}

float FindAngleUsingDiff(double xDif, double yDif)
{
	double ratio=0;
	if(yDif==0){
		if(xDif>0)
			return 90;
		if(xDif<0)
			return -90;
		if(xDif==0)
			return 0;
	}

	if(yDif<0){
		yDif*=-1;
		ratio=xDif/yDif;
		yDif*=-1;
	} else if(yDif>0){
		ratio=xDif/yDif;
	}

	if(yDif>0){
		if(xDif<0){
			return -180-atanf(ratio)*RADIANS_TO_DEGREES;
		}else{
			return 180-atanf(ratio)*RADIANS_TO_DEGREES;
		}
	}else{
		return atanf(ratio)*RADIANS_TO_DEGREES;
	}
}

float FindAngleRaw(double x1, double y1, double x2, double y2)
{
	double xDif=x1-x2;
	double yDif=y1-y2;
	return(FindAngleUsingDiff(xDif, yDif));
}

double GetDistance(double x1, double y1, double x2, double y2)
{
	double xDif=x1-x2;
	double yDif=y1-y2;
	return sqrt(xDif*xDif + yDif*yDif);
}

void SetPCAngle()
{
	//need to look at mouse input.
	int pc=GetPCIndex();
	float xDif=msX-posComps[pc].x;
	float yDif=msY-posComps[pc].y;

	posComps[pc].angle=FindAngleUsingDiff(xDif, yDif);
}

void SetPCVel(int index)
{
	velComps[index].xVel=0;
	velComps[index].yVel=0;
	if(kStore&kLeft)
		velComps[index].xVel-=PC_MAX_SPEED_PER_FRAME;
	if(kStore&kRight)
		velComps[index].xVel+=PC_MAX_SPEED_PER_FRAME;
	if(kStore&kUp)
		velComps[index].yVel+=PC_MAX_SPEED_PER_FRAME;		//flip later in SetVelocities
	if(kStore&kDown)
		velComps[index].yVel-=PC_MAX_SPEED_PER_FRAME;

	//normalize velocity
	if(std::abs(velComps[index].xVel)!=0 && std::abs(velComps[index].yVel)!=0){
		velComps[index].xVel*=0.7072;
		velComps[index].yVel*=0.7072;
	}
}

void SetHunterOrSniperAngle(int index, const double MAX_TURN)
{
	int pc=GetPCIndex();

	double wdDf, htDf, trueAngle, anglDf;
	wdDf=posComps[pc].x-posComps[index].x;
	htDf=posComps[pc].y-posComps[index].y;
	trueAngle=FindAngleUsingDiff(wdDf, htDf);
	anglDf=posComps[index].angle-trueAngle;
		if(anglDf>0&&anglDf<=180||anglDf<=-180){
		posComps[index].angle-=MAX_TURN;
	}else{
		posComps[index].angle+=MAX_TURN;
	}
	if(posComps[index].angle<-180)posComps[index].angle+=360;
	if(posComps[index].angle>180)posComps[index].angle-=360;
}

void SetSniperAngle(int index)
{
	double SNIPER_TURN_PER_FRAME=60/FRAMES_PER_SECOND;
	//can only turn so much per frame.
	if(!snipComps[index].firing){
		SetHunterOrSniperAngle(index, SNIPER_TURN_PER_FRAME);
	}
}

//Implicitly sets the angle as well.
void SetHunterVel(int index)
{
	huntComps[index].chargeLeft-=FRAME_TIME;
	huntComps[index].cooldownCharge-=FRAME_TIME;

	//if the hunter is not currently charging
	if(huntComps[index].chargeLeft<=0){

		SetHunterOrSniperAngle(index, ANGL_FM);

		velComps[index].xVel=sin(posComps[index].angle*DEGREES_TO_RADIANS)*HUNTER_DIS_PER_FRAME;
		velComps[index].yVel=cos(posComps[index].angle*DEGREES_TO_RADIANS)*HUNTER_DIS_PER_FRAME;

		//if the hunter is close enough to charge
		if((GetDistance(posComps[index].x, posComps[index].y, posComps[GetPCIndex()].x, posComps[GetPCIndex()].y) < 15.0)){
			//if they're not cooling down.
			if(huntComps[index].cooldownCharge<=0){
				//if the angle is close enough
				double realAngle=FindAngleRaw(posComps[GetPCIndex()].x, posComps[GetPCIndex()].y, posComps[index].x, posComps[index].y);
				if(std::abs(realAngle-posComps[index].angle)<10.0){
					huntComps[index].chargeLeft=800;
					huntComps[index].cooldownCharge=1000;
					velComps[index].xVel*=2;
					velComps[index].yVel*=2;
				}
			}
		}
	}

	//Don't update position here anymore.
}

void SetTurretAngle(int index)
{
	int pcIndex=0;
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&PC_COMP){
			pcIndex=i;
			break;
		}
	}
	
	double xDif, yDif;
	xDif=posComps[pcIndex].x-posComps[index].x;
	yDif=posComps[pcIndex].y-posComps[index].y;
	posComps[index].angle=FindAngleUsingDiff(xDif, yDif);
}

void SetEdgeTurretAngle(int index){
	//turret stores true angle.
	float turnSpeed=40.0/(float)FRAMES_PER_SECOND;
	if(edgeComps[index].right){
		posComps[index].angle+=turnSpeed;
	}else{
		posComps[index].angle-=turnSpeed;
	}
	if(std::abs(posComps[index].angle-edgeComps[index].middleAngle) > 80.0)
		edgeComps[index].right=(!edgeComps[index].right);
}

void SetVelocities()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&VEL_COMP){
			if(allEntities.components[i]&PC_COMP)
				SetPCVel(i);
			if(allEntities.components[i]&HUNTER_COMP){
				SetHunterVel(i);
				//printf("here\n");
			}
		}
		if(allEntities.components[i]&TURRET_COMP){
			SetTurretAngle(i);		//this doesn't belong here, but I'm lazy right now.
		}
		if(allEntities.components[i]&EDGE_TURRET_COMP){
			SetEdgeTurretAngle(i);
		}
		if(allEntities.components[i]==SNIPER_MASK)
			SetSniperAngle(i);
	}
}

void FireBolt(int turIndex)
{
	int pcIndex = GetPCIndex();

	//Creates a new bolt with default values as side effect
	int boltIndex=SetUpNewEntity(BOLT_MASK);

	posComps[boltIndex].x=posComps[turIndex].x;
	posComps[boltIndex].y=posComps[turIndex].y;

	//now shift to the front of the turret
	double distance=(BOLT_SIZE+TURRET_SIZE)*1.05;
	posComps[boltIndex].x+=sin(posComps[turIndex].angle*DEGREES_TO_RADIANS)*distance;
	posComps[boltIndex].y-=cos(posComps[turIndex].angle*DEGREES_TO_RADIANS)*distance;

	//posComps[boltIndex].angle=FindAngleRaw(posComps[pcIndex].x, posComps[pcIndex].y, posComps[turIndex].x, posComps[turIndex].y);
	posComps[boltIndex].angle=posComps[turIndex].angle;
	float tempDis=BOLT_DIS_PER_FRAME*4;
	velComps[boltIndex].xVel=tempDis*sin(posComps[boltIndex].angle*DEGREES_TO_RADIANS);
	velComps[boltIndex].yVel=tempDis*cos(posComps[boltIndex].angle*DEGREES_TO_RADIANS);		//don't flip now
	//velComps[boltIndex].xVel=BOLT_DIS_PER_FRAME*2*sin(posComps[boltIndex].angle*DEGREES_TO_RADIANS);
	//velComps[boltIndex].yVel=BOLT_DIS_PER_FRAME*2*cos(posComps[boltIndex].angle*DEGREES_TO_RADIANS);		//don't flip now
}

//First Unfires sniper if necessary
void FireSniper(Timer eTimer, int index)
{
	//actually fire if fully charged.
	if(snipComps[index].charging){
		if(eTimer.getTicks()-snipComps[index].chargeStart>100){
			snipComps[index].charging=false;
			snipComps[index].fireStart=eTimer.getTicks();
			snipComps[index].firing=true;
		}
	}

	//unfire all the snipers after x milliseconds.
	if(eTimer.getTicks()-snipComps[index].fireStart>300){
		snipComps[index].firing=false;
	}

	//Find if angle == angle to pc (to within 1 degrees)  (must be high because otherwise pc can cross sometimes without registering)
	float realAngle=FindAngleRaw(posComps[GetPCIndex()].x, posComps[GetPCIndex()].y, posComps[index].x, posComps[index].y);
	if(posComps[index].angle>=realAngle-1 && posComps[index].angle<=realAngle+1){
		//then if not already firing or charging.
		if(!snipComps[index].charging){
			snipComps[index].chargeStart=eTimer.getTicks();
			snipComps[index].charging=true;
		}
	}
}

void FireSnipers(Timer eTimer)
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==SNIPER_MASK){
			FireSniper(eTimer, i);
		}
	}
}

void FireTurrets(Timer eTimer)
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&TURRET_COMP){
			if(eTimer.getTicks()-turComps[i].lastShot>200/gameSpeed){
				turComps[i].lastShot=eTimer.getTicks();
				FireBolt(i);
			}
		}
		if(allEntities.components[i]&EDGE_TURRET_COMP){
			if(eTimer.getTicks()-edgeComps[i].lastShot>400/gameSpeed){
				edgeComps[i].lastShot=eTimer.getTicks();
				FireBolt(i);
			}
		}
	}
}

//Places the orb x distance in front of the PC always.
void MoveOrb()
{
	//considering writing a function that takes a mask and returns the first entity of that type.
	//Like GetIndex(HUNTER_MASK);		and it returns the index.
	int pc=GetPCIndex();
	int orb=-1;
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==ORB_MASK){
			orb=i;
			break;
		}
	}
	if(orb==-1) return;		//otherwise crash.
	double angle=posComps[pc].angle*DEGREES_TO_RADIANS;

	posComps[orb].x=posComps[pc].x+sin(angle)*orbComps[orb].distanceFromPC;
	posComps[orb].y=posComps[pc].y-cos(angle)*orbComps[orb].distanceFromPC;
}

void BurnspotLogic()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==BURN_MASK){
			burnComps[i].sinceMoved+=FRAME_TIME;
			if(burnComps[i].sinceMoved>500/gameSpeed){		//player gets x ms of warning
				burnComps[i].active=true;
			}else{
				burnComps[i].active=false;
			}
			//every x ms move burnspot
			if(burnComps[i].sinceMoved>2000/gameSpeed){
				//move burnspot
				posComps[i].x=rand()%20*5;
				posComps[i].y=rand()%20*5;
				burnComps[i].active=false;
				burnComps[i].sinceMoved=0;
			}
		}
	}
}

//respawn pickups if their time has passed
void MovePickups()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==PICKUP_MASK){
			pickComps[i].sinceSpawned+=FRAME_TIME;
			if(pickComps[i].sinceSpawned>3000){
				pickComps[i].moveNow=true;
			}

			//now move the pickup
			if(pickComps[i].moveNow){
				//always cluster around the center.
				posComps[i].x=rand()%10*5+25;
				posComps[i].y=rand()%10*5+25;
				//check if colliding with PC or stationary entity, and respawn
				//but not now
				pickComps[i].sinceSpawned=0;
				pickComps[i].moveNow=false;
			}
		}
	}
}

void KeepInBounds(int index)
{
	if(posComps[index].x<0)
		posComps[index].x=0;
	if(posComps[index].x>100)
		posComps[index].x=100;
	if(posComps[index].y<0)
		posComps[index].y=0;
	if(posComps[index].y>100)
		posComps[index].y=100;
}
//So elegant
void MoveEntities()
{
	//slow everything down by the gameSpeed variable.
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&VEL_COMP){
			posComps[i].x+=velComps[i].xVel*gameSpeed;
			posComps[i].y-=velComps[i].yVel*gameSpeed;		//flipping y.

			KeepInBounds(i);
		}
	}
	//orb does not have velocity. Readjusts to the player.
	MoveOrb();
	BurnspotLogic();
	MovePickups();
}

//if distance between centers is less than their radius's added together, they're overlapping
bool TestCollision(int index1, int index2)
{
	double maxDisNoHit=disComps[index1].radius+disComps[index2].radius;
	//distance in each axis squared.
	double xDis=std::pow(posComps[index1].x-posComps[index2].x, 2);
	double yDis=std::pow(posComps[index1].y-posComps[index2].y, 2);
	double trueDistance=std::sqrt(xDis+yDis);
	if(trueDistance<=maxDisNoHit){
		return true;
	}else {
		return false;
	}
}

void StoreCollision(int index1, int index2){
	for(int i=0; i<MAX_ENTITIES; i++){
		if(collisionsQ[i][0]==-1){	//free spot
			collisionsQ[i][0]=index1;
			collisionsQ[i][1]=index2;
			//std::cout << "collision stored\n";
			break;			//missing break caused constant buffer overflow.
		}
	}
}

//Since the collision Q cannot store all theoretical collisions I should first check NO_COMP before storing the collision.
void CheckCollisions()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]!=NO_COMP){
			for(int j=i+1; j<MAX_ENTITIES; j++){
				if(allEntities.components[j]!=NO_COMP){
					if(TestCollision(i, j)){
						//store collision in collision q
						StoreCollision(i, j);
					}
				}
			}
		}
	}
}

//index1 is always the moveable entity.
void SeperateFirstMoveable(int index1, int index2, float pushDistance, float xRatio, float yRatio)
{
	//now just push the first entity the whole distance
	if(posComps[index1].x<posComps[index2].x){
		posComps[index1].x-=pushDistance*xRatio;
	}else{
		posComps[index1].x+=pushDistance*xRatio;
	}
		
	if(posComps[index1].y<posComps[index2].y){
		posComps[index1].y-=pushDistance*yRatio;
	}else{
		posComps[index1].y+=pushDistance*yRatio;
	}
}

void SeperateBothMoveable(int index1, int index2, float pushDistance, float xRatio, float yRatio)
{
	//push the entities away in the correct distances. push each half the necessary distance
	if(posComps[index1].x<posComps[index2].x){
		posComps[index1].x-=pushDistance/2*xRatio;
		posComps[index2].x+=pushDistance/2*xRatio;
	}else{
		posComps[index1].x+=pushDistance/2*xRatio;
		posComps[index2].x-=pushDistance/2*xRatio;
	}

	if(posComps[index1].y<posComps[index2].y){
		posComps[index1].y-=pushDistance/2*yRatio;
		posComps[index2].y+=pushDistance/2*yRatio;
	}else{
		posComps[index1].y+=pushDistance/2*yRatio;
		posComps[index2].y-=pushDistance/2*yRatio;
	}
}

//determines amount of overlap, then "pushes" the objects away until they are no longer overlapping
//handles all objects, with logic for moveable and immovable objects.
void SeperateEntities(int index1, int index2)
{
	Uint32 invalidMask=HEALTH_COMP;		//health packs don't have physical dimensions
	//if one of the entities does not have physical dimensions, return.
	if(allEntities.components[index1]^invalidMask || allEntities.components[index2]^invalidMask)
		return;

	//do a check immediately. If both not seperable, something weird has happened
	if(!(allEntities.components[index1]&MOVE_COMP) && !(allEntities.components[index2]&MOVE_COMP)){
		std::cout << "Two immovable object collided. Error\n";
		return;
	}
	//possibly should do some of this in TestCollision() to save computations. Probably fairly minor.
	//determine distance from each other.
	double endDistance=disComps[index1].radius+disComps[index2].radius*1.01;		//add a bit on as extra just so we guaranteed moved them apart
	//distance in each axis
	double xDis=posComps[index1].x-posComps[index2].x;
	double yDis=posComps[index1].y-posComps[index2].y;
	double trueDistance=std::sqrt(xDis*xDis+yDis*yDis);
	double pushDistance=endDistance-trueDistance;		//amount needed to push the two away.
	
	double xRatio;
	if(xDis+yDis!=0){
		xRatio=xDis/(xDis+yDis);
	}else{
		xRatio=1;
	}
	double yRatio=1-xRatio;
	
	//if one is immovable, push the first 100% of the distance.
	if(allEntities.components[index1]&MOVE_COMP ^ allEntities.components[index2]&MOVE_COMP){
		if(allEntities.components[index1]&MOVE_COMP){
			SeperateFirstMoveable(index1, index2, pushDistance, xRatio, yRatio);
		}else{
			SeperateFirstMoveable(index2, index1, pushDistance, xRatio, yRatio);
		}
	}
	//if both are moveable, push both away exactly the same distance.
	else if(allEntities.components[index1]&MOVE_COMP && allEntities.components[index2]&MOVE_COMP){		//both moveable.
		SeperateBothMoveable(index1, index2, pushDistance, xRatio, yRatio);
	}
}

void BounceEntitiesAway(int index1, int index2)
{

}

/********************************************************************
Three possiblilities:
1) neither are hunters. Easy
2) One is a hunter
3) Both are hunters
if 1, return
if 2, now check if the other is moveable or not.
--if immovable, bounce hunter off
--if moveable, go to step 3.
if 3, treat both like billiard balls. Conserve momentum.
********************************************************************/
void BounceHunter(int index1, int index2)
{
	Uint32 invalidMask=BOLT_COMP|TURRET_COMP|HEALTH_COMP;
	//must be a valid collision. Bolts do not count
	if(allEntities.components[index1]&invalidMask || allEntities.components[index2]&invalidMask)
		return;

	//first entity is a hunter, second is not
	if(allEntities.components[index1]==HUNTER_MASK && allEntities.components[index2]!=HUNTER_MASK){
		float angleBetween=FindAngleRaw(posComps[index1].x, posComps[index1].y, posComps[index2].x, posComps[index2].y);
		if(posComps[index1].angle>angleBetween){
			posComps[index1].angle=angleBetween+90.0;
		}else{
			posComps[index1].angle=angleBetween-90.0;
		}
	}
	//second entity is a hunter, first is not.
	if(allEntities.components[index2]==HUNTER_MASK && allEntities.components[index1]!=HUNTER_MASK){
		posComps[index2].angle-=180;
		huntComps[index2].chargeLeft=-10;
		/*
		float angleBetween=FindAngleRaw(posComps[index2].x, posComps[index2].y, posComps[index1].x, posComps[index1].y);
		if(posComps[index2].angle>angleBetween){
			posComps[index2].angle=angleBetween+90.0;
		}else{
			posComps[index2].angle=angleBetween-90.0;
		}*/
	}
	//both entities are hunters, move both away in opposite directions.
	if(allEntities.components[index1]==HUNTER_MASK && allEntities.components[index2]==HUNTER_MASK){
		float angleBetween=FindAngleRaw(posComps[index1].x, posComps[index1].y, posComps[index2].x, posComps[index2].y);
		if(posComps[index1].angle>angleBetween){
			posComps[index1].angle=angleBetween+90.0;
			posComps[index2].angle=angleBetween-90.0;
		}else{
			posComps[index1].angle=angleBetween-90.0;
			posComps[index2].angle=angleBetween+90.0;
		}
	}
}

//Will eventually be refactored
void DoCollisionEffects(int index1, int index2)
{
	//if we've culled anything last time around don't do the collision again.
	if(allEntities.components[index1]==NO_COMP)
		return;
	if(allEntities.components[index2]==NO_COMP)
		return;
	//deal damage if damageable
	//stun if stunable
	//other stuff.

	//check which one, if either, is hunter inside
	//needs more work.
	BounceHunter(index1, index2);

	//could turn into a function all of its own.
	//needs to be more general purpose.
	if(allEntities.components[index1]&DAM_COMP){
		if(damComps[index1].damageThis==allEntities.components[index2]){
			healthComps[index2].health-=damComps[index1].attackDamage;
		}
	}

	//cull any bolts that hit the player.
	if(allEntities.components[index1]==BOLT_MASK)
		if(allEntities.components[index2]==PC_MASK)
			CullEntity(index1);

	//add health to player if player hits pickup. Mark pickup ready to move.
	if(allEntities.components[index1]==PICKUP_MASK){
		if(allEntities.components[index2]==PC_MASK){
			if(pickComps[index1].type==HEALTH){
				healthComps[index2].health+=20;
				pickComps[index1].moveNow=true;
			}
		}
	}
}

//First real messy gameplay function.
//Solution is to have sub function where I handle the collision for each character one at a time.
void HandleSpecificCollision(int index1, int index2)
{
	//never do this for bolts
	if(allEntities.components[index1]!=BOLT_MASK && allEntities.components[index2]!=BOLT_MASK)
		SeperateEntities(index1, index2);

	DoCollisionEffects(index1, index2);
	DoCollisionEffects(index2, index1);

}

void HandleCollisions()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		while(true){		
			//never need to go past the first blank spot.
			if(collisionsQ[i][0]==-1)
				break;

			//deal with collisions appropriately. Better called overlaps.
			HandleSpecificCollision(collisionsQ[i][0], collisionsQ[i][1]);
			i++;
		}
	}
	EmptyCollisionQueue();		//should have handled all of them this frame.
}

bool CheckSniperFire(int index)
{
	int pc=GetPCIndex();
	float xDif=posComps[pc].x-posComps[index].x;
	float yDif=posComps[pc].y-posComps[index].y;
	float distance=std::sqrt(xDif*xDif + yDif*yDif);
	
	//law of sines
	float spreadAngle=asin(disComps[pc].radius/distance)*RADIANS_TO_DEGREES;
	//float spreadAngle=asin(disComps[pc].radius/distance);

	float trueAngle=FindAngleUsingDiff(xDif, yDif);
	
	if(posComps[index].angle>trueAngle-spreadAngle && posComps[index].angle<trueAngle+spreadAngle){
		//there has been a hit.
		return true;
	}

	return false;		//wow. don't forget that line again.
}

void CheckAllSniperFire()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==SNIPER_MASK){
			if(snipComps[i].firing){
				if(CheckSniperFire(i)){
					//eventually make this an event and deal with later.
					//std::cout << "Sniper hit. Deal with this later\n";
					//HACK
					healthComps[GetPCIndex()].health-=1;
				}
			}
		}
	}
}

//cull all dead entities
void CullDeadEntities()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&HEALTH_COMP){
			if(healthComps[i].health<=0)
				CullEntity(i);
		}
	}
}

void RegenPCHealth()
{
	double healthRegen=5.0/FRAMES_PER_SECOND;
	healthComps[GetPCIndex()].health+=healthRegen;

	if(healthComps[GetPCIndex()].health>100)
		healthComps[GetPCIndex()].health=100;
}

void UpdateEntities(Timer eTimer)
{
	SetPCAngle();
	SetVelocities();
	MoveEntities();

	//perform firing code for turret and bolts
	FireTurrets(eTimer);
	FireSnipers(eTimer);

	//Cull any bolts that went off screen
	CullBolts();

	//Check Collisions
	CheckCollisions();
	HandleCollisions();

	CheckAllSniperFire();

	//if PC dies, go back to menu
	if(healthComps[GetPCIndex()].health<=0){
		PG_ST=MENU;
	}

	RegenPCHealth();

	//don't want to call this before checking health. Would lead to errors.
	CullDeadEntities();

	//Updates the mouse position, because it moves when the player moves.
	//msX+=velComps[pc].xVel;
	//msY-=velComps[pc].yVel;
	//check if mouse off screen
}

void RenderCursor()
{
	//using time to get size and distance.
	float barX, barY;
	float percent=std::abs(sin((float)SDL_GetTicks()/200));		///1000 equals regular speed. /500 equals double speed.
	percent+=0.5;		//this way it never disappears.
	float barAx1=1.5*percent;	//used for width or height
	float barAx2=0.2*percent;	//used for opposite.
	//render left, right, up, down.
	barX=msX-barAx1;
	barY=msY+barAx2/2;
	RenderSprite(barX, barY, barAx1, barAx2, gameShader, &sprites.blackSprite, NULL, HUD);

	barX=msX+barAx1;
	RenderSprite(barX, barY, barAx1, barAx2, gameShader, &sprites.blackSprite, NULL, HUD);

	barX=msX-barAx2/2;
	barY=msY+barAx1;
	RenderSprite(barX, barY, barAx2, barAx1, gameShader, &sprites.blackSprite, NULL, HUD);

	barY=msY-barAx1;
	RenderSprite(barX, barY, barAx2, barAx1, gameShader, &sprites.blackSprite, NULL, HUD);
}

void RenderHealthBar(){
	//health bar always in the center of the screen
	float width=20;
	float height=width/4;
	float x=50;
	float y=3;
	//first render black background
	RenderSprite(x, y, width, height, gameShader, &sprites.blackSprite, NULL, HUD);

	//render slightly smaller white background inside.
	float insideBarWidth=width/30;
	width-=insideBarWidth*2;
	height-=insideBarWidth*2;
	RenderSprite(x, y, width, height, gameShader, &sprites.whiteSprite, NULL, HUD);

	//Render black bar for health
	int pcIndex=GetPCIndex();
	float newWidth=width*healthComps[pcIndex].health/100;
	x-=width*(1-(newWidth/width))/2;
	RenderSprite(x, y, newWidth, height, gameShader, &sprites.blackSprite, NULL, HUD);
}

//renders black dots away from PC until edge of screen.
void RenderAimLine()
{
	int pc=GetPCIndex();
	float tempX=posComps[pc].x;
	float tempY=posComps[pc].y;
	float gap=2;
	while(tempX>0&&tempX<100 && tempY>0&&tempY<100){
		tempX+=sin(posComps[pc].angle*DEGREES_TO_RADIANS)*gap;
		tempY-=cos(posComps[pc].angle*DEGREES_TO_RADIANS)*gap;

		RenderSprite(tempX, tempY, 0.2, gameShader, &sprites.blackSprite, NULL, PLAYER);
		gap*=1.5;
	}
}

void RenderSniperAimLines()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]==SNIPER_MASK){
			float gap=0.5;
			//shit. Need a timer. just do solid charging for now.
			float gapMultiple=1.5;
			if(snipComps[i].charging)
				gapMultiple=1.2;
			if(snipComps[i].firing)
				gapMultiple=1.0;
			float tempX=posComps[i].x;
			float tempY=posComps[i].y;
			while(tempX>0&&tempX<100 && tempY>0&&tempY<100){
				tempX+=sin(posComps[i].angle*DEGREES_TO_RADIANS)*gap;
				tempY-=cos(posComps[i].angle*DEGREES_TO_RADIANS)*gap;

				RenderSprite(tempX, tempY, 0.2, gameShader, &sprites.blackSprite, NULL, ENEMIES);
				gap*=gapMultiple;
			}
		}
	}
}

void RenderHUD()
{
	RenderCursor();
	RenderAimLine();
	RenderSniperAimLines();
	RenderHealthBar();
}

//sort of anti-functional programming here.
void PrepSpriteForRender(int index, Layers* pLay)
{
	if(allEntities.components[index]==BURN_MASK){
		if(burnComps[index].sinceMoved<500){
			spriteComps[index].pSprite=&sprites.blue;
		}else{
			spriteComps[index].pSprite=&sprites.red;
		}
		*pLay=FLOOR_EFFECTS;
	}else if(allEntities.components[index]==PICKUP_MASK){
		if(pickComps[index].type==HEALTH){
			spriteComps[index].pSprite=&sprites.healthPickupSprite;
		}else if (pickComps[index].type==WEAPONS){
			spriteComps[index].pSprite=&sprites.blue;
		}
		*pLay=PARTICLES;
	}else{
		*pLay=ENEMIES;
	}
}

//Still quite elegant.
void RenderEntities()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&SPRITE_COMP){
			Layers layer;
			PrepSpriteForRender(i, &layer);
			//don't flip y here.
			//render image at leftmost and topmost spot
			//also, need better angle logic
			RenderSprite(posComps[i].x, posComps[i].y, disComps[i].radius, gameShader, spriteComps[i].pSprite, &posComps[i].angle, layer);
		}
	}
}

//Returns pc index on success, -1 on failure. -1 will crash the program if not checked against.
int GetPCIndex()
{
	for(int i=0; i<MAX_ENTITIES; i++){
		if(allEntities.components[i]&PC_COMP) return i;
	}
	return -1;
}


