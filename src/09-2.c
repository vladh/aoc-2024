#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

#define DISK_LEN 20000 * 9 * 2

void disk_print(u16 *disk, i32 disk_len) {
    for (i32 i = 0; i < disk_len; i += 1) {
        i32 file_id = (i32)disk[i] - 1;
        if (file_id == -1) {
            eprintf(".");
        } else {
            eprintf("%d", file_id);
        }
    }
    eprintf("\n");
}

void disk_print_indices(
    i32 disk_len,
    i32 idx_left,
    i32 idx_left_end,
    i32 idx_right,
    i32 idx_right_start
) {
    for (i32 i = 0; i < disk_len; i += 1) {
        if (i == idx_left) {
            eprintf("[");
        } else if (i == idx_left_end) {
            eprintf("]");
        } else if (i == idx_right_start) {
            eprintf("(");
        } else if (i == idx_right) {
            eprintf(")");
        } else {
            eprintf(" ");
        }
    }
    eprintf("\n");
}

void disk_defragment(u16 *disk, i32 disk_len) {
    i32 idx_right = disk_len - 1;

    while (true) {
        i32 idx_left = 0;

        while (disk[idx_right] == 0) {
            if (idx_right == 0) {
                goto end;
            }
            idx_right -= 1;
        }

        u16 file_id_right = disk[idx_right];

        i32 idx_right_start = idx_right;
        while (idx_right_start > 0 && disk[idx_right_start - 1] == file_id_right) {
            idx_right_start -= 1;
        }

        while (idx_left < idx_right) {
            while (disk[idx_left] != 0) {
                if (idx_left >= idx_right) {
                    goto repeat;
                }
                idx_left += 1;
            }

            i32 idx_left_end = idx_left;
            while (disk[idx_left_end] == 0) {
                if (idx_left_end >= idx_right) {
                    break;
                }
                idx_left_end += 1;
            }
            idx_left_end -= 1;

            i32 left_len = idx_left_end - idx_left + 1;
            i32 right_len = idx_right - idx_right_start + 1;

            /* disk_print(disk, disk_len); */
            /* disk_print_indices(disk_len, idx_left, idx_left_end, idx_right, idx_right_start); */

            if (left_len >= right_len) {
                for (i32 i = 0; i < right_len; i += 1) {
                    disk[idx_left + i] = disk[idx_right - i];
                    disk[idx_right - i] = 0;
                    /* disk_print(disk, disk_len); */
                }
                break;
            }

            idx_left = idx_left_end + 1;
        }

repeat:
        if (idx_right_start == 0) {
            break;
        }
        idx_right = idx_right_start - 1;
    }

end:
    ;
    /* disk_print(disk, disk_len); */
}

i64 disk_checksum(u16 *disk, i32 disk_len) {
    i64 checksum = 0;
    for (i32 i = 0; i < disk_len; i += 1) {
        i32 file_id = (i32)disk[i] - 1;
        if (file_id >= 0) {
            checksum += i * file_id;
        }
    }
    return checksum;
}

int main() {
    int fd = open("data/09", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;

    u16 *disk = malloc(DISK_LEN * sizeof(u16));
    i32 disk_len = 0;
    i32 idx_file = 0;
    bool is_file = true;

    while (*curr) {
        i32 block_len = *curr - '0';
        assert(block_len < 10);
        if (*curr == '\n') {
            break;
        }
        if (is_file) {
            for (i32 i = disk_len; i < disk_len + block_len; i += 1) {
                disk[i] = idx_file + 1;
            }
            idx_file += 1;
        }
        disk_len += block_len;
        is_file = !is_file;
        curr += 1;
    }

    disk_defragment(disk, disk_len);

    i64 checksum = disk_checksum(disk, disk_len);

    printf("%" PRIi64 "\n", checksum);

    return 0;
}
