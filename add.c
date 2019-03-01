/**
 * File              : add.c
 * Author            : recurze
 * Date              : 21:51 01.03.2019
 * Last Modified Date: 21:51 01.03.2019
 */

#include "types.h"
#include "user.h"

int main(int argc, char ** argv) {
    if (argc != 3) {
        printf(2, "Usage: add a b\n");
    }
    printf(1, "%d\n", add(atoi(argv[1]), atoi(argv[2])));
    exit();
    return 0;
}
