#include <stdlib.h>
#include <assert.h>
#include "tron.h"
#include <curses.h>

/**
 * I know I'm going to forget in a bit so
 * because we are using a union of the light color and
 * the pointer of the bike, we can assume that if the
 * color is set, but the upper bytes of the cell aren't
 * set, we can assume that a light not a bike is there because
 * half the pointer is missing :)
 */
inline tron_cell_state_t tron_state_get_cell(tron_state_t* state, int x, int y) {
    tron_cell_t cell = state->grid[POS(x, y)];

    if ( cell.color == 0 ) {
        return EMPTY;
    }

    if ( (size_t)(cell.bike) >> 8 == 0 ) {
        return LIGHT;
    }

    return BIKE;
}

void _tron_state_destroy_bike(tron_state_t* state, tron_bike_t* bike) {
    int x = bike->x;
    int y = bike->y;

    int color = bike->color;
    state->grid[POS(x, y)].bike = 0;

    for ( int sx = 0; sx < state->width; sx++ ) {
        for ( int sy = 0; sy < state->height; sy++ ) {
            if ( tron_state_get_cell(state, sx, sy) == LIGHT ) {
                if ( state->grid[POS(sx, sy)].color == color ) {
                    state->grid[POS(sx, sy)].bike = 0;
                }
            }
        }
    }

    for ( int i = 0; i < state->bike_cnt; i++ ) {
        if ( state->bikes[i] == bike ) {
            state->bikes[i] = NULL;
        }
    }

    if ( state->player == bike ) {
        state->player = NULL;
    }

    free(bike);
}

static inline bool _tron_state_safe(tron_state_t* state, int x, int y) {
    return !(x < 1 || y < 1 || x >= state->width - 1 || y >= state->height - 1 || state->grid[POS(x, y)].color != 0);
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

        if ( state->bikes[i]->ai ) {
            tron_state_ai(state, state->bikes[i]);
        }

        int x = state->bikes[i]->x;
        int y = state->bikes[i]->y;

        int nx = x + dx[state->bikes[i]->dir];
        int ny = y + dy[state->bikes[i]->dir];

        if ( !_tron_state_safe(state, nx, ny) ) {
            _tron_state_destroy_bike(state, state->bikes[i]);
        }
        else {
            state->bikes[i]->score++;
            state->bikes[i]->x = nx;
            state->bikes[i]->y = ny;
            state->grid[POS(nx, ny)].bike = state->bikes[i];

            state->grid[POS(x, y)].bike = 0;
            state->grid[POS(x, y)].color = state->bikes[i]->color;
        }
    }
}

void tron_state_spawn_bike(tron_state_t* state, int x, int y, bool ai) {
    if ( !ai ) {
        assert(state->player == NULL);
    }

    tron_bike_t* bike = (tron_bike_t*) malloc(sizeof(tron_bike_t));
    bike->dir = NORTH;
    bike->color = (state->bike_cnt % 4) + 1;
    bike->ai = ai;
    bike->x = x;
    bike->y = y;

    state->grid[POS(x, y)].bike = bike;

    if ( state->bike_cnt >= state->bike_alloc) {
        state->bike_alloc = state->bike_alloc * 2;
        state->bikes = realloc(state->bikes, state->bike_alloc); // wew no safety lads
    }

    state->bikes[state->bike_cnt++] = bike;

    if ( !ai ) {
        state->player = bike;
    }

    bike->score = 0;
}

tron_state_t* tron_state_init(int width, int height) {
    tron_state_t* state = (tron_state_t*) malloc(sizeof(tron_state_t));

    state->width = width;
    state->height = height;
    state->grid = malloc(sizeof(tron_cell_t) * (width * height));

    state->player = NULL;
    state->bike_cnt = 0;
    state->bike_alloc = 4;
    state->bikes = malloc(sizeof(tron_bike_t) * state->bike_alloc);

    state->game_state = READY;

    for ( int x = 0; x < state->width; x++ ) {
        for ( int y = 0; y < state->height; y++ ) {
            tron_cell_t cell = {
                .bike = 0
            };

            state->grid[POS(x, y)] = cell;
        }
    }

    return state;
}

void tron_state_clean(tron_state_t* tron) {
    for ( int i = 0; i < tron->bike_cnt; i++ ) {
        if ( tron->bikes[i] != NULL ) {
            free(tron->bikes[i]);
        }
    }

    free(tron->grid);
    free(tron->bikes);
    free(tron);
}
