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
    i32 width,
    i32 height
) {
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

bool is_in_bounds(i32 x, i32 y, i32 width, i32 height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void find_clusters(
    i32 grid[GRID_SIZE][GRID_SIZE],
    i32 clusters[GRID_SIZE][GRID_SIZE],
    i32 *n_clusters,
    i32 cluster_sizes[GRID_SIZE],
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    i32 cluster
) {
    if (grid[x][y] == 0) {
        return;
    }
    if (clusters[x][y] != 0) {
        return;
    }
    if (cluster == 0) {
        cluster = *n_clusters + 1;
        *n_clusters += 1;
    }
    clusters[x][y] = cluster;
    cluster_sizes[cluster] += 1;
    for (i32 dy = -1; dy <= 1; dy += 1) {
        for (i32 dx = -1; dx <= 1; dx += 1) {
            if (abs(dx - dy) != 1) {
                continue;
            }
            if (!is_in_bounds(x + dx, y + dy, width, height)) {
                continue;
            }
            find_clusters(
                grid,
                clusters,
                n_clusters,
                cluster_sizes,
                x + dx,
                y + dy,
                width,
                height,
                cluster);
        }
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
        /* eprintf("(px %ld) (py %ld) (vx %ld) (vy %ld)\n", px, py, vx, vy); */
    }

    i32 t = 0;
    i32 largest_cluster = 0;
    i32 second_largest_cluster = 0;
    while (true) {
        i32 grid[GRID_SIZE][GRID_SIZE] = {0};
        i32 clusters[GRID_SIZE][GRID_SIZE] = {0};
        i32 cluster_sizes[GRID_SIZE * GRID_SIZE] = {0};
        i32 n_clusters = 0;
        for (i32 i = 0; i < n_robots; i += 1) {
            struct Robot *robot = &robots[i];
            robot->px += robot->vx;
            robot->py += robot->vy;
            robot->px = mod(robot->px, width);
            robot->py = mod(robot->py, height);
            grid[robot->px][robot->py] += 1;
        }
        t += 1;

        for (i32 y = 0; y < height; y += 1) {
            for (i32 x = 0; x < width; x += 1) {
                find_clusters(
                    grid,
                    clusters,
                    &n_clusters,
                    cluster_sizes,
                    x, y,
                    width, height,
                    0);
            }
        }

        for (i32 i = 0; i < n_clusters; i += 1) {
            if (cluster_sizes[i] > second_largest_cluster) {
                if (cluster_sizes[i] > largest_cluster) {
                    second_largest_cluster = largest_cluster;
                    largest_cluster = cluster_sizes[i];
                } else {
                    second_largest_cluster = cluster_sizes[i];
                }
                eprintf("%06d - %03d out of %03d\n", t, largest_cluster, n_robots);
            }
        }

        if (largest_cluster + second_largest_cluster >= n_robots / 2) {
            print_grid(grid, width, height);
            printf("%d\n", t);
            break;
        }
    }

    return 0;
}
