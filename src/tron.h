#ifndef _TRON_H
#define _TRON_H

#include <stdbool.h>

#define POS(x, y) (y * state->width) + x

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} tron_direction_t;

typedef struct {
    tron_direction_t dir;
    int color;
    bool ai;

    int x;
    int y;

    unsigned int score;
} tron_bike_t;

typedef enum {
    EMPTY,
    BIKE,
    LIGHT
} tron_cell_state_t;

typedef struct {
    union {
        char color;
        tron_bike_t* bike;
    };
} tron_cell_t;

typedef enum {
    READY,
    PLAY,
    OVER
} tron_game_state_t;

typedef struct {
    int width;
    int height;

    tron_game_state_t game_state;
    tron_cell_t* grid;

    tron_bike_t* player;
    tron_bike_t** bikes;
    unsigned int bike_cnt;
    unsigned int bike_alloc;
} tron_state_t;

void tron_state_step(tron_state_t* state);
void tron_state_spawn_bike(tron_state_t* state, int x, int y, bool ai);
void tron_bike_turn(tron_bike_t* bike, tron_direction_t dir);
tron_state_t* tron_state_init(int width, int height);
void tron_state_clean(tron_state_t* tron);
tron_cell_state_t tron_state_get_cell(tron_state_t* state, int x, int y);

#endif
