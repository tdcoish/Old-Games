#include "Events.h"
#include "GLOBALS.h"

//Just setting it up to not have any keys stored when initialized.
void SetUpKeyStore(KeyPress* pKeyStore)
{
    pKeyStore->store=0;     //I could & 0 if I wanted to be cool.
}

//I need to add mouse wheel scrolling for weapon changing.
void HandleInput(SDL_Event* pE, KeyPress* pKeyStore, GmMouse* pMs)
{
    while(SDL_PollEvent(pE)!=0){
        if(pE->type==SDL_KEYDOWN){
            HandleKey(pE->key.keysym.sym, pKeyStore);
        }else if(pE->type==SDL_KEYUP){
            HandleKeyUp(pE->key.keysym.sym, pKeyStore);
        }else if(pE->type==SDL_MOUSEBUTTONDOWN){
            pKeyStore->store=(pKeyStore->store|pKeyStore->mouse);
        }else if(pE->type==SDL_MOUSEBUTTONUP){
            if(pKeyStore->store&pKeyStore->mouse){  //special case changing state from menu, but good safety anyway.
                pKeyStore->store=(pKeyStore->store^pKeyStore->mouse);
            }
        }//else if(pE->type==SDL_MouseWheelEvent){
           // switch(pE->type)
        //}
    }
    //Update Mouse at the end.
    UpdateMouse(pMs, *pKeyStore);
}

void HandleKey(SDL_Keycode key, KeyPress* pKeyStore)
{
    switch(key){
        case SDLK_ESCAPE: pKeyStore->store=(pKeyStore->store|pKeyStore->escape);break;
        case SDLK_w: pKeyStore->store=(pKeyStore->store|pKeyStore->up); break;
        case SDLK_s: pKeyStore->store=(pKeyStore->store|pKeyStore->down); break;
        case SDLK_d: pKeyStore->store=(pKeyStore->store|pKeyStore->right); break;
        case SDLK_a: pKeyStore->store=(pKeyStore->store|pKeyStore->left); break;
        case SDLK_LSHIFT: pKeyStore->store=(pKeyStore->store|pKeyStore->shift); break;
    }
}

void HandleKeyUp(SDL_Keycode key, KeyPress* pKeyStore)
{
    //XOR everything. Since it should be true, it is made false.
    switch(key){
        case SDLK_ESCAPE: pKeyStore->store=(pKeyStore->store^pKeyStore->escape); break;
        case SDLK_w: pKeyStore->store=(pKeyStore->store^pKeyStore->up); break;
        case SDLK_s: pKeyStore->store=(pKeyStore->store^pKeyStore->down); break;
        case SDLK_d: pKeyStore->store=(pKeyStore->store^pKeyStore->right); break;
        case SDLK_a: pKeyStore->store=(pKeyStore->store^pKeyStore->left); break;
        case SDLK_LSHIFT: pKeyStore->store=(pKeyStore->store^pKeyStore->shift); break;
    }
}


///Below is for the Menus
///Stores the location of the mouse, as well as if any button is pressed
void UpdateMouse(GmMouse* pMs, const KeyPress keys)
{
    //eventually have a setting where the user can tweak mouse responsiveness
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    //increment x/y by the movement in pixels, divided by the scale factor
    pMs->clBx.x+=(float)x/GFX.scl;
    pMs->clBx.y+=(float)y/GFX.scl;
    if(pMs->clBx.x<0)pMs->clBx.x=0;
    if(pMs->clBx.x>100)pMs->clBx.x=100;
    if(pMs->clBx.y<0)pMs->clBx.y=0;
    if(pMs->clBx.y>100)pMs->clBx.y=100;

    //just and the keydown value to see if the mouse is pressed.
    pMs->pressed=false;
    if(keys.store&keys.mouse){
        pMs->pressed=true;
    }
}

void SetUpMouse(GmMouse* pMs)
{
    //logic won't allow the mouse to move off camera.
    pMs->clBx.x=50.f;
    pMs->clBx.y=50.f;
    pMs->clBx.w=2.f;
    pMs->clBx.h=2.f;

    if(!LoadTexture(bsPth+"\\Img\\Gameplay\\ms1.png", &pMs->pImg[0])) return;
    if(!LoadTexture(bsPth+"\\Img\\Gameplay\\ms2.png", &pMs->pImg[1])) return;
    if(!LoadTexture(bsPth+"\\Img\\Gameplay\\ms3.png", &pMs->pImg[2])) return;

    pMs->pressed=false;
}











///Below is for the Menus
///Stores the location of the mouse, as well as if any button is pressed
void UpdateMouse(UIMouse* pMs, SDL_Event e)
{
    //eventually have a setting where the user can tweak mouse responsiveness
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    //increment x/y by the movement in pixels, divided by the scale factor
    pMs->clBx.x+=(float)x/GFX.scl;
    pMs->clBx.y+=(float)y/GFX.scl;
    if(pMs->clBx.x<0)pMs->clBx.x=0;
    if(pMs->clBx.x>100)pMs->clBx.x=100;
    if(pMs->clBx.y<0)pMs->clBx.y=0;
    if(pMs->clBx.y>100)pMs->clBx.y=100;
    SetRenderBox(pMs->clBx, &pMs->rndBx);

    //workaround due to constant clicking otherwise.
    pMs->pressed=false;
    if(e.button.type==SDL_MOUSEBUTTONUP)pMs->stillPressed=false;
    if(!pMs->stillPressed){
        if(e.button.type==SDL_MOUSEBUTTONDOWN){
            pMs->stillPressed=true;
            pMs->pressed=true;
        }
    }
}

///Puts the mouse in the center of the screen, initializes values correctly
void SetUpMouse(UIMouse* pMs)
{
    pMs->clBx.x=50.f;
    pMs->clBx.y=50.f;
    pMs->clBx.w=2.f;
    pMs->clBx.h=2.f;

    //Make function that sets up render box.
    SetRenderBox(pMs->clBx, &pMs->rndBx);

    if(!LoadTexture(bsPth+"\\Img\\Menu\\ms.png", &pMs->pImg)) return;
    pMs->pressed=false;
    pMs->stillPressed=false;
}
