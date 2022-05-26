/**
 * @file game.h
 * @author Nathan Schiffmacher (nathan.schiffmacher@student.isae-supaero.fr)
 * @brief 
 * @date 24-05-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */ 

#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "options.h"
#include "SDL_render.h"

#define GRID_SIZE 9
#define MAX_NUM_WALLS 20
#define MAX_NAME_SIZE 150

// DRAW OPTIONS
#define DRAW_DEBUG_ON_START false

#define GRID_OFFSET 1/20 // percentage of the size
#define GRID_EFFECTIVE_SIZE (SIZE - 2 * SIZE * GRID_OFFSET) //(SIZE * (1 - 2 * GRID_OFFSET))
#define GRID_OFFSET_SIZE SIZE * GRID_OFFSET
#define GAME_COLOR 200,200,200,255
#define GRID_COLOR 255,255,255,255
#define GRID_WIDTH 10
#define OUTER_GRID_COLOR 255,0,0,255
#define OUTER_GRID_WIDTH 15

#define GRID_CELL_SIZE (GRID_EFFECTIVE_SIZE/GRID_SIZE)

// WALL
#define WALL_COLOR 0,0,255,255
#define WALL_WIDTH (GRID_WIDTH * 16 / 10)

#define PLAYER_RADIUS_PRC 6/10 // prc of the grid size
#define PLAYER_RADIUS (GRID_CELL_SIZE * PLAYER_RADIUS_PRC / 2) 

// Player A
#define PLAYERA_NAME "Player A"
#define PLAYERA_POSX 0
#define PLAYERA_POSY 4
#define PLAYERA_COLOR 255,0,0,255

// Player B
#define PLAYERB_NAME "Player B"
#define PLAYERB_POSX 8
#define PLAYERB_POSY 4
#define PLAYERB_COLOR 0,255,0,255

typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[MAX_NAME_SIZE];
    Point *pos; 
} Player;

typedef struct {
    Point *endA;
    Point *endB;

    Player *placed_by;
} Wall;

typedef struct {
    Player *playerA;
    Player *playerB;

    Wall **placed_walls;
    int num_placed_walls;
} Game;

// POINT
Point *create_point(int x, int y);
void delete_point(Point *point);

// PLAYER
Player *create_player(char name[], Point *base_pos);
void delete_player(Player *player);

// WALL 
Wall *create_wall(Point *endA, Point *endB, Player *player);
void delete_wall(Wall *wall);
void add_wall_to_game(Game *game, Player *player, Point a, Point b);

// GAME
#include "app.h"

Game *create_game(char playerA_name[], Point *playerA_pos, char playerB_name[], Point *playerB_pos);
void delete_game(Game *game);

/**
 * @brief transforms a grid position (3,4) for instance to a pixel position on the screen
 */
void player_pos_to_pixel_pos(Player *player, Point *pixel_pos);
void wall_pos_to_pixel_pos(Wall *wall, Point *a, Point *b);
void draw_game(App *app);
void draw_players(App *app);
void draw_walls(App *app);
void draw_grid(App *app);

#endif