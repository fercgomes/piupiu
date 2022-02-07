#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 5000
#define MAXLINE 1024

// Driver code
int main()
{
    const int          port   = 5000;
    const char*        ipaddr = "127.0.0.1";
    int                sockfd;
    struct sockaddr_in si_me, si_other;
    char               buffer[1024];
    socklen_t          addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&si_me, '\0', sizeof(si_me));
    si_me.sin_family      = AF_INET;
    si_me.sin_port        = htons(port);
    si_me.sin_addr.s_addr = inet_addr(ipaddr);

    bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me));
    addr_size = sizeof(si_other);

    std::cout << "Running server..." << std::endl;

    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&si_other, &addr_size);
    printf("[+]Data Received: %s\n", buffer);

    return 0;
}