#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "intrinsics.h"

#define MAX_PROGRAM_SIZE 100

struct State {
    u64 ra;
    u64 rb;
    u64 rc;
    u8 prog[MAX_PROGRAM_SIZE];
    i32 prog_len;
    i32 pc;
};

u64 eval_combo_op(struct State *state, u8 op) {
    switch (op) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return state->ra;
    case 5:
        return state->rb;
    case 6:
        return state->rc;
    default:
        assert(false);
    }
}

void adv(struct State *state, u8 op) {
    state->ra = state->ra / pow(2, eval_combo_op(state, op));
}

void bxl(struct State *state, u8 op) {
    state->rb = state->rb ^ op;
}

void bst(struct State *state, u8 op) {
    state->rb = eval_combo_op(state, op) % 8;
}

void jnz(struct State *state, u8 op) {
    if (state->ra == 0) {
        state->pc += 2;
    } else {
        state->pc = op;
    }
}

void bxc(struct State *state) {
    state->rb = state->rb ^ state->rc;
}

void out(struct State *state, u8 op) {
    printf("%hhu,", eval_combo_op(state, op) % 8);
}

void bdv(struct State *state, u8 op) {
    state->rb = state->ra / pow(2, eval_combo_op(state, op));
}

void cdv(struct State *state, u8 op) {
    state->rc = state->ra / pow(2, eval_combo_op(state, op));
}

int main() {
    struct State state = {0};

    FILE *f = fopen("data/17", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }

    assert(fscanf(f, "Register A: %lu ", &state.ra) == 1);
    assert(fscanf(f, "Register B: %lu ", &state.rb) == 1);
    assert(fscanf(f, "Register C: %lu ", &state.rc) == 1);
    assert(fscanf(f, " Program: ") == 0);

    u8 part;
    char c;
    while (true) {
        if (fscanf(f, "%hhu%c", &part, &c) != 2) {
            break;
        }
        state.prog[state.prog_len] = part;
        state.prog_len += 1;
        if (c == '\n') {
            break;
        }
    }

    while (state.pc < state.prog_len) {
        u8 inst = state.prog[state.pc];
        u8 op = state.prog[state.pc + 1];
        eprintf("pc: %d\n", state.pc);
        eprintf("ra: %lu\n", state.ra);
        eprintf("rb: %lu\n", state.rb);
        eprintf("rc: %lu\n", state.rc);
        eprintf("-> %lu %lu\n", inst, op);
        eprintf("\n");
        bool inc_pc = true;
        switch (inst) {
        case 0:
            adv(&state, op);
            break;
        case 1:
            bxl(&state, op);
            break;
        case 2:
            bst(&state, op);
            break;
        case 3:
            jnz(&state, op);
            inc_pc = false;
            break;
        case 4:
            bxc(&state);
            break;
        case 5:
            out(&state, op);
            break;
        case 6:
            bdv(&state, op);
            break;
        case 7:
            cdv(&state, op);
            break;
        }
        if (inc_pc) {
            state.pc += 2;
        }
    }

    printf("\n");

    return 0;
}
