#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void stat_file(char *filename)
{
    struct stat sb;
    char dest[1024];
    if (lstat(filename, &sb) < 0) {
        perror(filename);
        return;
    }

    printf("info for file %s\n", filename);
    printf("device %u, inode %llu\n",
           sb.st_dev, sb.st_ino);
    printf("mode %o\n", sb.st_mode);
    switch (sb.st_mode & S_IFMT) {
        case S_IFREG:
            printf("regular file\n");
            break;
        case S_IFDIR:
            printf("directory\n");
            break;
        case S_IFIFO:
            printf("FIFO\n");
            break;
        case S_IFLNK:
            memset(dest, 0, 1024);
            readlink(filename, dest, 1024);
            if (dest[1023] != 0) {
                fprintf(stderr, "%s: link too long\n", filename);
            } else {
                printf("symbolic link to %s\n", dest);
            }
            break;
    }
    if (S_ISREG(sb.st_mode)) {
        printf("it's really a regular file. i checked.\n");
    }
    printf("it has %d hard links\n", sb.st_nlink);
    printf("owner %d, group %d\n",
           sb.st_uid, sb.st_gid);
    printf("has size %lld, uses %lld bytes",
           sb.st_size, sb.st_blocks * 512);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "no arguments\n");
        return 2;
    }

    for (int i = 1; i < argc; i++) {
        stat_file(argv[i]);
    }

    return 0;
}
