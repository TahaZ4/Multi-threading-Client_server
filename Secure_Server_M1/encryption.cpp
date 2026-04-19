#include "encryption.h"

// Client-side: Shift ASCII up by 3
std::string encrypt(std::string data) {
    for (int i = 0; i < data.length(); i++) {
        data[i] = data[i] + 3;
    }
    return data;
}

// Server-side: Shift ASCII down by 3
std::string decrypt(std::string data) {
    for (int i = 0; i < data.length(); i++) {
        data[i] = data[i] - 3;
    }
    return data;
}
