#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

i32 const MAX_N_ROWS = 200;
i32 const MAX_N_COLS = 200;

bool grid_eq(char target, char grid[MAX_N_ROWS][MAX_N_COLS], i32 row, i32 col, i32 n_rows, i32 n_cols) {
    return row >= 0 && row < n_rows && col >= 0 && col < n_cols && grid[row][col] == target;
}

i8 have_xmas(char grid[MAX_N_ROWS][MAX_N_COLS], i32 row, i32 col, i32 n_rows, i32 n_cols) {
    i32 count = 0;
    if (
        grid_eq('A', grid, row + 0, col + 0, n_rows, n_cols) &&
        grid_eq('M', grid, row - 1, col - 1, n_rows, n_cols) &&
        grid_eq('S', grid, row + 1, col + 1, n_rows, n_cols) &&
        grid_eq('M', grid, row + 1, col - 1, n_rows, n_cols) &&
        grid_eq('S', grid, row - 1, col + 1, n_rows, n_cols)
    ) {
        printf("xmas;");
        count += 1;
    }
    if (
        grid_eq('A', grid, row + 0, col + 0, n_rows, n_cols) &&
        grid_eq('S', grid, row - 1, col - 1, n_rows, n_cols) &&
        grid_eq('M', grid, row + 1, col + 1, n_rows, n_cols) &&
        grid_eq('M', grid, row + 1, col - 1, n_rows, n_cols) &&
        grid_eq('S', grid, row - 1, col + 1, n_rows, n_cols)
    ) {
        printf("xmas;");
        count += 1;
    }
    if (
        grid_eq('A', grid, row + 0, col + 0, n_rows, n_cols) &&
        grid_eq('M', grid, row - 1, col - 1, n_rows, n_cols) &&
        grid_eq('S', grid, row + 1, col + 1, n_rows, n_cols) &&
        grid_eq('S', grid, row + 1, col - 1, n_rows, n_cols) &&
        grid_eq('M', grid, row - 1, col + 1, n_rows, n_cols)
    ) {
        printf("xmas;");
        count += 1;
    }
    if (
        grid_eq('A', grid, row + 0, col + 0, n_rows, n_cols) &&
        grid_eq('S', grid, row - 1, col - 1, n_rows, n_cols) &&
        grid_eq('M', grid, row + 1, col + 1, n_rows, n_cols) &&
        grid_eq('S', grid, row + 1, col - 1, n_rows, n_cols) &&
        grid_eq('M', grid, row - 1, col + 1, n_rows, n_cols)
    ) {
        printf("xmas;");
        count += 1;
    }
    return count;
}

int main() {
    int fd = open("data/04", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;
    char grid[MAX_N_ROWS][MAX_N_COLS];
    i32 col = 0;
    i32 row = 0;
    i32 n_cols = 0;

    while (*curr) {
        if (*curr == '\n') {
            row += 1;
            n_cols = col;
            col = 0;
            curr += 1;
        } else {
            grid[row][col] = *curr;
            col += 1;
            curr += 1;
        }
    }

    i32 n_rows = row;

    i32 count = 0;

    for (i32 row = 0; row < n_rows; row += 1) {
        for (i32 col = 0; col < n_cols; col += 1) {
            i32 new_count = have_xmas(grid, row, col, n_rows, n_cols);
            if (new_count > 0) {
                printf("%d at (%d, %d)\n", new_count, row, col);
                count += new_count;
            }
        }
    }

    printf("%d\n", count);

    return 0;
}
