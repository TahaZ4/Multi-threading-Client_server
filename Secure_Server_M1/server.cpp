#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h> 
#include "encryption.h"

#define PORT 8080

// Function that runs in a separate thread for each client 
void* handle_client(void* arg) {
    // Get the socket ID and then free the memory
    int clientSocket = *((int*)arg);
    delete (int*)arg; 

    char buffer[1024];
    memset(buffer, 0, 1024);

    // Read the encrypted data from the client 
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (bytesReceived > 0) {
        // Turn the scrambled data back into plain text 
        std::string decryptedMsg = decrypt(std::string(buffer, bytesReceived)); 
        std::string username = "taha", password = "1234";

        
        size_t first = decryptedMsg.find(':');
        size_t second = decryptedMsg.find(':', first + 1);

        if (first != std::string::npos && second != std::string::npos) {
            std::string rUser = decryptedMsg.substr(0, first);
            std::string rPass = decryptedMsg.substr(first + 1, second - first - 1);
            
            // Verify the employee credentials 
            if (rUser == username && rPass == password) {
                std::cout << "Success: Client authorized." << std::endl;
                std::cout << "Message: " << decryptedMsg.substr(second + 1) << std::endl;
            } else {
                std::cout << "Error: Wrong username or password." << std::endl;
            }
        }
    }

    // Close the connection for this specific client
    close(clientSocket);
    return nullptr;
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create the TCP socket 
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Help the port clear faster when restarting the server
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Port is busy. Try again in a moment." << std::endl;
        return 1;
    }

    // Set the server to listen for clients
    listen(serverSocket, 10);
    std::cout << "Server is running on port " << PORT << ". Waiting for clients..." << std::endl;

    while (true) {
        // Use 'new' to give each client its own memory space for the socket
        int* clientSocket = new int;
        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (*clientSocket >= 0) {
            pthread_t threadId;
            // Create a thread so multiple clients can connect at once 
            if (pthread_create(&threadId, nullptr, handle_client, (void*)clientSocket) != 0) {
                std::cerr << "Could not start thread." << std::endl;
                delete clientSocket;
            } else {
                // Let the thread finish and clean up on its own
                pthread_detach(threadId);
            }
        }
    }

    close(serverSocket);
    return 0;
}