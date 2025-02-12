#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

// Function to capitalize each word in a string
void capitalizeWords(char *str) {
    int capitalize = 1; // Flag to determine if the next character should be capitalized
    for (int i = 0; str[i] != '\0'; i++) {
        if (isspace(str[i])) {
            capitalize = 1; // Set the flag when a space is encountered
        } else if (capitalize && isalpha(str[i])) {
            str[i] = toupper(str[i]); // Capitalize the character
            capitalize = 0; // Reset the flag
        }
    }
}

int main() {
    int server, newSock;
    char buffer[1024];
    struct sockaddr_in servAddr;
    struct sockaddr_storage store;
    socklen_t addrSize;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("Socket failed");
        return -1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(6265);
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(server, 5) == 0) {
        printf("Server is listening...\n");
    } else {
        perror("Listen failed");
        return -1;
    }

    while (1) {
        // Accepting new client connection
        addrSize = sizeof(store);
        newSock = accept(server, (struct sockaddr *)&store, &addrSize);
        if (newSock < 0) {
            perror("Accept failed");
            continue; // Skip to the next iteration if accepting fails
        }

        printf("Client connected.\n");

        while (1) {
            // Clear buffer before receiving data
            memset(buffer, 0, sizeof(buffer));

            // Receive data from the client
            int valread = recv(newSock, buffer, sizeof(buffer) - 1, 0);
            if (valread > 0) {
                buffer[valread] = '\0'; // Null-terminate the string
                printf("Received from client: %s\n", buffer);

                // If client sends 'exit', close the connection
                if (strcmp(buffer, "exit") == 0) {
                    printf("Client exited. Closing connection...\n");
                    break;
                }

                // Capitalize the words in the received message
                capitalizeWords(buffer);
                printf("Capitalized message: %s\n", buffer);

                // Send the capitalized message back to the client
                send(newSock, buffer, strlen(buffer), 0);
            } else if (valread == 0) {
                printf("Client disconnected.\n");
                break;
            } else {
                perror("recv failed");
                break;
            }
        }

        printf("Waiting for new client...\n");
        close(newSock); // Close client socket
    }
    close(server); // Close the server socket
    return 0;
}
