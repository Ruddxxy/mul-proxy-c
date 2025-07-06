#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>


#define PORT 8080
#define BUFFER_SIZE 4096

char *parse_host(char *request);
int forward_request(char *hostname, char *client_request, SOCKET client_socket);

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    int total_requests = 0; // Initialize total requests count

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set SO_REUSEADDR
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        printf("setsockopt failed.\n");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("[INFO] Listening on port %d\n", PORT);

    int running = 1;
    while (running) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }

        printf("[INFO] Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));  // Log the client's IP address and port
        total_requests++; // Increment the total requests count
        printf("[INFO] Total requests handled: %d\n", total_requests); // Log the total requests count

        printf("[INFO] Accepted connection\n");

        int bytes = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("[RECV]\n%s\n", buffer);

            // Limit the request size to prevent buffer overflow
            if (bytes >= BUFFER_SIZE -1) {
                printf("[SECURITY] Request too large.Closing Connection.\n");
                closesocket(new_socket);
                continue;
            }
            //parse the host header
            char *host = parse_host(buffer);
            if (host) {
                if (strstr(host, "localhost") || strstr(host, "127.0.1") || strstr(host, "192.168") || strstr(host, "10.")) {
                    printf("[SECURITY] Request to localhost or private IP detected. Closing Connection.\n");
                    closesocket(new_socket);
                    continue;
                }
                printf("[INFO]Fowarding to host: %s\n", host);
                forward_request(host,buffer, new_socket);
                printf("[INFO] Total requests handled: %d\n", total_requests); // Log the total requests count
                continue;
            } else {
                printf("[ERROR] Could not parse host Header.\n");
            }
        }
        closesocket(new_socket); // Properly close the socket after handling the request
        // Optionally, add a condition to break the loop, e.g., on a special request or signal
        // if (some_exit_condition) running = 0;
    }

    // Cleanup
    closesocket(server_fd);
    WSACleanup();
    return 0;
}

char *parse_host(char *request){
    static char hostname[256];
    char *host_line = strstr(request, "Host:");
    if(host_line){
        sscanf(host_line, "Host: %255s", hostname);
        return hostname;
    }
    return NULL;
    }
// This function forwards the request to the specified hostname and sends the response back to the client.
int forward_request(char *hostname, char *client_request, SOCKET client_socket) {
    SOCKET server_socket;
    struct sockaddr_in server_addr;
    struct hostent *he;
send(server_socket, client_request, strlen(client_request), 0);

    if((he = gethostbyname(hostname)) == NULL) {  // It uses the `gethostbyname` function to resolve the hostname and establishes a connection to the server.
        printf("gethostbyname failed for the host: %s\n", hostname);
        return -1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);

    if(connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Connection to server failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        return -1;
    }

send(server_socket, client_request, strlen(client_request), 0);

char buffer[BUFFER_SIZE];
int bytes;
while ((bytes = recv(server_socket, buffer, BUFFER_SIZE, 0)) > 0){
    send(client_socket, buffer, bytes, 0);
}

closesocket(server_socket);
memset(buffer, 0, BUFFER_SIZE); //zeroing out the buffer
printf("[INFO] Request forwarded to %s\n", hostname);
return 0;
}   

// This code implements a simple TCP proxy server in C using Winsock on Windows.
// It listens on a specified port, accepts incoming connections, and receives data from clients.
// The function returns 0 on success and -1 on failure.