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
    player->num_walls = MAX_NUM_WALLS / 2;

    return player;
}
bool player_can_move_pawn(Player *player, Game *game){
    return (game->current_turn == player) && (game->corner_placed->x == -1);
    // return player == game->playerA;
}
bool wall_block_path(Game *game, Point a, Point b){
    for (int i = 0; i < game->num_placed_walls; i++){
        Wall *wall = game->placed_walls[i];
        if (a.x == b.x){
            // in the vertical position
            // so a wall can block only if the wall is horizontal
            if (wall->endA->y == wall->endB->y){
                if (((a.y < b.y) && (a.y < wall->endA->y && wall->endA->y <= b.y)) || ((a.y > b.y) && (b.y < wall->endA->y && wall->endA->y <= a.y))) {
                    if (((wall->endA->x < wall->endB->x) && (wall->endA->x <= a.x && a.x < wall->endB->x)) || ((wall->endA->x > wall->endB->x) && (wall->endB->x <= a.x && a.x < wall->endA->x))){
                        return true;
                    }
                }
            }
        } else if (a.y == b.y){
            // in the horizontal position
            // so a wall can block only if the wall is vertical
            if (wall->endA->x == wall->endB->x){
                if (((a.x < b.x) && (a.x < wall->endA->x && wall->endA->x <= b.x)) || ((a.x > b.x) && (b.x < wall->endA->x && wall->endA->x <= a.x))) {
                    if (((wall->endA->y < wall->endB->y) && (wall->endA->y <= a.y && a.y < wall->endB->y)) || ((wall->endA->y > wall->endB->y) && (wall->endB->y <= a.y && a.y < wall->endA->y))){
                        return true;
                    }
                }
            }
        }
    } 
    return false;
}
void update_next_pawn_position(Game *game, int *current_overflow_index, int i, int dx, int dy){
    Player *other_player = NULL;
    if (game->current_turn == game->playerA)    other_player = game->playerB;
    else                                        other_player = game->playerA;

    // printf("%d,%d => %d,%d, (%d)\n", game->available_positions[i]->x,game->available_positions[i]->y,other_player->pos->x,other_player->pos->y,(!(game->available_positions[i]->x == other_player->pos->x && game->available_positions[i]->y == other_player->pos->y)));

    game->available_positions[i]->x = game->current_turn->pos->x+dx;
    game->available_positions[i]->y = game->current_turn->pos->y+dy;
    
    if (wall_block_path(game, *(game->available_positions[i]), *(game->current_turn->pos))){
        // BLOCKED SO WE SKIP THIS ONE
        game->available_positions[i]->x = -1;
        game->available_positions[i]->y = -1;
        return;
    }
        
    if ((game->available_positions[i]->x == other_player->pos->x && game->available_positions[i]->y == other_player->pos->y))
    {
        // a player blocks this position
        game->available_positions[i]->x += dx;
        game->available_positions[i]->y += dy;
        if (wall_block_path(game, *(game->available_positions[i]), *(game->current_turn->pos))){
            // BLOCKED SO WE CHECK THE OTHER ONES

            Point tmp_next;
            if (dy == 0){
                tmp_next.x = game->current_turn->pos->x+dx; 
                
                for (int ddy = -1; ddy <= 1; ddy += 2){
                    tmp_next.y = game->current_turn->pos->y - ddy;
                    if (!wall_block_path(game, tmp_next, *(other_player->pos))){
                        game->available_positions[*current_overflow_index]->x = tmp_next.x;
                        game->available_positions[*current_overflow_index]->y = tmp_next.y;
                        (*current_overflow_index)++;
                    }
                }
            } else if (dx == 0){
                tmp_next.y = game->current_turn->pos->y+dy; 
                
                for (int ddx = -1; ddx <= 1; ddx += 2){
                    tmp_next.x = game->current_turn->pos->x - ddx;
                    if (!wall_block_path(game, tmp_next, *(other_player->pos))){
                        game->available_positions[*current_overflow_index]->x = tmp_next.x;
                        game->available_positions[*current_overflow_index]->y = tmp_next.y;
                        (*current_overflow_index)++;
                    }
                }
            } else {
                printf("[ERROR] In update_next_pawn_position\n");
            }
            game->available_positions[i]->x = -1;
            game->available_positions[i]->y = -1;
        }
    }

}
void update_available_pawn_positions(Game *game){
    // check to the left
    // Player *other_player = NULL;
    // if (game->current_turn == game->playerA)    other_player = game->playerB;
    // else                                        other_player = game->playerA;
    // clear available positions
    for (int i = 0; i < 10; i++){
        game->available_positions[i]->x = -1;
        game->available_positions[i]->y = -1;
    }
    int current_overflow_index = 4;


    update_next_pawn_position(game, &current_overflow_index, 0, 1, 0);
    update_next_pawn_position(game, &current_overflow_index, 1, -1, 0);
    update_next_pawn_position(game, &current_overflow_index, 2, 0, 1);
    update_next_pawn_position(game, &current_overflow_index, 3, 0, -1);
}
bool player_can_place_pawn(Game *game, Point p){
    // ie is p in the available positions,
    for (int i = 0; i < MAX_AVAILABLE_POSITIONS; i++){
        if (p.x == game->available_positions[i]->x && p.y == game->available_positions[i]->y){
            return true;
        }
    }    
    return false;
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
bool wall_placable(App *app, Point *a, Point *b){
    if (app->game->current_turn->num_walls > 0){
        if (a->x == b->x){
            // check if vertical wall is placable
            // TODO
            return abs(a->y - b->y) == 2;
        } else if (a->y == b->y){
            // check if horizontal wall is placable
            // TODO
            return abs(a->x - b->x) == 2;
        }
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

    game->available_positions = malloc(MAX_AVAILABLE_POSITIONS * sizeof(Point *));
    for (int i = 0; i < MAX_AVAILABLE_POSITIONS; i++){
        game->available_positions[i] = create_point(-1,-1);
    }

    return game;
}
void change_turn(App *app){
    if (app->game->current_turn == app->game->playerA){
        app->game->current_turn = app->game->playerB;
    } else if (app->game->current_turn == app->game->playerB){
        app->game->current_turn = app->game->playerA;
    } else {
        printf("[ERROR] Invalid current player\n");
        app->continuer = false;
    }
}

void delete_game(Game *game){
    delete_player(game->playerA);
    delete_player(game->playerB);
    for (int i = 0; i < game->num_placed_walls; i++){
        delete_wall(game->placed_walls[i]);
    }
    for (int i = 0; i < MAX_AVAILABLE_POSITIONS; i++){
        free(game->available_positions[i]);
    }
    free(game->available_positions);
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
    SDL_Rect r = {0, 0, WIDTH, GRID_CELL_SIZE};

    // DEBUG DRAW TO SHOW WHICH PLAYER IS PLAYING
    if (game->current_turn == game->playerA){
        SDL_SetRenderDrawColor(app->renderer, PLAYERA_COLOR);
    } else if (game->current_turn == game->playerB) {
        SDL_SetRenderDrawColor(app->renderer, PLAYERB_COLOR);
    } else {
        app->continuer = false;
        printf("[ERROR] Invalid current player\n");
    }
    SDL_RenderFillRect(app->renderer, &r);

    draw_remaining_tiles(app);
    draw_grid(app);
    draw_walls(app);
    draw_players(app);

    // draw hover corner if possible
    if (game->can_place_wall && game->close_to_corner){
        corner_pos_to_pixel_pos(game->corner_hovered, &p);

        if (game->corner_placed->x == -1 || game->corner_placed->y == -1){
            // not currently placing a wall
            SDL_SetRenderDrawColor(app->renderer, CORNER_HOVER_COLOR);
        } else if (wall_placable(app, game->corner_placed, game->corner_hovered)){
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

void draw_player_remaining_tiles(SDL_Renderer *renderer, int remaining_walls, SDL_Rect *r){
    int y_delta = GRID_SIZE * GRID_CELL_SIZE / MAX_NUM_WALLS;
    int start_y = GRID_OFFSET_SIZE + y_delta * MAX_NUM_WALLS/4;
    r->y = GRID_OFFSET_SIZE;
    
    for (int i = 0; i < remaining_walls; i++){
        r->y = start_y + i * y_delta;
        SDL_RenderFillRect(renderer, r);

    }
}
void draw_remaining_tiles(App *app){
    SDL_Rect r;
    
    // Draw for player A on the left
    r.x = GRID_OFFSET_SIZE / 2;
    r.w = GRID_OFFSET_SIZE / 2;
    r.h = WALL_WIDTH;
    SDL_SetRenderDrawColor(app->renderer, WALL_COLOR);
    draw_player_remaining_tiles(app->renderer, app->game->playerA->num_walls, &r);

    r.x = GRID_OFFSET_SIZE + GRID_SIZE * GRID_CELL_SIZE;
    draw_player_remaining_tiles(app->renderer, app->game->playerB->num_walls, &r);
}
void draw_players(App *app){
    Point p;
    Game *game = app->game;

    // draw available positions for the current player
    if (game->dragged_player != NULL){
        SDL_SetRenderDrawColor(app->renderer, AVAILABLE_POSITION_COLOR);
        for (int i = 0; i < MAX_AVAILABLE_POSITIONS; i++){
            Point cur = *(app->game->available_positions[i]);
            Point p = {0,0};

            if (cur.x >= 0 && cur.x < GRID_SIZE && cur.y >= 0 && cur.y < GRID_SIZE){
                grid_pos_to_pixel_pos(&cur, &p);
                SDL_RenderFillCircle(app->renderer, p.x, p.y, PLAYER_RADIUS);
            }
        }
    }

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