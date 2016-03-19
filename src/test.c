#include "tron.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern inline int _filled(tron_state_t* state, int x, int y);
extern void _floodfill(tron_state_t* state, int* map, int x, int y, int ox, int oy);
extern int* floodfill(tron_state_t* state, int x, int y);

void print_map(int* map, int width, int height) {
    for ( int x = 0; x < width; x++ ) {
        for ( int y = 0; y < height; y++ ) {
            printf("%3d ", map[(y * width) + x]);
        }

        printf("\n");
    }
}

int test_main(int argc, char* argv[]) {
    tron_state_t* state = tron_state_init(10, 10);
    tron_state_spawn_bike(state, 7, 7, false);
    tron_state_spawn_bike(state, 4, 4, true);

    for ( int i = 0; i < 10; i++ ) {
        tron_state_step(state);

        tron_bike_t* bike = state->bikes[1];

        if ( bike == NULL ) {
            printf("Bike dead.\n");
            break;
        }

        assert(bike->ai);

        printf("%d %d\n", bike->x, bike->y);
        int* tmp = floodfill(state, bike->x, bike->y);
        print_map(tmp, 10, 10);
        free(tmp);
        printf("\n");
    }

    return 0;
}
