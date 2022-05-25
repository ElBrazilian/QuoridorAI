#include "game.h"


// POIN
Point *create_point(int x, int y){
    Point *point  = malloc(sizeof(Point));
    point->x       = x;
    point->y       = y;

    return point;
}

void delete_point(Point *point){
    free(point);
}


// PLAYER
Player *create_player(char name[], Point *base_pos){
    Player *player  = malloc(sizeof(Player));
    strcpy(player->name, name);
    player->pos     = base_pos;

    return player;
}

void delete_player(Player *player){
    free(player->pos);
    free(player);
}

// WALL 
Wall *create_wall(Point *endA, Point *endB, Player *player){
    Wall *wall      = malloc(sizeof(Wall));
    wall->endA      = endA; 
    wall->endB      = endB;
    wall->placed_by = player;

    return wall; 
}

void delete_wall(Wall *wall){
    free(wall->endA);
    free(wall->endB);
    free(wall->placed_by);
    free(wall);
}

// GAME
Game *create_game(char playerA_name[], Point *playerA_pos, char playerB_name[], Point *playerB_pos){
    Game *game = malloc(sizeof(Game));
    game->num_placed_walls = 0;
    game->placed_walls = malloc(MAX_NUM_WALLS * sizeof(Wall *));
    game->playerA = create_player(playerA_name, playerA_pos);
    game->playerB = create_player(playerB_name, playerB_pos);

    return game;
}

void delete_game(Game *game){
    delete_player(game->playerA);
    delete_player(game->playerB);
    for (int i = 0; i < game->num_placed_walls; i++){
        delete_wall(game->placed_walls[i]);
    }
    free(game);
}

void draw_grid(App *app){
    // draw game background

    SDL_Rect r;
    r.x = GRID_OFFSET_SIZE;
    r.y = GRID_OFFSET_SIZE;
    r.w = GRID_EFFECTIVE_SIZE;
    r.h = GRID_EFFECTIVE_SIZE;
    SDL_SetRenderDrawColor(app->renderer, GAME_COLOR);
    SDL_RenderFillRect(app->renderer, &r);
   
    // draw inner 
    SDL_SetRenderDrawColor(app->renderer, GRID_COLOR);
    for (int index = 1; index < GRID_SIZE; index++){
        int local_pos = index * GRID_CELL_SIZE;

        // vertical line
        r.x = GRID_OFFSET_SIZE + local_pos - GRID_WIDTH/2;
        r.y = GRID_OFFSET_SIZE + 0 - GRID_WIDTH/2;
        r.w = GRID_WIDTH;
        r.h = GRID_EFFECTIVE_SIZE + GRID_WIDTH;
        SDL_RenderFillRect(app->renderer, &r);

        // horizontal line
        r.x = GRID_OFFSET_SIZE + 0 - GRID_WIDTH/2;
        r.y = GRID_OFFSET_SIZE + local_pos - GRID_WIDTH/2;
        r.w = GRID_WIDTH + GRID_EFFECTIVE_SIZE;
        r.h = GRID_WIDTH;
        SDL_RenderFillRect(app->renderer, &r);

        // thickLineRGBA(app->renderer, GRID_OFFSET_SIZE + local_pos, GRID_OFFSET_SIZE, GRID_OFFSET_SIZE + local_pos, GRID_OFFSET_SIZE + GRID_EFFECTIVE_SIZE, GRID_WIDTH, GRID_COLOR);
        // thickLineRGBA(app->renderer, GRID_OFFSET_SIZE, GRID_OFFSET_SIZE + local_pos, GRID_OFFSET_SIZE + GRID_EFFECTIVE_SIZE, GRID_OFFSET_SIZE + local_pos, GRID_WIDTH, GRID_COLOR);
    }

    // draw outer

    SDL_SetRenderDrawColor(app->renderer, OUTER_GRID_COLOR);
    // left line
    r.x = GRID_OFFSET_SIZE + 0 - OUTER_GRID_WIDTH/2;
    r.y = GRID_OFFSET_SIZE + 0 - OUTER_GRID_WIDTH/2;
    r.w = OUTER_GRID_WIDTH;
    r.h = GRID_EFFECTIVE_SIZE + OUTER_GRID_WIDTH;
    SDL_RenderFillRect(app->renderer, &r);
    // right line
    r.x = GRID_OFFSET_SIZE + GRID_EFFECTIVE_SIZE - OUTER_GRID_WIDTH/2;
    r.y = GRID_OFFSET_SIZE + 0 - OUTER_GRID_WIDTH/2;
    r.w = OUTER_GRID_WIDTH;
    r.h = GRID_EFFECTIVE_SIZE + OUTER_GRID_WIDTH;
    SDL_RenderFillRect(app->renderer, &r);
    // top line
    r.x = GRID_OFFSET_SIZE + 0 - OUTER_GRID_WIDTH/2;
    r.y = GRID_OFFSET_SIZE + 0 - OUTER_GRID_WIDTH/2;
    r.w = GRID_EFFECTIVE_SIZE + OUTER_GRID_WIDTH;
    r.h = OUTER_GRID_WIDTH;
    SDL_RenderFillRect(app->renderer, &r);
    // bottom line
    r.x = GRID_OFFSET_SIZE + 0 - OUTER_GRID_WIDTH/2;
    r.y = GRID_OFFSET_SIZE + GRID_EFFECTIVE_SIZE - OUTER_GRID_WIDTH/2;
    r.w = GRID_EFFECTIVE_SIZE + OUTER_GRID_WIDTH;
    r.h = OUTER_GRID_WIDTH;
    SDL_RenderFillRect(app->renderer, &r);
}