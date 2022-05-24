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

#include "app.h"

#define GRID_SIZE 9
#define MAX_NUM_WALLS 20
#define MAX_NAME_SIZE 150

typedef struct Point {
    int x, y;
} Point;

typedef struct Player {
    char name[MAX_NAME_SIZE];
    Point *pos; 
} Player;

typedef struct Wall {
    Point *endA;
    Point *endB;

    Player *placed_by;
} Wall;

typedef struct Game {
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
#endif