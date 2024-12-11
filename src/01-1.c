#include <stdio.h>
#include <stdlib.h>
#include "intrinsics.h"

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
        eprintf("Could not open file\n");
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

    qsort(list_a, list_len, sizeof(u32), cmp_u32);
    qsort(list_b, list_len, sizeof(u32), cmp_u32);

    i32 sum = 0;

    for (i32 i = 0; i < list_len; i += 1) {
        sum += abs(list_b[i] - list_a[i]);
    }

    printf("%d\n", sum);

    return 0;
}
