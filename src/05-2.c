#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#define MAX_N 100
#define MAX_N_PAIRS 5000

struct Pair {
    i32 before;
    i32 after;
};

void reset_seen(i32 seen[MAX_N]) {
    for (i32 i = 0; i < MAX_N; i += 1) {
        seen[i] = -1;
    }
}

i32 get_fix_pos(
    struct Pair pairs[MAX_N],
    i32 n_pairs,
    i32 seen[MAX_N],
    i32 n
) {
    for (i32 idx = 0; idx < n_pairs; idx += 1) {
        i32 before = pairs[idx].before;
        i32 after = pairs[idx].after;
        if (n == before) {
            fprintf(stderr, "|%d %d| ", before, after);
        }
        if (n == before && seen[after] >= 0) {
            fprintf(stderr, "[!%d] ", after);
            return seen[after];
        }
    }
    return -1;
}

int main() {
    FILE *f = fopen("data/05", "r");
    if (!f) {
        fprintf(stderr, "Could not open file\n");
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

        fprintf(stderr, "o> ");
        bool met_rules = true;
        while (true) {
start:
            i32 seen[MAX_N];
            reset_seen(seen);
            for (i32 idx = 0; idx < n_numbers; idx += 1) {
                i32 n = row[idx];
                fprintf(stderr, "\n\t(%d @ %d) ", n, idx);
                i32 fix_pos = get_fix_pos(pairs, n_pairs, seen, n);
                fprintf(stderr, "(fp %d) ", fix_pos);
                if (fix_pos >= 0) {
                    row[idx] = row[fix_pos];
                    row[fix_pos] = n;
                    met_rules = false;
                    fprintf(stderr, "{%d<->%d} ", idx, fix_pos);
                    goto start;
                } else {
                    seen[n] = idx;
                }
            }
            break;
        }
        fprintf(stderr, "\n");

        fprintf(stderr, "n> ");
        for (i32 idx = 0; idx < n_numbers; idx += 1) {
            i32 n = row[idx];
            fprintf(stderr, "%d ", n);
        }
        fprintf(stderr, "\n\n");

        if (!met_rules) {
            i32 mid = row[n_numbers / 2];
            sum += mid;
        }
    }
end:

    printf("%d\n", sum);

    return 0;
}
