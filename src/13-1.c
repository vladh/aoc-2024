#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

bool is_integer(f64 n) {
    return fabs(round(n) - n) <= 0.0001f;
}

int main() {
    FILE *f = fopen("data/13", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }
    i32 idx_problem = 0;

    u64 total_cost = 0;

    while (true) {
        f64 d_ax, d_ay;
        if (fscanf(f, "Button A: X+%lf, Y+%lf ", &d_ax, &d_ay) != 2) {
            break;
        }
        f64 d_bx, d_by;
        assert(fscanf(f, "Button B: X+%lf, Y+%lf ", &d_bx, &d_by) == 2);
        f64 prize_x, prize_y;
        assert(fscanf(f, "Prize: X=%lf, Y=%lf ", &prize_x, &prize_y) == 2);

        prize_x += 10000000000000;
        prize_y += 10000000000000;

        idx_problem += 1;
        eprintf("\nProblem %d:\n", idx_problem);

        f64 A[2][2] = {
            { d_ax, d_bx },
            { d_ay, d_by }
        };
        f64 B[2][1] = {
            { prize_x },
            { prize_y }
        };
        f64 A_det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
        f64 A_inv[2][2] = {
            {  A[1][1], -A[0][1] },
            { -A[1][0],  A[0][0] },
        };
        f64 A_inv_B[2][1] = {
            { B[0][0] * A_inv[0][0] + B[1][0] * A_inv[0][1] },
            { B[0][0] * A_inv[1][0] + B[1][0] * A_inv[1][1] },
        };
        f64 a_float = A_inv_B[0][0] / A_det;
        f64 b_float = A_inv_B[1][0] / A_det;
        eprintf("(a_float %lf) (b_float %lf)\n", a_float, b_float);
        if (!is_integer(a_float) || !is_integer(b_float)) {
            eprintf("No solution\n");
            continue;
        }
        u64 a = round(a_float);
        u64 b = round(b_float);
        eprintf("(a %ld) (b %ld)\n", a, b);
        total_cost += a * 3 + b;
    }

    printf("%ld\n", total_cost);

    return 0;
}
