#include "encryption.h"

std::string encrypt(std::string data) {
    for (int i = 0; i < data.length(); i++) {
        data[i] = data[i] + 3; // Shift ASCII up by 3
    }
    return data;
}

std::string decrypt(std::string data) {
    for (int i = 0; i < data.length(); i++) {
        data[i] = data[i] - 3; // Shift ASCII down by 3
    }
    return data;
}