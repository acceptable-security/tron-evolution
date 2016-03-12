#include <ctype.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "tron.h"
#include <unistd.h>

unsigned int rgb2pair(int rgb) {
    switch ( rgb ) {
        case 0x000000: return 0;
        case 0xFF0000: return 1;
        case 0x00FF00: return 2;
        case 0x0000FF: return 3;
        case 0xFF00FF: return 4;
        default:       return 5;
    }
}

int game() {
    tron_state_t* tron = tron_state_init();

    tron_state_spawn_bike(tron, 5, 5, false);
    tron_state_spawn_bike(tron, 10, 10, true);

    while ( true ) {
        switch ( getch() ) {
            case 'w':
                if ( tron->player != NULL ) {
                    tron_bike_turn(tron->player, NORTH);
                }

                break;
            case 's':
                if ( tron->player != NULL ) {
                    tron_bike_turn(tron->player, SOUTH);
                }

                break;
            case 'd':
                if ( tron->player != NULL ) {
                    tron_bike_turn(tron->player, EAST);
                }

                break;

            case 'a':
                if ( tron->player != NULL ) {
                    tron_bike_turn(tron->player, WEST);
                }

                break;

            case ' ':
                tron_state_step(tron);
                break;

            case 'q':
                goto lose;
                return 1;

            default:
                break;
        }

        tron_state_step(tron);

        for ( int x = 0; x < GRID_W; x++ ) {
            for ( int y = 0; y < GRID_H; y++ ) {
                move(y, x);

                if ( tron->grid[x][y].state == EMPTY ) {
                    if ( x != 0 || y != 0 || x != GRID_W - 1 || y != GRID_H - 1 ) {
                        addch(' ');
                    }
                }
                else if ( tron->grid[x][y].state == LIGHT ) {
                    color_set(rgb2pair(tron->grid[x][y].color), NULL);
                    addch('#');
                }
                else if ( tron->grid[x][y].state == BIKE ) {
                    color_set(rgb2pair(tron->grid[x][y].bike->color), NULL);

                    switch ( tron->grid[x][y].bike->dir ) {
                        case NORTH: addch('^');
                        case EAST:  addch('>');
                        case WEST:  addch('<');
                        case SOUTH: addch('V');
                    }
                }
            }
        }

        color_set(0, NULL);
        box(stdscr, 0, 0);
        refresh();

        if ( tron->bikes[0] == NULL ) {
            goto lose;
        }

        usleep(60000);
    }

    lose:
    tron_state_clean(tron);
    color_set(1, NULL);
    mvprintw((GRID_H - 1) / 2, ((GRID_W - 1) / 2) - 7, "You lose! Space to retry.");
    refresh();
    sleep(2);

    if ( getch() == ' ' ) {
        return game();
    }

    return 0;
}

int main(int argc, char* argv[]) {
    WINDOW* window = initscr();

    if ( window == NULL ) {
        fprintf(stderr, "Unable to initalize curses window.\n");
        return 1;
    }

    noecho();

    start_color();

    init_pair(1,  COLOR_RED,     COLOR_BLACK);
	init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
	init_pair(3,  COLOR_BLUE,    COLOR_BLACK);
	init_pair(4,  COLOR_YELLOW,  COLOR_BLACK);
    init_pair(5,  COLOR_MAGENTA, COLOR_BLACK);

    resize_term(GRID_H + 1, GRID_W + 1);
    nodelay(window, true);
    box(stdscr, 0, 0);

    game();

    delwin(window);
    endwin();

    return 0;
}
