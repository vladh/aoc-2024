#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

#define GRID_SIZE 200
#define MAX_N_GROUPS 200 * 200

struct Group {
    char letter;
    u64 perimeter;
    u64 area;
};

struct Perimeter {
    u64 sum;
    bool top;
    bool bottom;
    bool left;
    bool right;
};

bool is_in_bounds(i32 x, i32 y, i32 grid_width, i32 grid_height) {
    return x >= 0 && x < grid_width && y >= 0 && y < grid_height;
}

struct Perimeter plot_calc_perimeter(
    char plots[GRID_SIZE][GRID_SIZE],
    i32 x,
    i32 y,
    i32 grid_width,
    i32 grid_height
) {
    char letter = plots[x][y];
    struct Perimeter perimeter = {0};

    for (i32 dy = -1; dy <= 1; dy += 1) {
        for (i32 dx = -1; dx <= 1; dx += 1) {
            if (abs(dx - dy) != 1) {
                continue;
            }
            if (
                !is_in_bounds(x + dx, y + dy, grid_width, grid_height) ||
                plots[x + dx][y + dy] != letter
            ) {
                perimeter.sum += 1;
                continue;
            }
            if (dx == -1) {
                perimeter.left = true;
            }
            if (dx == 1) {
                perimeter.right = true;
            }
            if (dy == -1) {
                perimeter.top = true;
            }
            if (dy == 1) {
                perimeter.bottom = true;
            }
        }
    }

    return perimeter;
}

void group_fill(
    char plots[GRID_SIZE][GRID_SIZE],
    i32 x,
    i32 y,
    i32 grid_width,
    i32 grid_height,
    struct Group *group,
    bool checked[GRID_SIZE][GRID_SIZE]
) {
    char letter = plots[x][y];
    group->letter = letter;
    group->area += 1;

    eprintf("%c at (%d,%d)\n", letter, x, y);
    struct Perimeter perimeter = plot_calc_perimeter(plots, x, y, grid_width, grid_height);
    for (i32 dy = -1; dy <= 1; dy += 1) {
        for (i32 dx = -1; dx <= 1; dx += 1) {
            if (
                abs(dx - dy) != 1 ||
                !is_in_bounds(x + dx, y + dy, grid_width, grid_height) ||
                !checked[x + dx][y + dy] ||
                plots[x + dx][y + dy] != letter
            ) {
                continue;
            }
            eprintf("\tfound friendly at (%d,%d)\n", x + dx, y + dy);
            eprintf("\tperimeter.sum before: %d\n", perimeter.sum);
            struct Perimeter candidate_perimeter = plot_calc_perimeter(
                plots, x + dx, y + dy, grid_width, grid_height);
            if (dx == -1 || dx == 1) {
                if (!perimeter.top && !candidate_perimeter.top) {
                    perimeter.sum -= 1;
                }
                if (!perimeter.bottom && !candidate_perimeter.bottom) {
                    perimeter.sum -= 1;
                }
            }
            if (dy == -1 || dy == 1) {
                if (!perimeter.left && !candidate_perimeter.left) {
                    perimeter.sum -= 1;
                }
                if (!perimeter.right && !candidate_perimeter.right) {
                    perimeter.sum -= 1;
                }
            }
            eprintf("\tperimeter.sum after: %d\n", perimeter.sum);
        }
    }
    group->perimeter += perimeter.sum;

    checked[x][y] = true;

    for (i32 dy = -1; dy <= 1; dy += 1) {
        for (i32 dx = -1; dx <= 1; dx += 1) {
            if (abs(dx - dy) != 1) {
                continue;
            }
            i32 new_x = x + dx;
            i32 new_y = y + dy;
            if (!is_in_bounds(new_x, new_y, grid_width, grid_height)) {
                continue;
            }
            char target_letter = plots[x + dx][y + dy];
            if (checked[new_x][new_y] || target_letter != letter) {
                continue;
            }
            group_fill(plots, new_x, new_y, grid_width, grid_height, group, checked);
        }
    }
}

int main() {
    i32 grid_width = 0;
    i32 grid_height = 0;
    char plots[GRID_SIZE][GRID_SIZE] = {0};
    bool checked[GRID_SIZE][GRID_SIZE] = {0};
    struct Group groups[MAX_N_GROUPS] = {0};
    i32 n_groups = 0;

    int fd = open("data/12", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;
    i32 x = 0;
    i32 y = 0;

    while (*curr) {
        if (*curr == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            plots[x][y] = *curr;
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (checked[x][y]) {
                continue;
            }
            struct Group *group = &groups[n_groups];
            n_groups += 1;
            group_fill(plots, x, y, grid_width, grid_height, group, checked);
        }
    }

    u64 total = 0;

    for (i32 idx_group = 0; idx_group < n_groups; idx_group += 1) {
        struct Group *group = &groups[idx_group];
        total += group->area * group->perimeter;
        eprintf("%c: (area %lu) (perimeter %lu) (product %lu)\n",
            group->letter, group->area, group->perimeter, group->area * group->perimeter);
    }

    printf("%lu\n", total);

    return 0;
}
