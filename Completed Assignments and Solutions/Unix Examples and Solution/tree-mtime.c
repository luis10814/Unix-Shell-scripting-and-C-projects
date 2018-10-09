#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static time_t find_mtime(const char *path)
{
    struct stat sb;
    time_t mtime, newest_mtime;
    DIR *dir;
    struct dirent *entry;
    size_t path_len = strlen(path);

    if (stat(path, &sb) < 0) {
        perror(path);
        return -1;
    }

    newest_mtime = sb.st_mtime;
    
    if (!S_ISDIR(sb.st_mode)) {
        return newest_mtime;
    }

    dir = opendir(path);
    if (!dir) {
        perror(path);
        return newest_mtime;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        size_t name_len = strlen(entry->d_name);
        char *d_path = malloc(path_len + 1 + name_len + 1);
        strcpy(d_path, path);
        d_path[path_len] = '/';
        strcpy(d_path + path_len + 1, entry->d_name);
        // strcpy gave us the null byte
        
        mtime = find_mtime(d_path);
        if (mtime > newest_mtime) {
            newest_mtime = mtime;
        }

        free(d_path);
    }
    closedir(dir);

    return newest_mtime;
}

int main(int argc, const char *argv[])
{
    int i;
    int status = 0;

    for (i = 1; i < argc; i++) {
        time_t mtime = find_mtime(argv[i]);
        if (mtime < 0) {
            status = 1;
        } else {
            char buffer[100];
            struct tm *ltime = localtime(&mtime);
            strftime(buffer, 100, "%a, %b %d %Y %H:%M:%S %Z", ltime);
            printf("%s: %s\n", argv[i], buffer);
        }
    }

    return status;
}
