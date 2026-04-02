#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "encryption.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::string message;
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    std::string fullMessage = "rohat:1234:" + message;
    std::string encryptedMsg = encrypt(fullMessage); 

    send(sock, encryptedMsg.c_str(), encryptedMsg.size(), 0);
    close(sock);
    return 0;
}