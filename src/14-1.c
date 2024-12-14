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

void print_grid(i32 grid[GRID_SIZE][GRID_SIZE], i32 width, i32 height) {
    for (i32 y = 0; y < height; y += 1) {
        for (i32 x = 0; x < width; x += 1) {
            if (grid[x][y] == 0) {
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
    i32 dt = 100;

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

    i32 grid[GRID_SIZE][GRID_SIZE] = {0};
    i64 quadrant_robot_counts[4] = {0};

    for (i32 i = 0; i < n_robots; i += 1) {
        struct Robot *robot = &robots[i];
        robot->px += robot->vx * dt;
        robot->py += robot->vy * dt;
        robot->px = mod(robot->px, width);
        robot->py = mod(robot->py, height);
        grid[robot->px][robot->py] += 1;
        i32 quadrant = 0;
        if (robot->px != (width / 2) && robot->py != (height / 2)) {
            if (robot->px > (width / 2)) {
                quadrant += 1;
            }
            if (robot->py > (height / 2)) {
                quadrant += 2;
            }
            eprintf("(quadrant %ld)\n", quadrant);
            quadrant_robot_counts[quadrant] += 1;
        } else {
            eprintf("(quadrant nothing!)\n");
        }
    }

    print_grid(grid, width, height);

    i64 product = quadrant_robot_counts[0] *
        quadrant_robot_counts[1] *
        quadrant_robot_counts[2] *
        quadrant_robot_counts[3];

    printf("%ld\n", product);

    return 0;
}
