#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "../include/hashmap.h"
#include "../include/hashmap.c"

struct Entry {
    u64 n;
    i32 n_steps;
    u64 result;
};

int entry_compare(const void *ap, const void *bp, void *udata) {
    const struct Entry *a = ap;
    const struct Entry *b = bp;
    if (b->n - a->n == 0) {
        return b->n_steps - a->n_steps;
    } else {
        return b->n - a->n;
    }
}

u64 entry_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct Entry *entry = item;
    return hashmap_sip(entry, sizeof(u64) + sizeof(i32), seed0, seed1);
}

void split_in_half(u64 n, u64 *l, u64 *r) {
    i32 n_digits = (i32)log10(n) + 1;
    u64 half_digit_power = (u64)pow(10, n_digits / 2);
    *l = (u64)(n / half_digit_power);
    u64 l_big = *l * half_digit_power;
    *r = n - l_big;
}

u64 blink(u64 n, i32 n_steps, struct hashmap *map) {
    if (n_steps == 0) {
        return 1;
    }

    struct Entry entry = (struct Entry) {
        .n = n,
        .n_steps = n_steps,
        .result = 0,
    };

    void const *found_entry_p = hashmap_get(map, &entry);
    if (found_entry_p) {
        struct Entry const *found_entry = found_entry_p;
        return found_entry->result;
    }

    i32 n_digits = (i32)log10(n) + 1;
    if (n == 0) {
        entry.result = blink(1, n_steps - 1, map);
    } else if (n_digits % 2 == 0) {
        u64 l;
        u64 r;
        split_in_half(n, &l, &r);
        entry.result = blink(l, n_steps - 1, map) + blink(r, n_steps - 1, map);
    } else {
        entry.result = blink(n * 2024, n_steps - 1, map);
    }

    hashmap_set(map, &entry);

    return entry.result;
}

int main() {
    FILE *f = fopen("data/11", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }

    struct hashmap *map = hashmap_new(
        sizeof(struct Entry), 0, 0, 0, entry_hash, entry_compare, NULL, NULL);
    i32 N_STEPS = 75;
    u64 n_stones = 0;
    u64 n = 0;

    while (fscanf(f, "%" SCNu64, &n) == 1) {
        n_stones += blink(n, N_STEPS, map);
    }

    printf("%" PRIu64 "\n", n_stones);

    return 0;
}
