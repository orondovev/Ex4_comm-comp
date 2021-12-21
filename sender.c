#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 8080 //Port 8080 is typically used for a personally hosted web server.

int main(int argc, char **argv) {

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT); //cast as "host to network".

    char file_buf[1025] = "";

    char buf[256];
    socklen_t len;
    int sock = 0;

    FILE *fp;
    char *f_name = "1mb.txt";


    //init a new socket and open the connection with server -->
    sock = socket(AF_INET, SOCK_STREAM, 0);
    socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket error\n");
        return -1;
    }
    printf("socket has created.\n");

    //converting ip address to binary.
    int convert_ip_status = inet_pton(AF_INET, "10.0.0.27", &server_address.sin_addr);
    if (convert_ip_status <= 0) {
        printf("Invalid address \n");
        return -1;
    }


    //open and read from given file.
    fp = fopen(f_name, "r");
    if (fp == NULL) {
        perror("error reading the given file");
        exit(1);
    }

    //connecting to server.
    int con_status = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
    if (con_status < 0) {
        perror("Connection Failed\n");
        return -1;
    }
    printf("successfully connected to measure\n");

    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt\n");
        return -1;
    }

    printf("Current cc: %s\n", buf);
    //start the first deliveries.
    for (int i = 0; i < 10; ++i) {

        // the 5 last packets will send in "reno" cc;
        if (i >= 5) {
            strcpy(buf, "reno");
            len = strlen(buf);
            if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
                perror("setsockopt");
                return -1;
            }
            len = sizeof(buf);
            if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
                perror("getsockopt");
                return -1;
            }
            printf("New cc: %s\n", buf);
        }



        //send the file to server (measure).
        while (fgets(file_buf, 1025, fp) != NULL) {
            long send_status = send(sock, file_buf, sizeof(file_buf), 0);
            if (send_status == -1) {
                perror("Error reading the file data\n");
                exit(1);
            }

            //clear the file buffer.
            bzero(file_buf, 1025);
        }

        printf("finished to send file\n");
    }
    close(sock);


    return 0;
}

