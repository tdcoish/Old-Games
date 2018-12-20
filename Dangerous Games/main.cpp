#include <SDL.h>
#include <cstdio>
#include "GLOBALS.h"
#include "Helpers.h"
#include "Menus.h"
#include "Games.h"


int main(int args, char* argc[])
{
    //Init is now even more important, since it initializes the graphics struct
    if(!Init()){
        printf("Failed initialization\n");
        return -1;
    }

    bool quitGame=false;
    while(!quitGame){
        switch(PG_ST){      //works even though PG_ST is a Uint16. Maybe should write the case's different.
            case MENU: if(RunMenu()!=0) return 1; break;
            case GAME: if(RunGames()!=0)return 1; break;
            case QUIT: quitGame=true; break;
        }
    }

    return 0;
}
