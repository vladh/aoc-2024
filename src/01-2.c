#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int cmp_u32(const void *ap, const void *bp) {
    u32 a = *((u32 *)ap);
    u32 b = *((u32 *)bp);
    if (a > b) {
        return 1;
    }
    if (a < b) {
        return -1;
    }
    return 0;
}

int main() {
    u32 *list_a = malloc(sizeof(u32) * 2000);
    u32 *list_b = malloc(sizeof(u32) * 2000);
    i32 list_len = 0;
    u32 a = 0;
    u32 b = 0;

    FILE *f = fopen("data/01", "r");
    if (!f) {
        fprintf(stderr, "Could not open file\n");
        exit(1);
    }

    while (true) {
        if (fscanf(f, "%d %d", &a, &b) <= 0) {
            break;
        }
        list_a[list_len] = a;
        list_b[list_len] = b;
        list_len += 1;
    }

    i32 sum = 0;

    for (i32 idx_a = 0; idx_a < list_len; idx_a += 1) {
        i32 a = list_a[idx_a];
        i32 n_occurences = 0;
        for (i32 idx_b = 0; idx_b < list_len; idx_b += 1) {
            i32 b = list_b[idx_b];
            if (a == b) {
                n_occurences += 1;
            }
        }
        sum += a * n_occurences;
    }

    printf("%d\n", sum);

    return 0;
}
