#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"
#include "../include/pstr.h"
#include "../include/pstr.c"

int main() {
    int fd = open("data/03", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;
    bool is_mul_enabled = true;
    i32 sum = 0;

    while (*curr) {
        if (pstr_starts_with(curr, "do()")) {
            fprintf(stderr, "ENABLED\n");
            is_mul_enabled = true;
        }
        if (pstr_starts_with(curr, "don't()")) {
            fprintf(stderr, "DISABLED\n");
            is_mul_enabled = false;
        }
        if (pstr_starts_with(curr, "mul(")) {
            curr += 4;

            char a_str[128];
            i32 a_str_len = 0;
            while (isdigit(*curr)) {
                a_str[a_str_len] = *curr;
                a_str_len += 1;
                curr += 1;
            }
            if (a_str_len == 0) {
                continue;
            }
            a_str[a_str_len] = '\0';
            a_str_len += 1;
            char *a_str_end = a_str + a_str_len;
            u32 a = strtol(a_str, &a_str_end, 10);

            if (*curr != ',') {
                continue;
            }
            curr += 1;

            char b_str[128];
            i32 b_str_len = 0;
            while (isdigit(*curr)) {
                b_str[b_str_len] = *curr;
                b_str_len += 1;
                curr += 1;
            }
            if (b_str_len == 0) {
                continue;
            }
            b_str[b_str_len] = '\0';
            b_str_len += 1;
            char *b_str_end = b_str + b_str_len;
            u32 b = strtol(b_str, &b_str_end, 10);

            if (*curr != ')') {
                continue;
            }

            if (is_mul_enabled) {
                fprintf(stderr, "%d * %d\n", a, b);
                sum += a * b;
            } else {
                fprintf(stderr, "DISABLED %d * %d\n", a, b);
            }
        }
        curr += 1;
    }

    printf("%d\n", sum);

    return 0;
}
