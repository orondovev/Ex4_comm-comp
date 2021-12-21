#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0

// get the 1mb.txt file

void get_file(int socket) {
    char buff_file[1024];
    long mb = 0;
    int f;

    while (TRUE) {
        f = recv(socket, buff_file, 1024, 0);
        if (f <= 0) {
            break;
        }
        mb += f;
        bzero(buff_file, 1024);
    }
    return;
}


int main(int argc, char **argv) {
    char buf[256];
    struct timeval start, end;
    long long seconds, mil_sec;
    double reno_time = 0, cubic_time = 0, time_keeper = 0;
    struct sockaddr_in address;
    int new_sock;
    socklen_t len;


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);


    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sock, 10) == 0) {
        printf("Listening\n ");
    } else {
        perror("Listening failed\n ");
        exit(EXIT_FAILURE);
    }

    unsigned long addr_len = sizeof(address);
    for (int i = 0; i < 5; i++) {
        if ((new_sock = accept(sock, (struct sockaddr *) &address, (socklen_t *) &addr_len) < 0)) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        len = sizeof(buf);
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
            perror("getsockopt");
            return -1;
        }
        printf("Current cc is: %s\n", buf);

        gettimeofday(&start, 0);
        get_file(new_sock);
        gettimeofday(&end, 0);
        seconds = end.tv_sec - start.tv_sec;
        mil_sec = end.tv_usec - start.tv_usec;
        time_keeper += (seconds + mil_sec * 1e-6);// most accurate

    }

    cubic_time = (time_keeper / 5);

    time_keeper = 0;

    printf("Current: %s\n", buf);

    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("setsockopt");
        return -1;
    }
    len = sizeof(buf);
    if (
            getsockopt(sock, IPPROTO_TCP,
                       TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    printf("New: %s\n", buf);
    close(sock);
    return 0;
}