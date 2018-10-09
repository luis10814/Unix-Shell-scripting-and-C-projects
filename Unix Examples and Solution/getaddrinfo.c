#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

int main(int argc, char *argv[])
{
    struct addrinfo *addresses;
    struct addrinfo hints = {
        0, AF_INET, SOCK_STREAM, 0
    };
    int code;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

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
        code = getnameinfo(cur->ai_addr, cur->ai_addrlen,
                           host, NI_MAXHOST, service, NI_MAXSERV,
                           NI_NUMERICHOST | NI_NUMERICSERV);
        if (code < 0) {
            perror("getnameinfo");
            exit(2);
        }
        printf("address: %s:%s\n", host, service);
        code = getnameinfo(cur->ai_addr, cur->ai_addrlen,
                           host, NI_MAXHOST, service, NI_MAXSERV,
                           0);
        if (code < 0) {
            perror("getnameinfo");
            exit(2);
        }
        printf("reverse lookup: %s:%s\n", host, service);
        cur = cur->ai_next;
    }

    freeaddrinfo(addresses);

    return 0;
}
