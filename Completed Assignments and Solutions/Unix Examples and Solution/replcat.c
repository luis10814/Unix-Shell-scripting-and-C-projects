#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void process_file(const char *srch, const char *repl, FILE *file)
{
    size_t size = 512;
    char *buf = malloc(size);
    char *rbuf;
    size_t srch_len = strlen(srch);
    if (!buf) abort();

    // we try to loop over lines
    while ((rbuf = fgets(buf, size, file))) {
        size_t slen = strlen(buf);
        // we check if we have a whole line
        while (slen > 0 && rbuf[slen-1] != '\n') {
            // no newline, keep expanding until we have read a whole line
            // subsequent reads will use our new, longer buffer
            size *= 2;
            buf = realloc(buf, size);
            if (!buf) abort();
            rbuf = fgets(buf + slen, size - slen, file);
            slen = strlen(buf);
            if (!rbuf) break;
        }

        char *pos = buf;
        char *npos;
        while ((npos = strstr(pos, srch))) {
            *npos = 0; // zero the character
            fputs(pos, stdout);
            fputs(repl, stdout);
            pos = npos + strlen(srch);
        }
        fputs(pos, stdout);
    }

    if (!feof(file)) {
        perror("replcat");
    }

    free(buf);
}

int main(int argc, char *argv[])
{
    const char *srch, *repl;

    if (argc < 3) {
        fprintf(stderr, "insufficient arguments\n");
        return 2;
    }

    srch = argv[1];
    repl = argv[2];

    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (!file) {
                perror(argv[i]);
                return 1;
            }
            process_file(srch, repl, file);
            fclose(file);
        }
    } else {
        process_file(srch, repl, stdin);
    }
}
