#include "game.h"

void update(App *app){
}

void handle_events(App *app){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            app->continuer = false;
        }
    }
}

void draw(App *app){
}