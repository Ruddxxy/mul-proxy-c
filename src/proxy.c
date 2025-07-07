#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h> //for threading
#include <stdint.h>


unsigned __stdcall client_handler(void *socket_desc);
//char *parse_host(char *request, char *hostname, int size);


#define PORT 8080
#define BUFFER_SIZE 4096

int total_requests = 0; // Global variable to keep track of total requests
int blocked_requests = 0; // Global variable to keep track of blocked requests
int active_threads = 0; // Global variable to keep track of active threads


int main() {
    WSADATA wsa;
    SOCKET server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

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
        SOCKET *new_sock = malloc(sizeof(SOCKET));
        *new_sock = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (*new_sock == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            free(new_sock);
            continue;
        }

        printf("[INFO] Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));  // Log the client's IP address and port
        
        uintptr_t thread_id = _beginthreadex(NULL, 0, client_handler, (void*)new_sock, 0, NULL);
        if (thread_id == 0) {
            printf("[ERROR] Failed to create thread for client handler. Error Code: %lu\n", GetLastError()); //unsigned long for GetLastError
            closesocket(*new_sock);
            free(new_sock);
        } else {
            CloseHandle((HANDLE)thread_id); // Close the thread handle, we don't need it
        }

       
               
        }
         
            closesocket(server_fd);
             WSACleanup();
            return 0; 
        // Optionally, add a condition to break the loop, e.g., on a special request or signal
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
printf("[INFO] Request forwarded to %s\n", hostname);
return 0;
}   

unsigned __stdcall client_handler(void *socket_desc){
    SOCKET new_socket = *(SOCKET*)socket_desc;
    free(socket_desc); // Free the allocated memory for the socket descriptor
    active_threads++; // Increment the active threads count
    printf("[INFO] New Thread Started.Active threads: %d\n", active_threads);
    char buffer[BUFFER_SIZE];
    int bytes;

    bytes = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[RECV]\n%s\n", buffer);

        total_requests++;
        printf("[INFO] Total requests handled: %d | Blocked: %d | Active Threads: %d\n", total_requests, blocked_requests, active_threads);

        if(strncmp(buffer, "CONNECT", 7) == 0){
            printf("[INFO] CONNECT request received. This is not supported in this proxy.\n");
            closesocket(new_socket);
            return 0; // Exit the thread
        } else if (strncmp(buffer, "GET", 3) ==0){
            printf("[INFO] GET request received.\n");
        }else if (strncmp(buffer, "POST", 4) == 0) {
            printf("[INFO] POST request received.\n");
        } else {
            printf("[INFO] Other request type received.\n");
        }

        // Limit the request size to prevent buffer overflow
        if (bytes >= BUFFER_SIZE -1) {
            printf("[SECURITY] Request too large. Closing Connection.\n");
            blocked_requests++;
            printf("[INFO] Total requests handled: %d | Blocked: %d | Active Threads: %d\n", total_requests, blocked_requests, active_threads);
            closesocket(new_socket);
            active_threads--;
            printf("[INFO] Active threads: %d\n", active_threads);
            return 0; // Exit the thread
        }

        // Parse the host header
        char *host = parse_host(buffer);
        if (host) {
            if (strstr(host, "localhost") || strstr(host, "127.0.1") || strstr(host, "192.168") || strstr(host, "10.")) {
                printf("[SECURITY] Request to localhost or private IP detected. Closing Connection.\n");
                blocked_requests++;
                printf("[INFO] Total requests handled: %d | Blocked: %d | Active Threads: %d\n", ++total_requests, blocked_requests, active_threads);
                closesocket(new_socket);
                active_threads--;
                printf("[INFO] Active threads: %d\n", active_threads);
                return 0; // Exit the thread
            }
            printf("[INFO] Forwarding to host: %s\n", host);
            forward_request(host, buffer, new_socket);
        } else {
            printf("[ERROR] Could not parse host Header.\n");
        }
        total_requests++;
    }
    closesocket(new_socket); // Properly close the socket after handling the request
    active_threads--; // Decrement the active threads count
    printf("[INFO] Active threads: %d\n", active_threads);
    return 0; // Exit the thread
}
// Note: The code is designed to handle basic HTTP requests and does not support HTTPS or advanced features.
// It is a simple proxy server that can be extended with more features as needed.