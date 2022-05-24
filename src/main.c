#include "main.h"

#define TARGET_FPS 60


void initialize_app(App *app){
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the window
    app->window = SDL_CreateWindow(app->windowTitle, BASE_WINDOW_X_POS, BASE_WINDOW_Y_POS, WIDTH, HEIGHT, app->windowFlags);
    if (!app->window){
        printf("Failed to open the window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the renderer
    app->renderer = SDL_CreateRenderer(app->window, -1, app->rendererFlags);
    if (!app->renderer){
        printf("Failed to open the renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the timekeeper object
    app->keeper = create_timekeeper(TARGET_FPS);

    // Load the debug font and init TTF
    TTF_Init();
    app->debug_font = TTF_OpenFont("fonts/SpaceMono-Regular.ttf", DEBUG_FONT_SIZE);


    app->continuer = true;
}

void destroy_app(App *app){
    destroy_timekeeper(app->keeper);

    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    TTF_Quit();
    SDL_Quit();
    free(app);
}

int main(int argc, char *argv[]){
    // Creating and initializing the app
    App *app = malloc(sizeof(App));
    app->rendererFlags  = SDL_RENDERER_ACCELERATED;
    app->windowFlags    = 0;
    strcpy(app->windowTitle, BASE_WINDOW_TITLE);

    initialize_app(app);

    while(app->continuer){
        // HANDLING EVENTS
        handle_events(app);
        update_timekeeper_handle(app->keeper);

        // UPDATE 
        update(app);
        update_timekeeper_update(app->keeper);

        // DRAW
        SDL_SetRenderDrawColor(app->renderer, 0,0,0,255);
        SDL_RenderClear(app->renderer);
        timekeeper_draw_debug_info(app->keeper, app->renderer, app->debug_font);
        draw(app);
        SDL_RenderPresent(app->renderer);
        update_timekeeper_draw(app->keeper);


        // DELAY TO STAY AT TARGET FPS
        timekeeper_limit(app->keeper);
        timekeeper_computeFPS(app->keeper);

        //printf("FPS: %f\n", app->keeper->currentFPS);
        // printf("frame: %d\n", app->keeper->frame_length_raw);
    }


    // Destroy everything
    destroy_app(app);
    return 0;
}