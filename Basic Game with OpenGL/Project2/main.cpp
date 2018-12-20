/*********************
This time I'm going to get everything up and running without SVN.

Beautiful. Everything is up and running. The SDL2.dll is in the right place.

Okay so now I have this really simple program that handles pressing kEscape over multiple files.
Next step is to get SVN running on this so that I can play around with SVN.

Easy way to make changes is simply to fill in the key functions.

Now for revision 4 I'm going to add a new file that adds a timer. I'm not taking any code 
from here so this should work just fine.

Ohhhhhhhhhhhhhhhhhhhhhhhhhhhh!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Looks like it reverted just peachily and then reverted back beautifully.
The only difference is that the Timer.h and Timer.cpp files were automatically closed when
reverting (since they no longer existed), and then weren't automatically re-opened when I switched
forward again.

More interesting stuff. So I reverted, and then started commiting again. I should have made a fork, because
now SVN is going a bit crazy.

Wow. So I reverted twice, then tried to commit, then updated the working copy and manually edited the Globals.h
file to get everything working. Then SVN decided to just delete most of the body of the file. Wow thanks.
Anyway, looks like SVN made my decision for me in terms of which way to go. I'm not using any structs in the Input
handling yet.

Threw an update that involved creating two files (the Init's), and creating two global variables. All told touched 5 files.
SVN handled it beautifully. I felt so confident knowing that I could always just revert.

Honestly, crazy as it sounds. I truly believe that now is the time to start heavily designing the game, all the way from 
the top. I certainly need some more help with OpenGL though.

Crazy thing happened. SVN made the project open in a new folder, Project 8. No idea why this happened.
Christ. Caused me a lot of grief.

--------------------------------------------------------------------------------------------------------------------------

3/31/2018 Preliminary notes:
One thing that stands out to me, looking through this code, is the total lack of OOP. It's one thing to not use poor practices,
but there's really no reason to have zero objects, especially things like entities, input, and various queues. Remember, OOP is
just the synergy of data and functions, no need to be idealogical. The very first thing I have done with my new project is to turn
the above mentioned items into classes of their own.

Secondly, and more importantly, there are some horrible architectural decisions made here. Even this main function is terrible. It 
would be much better to create something like:

class Level{
	virtual Input();		//handle input level specific way
	virtual Logic();		//level logic
	virtual Render();		//render level
}

class MainMenu : Level {
	//write implementations here
}

Levels need logic to go to the next level, and the way to do that is with another LevelManager class, so we can do things like press a 
button in the main menu, which fires of "I've been pressed" to the LevelManager event queue, which then switches levels for us.

Thirdly, and I'm not even sure I should touch on this now given how ridiculous it is, but the Entities system is just terrible. I almost 
appreciate what I was trying to go for here, but I clearly failed. It would actually be much, much better to just shove every single last 
detail of every enemy into one single enemyType class, than do my horrible parts of arrays garbage. To expand:

class Entities{
	//all general stuffs
	velocity[256];
	position[256];

	//every non-general thing
	hunterComponent[256];
	sniperComponent[256];

	//enum for type and general flags.
	entityType type[256];
	Uint32 flags[256];
};

This system is pretty bad, due to the fact that we have massively wasteful memory requirements, however, it is leagues above my idea, which was a total
mess. We still have the problem of constantly having to check the types before then doing the specific logic to those things, which can be solved through 
architectural changes. 

We can either store function pointers that do the right update, or we can create entities as specific objects, which then call update()
on themselves, and therefore don't need the if statements, since it's all implied in the first place. We can then iterate through the objects, and selectively
add them to some queues. So, for instance, we add a pointer to the object to the collisionQueue, which checks collisions, do this for all objects that are relevant,
and then perform functions on that data. Example:

class Entity{
	//all the typical stuff here
	velocity;
	position;

	Uint32 flags;

	AddSelfToColQueue(){
		colQ.AddItem(this);
	}
};

class Hunter : Entity {
	bool lastCharge;
	int someStuff;
};

This is a whole hell of a lot better than our current code. I don't think it can be understated, the main impediment is Timothy Time, not CPU time.

However, if we wanted to go the function pointer route that is also very interesting. Every update function needs only to take the index of the enemy type, and it 
can be stored in the entity at creation. We first need a variable that stores a function pointer, let's get one for update:

class Entity{
	//everything else

	//update function pointer
	void (*Update)(int);		//as long as all updates return void or the same value.
}

When we create the object in the first part, we then set this function pointer.

CreateEntity(enum type){
	if (type == PC){
		//set vars, blah blah.
		//more interesting, set function pointers
		Update = &UpdatePC;		//function UpdatePC exists somewhere.
	}
	if(type == HUNTER){
		//set vars, blah blah
		Update = &UpdateHunter;
	}
}

Later in our UpdateAll method:
{
	for(int i=0; i<MAX_ENTITIES; i++){
		(*Update)(i);		//calls the specific update function for this entity type
	}
}

This is so elegant that I think I might actually go this route. Especially because I want to ask James what data structure we can hold all the entities in. If
he can't give me a good answer then I think I might actually go with this. The other way does seem better, I just can't for the life of me even figure out how 
to pack all the entities together.

Actually I think James might have a very good answer here in the Factory design pattern. In that you create an array of type superclass, and then you have a seperate
function call that creates the subclasses in that function. So for example:

//Entitites super class
class Entity{
	//vars
	public:
		static Entity* MakeEntity(entType TYPE);
	//other things
	virtual void Update() = 0;		//why not pure virtual?
};

class Hunter : Entity{
	//hunter stuff
	//also
	void Update() {};
};

//other derived classes, like player character

//create our array of entities pointers.		//this is cache poor, but coder friendly.
Entity* entitiesPointers[MAX_SIZE];

Entity* Stooge::make_stooge(entType TYPE)	//entType enum
{
	if (type==PC){
		return new PC;
	}
	else if (type == HUNTER){
		return new Hunter;
	}
	else if (type == JACKAL){
		return new Jackal;
	}
}

This probably won't work for objects inheriting from two different objects at the top level, but that doesn't seem too common. More practically, I want to make sure
that in the case of multiple levels of inheritance, such as:

class Entity{};

class Actor : Entity {};

class Hunter : Actor {};

That we can still call the factory and have it correctly store that pointer in our array. 

If all that is possible we can clearly see the difference between the two architectures. My initial concept, which I'll call the flat style, has massive benefits 
in terms of cache coherency, at the expense of elegance, readability, and memory bloat. The inheritance style has massively more cache misses, but is extremely 
readable, and it is probably much easier to add new enemy types to. Having said all that, using function pointers makes me feel like a badass. I wonder, could it 
be possible to store all the objects together, like in the first example, but then intelligently "disable" the unused parts? If so that might very well be the best 
of both worlds. The more I think about this the more it appears to be tantalizingly close. 

*********************/

#include <cstdio>

#include "Globals.h"
#include "Init.h"
#include "Games.h"
#include "Menu.h"

int main(int args, char** argc)
{
	//handles window creation, glContext creation, vsync --- for now
	Init();
	printf("Base path: %s\n", bsPth.c_str());

	bool quitGame=false;
	while(!quitGame){
		switch(PG_ST){
		case MENU: if(Menu()!=0)return -1; break;
		case GAME: if(Game()!=0)return -1; break;
		case QUIT: quitGame=true; break;
		}
	}
	
	return 0;
}


