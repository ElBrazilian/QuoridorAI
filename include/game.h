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

#define GRID_SIZE 9
#define MAX_NUM_WALLS 20
#define MAX_NAME_SIZE 150

// DRAW OPTIONS
#define DRAW_DEBUG_ON_START false

#define GRID_OFFSET 1/20 // percentage of the size
#define GRID_EFFECTIVE_SIZE (SIZE - 2 * SIZE * GRID_OFFSET) //(SIZE * (1 - 2 * GRID_OFFSET))
#define GRID_OFFSET_SIZE SIZE * GRID_OFFSET
#define GAME_COLOR 50,50,50,255
#define GRID_COLOR 255,255,255,255
#define GRID_WIDTH 10
#define OUTER_GRID_COLOR 255,0,0,255
#define OUTER_GRID_WIDTH 20

#define GRID_CELL_SIZE GRID_EFFECTIVE_SIZE/GRID_SIZE

// Player A
#define PLAYERA_NAME "Player A"
#define PLAYERA_POSX 0
#define PLAYERA_POSY 4

// Player B
#define PLAYERB_NAME "Player B"
#define PLAYERB_POSX 8
#define PLAYERB_POSY 4

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

// GAME
Game *create_game(char playerA_name[], Point *playerA_pos, char playerB_name[], Point *playerB_pos);
void delete_game(Game *game);


#include "app.h"
void draw_grid(App *app);
#endif