#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

#define GRID_SIZE 150

enum Tile { EMPTY, WALL, BOX_LEFT, BOX_RIGHT, ROBOT };

bool is_in_bounds(i32 x, i32 y, i32 grid_width, i32 grid_height) {
    return x >= 0 && x < grid_width && y >= 0 && y < grid_height;
}

void print_grid(enum Tile grid[GRID_SIZE][GRID_SIZE], i32 grid_width, i32 grid_height) {
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (grid[x][y] == WALL) {
                eprintf("#");
            } else if (grid[x][y] == BOX_LEFT) {
                eprintf("[");
            } else if (grid[x][y] == BOX_RIGHT) {
                eprintf("]");
            } else if (grid[x][y] == ROBOT) {
                eprintf("@");
            } else {
                eprintf(".");
            }
        }
        eprintf("\n");
    }
    eprintf("\n");
}

bool move(
    enum Tile grid[GRID_SIZE][GRID_SIZE],
    i32 grid_width,
    i32 grid_height,
    i32 *x,
    i32 *y,
    i32 dx,
    i32 dy,
    bool dry_run,
    bool came_from_friend
) {
    eprintf("(x %d) (y %d) (came_from_friend %d) (dry_run %d)\n", *x, *y, came_from_friend, dry_run);
    if (grid[*x][*y] == BOX_LEFT) {
        assert(grid[*x + 1][*y] == BOX_RIGHT);
    }
    if (grid[*x][*y] == BOX_RIGHT) {
        assert(grid[*x - 1][*y] == BOX_LEFT);
    }
    if (!is_in_bounds(*x, *y, grid_width, grid_height)) {
        return false;
    }
    if (grid[*x][*y] == EMPTY) {
        return true;
    }
    if (
        grid[*x][*y] == WALL ||
        !is_in_bounds(*x + dx, *y + dy, grid_width, grid_height) ||
        grid[*x + dx][*y + dy] == WALL ||
        (
            !came_from_friend && grid[*x][*y] == BOX_LEFT && (
                !is_in_bounds(*x + 1 + dx, *y + dy, grid_width, grid_height) ||
                grid[*x + 1 + dx][*y + dy] == WALL
            )
        ) ||
        (
            !came_from_friend && grid[*x][*y] == BOX_RIGHT && (
                !is_in_bounds(*x - 1 + dx, *y + dy, grid_width, grid_height) ||
                grid[*x - 1 + dx][*y + dy] == WALL
            )
        )
    ) {
        eprintf("invalid move\n");
        return false;
    }

    bool bumpee_is_friend = false;
    if (
        (grid[*x][*y] == BOX_LEFT && dx == 1) ||
        (grid[*x][*y] == BOX_RIGHT && dx == -1)
    ) {
        bumpee_is_friend = true;
    }

    i32 bumpee_x = *x + dx;
    i32 bumpee_y = *y + dy;
    bool did_move_bumpee = move(grid, grid_width, grid_height,
        &bumpee_x, &bumpee_y,
        dx, dy,
        dry_run,
        bumpee_is_friend);

    bool did_move_friend;
    i32 friend_x;
    i32 friend_y = *y;
    bool have_checkable_friend = (
        !bumpee_is_friend && !came_from_friend && (grid[*x][*y] == BOX_LEFT || grid[*x][*y] == BOX_RIGHT)
    );
    if (have_checkable_friend) {
        if (grid[*x][*y] == BOX_LEFT) {
            friend_x = *x + 1;
        }
        if (grid[*x][*y] == BOX_RIGHT) {
            friend_x = *x - 1;
        }
        did_move_friend = move(grid, grid_width, grid_height,
            &friend_x, &friend_y,
            dx, dy,
            dry_run,
            true);
    }

    if (dry_run) {
        eprintf("success - (have_checkable_friend %d) (did_move_friend %d) (did_move_bumpee %d)\n",
            have_checkable_friend, did_move_friend, did_move_bumpee);
        return (!have_checkable_friend || did_move_friend) && did_move_bumpee;
    } else {
        if (have_checkable_friend && !did_move_friend) {
            eprintf("could not move friend\n");
            return false;
        }
        if (grid[*x + dx][*y + dy] != EMPTY) {
            eprintf("space ahead not empty\n");
            return false;
        }
        grid[*x + dx][*y + dy] = grid[*x][*y];
        grid[*x][*y] = EMPTY;
        *x = *x + dx;
        *y = *y + dy;
        eprintf("moved\n");
        return true;
    }

    assert(false);
}

int main() {
    i32 grid_width = 0;
    i32 grid_height = 0;
    enum Tile grid[GRID_SIZE][GRID_SIZE] = {0};

    int f = open("data/15", O_RDONLY);
    if (f == -1) {
        eprintf("Could not open file\n");
        exit(1);
    }
    int len = lseek(f, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, f, 0);
    char *curr = data;
    i32 x = 0;
    i32 y = 0;
    i32 x_robot;
    i32 y_robot;

    bool saw_newline = false;
    while (*curr) {
        if (*curr == '\n') {
            if (saw_newline) {
                break;
            }
            y += 1;
            grid_width = x;
            x = 0;
            saw_newline = true;
        } else {
            saw_newline = false;
            if (*curr == '#') {
                grid[x][y] = WALL;
                grid[x + 1][y] = WALL;
            } else if (*curr == 'O') {
                grid[x][y] = BOX_LEFT;
                grid[x + 1][y] = BOX_RIGHT;
            } else if (*curr == '@') {
                grid[x][y] = ROBOT;
                grid[x + 1][y] = EMPTY;
                x_robot = x;
                y_robot = y;
            }
            x += 2;
        }
        curr += 1;
    }

    grid_height = y;

    print_grid(grid, grid_width, grid_height);

    i32 t = 0;
    while (*curr) {
        i32 dx = 0;
        i32 dy = 0;
        if (*curr == '<') {
            dx = -1;
        } else if (*curr == '>') {
            dx = 1;
        } else if (*curr == '^') {
            dy = -1;
        } else if (*curr == 'v') {
            dy = 1;
        }
        if (dx != 0 || dy != 0) {
            eprintf("t = %d, moving %c:\n", t, *curr);
            bool can_move = move(grid,
                grid_width, grid_height,
                &x_robot, &y_robot,
                dx, dy,
                true,
                false);
            if (can_move) {
                move(grid,
                    grid_width, grid_height,
                    &x_robot, &y_robot,
                    dx, dy,
                    false,
                    false);
            }
            /* print_grid(grid, grid_width, grid_height); */
            t += 1;
        }
        curr += 1;
    }

    i32 sum = 0;
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (grid[x][y] == BOX_LEFT) {
                sum += 100 * y + x;
            }
        }
    }

    printf("%d\n", sum);

    return 0;
}
