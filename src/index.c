
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void inspect_address(const struct addrinfo *result)
{
    char ip_string[INET6_ADDRSTRLEN];

    if (result->ai_addr == NULL) {
        printf("No address available.\n");
        return;
    }

    struct sockaddr *address = result->ai_addr;

    printf("\n------------------------------\n");

    if (address->sa_family == AF_INET) {
        struct sockaddr_in *ipv4 =
            (struct sockaddr_in *)address;

        if (inet_ntop(AF_INET, &ipv4->sin_addr,
                      ip_string, sizeof(ip_string)) == NULL) {
            perror("inet_ntop");
            return;
        }

        printf("Address family: IPv4\n");
        printf("IP address: %s\n", ip_string);
        printf("Port: %u\n",
               (unsigned int)ntohs(ipv4->sin_port));

        printf("Raw IPv4 value: 0x%08x\n",
               (unsigned int)ntohl(ipv4->sin_addr.s_addr));

    } else if (address->sa_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 =
            (struct sockaddr_in6 *)address;

        if (inet_ntop(AF_INET6, &ipv6->sin6_addr,
                      ip_string, sizeof(ip_string)) == NULL) {
            perror("inet_ntop");
            return;
        }

        printf("Address family: IPv6\n");
        printf("IP address: %s\n", ip_string);
        printf("Port: %u\n",
               (unsigned int)ntohs(ipv6->sin6_port));
        printf("IPv6 address size: %zu bytes\n",
               sizeof(ipv6->sin6_addr));

    } else {
        printf("Unknown address family: %d\n",
               address->sa_family);
        return;
    }

    printf("Socket type: ");

    if (result->ai_socktype == SOCK_STREAM) {
        printf("SOCK_STREAM (TCP)\n");
    } else if (result->ai_socktype == SOCK_DGRAM) {
        printf("SOCK_DGRAM (UDP)\n");
    } else {
        printf("%d\n", result->ai_socktype);
    }

    printf("Protocol: %d\n", result->ai_protocol);
    printf("Address structure size: %u bytes\n",
           (unsigned int)result->ai_addrlen);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <hostname-or-IP> <port>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8080\n", argv[0]);
        printf("Example: %s example.com 80\n", argv[0]);
        return 1;
    }

    char *end;
    long port_number = strtol(argv[2], &end, 10);

    if (*argv[2] == '\0' || *end != '\0' ||
        port_number < 1 || port_number > 65535) {
        fprintf(stderr, "Invalid port number.\n");
        return 1;
    }

    struct addrinfo hints = {0};
    struct addrinfo *results = NULL;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(
        argv[1],
        argv[2],
        &hints,
        &results
    );

    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n",
                gai_strerror(status));
        return 1;
    }

    printf("Requested host: %s\n", argv[1]);
    printf("Requested port: %ld\n", port_number);

    int count = 0;

    for (struct addrinfo *current = results;
         current != NULL;
         current = current->ai_next) {

        inspect_address(current);
        count++;
    }

    printf("\nTotal address results: %d\n", count);

    freeaddrinfo(results);

    return 0;
}
