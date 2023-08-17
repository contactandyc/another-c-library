#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *do_main( void *arg ) {
    int client_socket;
    struct sockaddr_in server_address;
    char server_ip[] = "127.0.0.1";  // Replace with the server IP
    int server_port = 1234;            // Replace with the server port

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send three HTTP GET requests
    char request_template[] = "GET /

    \}+HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n";
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int i;
    for (i = 0; i < 100000; i++) {
        sprintf(request, request_template, server_ip);
        if (send(client_socket, request, strlen(request), 0) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

        // Receive response from the server
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';

        // printf("Response %d:\n%s\n", i + 1, buffer);

        // Delay between requests (optional)
        // sleep(1);
    }
    printf( "%s\n", buffer );
    // Close the socket
    close(client_socket);
    return NULL;
}


int main( int argc, char *argv[] ) {
    int num_threads = 20;
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    for( int i=0; i<num_threads; i++ )
        pthread_create(threads+i, NULL, do_main, NULL);
    for( int i=0; i<num_threads; i++ )
        pthread_join(threads[i], NULL);
    free(threads);
    return 0;
}