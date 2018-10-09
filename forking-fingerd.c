#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <sysexits.h>
#include <unistd.h>
#include <errno.h>

ssize_t lookup_address(char *host, char *port, struct sockaddr **addr)
{
    struct addrinfo hints = {
        AI_V4MAPPED,
        AF_INET6,
        SOCK_DGRAM,
        0
    };
    struct addrinfo *ai;
    ssize_t len;
    int code;
    code = getaddrinfo(host, port, &hints, &ai);
    if (code < 0) {
        return -1;
    }

    // now we copy the socket address
    len = ai->ai_addrlen;
    *addr = malloc(len);
    memcpy(*addr, ai->ai_addr, len);
    freeaddrinfo(ai);
    return len;
}

int handle_request(int fd)
{
    char inbuf[1024];
    char outbuf[1536];
    ssize_t ret;
    size_t n = 0;
    size_t len;

    memset(inbuf, 0, 1024);

    while (n < 1023 && (ret = read(fd, inbuf + n, 1023 - n)) > 0) {
        n += ret;
        // n never hits 1023, so we have at least one null byte
        if (strchr(inbuf, '\n')) {
            break;  // we have a new line
        }
    }
    if (ret < 0) {
        return -1;
    }
    // make sure it's null-terminated
    if (n < 1024) {
        inbuf[n] = 0;
    } else {
        errno = EOVERFLOW;
        return -1;
    }

    // now we have data!  Find first whitespace.
    for (char *p = inbuf; p - inbuf < 1024 && *p; p++) {
        if (isspace(*p)) {
            *p = 0;
            break;
        }
    }

    fprintf(stderr, "responding for %s\n", inbuf);

    // prepare output - inbuf is 0-truncated to be the user
    snprintf(outbuf, 1536, "%s did it\r\n", inbuf);
    len = strlen(outbuf);
    n = 0;
    while (n < len) {
        ret = write(fd, outbuf + n, len - n);
        if (ret < 0) {
            return -1;
        } else {
            n += ret;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *host = "::";
    char *port = "finger";
    char act_host[NI_MAXHOST];
    char act_port[NI_MAXSERV];
    int code;
    ssize_t result;
    size_t bytes_done, req_len;
    struct sockaddr *addr;
    ssize_t addr_len;

    if (argc > 1) {
        port = argv[1];
        if (argc > 2) {
            host = argv[2];
        }
    }

    addr_len = lookup_address(host, port, &addr);
    if (addr_len < 0) {
        fprintf(stderr, "%s:%s: %s\n", host, port, gai_strerror(addr_len));
        exit(EXIT_FAILURE);
    }

    getnameinfo(addr, addr_len, act_host, NI_MAXHOST, act_port, NI_MAXSERV,
                NI_NUMERICHOST | NI_NUMERICSERV);
    fprintf(stderr, "listening on %s:%s\n", act_host, act_port);

    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    code = bind(sock, addr, addr_len);
    if (code < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    code = listen(sock, 5);
    if (code < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        fprintf(stderr, "waiting for connection\n");
        struct sockaddr_in6 c_addr;
        socklen_t c_addr_len = sizeof(struct sockaddr_in6);
        int client = accept(sock, (void*) &c_addr, &c_addr_len);
        if (client < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pid_t child = fork();
        if (child < 0) {
            perror("fork");
            exit(EX_OSERR);
        } else if (child) {
            // parent process
            close(client);
        } else {
            // child process
            close(sock);
            getnameinfo((struct sockaddr*) &c_addr, c_addr_len,
                        act_host, NI_MAXHOST, act_port, NI_MAXSERV,
                        NI_NUMERICHOST | NI_NUMERICSERV);
            fprintf(stderr, "handling connection from %s:%s in process %d\n",
                    act_host, act_port, getpid());

            code = handle_request(client);
            if (code < 0) {
                perror("handle_request");
            }
            close(client);
            exit(0);
        }
    }

    close(sock);
    free(addr);

    return 0;
}
