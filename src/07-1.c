#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "intrinsics.h"

#define MAX_N_TERMS 20

enum Op {
    OP_FIRST = 0,
    OP_ADD = 0,
    OP_MUL = 1,
    OP_LAST = 1,
    OP_COUNT = 2,
};

u64 apply_op(enum Op op, u64 a, u64 b) {
    if (op == OP_ADD) {
        return a + b;
    } else if (op == OP_MUL) {
        return a * b;
    }
    assert(false);
}

u64 apply_ops(
    enum Op ops[MAX_N_TERMS],
    u64 terms[MAX_N_TERMS],
    i32 n_terms
) {
    u64 result = terms[0];
    for (i32 idx_term = 1; idx_term < n_terms; idx_term += 1) {
        result = apply_op(ops[idx_term - 1], result, terms[idx_term]);
    }
    return result;
}

bool find_ops(
    enum Op ops[MAX_N_TERMS],
    u64 target_result,
    u64 terms[MAX_N_TERMS],
    i32 n_terms
) {
    i32 n_combinations = pow(OP_COUNT, n_terms - 1);
    /* eprintf("%d terms, %d ops, %d combinations\n", n_terms, n_terms - 1, n_combinations); */
    for (i32 idx_comb = 0; idx_comb < n_combinations; idx_comb += 1) {
        for (i32 idx_op = 0; idx_op < n_terms - 1; idx_op += 1) {
            // Treat idx_comb as a base b number, and get its i-th digit
            // Where b is the number of possible operators
            // And i is the operator index we are using
            ops[idx_op] = (idx_comb / (u64)pow(OP_COUNT, idx_op)) % OP_COUNT;
            /* eprintf("%" PRIu64 " [%d] ", terms[idx_op], ops[idx_op]); */
        }
        u64 test_result = apply_ops(ops, terms, n_terms);
        /* eprintf("%" PRIu64" = %" PRIu64 " ?= %" PRIu64 "\n", terms[n_terms - 1], test_result, target_result); */
        if (test_result == target_result) {
            return true;
        }
    }
    return false;
}

int main() {
    FILE *f = fopen("data/07", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }

    u64 sum = 0;

    while (true) {
        u64 target_result;
        u64 term;
        char c;
        u64 terms[MAX_N_TERMS] = {0};
        i32 n_terms = 0;

        if (fscanf(f, "%" SCNu64 ": ", &target_result) != 1) {
            break;
        }

        while (true) {
            assert(fscanf(f, "%lu%c", &term, &c) == 2);
            terms[n_terms] = term;
            n_terms += 1;
            assert(n_terms <= MAX_N_TERMS);
            if (c == '\n') {
                break;
            }
        }

        enum Op ops[MAX_N_TERMS] = {0};

        bool did_find_ops = find_ops(ops, target_result, terms, n_terms);
        if (did_find_ops) {
            sum += target_result;
        }
    }

    printf("%" PRIu64 "\n", sum);

    return 0;
}
