#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <sysexits.h>

int main(int argc, char *argv[])
{
    struct addrinfo *addresses;
    struct addrinfo hints = {
        0, AF_INET, SOCK_STREAM, 0
    };
    int code;

    if (argc < 3) {
        fprintf(stderr, "usage: netcat host port\n");
        exit(EX_USAGE);
    }

    code = getaddrinfo(argv[1], argv[2], &hints, &addresses);
    if (code) {
        fprintf(stderr, "%s:%s: %s\n", argv[1], argv[2], gai_strerror(code));
        exit(2);
    }

    struct addrinfo *cur = addresses;
    while (cur) {
        printf("canonical name: %s\n", cur->ai_canonname);
        printf("address length: %d\n", cur->ai_addrlen);
        struct sockaddr_in *addr = (struct sockaddr_in*) cur->ai_addr;
        printf("address: %x\n", addr->sin_addr.s_addr);
        cur = cur->ai_next;
    }

    freeaddrinfo(addresses);

    return 0;
}
