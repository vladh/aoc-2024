#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

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

void disk_defragment(u16 *disk, i32 disk_len) {
    i32 idx_left = 0;
    i32 idx_right = disk_len - 1;

    /* disk_print(disk, disk_len); */

    u16 left_file_id = (i32)disk[idx_left];
    u16 right_file_id = (i32)disk[idx_right];
    while (right_file_id == 0) {
        idx_right -= 1;
        right_file_id = disk[idx_right];
        if (idx_right == 0) {
            return;
        }
    }

    while (true) {
        if (idx_left >= idx_right) {
            goto end;
        }

        left_file_id = disk[idx_left];
        right_file_id = disk[idx_right];

        if (left_file_id == 0) {
            disk[idx_right] = 0;
            disk[idx_left] = right_file_id;
            while (disk[idx_right] == 0) {
                if (idx_left >= idx_right) {
                    goto end;
                }
                idx_right -= 1;
            }
            /* disk_print(disk, disk_len); */
        }

        idx_left += 1;
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
