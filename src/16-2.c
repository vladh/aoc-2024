#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

#define GRID_SIZE 160
#define ROTATION_COST 1000
#define MAX_N_PREVS 4

struct Pos {
    i32 x;
    i32 y;
};

struct PosOri {
    i32 x;
    i32 y;
    i32 o;
};

bool is_in_bounds(i32 x, i32 y, i32 grid_width, i32 grid_height) {
    return x >= 0 && x < grid_width && y >= 0 && y < grid_height;
}

i32 traverse(
    i32 x,
    i32 y,
    i32 o,
    i32 grid_width,
    i32 grid_height,
    struct PosOri start,
    bool traversal_seen[GRID_SIZE][GRID_SIZE],
    bool walls[GRID_SIZE][GRID_SIZE],
    i32 dist[GRID_SIZE][GRID_SIZE][4],
    struct PosOri prev[GRID_SIZE][GRID_SIZE][4][MAX_N_PREVS],
    i32 n_prevs[GRID_SIZE][GRID_SIZE][4]
) {
    i32 traversed_sum = 1;
    if (traversal_seen[x][y]) {
        traversed_sum = 0;
    }
    traversal_seen[x][y] = true;

    eprintf("%d %d %d\n", x, y, o);

    if (x == start.x && y == start.y && o == start.o) {
        return traversed_sum;
    }

    for (i32 idx_prev = 0; idx_prev < n_prevs[x][y][o]; idx_prev += 1) {
        struct PosOri *p = &prev[x][y][o][idx_prev];
        traversed_sum += traverse(p->x, p->y, p->o,
            grid_width, grid_height,
            start,
            traversal_seen,
            walls, dist, prev, n_prevs);
    }

    return traversed_sum;
}

bool is_orientation_same(i32 o, i32 dx, i32 dy) {
    return (o == 0 && dx == -1) ||
        (o == 1 && dx == 1) ||
        (o == 2 && dy == -1) ||
        (o == 3 && dy == 1);
}

bool is_orientation_opposite(i32 o, i32 dx, i32 dy) {
    return (o == 1 && dx == -1) ||
        (o == 0 && dx == 1) ||
        (o == 3 && dy == -1) ||
        (o == 2 && dy == 1);
}

i32 get_rotation_cost(i32 o, i32 dx, i32 dy) {
    if (is_orientation_same(o, dx, dy)) {
        return 0;
    } else if (is_orientation_opposite(o, dx, dy)) {
        return ROTATION_COST * 2;
    } else {
        return ROTATION_COST;
    }
}

i32 get_new_orientation(i32 dx, i32 dy) {
    if (dx == -1) {
        return 0;
    } else if (dx == 1) {
        return 1;
    } else if (dy == -1) {
        return 2;
    } else if (dy == 1) {
        return 3;
    }
    assert(false);
}

void print_grid(
    bool walls[GRID_SIZE][GRID_SIZE],
    i32 grid_width, i32 grid_height,
    struct PosOri start,
    struct Pos end,
    i32 dist[GRID_SIZE][GRID_SIZE][4]
) {
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (walls[x][y]) {
                eprintf("#");
            } else if (start.x == x && start.y == y) {
                eprintf("S");
            } else if (end.x == x && end.y == y) {
                eprintf("E");
            } else {
                eprintf(".");
            }
            eprintf("(");
            for (i32 o = 0; o < 4; o += 1) {
                i32 print_dist = dist[x][y][o];
                if (print_dist > 9999) {
                    eprintf("    ");
                } else if (print_dist < 0) {
                    eprintf("????");
                } else {
                    eprintf("%04d", print_dist);
                }
                if (o < 3) {
                    eprintf(",");
                }
            }
            eprintf(") ");
        }
        eprintf("\n");
    }
    eprintf("\n");
}

