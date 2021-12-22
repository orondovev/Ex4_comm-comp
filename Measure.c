#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define TRUE 1

#define PORT 8080


// get the 1mb.txt file

void read_file(int socket) {
    char buff_file[256];
    int f;

    while (TRUE) {
        f = recv(socket, buff_file, 256, 0);
        if (f <= 0) {
            break;
        }
        bzero(buff_file, 256); //clearing the buffer
    }
    return;
}


int main(int argc, char **argv) {
    char buf[256];
    struct timeval start, end;
    long double reno_time = 0, cubic_time = 0;
    long double curr_time1 = 0, curr_time2 = 0;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int new_sock;
    socklen_t len;


    //open new socket.
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }


    len = strlen(buf);
    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    }



    //bind socket to port
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

    printf("Waiting for Connections..\n");

    socklen_t addr_len = sizeof(address);

    //ready to receive first 5 packets.
    for (int i = 0; i < 5; i++) {
        memset(&address, 0, sizeof(address));
        if ((new_sock = accept(sock, (struct sockaddr *) &address, (socklen_t *) &addr_len) < 0)) {
            perror("got error from accept");
            exit(EXIT_FAILURE);
        }

        len = sizeof(buf);
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
            perror("got error from getsockopt");
            return -1;
        }
        printf("Current cc: %s\n", buf);//todo change

        gettimeofday(&start, 0);
        read_file(new_sock);
        gettimeofday(&end, 0);
        double seconds = end.tv_sec - start.tv_sec;
        double mic_sec = end.tv_usec - start.tv_usec;
        curr_time1 += (double)(seconds + mic_sec * 1e-6);// most accurate
    }
    cubic_time = (curr_time1 / 5.0);
    printf("cubic average time: %lf\n", cubic_time);

    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("error in setsockopt");
        return -1;
    }
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    printf("New cc: %s\n", buf);

    //ready to receive second 5 packets.
    for (int i = 0; i < 5; i++) {
        memset(&address, 0, sizeof(address));
        if ((new_sock = accept(sock, (struct sockaddr *) &address, (socklen_t *) &addr_len) < 0)) {
            perror("got error from accept");
            exit(EXIT_FAILURE);
        }

        len = sizeof(buf);
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
            perror("got error from getsockopt");
            return -1;
        }
        printf("Current cc: %s\n", buf);

        gettimeofday(&start, 0);
        read_file(new_sock);
        gettimeofday(&end, 0);
        double seconds = end.tv_sec - start.tv_sec;
        double mic_sec = end.tv_usec - start.tv_usec;
        curr_time2 += seconds + mic_sec * 1e-6;// most accurate

    }
    reno_time = (curr_time2 / 5.0);

    printf("reno average time: %lf\n", reno_time);
    close(sock);
    return 0;
}