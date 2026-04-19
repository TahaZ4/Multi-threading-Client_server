#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "encryption.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"

using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Connection Failed. Is the server running?" << endl;
        return 1;
    }

    // 1. DYNAMIC LOGIN - No more recompiling!
    string user, pass, command;
    cout << "--- SECURE LOGIN ---" << endl;
    cout << "Username: "; cin >> user;
    cout << "Password: "; cin >> pass;
    
    cout << "\n--- COMMAND MENU ---" << endl;
    cout << "Enter command (ls, read, copy, edit, ftp): ";
    cin >> command;

    // 2. Format the packet
    string fullPacket = user + ":" + pass + ":" + command;

    // 3. Encrypt and Send
    string encryptedMsg = encrypt(fullPacket); 
    send(sock, encryptedMsg.c_str(), encryptedMsg.size(), 0);

    cout << "\n[CLIENT] Encrypted packet sent to server." << endl;

    close(sock);
    return 0;
}
