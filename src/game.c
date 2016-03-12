#include <ctype.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "tron.h"

unsigned int rgb2pair(int rgb) {
    switch ( rgb ) {
        case 0xFF0000: return 1;
        case 0x00FF00: return 2;
        case 0x0000FF: return 3;
        case 0xFF00FF: return 4;
        default:       return 5;
    }
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

    resize_term(GRID_W + 1, GRID_H + 1);
    nodelay(window, true);
    box(stdscr, 0, 0);

    tron_state_t* tron = tron_state_init();
    tron_state_spawn_bike(tron, 5, 5, false);
    tron_state_spawn_bike(tron, GRID_W - 1, GRID_H - 1, false);

    int a = 0;

    while ( true ) {
        mvprintw(2, 2, "A: %d", a++);
        switch ( getch() ) {
            case 'w':
                tron->bikes[0]->dir = NORTH;
                break;
            case 's':
                tron->bikes[0]->dir = SOUTH;
                break;
            case 'd':
                tron->bikes[0]->dir = EAST;
                break;
                tron->bikes[0]->dir = WEST;
            case 'a':
                break;

            case ' ':
                tron_state_step(tron);
                break;

            case 'q':
                goto end;
                return 1;

            default:
                break;
        }

        int b = 3;

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
                    mvprintw(b++, 2, "%d %d", x, y);
                    move(y, x);
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
        sleep(1);
    }

    end:
    delwin(window);
    endwin();

    return 0;
}
