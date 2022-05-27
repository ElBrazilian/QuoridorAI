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
bool player_can_move_pawn(Player *player, Game *game){
    return true;
    // return player == game->playerA;
}
bool player_can_place_pawn(Game *game, Point p){
    return true;
}

void delete_player(Player *player){
    free(player->pos);
    free(player);
}

// WALL 
Wall *create_wall(Point *endA, Point *endB, Player *player){
    Wall *wall      = malloc(sizeof(Wall));
    if (endA->y == endB->y){
        // horizontal wall
        if (endA->x > endB->x){
            wall->endA      = endB; 
            wall->endB      = endA;
        } else {
            wall->endA      = endA; 
            wall->endB      = endB;
        }
    } else {
        // vertical wall
        if (endA->y > endB->y){
            wall->endA      = endB; 
            wall->endB      = endA;
        } else {
            wall->endA      = endA; 
            wall->endB      = endB;
        }
    }
    wall->placed_by = player;

    return wall; 
}

void delete_wall(Wall *wall){
    free(wall->endA);
    free(wall->endB);
    free(wall->placed_by);
    free(wall);
}
bool wall_placable(Point *a, Point *b){
    if (a->x == b->x){
        // check if vertical wall is placable
        // TODO
        return abs(a->y - b->y) == 2;
    } else if (a->y == b->y){
        // check if horizontal wall is placable
        // TODO
        return abs(a->x - b->x) == 2;
    }
    return false;
}
void add_wall_to_game(Game *game, Player *player, Point a, Point b){
    if (game->num_placed_walls < MAX_NUM_WALLS){
        // Create the wall
        Wall *wall = create_wall(
            create_point(a.x, a.y),
            create_point(b.x, b.y),
            player
        );

        // Add it to the game
        game->placed_walls[game->num_placed_walls] = wall;
        game->num_placed_walls++;
    } 
}

