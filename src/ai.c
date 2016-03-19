#include "tron.h"
#include <stdlib.h>
#include <curses.h>
#include <string.h>

static inline int _filled(tron_state_t* state, int x, int y) {
    return (x < 1 || y < 1 || x >= state->width - 1 || y >= state->height - 1 || state->grid[POS(x, y)].color != 0);
}

void _floodfill(tron_state_t* state, int* map, int x, int y, int ox, int oy) {
    if ( _filled(state, x, y) && !(x == ox && y == oy) ) {
        return;
    }

    if ( map[POS(x, y)] > -1 ) {
        return;
    }

    map[POS(x, y)] = ((ox - x)*(ox - x)) + ((oy - y)*(oy - y));

    _floodfill(state, map, x    , y - 1, ox, oy);
    _floodfill(state, map, x    , y + 1, ox, oy);
    _floodfill(state, map, x + 1, y    , ox, oy);
    _floodfill(state, map, x - 1, y    , ox, oy);
}

int* floodfill(tron_state_t* state, int x, int y) {
    int* map = (int*) malloc(sizeof(int) * (state->width * state->height));
    memset(map, -1, sizeof(int) * (state->width * state->height));

    _floodfill(state, map, x, y, x, y);
    return map;
}

unsigned int evaluate_pos(tron_state_t* state, int my_x, int my_y,
                                               int their_x, int their_y,
                                               int* p1score, int* p2score,
                                               int** p1dist, int** p2dist) {
    *p1dist = floodfill(state, my_x, my_y);
    *p2dist = floodfill(state, their_x, their_y);

    unsigned int score = 0;
    *p1score = 0;
    *p2score = 0;

    for ( int x = 1; x < state->width - 1; x++ ) {
        for ( int y = 1; y < state->height - 1; y++ ) {
            if ( _filled(state, x, y) ) {
                continue;
            }

            if ( (*p2dist)[POS(x, y)] == -1 ) {
                if ( (*p1dist)[POS(x, y)] > -1 ) {
                    (*p2score)++;
                    score++;
                }

                continue;
            }

            if ( (*p1dist)[POS(x, y)] == -1 ) {
                if ( (*p2dist)[POS(x, y)] > -1 ) {
                    (*p1score)++;
                    score--;
                }

                continue;
            }

            int d = (*p1dist)[POS(x, y)] - (*p2dist)[POS(x, y)];

            if ( d < 0 ) {
                (*p2score)++;
                score++;
            }
            else {
                (*p1score)++;
                score--;
            }
        }
    }

    return score;
}

unsigned int negamax(tron_state_t* state, int my_x, int my_y,
                                          int their_x, int their_y,
                                          int* p1score, int* p2score,
                                          int** p1dist, int** p2dist,
                                          int depth, int a, int b, int* p2move) {

    if ( depth == 0) {
        free(*p1dist);
        free(*p2dist);

        unsigned int res = evaluate_pos(state, my_x, my_y, their_x, their_y,
                                               p1score, p2score, p1dist, p2dist);

        return res;
    }

    int bestmove = 0;
    int bestaltmove = 0;

    // printf("Depth: %d\n", depth);
    for ( int move = 0; move < 4; move++ ) {
        printf("Depth %d\n", depth);
        // printf("Looking at move %d\n", move);

        int x = my_x + dx[move];
        int y = my_y + dy[move];

        if ( _filled(state, x, y) ) {
            printf("Not considering move %d\n", move);
            continue;
        }

        state->grid[POS(x, y)].color = 1;

        int* _p1 = *p1dist;
        int* _p2 = *p2dist;
        int _p1s = *p1score;
        int _p2s = *p2score;

        // printf("A: %d\tB: %d\n", a, b);

        int score = -negamax(state, their_x, their_y, my_x, my_y, p1score, p2score, p1dist, p2dist, depth - 1, -b, -a, p2move);

        state->grid[POS(x, y)].bike = 0;

        if ( score > a ) {
            printf("Found best score %d for move %d\n", score, move);
            a = score;
            bestmove = move;
            bestaltmove = *p2move;

            if ( a >= b ) {
                break;
            }
        }
        else {
            printf("%d not good enough\n", score);
            *p1dist = _p1;
            *p1score = _p1s;
            *p2dist = _p2;
            *p2score = _p2s;
        }

        // printf("Current A: %d\n\n", a);
    }

    *p2move = bestmove;
    printf("Move value: %d\n", a);
    return a;
}

void tron_state_ai(tron_state_t* state, tron_bike_t* bike) {
    if ( state->player == NULL ) {
        return;
    }

    int their_x = bike->x;
    int their_y = bike->y;
    int my_x = state->player->x;
    int my_y = state->player->y;
    int a = -1000000;
    int b = 1000000;
    int p2move = 0;

    int p1score = 0;
    int p2score = 0;
    int* p1dist = NULL;
    int* p2dist = NULL;

    int score = negamax(state, their_x, their_y, my_x, my_y, &p1score, &p2score, &p1dist, &p2dist, 6, a, b, &p2move);

    free(p1dist);
    free(p2dist);

    // printf("Score: %d\nDirection: %d\n", score, p2move);
    bike->dir = p2move;
}