int main() {
    i32 grid_width = 0;
    i32 grid_height = 0;
    bool walls[GRID_SIZE][GRID_SIZE] = {0};
    bool visited[GRID_SIZE][GRID_SIZE][4] = {0};
    i32 dist[GRID_SIZE][GRID_SIZE][4] = {0};
    struct PosOri prev[GRID_SIZE][GRID_SIZE][4][MAX_N_PREVS] = {0};
    i32 n_prevs[GRID_SIZE][GRID_SIZE][4] = {0};

    int f = open("data/16", O_RDONLY);
    if (f == -1) {
        eprintf("Could not open file\n");
        exit(1);
    }
    int len = lseek(f, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, f, 0);
    char *c = data;
    struct PosOri start;
    struct Pos end;
    i32 x = 0;
    i32 y = 0;

    while (*c) {
        if (*c == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            if (*c == '#') {
                walls[x][y] = true;
            } else if (*c == 'S') {
                start = (struct PosOri) { .x = x, .y = y, .o = 1 };
            } else if (*c == 'E') {
                end = (struct Pos) { .x = x, .y = y };
            }
            x += 1;
        }
        c += 1;
    }

    grid_height = y;

    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            for (i32 o = 0; o < 4; o += 1) {
                if (x == start.x && y == start.y && o == start.o) {
                    dist[x][y][o] = 0;
                } else {
                    dist[x][y][o] = INT_MAX;
                }
            }
        }
    }

    while (true) {
        /* print_grid(walls, */
        /*     grid_width, grid_height, */
        /*     start, end, */
        /*     dist); */

        struct PosOri curr = (struct PosOri) { .x = -1, .y = -1, .o = -1 };
        i32 min_dist = INT_MAX;
        for (i32 y = 0; y < grid_height; y += 1) {
            for (i32 x = 0; x < grid_width; x += 1) {
                for (i32 o = 0; o < 4; o += 1) {
                    if (
                        !visited[x][y][o] &&
                        dist[x][y][o] < min_dist
                    ) {
                        curr = (struct PosOri) { .x = x, .y = y, .o = o };
                        min_dist = dist[x][y][o];
                    }
                }
            }
        }

        eprintf("(min_dist %d) (curr %d %d %d)\n", min_dist, curr.x, curr.y, curr.o);
        if (
            (curr.x == -1 || curr.y == -1 || curr.o == -1) ||
            min_dist == INT_MAX
        ) {
            eprintf("Ending\n");
            break;
        }

        visited[curr.x][curr.y][curr.o] = true;

        for (i32 dy = -1; dy <= 1; dy += 1) {
            for (i32 dx = -1; dx <= 1; dx += 1) {
                if (
                    abs(dx - dy) != 1 ||
                    !is_in_bounds(curr.x + dx, curr.y + dy, grid_width, grid_height) ||
                    walls[curr.x + dx][curr.y + dy]
                ) {
                    continue;
                }
                i32 rotation_cost = get_rotation_cost(curr.o, dx, dy);
                i32 o_new = get_new_orientation(dx, dy);
                i32 new_dist = dist[curr.x][curr.y][curr.o] + 1 + rotation_cost;
                if (new_dist < dist[curr.x + dx][curr.y + dy][o_new]) {
                    dist[curr.x + dx][curr.y + dy][o_new] = new_dist;
                    i32 *node_n_prevs = &n_prevs[curr.x + dx][curr.y + dy][o_new];
                    prev[curr.x + dx][curr.y + dy][o_new][0] = (struct PosOri) { .x = curr.x, .y = curr.y, .o = curr.o };
                    *node_n_prevs = 1;
                } else if (new_dist == dist[curr.x + dx][curr.y + dy][o_new]) {
                    i32 *node_n_prevs = &n_prevs[curr.x + dx][curr.y + dy][o_new];
                    prev[curr.x + dx][curr.y + dy][o_new][*node_n_prevs] = (struct PosOri) { .x = curr.x, .y = curr.y, .o = curr.o };
                    *node_n_prevs += 1;
                }
            }
        }
    }

    i32 min_dist_to_end = INT_MAX;
    for (i32 o = 0; o < 4; o += 1) {
        if (dist[end.x][end.y][o] < min_dist_to_end) {
            min_dist_to_end = dist[end.x][end.y][o];
        }
    }

    i32 sum = 0;

    bool traversal_seen[GRID_SIZE][GRID_SIZE] = {0};
    for (i32 o = 0; o < 4; o += 1) {
        if (dist[end.x][end.y][o] == min_dist_to_end) {
            sum += traverse(end.x, end.y, o,
                grid_width, grid_height,
                start,
                traversal_seen,
                walls, dist, prev, n_prevs);
        }
    }

    printf("%d\n", sum);

    return 0;
}
