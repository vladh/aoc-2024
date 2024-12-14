#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

// Finds the greatest common divisor `g` of `p` and `q`, then returns some `a`
// and `b` such that `pa + qb = g`.
// https://cp-algorithms.com/algebra/extended-euclid-algorithm.html
i64 extended_euclid(i64 p, i64 q, i64 *a, i64 *b) {
    if (q == 0) {
        *a = 1;
        *b = 0;
        return p;
    }
    i64 a1, b1;
    i64 d = extended_euclid(q, p % q, &a1, &b1);
    *a = b1;
    *b = a1 - b1 * (p / q);
    return d;
}

// Finds a solution for `a` and `b` in `pa + qb = c`.
// https://cp-algorithms.com/algebra/linear-diophantine-equation.html
bool find_any_solution(i64 p, i64 q, i64 c, i64 *a, i64 *b, i64 *g) {
    *g = extended_euclid(p, q, a, b);
    if (c % *g) {
        return false;
    }
    *a *= c / *g;
    *b *= c / *g;
    return true;
}

// Move to the next solution, by a factor.
// https://cp-algorithms.com/algebra/linear-diophantine-equation.html
// https://en.wikipedia.org/wiki/Diophantine_equation#Linear_Diophantine_equations
void shift_solution(i64 *a, i64 *b, i64 p, i64 q, i64 cnt) {
    *a += cnt * q;
    *b -= cnt * p;
}

// Return the bounds of `a` such that `a_min <= a <= a_max` and `b_min <= b <=
// b_max`. `la` is the left bound, `ra` is the right bound. The function's
// return value is whether or not it succeeded.
// https://cp-algorithms.com/algebra/linear-diophantine-equation.html
bool constrain_solution(i64 p, i64 q, i64 c, i64 a_min, i64 a_max, i64 b_min, i64 b_max, i64 *la, i64 *ra) {
    i64 a, b, g;
    if (!find_any_solution(p, q, c, &a, &b, &g)) {
        return false;
    }
    p /= g;
    q /= g;

    i64 sign_p = p > 0 ? +1 : -1;
    i64 sign_q = q > 0 ? +1 : -1;

    shift_solution(&a, &b, p, q, (a_min - a) / q);
    if (a < a_min) {
        shift_solution(&a, &b, p, q, sign_q);
    }
    if (a > a_max) {
        return false;
    }
    i64 la1 = a;

    shift_solution(&a, &b, p, q, (a_max - a) / q);
    if (a > a_max) {
        shift_solution(&a, &b, p, q, -sign_q);
    }
    i64 ra1 = a;

    shift_solution(&a, &b, p, q, -(b_min - b) / p);
    if (b < b_min) {
        shift_solution(&a, &b, p, q, -sign_p);
    }
    if (b > b_max) {
        return false;
    }
    i64 la2 = a;

    shift_solution(&a, &b, p, q, -(b_max - b) / p);
    if (b > b_max) {
        shift_solution(&a, &b, p, q, sign_p);
    }
    i64 ra2 = a;

    if (la2 > ra2) {
        i64 tmp = la2;
        la2 = ra2;
        ra2 = tmp;
    }
    *la = max(la1, la2);
    *ra = min(ra1, ra2);

    if (*la > *ra) {
        return false;
    }

    return true;
}

int main() {
    FILE *f = fopen("data/13", "r");
    if (!f) {
        eprintf("Could not open file\n");
        exit(1);
    }
    i32 idx_problem = 0;
    i64 total_cost = 0;

    while (true) {
        i64 d_ax, d_ay;
        if (fscanf(f, "Button A: X+%lu, Y+%lu ", &d_ax, &d_ay) != 2) {
            break;
        }
        i64 d_bx, d_by;
        assert(fscanf(f, "Button B: X+%lu, Y+%lu ", &d_bx, &d_by) == 2);
        i64 prize_x, prize_y;
        assert(fscanf(f, "Prize: X=%lu, Y=%lu ", &prize_x, &prize_y) == 2);

        idx_problem += 1;
        eprintf("\nProblem %d:\n", idx_problem);

        eprintf("(d_ax %ld) (d_bx %ld)\n", d_ax, d_bx);
        i64 a_x_min = 0;
        i64 a_x_max = (i64)ceil((f64)prize_x / (f64)d_ax);
        i64 b_x_min = 0;
        i64 b_x_max = (i64)ceil((f64)prize_x / (f64)d_bx);
        i64 a_y_min = 0;
        i64 a_y_max = (i64)ceil((f64)prize_y / (f64)d_ay);
        i64 b_y_min = 0;
        i64 b_y_max = (i64)ceil((f64)prize_y / (f64)d_by);

        i64 lax, rax;
        bool could_constrain_x_solution = constrain_solution(d_ax, d_bx, prize_x, a_x_min, a_x_max, b_x_min, b_x_max, &lax, &rax);
        if (!could_constrain_x_solution) {
            eprintf("Could not constrain x solution\n");
            continue;
        }
        i64 lbx = (prize_x - (rax * d_ax)) / d_bx;
        i64 rbx = (prize_x - (lax * d_ax)) / d_bx;

        i64 lay, ray;
        bool could_constrain_y_solution = constrain_solution(d_ay, d_by, prize_y, a_y_min, a_y_max, b_y_min, b_y_max, &lay, &ray);
        if (!could_constrain_y_solution) {
            eprintf("Could not constrain y solution\n");
            continue;
        }
        i64 lby = (prize_y - (ray * d_ay)) / d_by;
        i64 rby = (prize_y - (lay * d_ay)) / d_by;

        eprintf("(lax %ld) (rax %ld) (lbx %ld) (rbx %ld)\n", lax, rax, lbx, rbx);
        eprintf("(lay %ld) (ray %ld) (lby %ld) (rby %ld)\n", lay, ray, lby, rby);

        i64 la = max(lax, lay);
        i64 lb = max(lbx, lby);
        i64 ra = min(rax, ray);
        i64 rb = min(rbx, rby);
        eprintf("(la %ld) (lb %ld) (ra %ld) (rb %ld)\n", la, lb, ra, rb);

        for (i64 a = la; a <= ra; a += 1) {
            for (i64 b = lb; b <= rb; b += 1) {
                i64 x = a * d_ax + b * d_bx;
                i64 y = a * d_ay + b * d_by;
                if (x == prize_x && y == prize_y) {
                    eprintf("(a %ld) (b %ld)\n", a, b);
                    i64 cost = a * 3 + b;
                    total_cost += cost;
                    goto end;
                }
            }
        }

        eprintf("no solution!\n");
end:
        continue;
    }

    printf("%d\n", total_cost);

    return 0;
}
