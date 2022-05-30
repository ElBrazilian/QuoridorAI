#include "main.h"


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
    app->debug_draw = DRAW_DEBUG_ON_START;

    // Create game
    app->game = create_game(
        PLAYERA_NAME, create_point(PLAYERA_POSX, PLAYERA_POSY),
        PLAYERB_NAME, create_point(PLAYERB_POSX, PLAYERB_POSY)
    );
    app->mouse_pos      = create_point(0,0);
    app->mouse_state    = 0;

    // Point a, b;
    // a.x = 1;
    // a.y = 1;
    // b.x = 3;
    // b.y = 1;
    // add_wall_to_game(app->game, app->game->playerA, a, b);
    
    // a.x = 2;
    // a.y = 1;
    // b.x = 2;
    // b.y = 3;
    // add_wall_to_game(app->game, app->game->playerA, a, b);

    app->game->can_place_wall   = true;
    app->game->corner_hovered   = create_point(-1,-1);
    app->game->close_to_corner  = false;
    app->game->corner_placed    = create_point(-1,-1);
    // app->game->tmp_wall         = malloc(sizeof(Wall));

    app->continuer              = true;

}

void destroy_app(App *app){
    destroy_timekeeper(app->keeper);
    // delete_game(app->game);

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
        // update mouse pos
        app->mouse_state = SDL_GetMouseState(&(app->mouse_pos->x), &(app->mouse_pos->y));

        // HANDLING EVENTS
        handle_events(app);
        update_timekeeper_handle(app->keeper);

        // UPDATE 
        update(app);
        update_timekeeper_update(app->keeper);

        // DRAW
        SDL_SetRenderDrawColor(app->renderer, 0,0,0,255);
        SDL_RenderClear(app->renderer);
        draw(app);

        if (app->debug_draw) timekeeper_draw_debug_info(app->keeper, app->renderer, app->debug_font);
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


void update(App *app){
    Game *game = app->game;
    if (game->dragged_player != NULL){
        update_available_pawn_positions(app->game);
    }
    game->can_place_wall = (game->dragged_player == NULL);
}

void handle_events(App *app){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            app->continuer = false;
        } else if (e.type == SDL_KEYDOWN){
            if (e.key.keysym.sym == SDLK_d){
                app->debug_draw = !app->debug_draw;
            } else if (e.key.keysym.sym == SDLK_ESCAPE){
                app->continuer = false;
            } 
        } else if (e.type == SDL_MOUSEBUTTONDOWN){
            if (e.button.button == SDL_BUTTON_LEFT){
                // On button press
                Point p;
                pixel_pos_to_player_pos(app->mouse_pos, &p);
                if (p.x == app->game->playerA->pos->x && p.y == app->game->playerA->pos->y){
                    if (player_can_move_pawn(app->game->playerA, app->game)){
                        app->game->dragged_player = app->game->playerA;
                    }
                } else if (p.x == app->game->playerB->pos->x && p.y == app->game->playerB->pos->y){
                    if (player_can_move_pawn(app->game->playerB, app->game)){
                        app->game->dragged_player = app->game->playerB;
                    }
                } else if (app->game->can_place_wall && app->game->close_to_corner) {
                    // placing a wall

                    if (app->game->corner_placed->x == -1 || app->game->corner_placed->y == -1){
                        // placing the first corner of the wall
                        if (app->game->corner_hovered->x != 0 && app->game->corner_hovered->y != 0 && app->game->corner_hovered->x != GRID_SIZE && app->game->corner_hovered->y != GRID_SIZE){
                            app->game->corner_placed->x = app->game->corner_hovered->x;
                            app->game->corner_placed->y = app->game->corner_hovered->y;
                        }
                    } else {
                        // placing the entire wall if possible
                        if (wall_placable(app, app->game->corner_placed, app->game->corner_hovered)){
                            printf("add wall\n");
                            add_wall_to_game(app->game, app->game->current_turn, *(app->game->corner_placed), *(app->game->corner_hovered));
                            app->game->corner_placed->x = -1;
                            app->game->corner_placed->y = -1;

                            // Change player turn etc
                            app->game->current_turn->num_walls--; 
                            change_turn(app);
                        }
                    }
                }
            } else if (e.button.button == SDL_BUTTON_RIGHT){
                if (app->game->corner_placed->x != -1){
                    app->game->corner_placed->x = -1;
                    app->game->corner_placed->y = -1;
                }
            }
        } else if (e.type == SDL_MOUSEBUTTONUP){
            if (e.button.button == SDL_BUTTON_LEFT){
                if (app->game->dragged_player != NULL){
                    Point p;
                    pixel_pos_to_player_pos(app->mouse_pos, &p);
                    if (player_can_place_pawn(app->game, p)){
                        // UPDATE THE PLAYERS POSITION
                        app->game->dragged_player->pos->x   = p.x;
                        app->game->dragged_player->pos->y   = p.y;
                        app->game->dragged_player           = NULL;

                        if (app->game->playerA->pos->x == GRID_SIZE-1){
                            // Player A win
                            app->continuer = false;
                            printf("Player A win\n");
                        } else if (app->game->playerB->pos->x == 0){
                            app->continuer = false;
                            printf("Player B win\n");
                        } else {
                            change_turn(app);
                        }
                    } else {
                        // release the player in it's prev position
                        app->game->dragged_player = NULL;
                    }
                }
            }
        }
    }

    // Checks if the mouse is close enough to a corner
    app->game->close_to_corner = find_closest_corner(app->mouse_pos, app->game->corner_hovered);
}

void draw(App *app){
    draw_game(app);
}