#include <ctype.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "tron.h"
#include <unistd.h>

int game(int width, int height) {
    tron_state_t* state = tron_state_init(width, height);

    tron_state_spawn_bike(state, 5, 10, false);
    tron_state_spawn_bike(state, 10, 10, true);

    bool paused = false;

    while ( true ) {
        switch ( getch() ) {
            case 'w':
                if ( state->player != NULL && !paused ) {
                    tron_bike_turn(state->player, NORTH);
                }

                break;
            case 's':
                if ( state->player != NULL && !paused ) {
                    tron_bike_turn(state->player, SOUTH);
                }

                break;
            case 'd':
                if ( state->player != NULL && !paused ) {
                    tron_bike_turn(state->player, EAST);
                }

                break;

            case 'a':
                if ( state->player != NULL && !paused ) {
                    tron_bike_turn(state->player, WEST);
                }

                break;

            case ' ':
                tron_state_step(state);
                break;

            case 'q':
                goto lose;
                return 1;

            case 'p':
                paused = !paused;
                box(stdscr, 0, 0);
                mvprintw(0, 5, "Paused");
                refresh();
                break;

            default:
                break;
        }

        if ( !paused ) {
            tron_state_step(state);

            for ( int x = 0; x < state->width; x++ ) {
                for ( int y = 0; y < state->height; y++ ) {
                    move(y, x);

                    if ( state->grid[POS(x, y)].color == 0 ) {
                        if ( x != 0 || y != 0 || x != state->width - 1 || y != state->height - 1 ) {
                            addch(' ');
                        }
                    }
                    else if ( tron_state_get_cell(state, x, y) == LIGHT ) {
                        color_set(state->grid[POS(x, y)].color, NULL);
                        addch('#');
                    }
                    else {
                        color_set(state->grid[POS(x, y)].bike->color, NULL);

                        switch ( state->grid[POS(x, y)].bike->dir ) {
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


            if ( state->bikes[0] == NULL ) {
                goto lose;
            }
            else {
                mvprintw(0, 5, "Score: %d", state->player->score);
            }

            refresh();
        }

        usleep(60000);
    }

    lose:
    color_set(1, NULL);
    mvprintw((state->height - 1) / 2, ((state->width - 1) / 2) - 7, "You lose! Space to retry.");
    tron_state_clean(state);
    refresh();
    sleep(2);

    int c = 0;

    do {
        c = getch();
        if ( c == ' ' ) return game(width, height);
    } while ( c != ERR );

    return 0;
}

int main(int argc, char* argv[]) {
    // return test_main(argc, argv);

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

    nodelay(window, true);
    box(stdscr, 0, 0);

    int w, h;

    getmaxyx(window, h, w);

    game(w, h);

    delwin(window);
    endwin();

    return 0;
}
