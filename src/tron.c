#include <stdlib.h>
#include <assert.h>
#include "tron.h"
#include <curses.h>

unsigned int bike_colors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0xFF00FF };
int bike_color_cnt = 4;

void _tron_state_destroy_bike(tron_state_t* state, tron_bike_t* bike) {
    int x = bike->x;
    int y = bike->y;

    assert(state->grid[x][y].state == BIKE);

    int color = bike->color;
    state->grid[x][y].state = EMPTY;

    for ( int sx = 0; sx < GRID_W; sx++ ) {
        for ( int sy = 0; sy < GRID_H; sy++ ) {
            if ( state->grid[sx][sy].state == LIGHT ) {
                if ( state->grid[sx][sy].color == color ) {
                    state->grid[sx][sy].state = EMPTY;
                }
            }
        }
    }

    for ( int i = 0; i < state->bike_cnt; i++ ) {
        if ( state->bikes[i] == bike ) {
            state->bikes[i] = NULL;
        }
    }

    free(bike);
}

void _tron_state_ai(tron_state_t* state, tron_bike_t* bike) {
    // TODO - Tron AI
}

void tron_bike_turn(tron_bike_t* bike, tron_direction_t dir) {
    if ( (bike->dir + 2) % 4 != dir ) {
        bike->dir = dir;
    }
}

void tron_state_step(tron_state_t* state) {
    for ( int i = 0; i < state->bike_cnt; i++ ) {
        if ( state->bikes[i] == NULL ) {
            continue;
        }

        int x = state->bikes[i]->x;
        int y = state->bikes[i]->y;

        int nx = x;
        int ny = y;

        switch ( state->bikes[i]->dir ) {
            case NORTH:
                nx += 0;
                ny -= 1;
                break;
            case EAST:
                nx += 1;
                ny += 0;
                break;
            case WEST:
                nx -= 1;
                ny += 0;
                break;
            case SOUTH:
                nx += 0;
                ny += 1;
                break;
        }

        if ( nx < 0 || ny < 0 || nx >= GRID_W || ny >= GRID_H || state->grid[nx][ny].state != EMPTY ) {
            _tron_state_destroy_bike(state, state->bikes[i]);
        }
        else {
            state->bikes[i]->x = nx;
            state->bikes[i]->y = ny;
            state->grid[nx][ny].state = BIKE;
            state->grid[nx][ny].bike = state->bikes[i];

            state->grid[x][y].state = LIGHT;
            state->grid[x][y].color = state->bikes[i]->color;
        }
    }
}

void tron_state_spawn_bike(tron_state_t* state, int x, int y, bool ai) {
    tron_bike_t* bike = (tron_bike_t*) malloc(sizeof(tron_bike_t));
    bike->dir = SOUTH;
    bike->color = bike_colors[state->bike_cnt % bike_color_cnt];
    bike->ai = ai;
    bike->x = x;
    bike->y = y;

    state->grid[x][y].state = BIKE;
    state->grid[x][y].bike = bike;

    if ( state->bike_cnt >= state->bike_alloc) {
        state->bike_alloc = state->bike_alloc * 2;
        state->bikes = realloc(state->bikes, state->bike_alloc); // wew no safety lads
    }

    state->bikes[state->bike_cnt++] = bike;
}

tron_state_t* tron_state_init() {
    tron_state_t* state = (tron_state_t*) malloc(sizeof(tron_state_t));

    state->bike_cnt = 0;
    state->bike_alloc = 4;
    state->bikes = malloc(sizeof(tron_bike_t) * state->bike_alloc);

    state->game_state = READY;

    for ( int x = 0; x < GRID_W; x++ ) {
        for ( int y = 0; y < GRID_H; y++ ) {
            tron_cell_t cell = {
                .state = EMPTY,
                .color = 0,
                .x = x,
                .y = y
            };

            state->grid[x][y] = cell;
        }
    }

    return state;
}

void tron_state_clean(tron_state_t* tron) {
    for ( int i = 0; i < tron->bike_cnt; i++ ) {
        free(tron->bikes[i]);
    }

    free(tron->bikes);
    free(tron);
}
