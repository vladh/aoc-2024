#include <stdio.h>
#include <stdlib.h>
#include "common.h"

i64 sign(i64 n) {
    return (n > 0) - (n < 0);
}

int main() {
    FILE *f = fopen("data/02", "r");
    if (!f) {
        fprintf(stderr, "Could not open file\n");
        exit(1);
    }

    i32 n_safe_reports = 0;
    i32 n = -1;
    i32 n_prev = -1;
    i64 d = 0;
    i64 d_prev = 0;
    char c = '\0';
    bool is_safe = true;
    i32 idx = 0;

    while (true) {
        if (fscanf(f, "%d%c", &n, &c) != 2) {
            break;
        }
        if (idx > 0) {
            d_prev = d;
            d = n - n_prev;
            fprintf(stderr,
                "(idx %d) (n %d) (n_prev %d) (d %d) (d_prev %d)",
                idx, n, n_prev, d, d_prev);
            if (
                (idx > 1 && (sign(d) != sign(d_prev))) ||
                (abs(d) < 1 || abs(d) > 3)
            ) {
                fprintf(stderr, " (not ok)\n");
                is_safe = false;
            } else {
                fprintf(stderr, " (ok)\n");
            }
        }
        if (c == '\n') {
            fprintf(stderr, "-> (safe? %d)\n", is_safe);
            idx = 0;
            if (is_safe) {
                n_safe_reports += 1;
            }
            is_safe = true;
        } else {
            idx += 1;
            n_prev = n;
            d_prev = d;
        }
    }

    printf("%d\n", n_safe_reports);

    return 0;
}
