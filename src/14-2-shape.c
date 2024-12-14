#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "intrinsics.h"

#define MAX_N_ROBOTS 1000
#define GRID_SIZE 200

struct Robot {
    i64 px;
    i64 py;
    i64 vx;
    i64 vy;
};

i64 mod(i64 a, i64 b) {
    i64 r = a % b;
    if (r < 0) {
        r = (b < 0) ? r - b : r + b;
    }
    return r;
}

void print_grid(
    i32 grid[GRID_SIZE][GRID_SIZE],
    bool mask[GRID_SIZE][GRID_SIZE],
    i32 width,
    i32 height
) {
    for (i32 y = 0; y < height; y += 1) {
        for (i32 x = 0; x < width; x += 1) {
            if (mask[x][y]) {
                eprintf("#");
            } else if (grid[x][y] == 0) {
                eprintf(".");
            } else {
                eprintf("%d", grid[x][y]);
            }
        }
        eprintf("\n");
    }
}

int main() {
    i32 width = 101;
    i32 height = 103;

    FILE *f = fopen("data/14", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }

    struct Robot robots[MAX_N_ROBOTS] = {0};
    i32 n_robots = 0;

    while (true) {
        i64 px, py, vx, vy;
        if (fscanf(f, "p=%ld,%ld v=%ld,%ld ", &px, &py, &vx, &vy) != 4) {
            break;
        }
        struct Robot *robot = &robots[n_robots];
        n_robots += 1;
        robot->px = px;
        robot->py = py;
        robot->vx = vx;
        robot->vy = vy;
        eprintf("(px %ld) (py %ld) (vx %ld) (vy %ld)\n", px, py, vx, vy);
    }

    i32 t = 0;
    i32 max_n_robots_in_tree = 0;
    while (true) {
        i32 grid[GRID_SIZE][GRID_SIZE] = {0};
        bool mask[GRID_SIZE][GRID_SIZE] = {0};
        i32 n_robots_in_tree = 0;
        for (i32 i = 0; i < n_robots; i += 1) {
            struct Robot *robot = &robots[i];
            bool is_robot_in_tree = true;
            robot->px += robot->vx;
            robot->py += robot->vy;
            robot->px = mod(robot->px, width);
            robot->py = mod(robot->py, height);
            grid[robot->px][robot->py] += 1;
            if (robot->py > (height / 2)) {
                is_robot_in_tree = false;
                goto skip_check;
            }
            for (i32 y = 0; y < (height / 2); y += 1) {
                if (
                    robot->py == y && (
                        robot->px >= (width / 2) + y + 1 ||
                        robot->px <= (width / 2) - y - 1
                    )
                ) {
                    is_robot_in_tree = false;
                    goto skip_check;
                }
                /* for (i32 x = (width / 2) + y + 1; x < width; x += 1) { */
                /*     mask[x][y] = true; */
                /* } */
                /* for (i32 x = (width / 2) - y - 1; x >= 0; x -= 1) { */
                /*     mask[x][y] = true; */
                /* } */
            }
skip_check:
            if (is_robot_in_tree) {
                n_robots_in_tree += 1;
            }
            ;
        }

        /* print_grid(grid, mask, width, height); */
        /* eprintf("\n"); */
        if (n_robots_in_tree > max_n_robots_in_tree) {
            max_n_robots_in_tree = n_robots_in_tree;
            eprintf("%06d - %03d out of %03d\n", t, n_robots_in_tree, n_robots);
        }
        if (n_robots_in_tree > n_robots / 2) {
            print_grid(grid, mask, width, height);
            eprintf("\n");
            printf("%d\n", t);
            break;
        }
        t += 1;
    }

    return 0;
}