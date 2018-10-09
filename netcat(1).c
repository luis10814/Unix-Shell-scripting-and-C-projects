#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

ssize_t lookup_address(const char *host, const char *port, struct sockaddr **addr)
{
    struct addrinfo hints = {
        0, AF_INET, SOCK_DGRAM, 0
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

int copy_data(int from, int to)
{
    char response[1024];
    ssize_t result = 0;
    result = read(from, response, 1024);
    if (result < 0) {
        if (errno == EAGAIN) {
            return 0;
        } else {
            return -1;
        }
    } else if (result == 0) {
        return -2;
    }

    size_t to_print = result;
    size_t bdone = 0;
    while (bdone < to_print) {
        result = write(to, response + bdone, to_print - bdone);
        if (result < 0) {
            return -1;
        }
        bdone += result;
    }

    return result;
}

int main(int argc, char *argv[])
{
    char *host = "localhost";
    char *port = NULL;
    int code;
    ssize_t result;
    size_t bytes_done, req_len;
    struct sockaddr *addr;
    ssize_t addr_len;
    
    if (argc < 3) {
        fprintf(stderr, "usage: netcat host port\n");
        exit(EX_USAGE);
    }
    host = argv[1];
    port = argv[2];

    addr_len = lookup_address(host, port, &addr);
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
    code = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (code < 0) {
        perror("fcntl(socket)");
        exit(EXIT_FAILURE);
    }
    code = fcntl(0, F_SETFL, O_NONBLOCK);
    if (code < 0) {
        perror("fcntl(stdin)");
        exit(EXIT_FAILURE);
    }

    bool send_done = false;
    bool recv_done = false;
    while (!send_done || !recv_done) {
        if (!send_done) {
            code = copy_data(0, sock);
            if (code == -2) {
                send_done = true;
            } else if (code < 0) {
                perror("send_data");
                exit(EXIT_FAILURE);
            }
        }
        if (!recv_done) {
            code = copy_data(sock, 1);
            if (code == -2) {
                recv_done = true;
            } else if (code < 0) {
                perror("recv_data");
                exit(EXIT_FAILURE);
            }
        }
    }

    close(sock);
    free(addr);

    return 0;
}
