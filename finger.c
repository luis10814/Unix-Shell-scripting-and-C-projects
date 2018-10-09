#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

ssize_t lookup_address(char *host, struct sockaddr **addr)
{
    struct addrinfo hints = {
        0, AF_INET, SOCK_DGRAM, 0
    };
    struct addrinfo *ai;
    ssize_t len;
    int code;
    code = getaddrinfo(host, "finger", &hints, &ai);
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

int send_request(int sock, char *user)
{
    size_t req_len = user ? strlen(user) + 3 : 3;
    char *request = malloc(req_len);
    if (user) {
        strcpy(request, user);
        strcat(request, "\r\n");
    } else {
        strcpy(request, "\r\n");
    }

    fprintf(stderr, "> sending request for user %s\n", user);

    size_t bytes_done = 0;
    ssize_t result;
    while (bytes_done < req_len) {
        result = write(sock, request + bytes_done, req_len - bytes_done);
        if (result < 0) {
            free(request);
            return -1;
        }
        bytes_done += result;
    }
    free(request);
    return 0;
}

int fetch_reply(int sock)
{
    char response[1024];
    ssize_t result = 0;
    while ((result = read(sock, response, 1024)) > 0) {
        size_t to_print = result;
        size_t bdone = 0;
        while (bdone < to_print) {
            result = write(1, response + bdone, to_print - bdone);
            if (result < 0) {
                return -1;
            }
            bdone += result;
        }
    }
    if (result < 0) {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *host = "localhost";
    char *user = NULL;
    int code;
    ssize_t result;
    size_t bytes_done, req_len;
    struct sockaddr *addr;
    ssize_t addr_len;

    if (argc > 1) {
        user = argv[1];
        if (argc > 2) {
            host = argv[2];
        }
    }

    addr_len = lookup_address(host, &addr);
    if (addr_len < 0) {
        perror(host);
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    code = connect(sock, addr, addr_len);
    if (code < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    if (send_request(sock, user) < 0) {
        perror("send_request");
        exit(EXIT_FAILURE);
    }
    if (fetch_reply(sock) < 0) {
        perror("fetch_reply");
        exit(EXIT_FAILURE);
    }

    close(sock);
    free(addr);

    return 0;
}
