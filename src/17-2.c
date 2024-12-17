#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "intrinsics.h"

#define MAX_PROGRAM_SIZE 100
#define MAX_SOL_BITS 32

struct State {
    u64 ra;
    u64 rb;
    u64 rc;
    u8 prog[MAX_PROGRAM_SIZE];
    i32 prog_len;
    i32 pc;
    u8 out[MAX_PROGRAM_SIZE];
    i32 out_len;
};

bool are_progs_equal(
    u8 prog1[MAX_PROGRAM_SIZE],
    i32 prog1_len,
    u8 prog2[MAX_PROGRAM_SIZE],
    i32 prog2_len
) {
    if (prog1_len != prog2_len) {
        return false;
    }
    for (i32 i = 0; i < prog1_len; i += 1) {
        if (prog1[i] != prog2[i]) {
            return false;
        }
    }
    return true;
}

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
    state->ra = state->ra / (u64)pow(2, eval_combo_op(state, op));
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
    state->out[state->out_len] = eval_combo_op(state, op) % 8;
    state->out_len += 1;
    assert(state->out_len < MAX_PROGRAM_SIZE);
}

void bdv(struct State *state, u8 op) {
    state->rb = state->ra / (u64)pow(2, eval_combo_op(state, op));
}

void cdv(struct State *state, u8 op) {
    state->rc = state->ra / (u64)pow(2, eval_combo_op(state, op));
}

void exec(struct State *state, u8 inst, u8 op) {
    bool inc_pc = true;
    switch (inst) {
    case 0:
        adv(state, op);
        break;
    case 1:
        bxl(state, op);
        break;
    case 2:
        bst(state, op);
        break;
    case 3:
        jnz(state, op);
        inc_pc = false;
        break;
    case 4:
        bxc(state);
        break;
    case 5:
        out(state, op);
        break;
    case 6:
        bdv(state, op);
        break;
    case 7:
        cdv(state, op);
        break;
    }
    if (inc_pc) {
        state->pc += 2;
    }
}

void print_state(struct State *state) {
    eprintf("pc: %d\n", state->pc);
    eprintf("ra: %lu\n", state->ra);
    eprintf("rb: %lu\n", state->rb);
    eprintf("rc: %lu\n", state->rc);
    eprintf("\n");
}

void sol_inc(u8 sol[MAX_SOL_BITS], i32 sol_len) {
    for (i32 i = 0; i < sol_len; i += 1) {
        if (sol[i] == 7) {
            sol[i] = 0;
        } else {
            sol[i] += 1;
            break;
        }
    }
}

u64 sol_to_u64(u8 sol[MAX_SOL_BITS], i32 sol_len) {
    u64 res = 0;
    for (i32 i = 0; i < sol_len; i += 1) {
        res += sol[i] * pow(8, i);
    }
    return res;
}

void sol_print(u8 sol[MAX_SOL_BITS], i32 sol_len) {
    for (i32 i = 0; i < sol_len; i += 1) {
        eprintf("%d ", sol[i]);
    }
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
        assert(state.prog_len < MAX_PROGRAM_SIZE);
        if (c == '\n') {
            break;
        }
    }

    struct State orig_state = state;
    u8 sol[MAX_SOL_BITS] = {0};
    sol[state.prog_len - 1] = 3;
    while (true) {
        sol_inc(sol, state.prog_len);
        u64 next_ra = sol_to_u64(sol, state.prog_len);
        if (next_ra % 100000 == 0) {
            sol_print(sol, state.prog_len);
            eprintf("(%lu)\n", next_ra);
        }
        state = orig_state;
        state.ra = next_ra;
        while (state.pc < state.prog_len) {
            u8 inst = state.prog[state.pc];
            u8 op = state.prog[state.pc + 1];
            /* print_state(&state); */
            /* eprintf("-> %lu %lu\n", inst, op); */
            exec(&state, inst, op);
        }

        if (are_progs_equal(state.prog, state.prog_len, state.out, state.out_len)) {
            sol_print(sol, state.prog_len);
            printf("(%lu)\n", next_ra);
            break;
        }
    }

    return 0;
}
