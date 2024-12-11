#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intrinsics.h"

#define MAX_N 100
#define MAX_N_PAIRS 5000

struct Pair {
    i32 before;
    i32 after;
};

bool are_rules_met(
    struct Pair pairs[MAX_N],
    i32 n_pairs,
    bool seen[MAX_N],
    i32 n
) {
    for (i32 idx = 0; idx < n_pairs; idx += 1) {
        i32 before = pairs[idx].before;
        i32 after = pairs[idx].after;
        if (n == before && seen[after]) {
            eprintf(" (OOPS! saw %d but already saw %d)", before, after);
            return false;
        }
    }
    return true;
}

int main() {
    FILE *f = fopen("data/05", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }

    i32 sum = 0;

    struct Pair pairs[MAX_N_PAIRS];
    i32 n_pairs = 0;
    i32 before;
    i32 after;

    while (true) {
        i64 fpos = ftell(f);
        if (fscanf(f, "%d|%d ", &before, &after) != 2) {
            fseek(f, fpos, SEEK_SET);
            break;
        }
        pairs[n_pairs] = (struct Pair) {
            .before = before,
            .after = after,
        };
        n_pairs += 1;
    }

    while (true) {
        i32 n;
        char c;
        i32 row[MAX_N];
        i32 n_numbers = 0;
        bool seen[MAX_N] = {0};

        while (true) {
            if (fscanf(f, "%d%c", &n, &c) != 2) {
                goto end;
            }
            row[n_numbers] = n;
            n_numbers += 1;
            if (c == '\n') {
                break;
            }
        }

        bool met_rules = true;
        for (i32 idx = 0; idx < n_numbers; idx += 1) {
            i32 n = row[idx];
            eprintf("%d ", n);
            if (!are_rules_met(pairs, n_pairs, seen, n)) {
                met_rules = false;
                break;
            }
            seen[n] = true;
        }
        if (met_rules) {
            i32 mid = row[n_numbers / 2];
            eprintf(" (VALID!) (mid is %d)", mid);
            sum += mid;
        }
        eprintf("\n");
    }
end:

    printf("%d\n", sum);

    return 0;
}
