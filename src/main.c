#include <string.h>

extern int game_main(int argc, char* argv[]);
extern int test_main(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    if ( argc > 1 ) {
        if ( strcmp(argv[1], "test") == 0 ) {
            return test_main(argc, argv);
        }
        else if ( strcmp(argv[1], "game") == 0 ) {
            return game_main(argc, argv);
        }
        else {
            return game_main(argc, argv);
        }
    }
    else {
        return game_main(argc, argv);
    }
}