// GAME
Game *create_game(char playerA_name[], Point *playerA_pos, char playerB_name[], Point *playerB_pos){
    Game *game = malloc(sizeof(Game));
    game->num_placed_walls = 0;
    game->placed_walls = malloc(MAX_NUM_WALLS * sizeof(Wall *));
    game->playerA = create_player(playerA_name, playerA_pos);
    game->playerB = create_player(playerB_name, playerB_pos);
    game->current_turn = game->playerA;
    game->dragged_player = NULL;

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

void player_pos_to_pixel_pos(Player *player, Point *pixel_pos){
    pixel_pos->x = GRID_OFFSET_SIZE + player->pos->x * GRID_CELL_SIZE + GRID_CELL_SIZE / 2;
    pixel_pos->y = GRID_OFFSET_SIZE + player->pos->y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2;
}
void grid_pos_to_pixel_pos(Point *grid_pos, Point *pixel_pos){
    pixel_pos->x = GRID_OFFSET_SIZE + grid_pos->x * GRID_CELL_SIZE + GRID_CELL_SIZE / 2;
    pixel_pos->y = GRID_OFFSET_SIZE + grid_pos->y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2;
}
void corner_pos_to_pixel_pos(Point *grid_pos, Point *pixel_pos){
    pixel_pos->x = GRID_OFFSET_SIZE + grid_pos->x * GRID_CELL_SIZE;
    pixel_pos->y = GRID_OFFSET_SIZE + grid_pos->y * GRID_CELL_SIZE;
}
void wall_pos_to_pixel_pos(Wall *wall, Point *a, Point *b){
    a->x = GRID_OFFSET_SIZE + wall->endA->x * GRID_CELL_SIZE;
    a->y = GRID_OFFSET_SIZE + wall->endA->y * GRID_CELL_SIZE;

    b->x = GRID_OFFSET_SIZE + wall->endB->x * GRID_CELL_SIZE;
    b->y = GRID_OFFSET_SIZE + wall->endB->y * GRID_CELL_SIZE;
}
void pixel_pos_to_player_pos(Point *pixel_pos, Point *player_pos){
    player_pos->x = (pixel_pos->x - GRID_OFFSET_SIZE) / GRID_CELL_SIZE;
    player_pos->y = (pixel_pos->y - GRID_OFFSET_SIZE) / GRID_CELL_SIZE;
}

void draw_game(App *app){
    Game *game = app->game;
    Point p;

    draw_grid(app);
    draw_walls(app);
    draw_players(app);

    // draw hover corner if possible
    if (game->can_place_wall && game->close_to_corner){
        corner_pos_to_pixel_pos(game->corner_hovered, &p);

        if (game->corner_placed->x == -1 || game->corner_placed->y == -1){
            // not currently placing a wall
            SDL_SetRenderDrawColor(app->renderer, CORNER_HOVER_COLOR);
        } else if (wall_placable(game->corner_placed, game->corner_hovered)){
            SDL_SetRenderDrawColor(app->renderer, SECOND_COLOR_VALID_COLOR);
        } else {
            SDL_SetRenderDrawColor(app->renderer, SECOND_CORNER_INVALID_COLOR);
        }
        SDL_RenderFillCircle(app->renderer, p.x, p.y, CORNER_HOVER_RADIUS);

    }
    
    // draw placed corners
    if (game->corner_placed->x != -1 && game->corner_placed->y != -1){
        corner_pos_to_pixel_pos(game->corner_placed, &p);
        SDL_SetRenderDrawColor(app->renderer, CORNER_PREPLACED_COLOR);
        SDL_RenderFillCircle(app->renderer, p.x, p.y, CORNER_HOVER_RADIUS);
    }
}

void draw_players(App *app){
    Point p;
    Game *game = app->game;

    // draw player A
    if (game->dragged_player == game->playerA){
        p.x = app->mouse_pos->x;
        p.y = app->mouse_pos->y;
    } else {
        player_pos_to_pixel_pos(game->playerA, &p);
    }
    SDL_SetRenderDrawColor(app->renderer, PLAYERA_COLOR);
    SDL_RenderFillCircle(app->renderer, p.x, p.y, PLAYER_RADIUS);

    // draw player B
    if (game->dragged_player == game->playerB){
        p.x = app->mouse_pos->x;
        p.y = app->mouse_pos->y;
    } else {
        player_pos_to_pixel_pos(game->playerB, &p);
    }
    SDL_SetRenderDrawColor(app->renderer, PLAYERB_COLOR);
    SDL_RenderFillCircle(app->renderer, p.x, p.y, PLAYER_RADIUS);
}

void draw_walls(App *app){
    Game *game = app->game;
    Point a, b;
    SDL_Rect r;
    
    SDL_SetRenderDrawColor(app->renderer, WALL_COLOR);
    for (int i = 0; i < game->num_placed_walls; i++){
        wall_pos_to_pixel_pos(game->placed_walls[i], &a, &b);

        if (a.y == b.y){
            // horizontal wall
            r.x = a.x - WALL_WIDTH / 2;
            r.y = a.y - WALL_WIDTH / 2;
            r.w = GRID_CELL_SIZE * 2 + WALL_WIDTH;
            r.h = WALL_WIDTH;
            SDL_RenderFillRect(app->renderer, &r);
        } else {
            // vertical wall
            r.x = a.x - WALL_WIDTH / 2;
            r.y = a.y - WALL_WIDTH / 2;
            r.w = WALL_WIDTH;
            r.h = GRID_CELL_SIZE * 2 + WALL_WIDTH;
            SDL_RenderFillRect(app->renderer, &r);
        }
    }
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

bool find_closest_corner(Point *mouse_pos, Point *res){
    res->x = (mouse_pos->x - GRID_OFFSET_SIZE + GRID_CELL_SIZE / 2) / GRID_CELL_SIZE;
    res->y = (mouse_pos->y - GRID_OFFSET_SIZE + GRID_CELL_SIZE / 2) / GRID_CELL_SIZE;

    Point tmp;
    corner_pos_to_pixel_pos(res, &tmp);

    Point p = {mouse_pos->x - tmp.x, mouse_pos->y - tmp.y};
    return p.x * p.x + p.y * p.y < MAX_DISTANCE_TO_CORNER_SQ;
}