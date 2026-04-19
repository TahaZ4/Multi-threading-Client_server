#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm> // Required for cleaning strings
#include "encryption.h"

#define PORT 8080
using namespace std;

struct User {
    string username;
    string password;
    int level; 
};

vector<User> userDB = {
    {"staff_entry", "pass123", 1},
    {"staff_med",   "pass456", 2},
    {"boss_top",    "admin99", 3}
};

bool check_access(int level, string cmd) {
    if (level == 3) return true;
    if (level == 2 && (cmd == "ls" || cmd == "read" || cmd == "copy" || cmd == "edit")) return true;
    if (level == 1 && (cmd == "ls" || cmd == "read")) return true;
    return false;
}

// Function to clean hidden characters from strings
string clean(string s) {
    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    return s;
}

void* handle_client(void* arg) {
    int clientSocket = *((int*)arg);
    delete (int*)arg; 

    char buffer[1024];
    memset(buffer, 0, 1024);

    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    
    if (bytesReceived > 0) {
        string decryptedMsg = decrypt(string(buffer, bytesReceived)); 
        cout << "\n[SERVER] New Packet: " << decryptedMsg << endl;

        stringstream ss(decryptedMsg);
        string u, p, cmd;
        getline(ss, u, ':');
        getline(ss, p, ':');
        getline(ss, cmd, ':');

        // CLEAN the strings to ensure match
        u = clean(u); p = clean(p); cmd = clean(cmd);

        int userLevel = 0;
        bool authSuccess = false;

        for (const auto& user : userDB) {
            if (user.username == u && user.password == p) {
                authSuccess = true;
                userLevel = user.level;
                break;
            }
        }

        if (authSuccess) {
            if (check_access(userLevel, cmd)) {
                string res = "SUCCESS";
                send(clientSocket, res.c_str(), res.length(), 0);
                cout << ">>> ACCESS GRANTED: " << u << " (Level " << userLevel << ")" << endl;
            } else {
                string res = "DENIED";
                send(clientSocket, res.c_str(), res.length(), 0);
                cout << ">>> PERMISSION DENIED: " << u << " (Level " << userLevel << ") attempted " << cmd << endl;
            }
        } else {
            send(clientSocket, "AUTH_FAILED", 11, 0);
            cout << ">>> AUTHENTICATION FAILED for input: [" << u << ":" << p << "]" << endl;
        }
    }
    close(clientSocket);
    return nullptr;
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        return 1;
    }
    listen(serverSocket, 10);
    cout << "SERVER READY FOR TESTING..." << endl;

    while (true) {
        int* clientSocketPtr = new int;
        *clientSocketPtr = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (*clientSocketPtr >= 0) {
            pthread_t threadId;
            pthread_create(&threadId, nullptr, handle_client, clientSocketPtr);
            pthread_detach(threadId); 
        }
    }
    return 0;
}
