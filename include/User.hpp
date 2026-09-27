#pragma once
#include <string>

struct User {
    int id = 0;
    std::string username;
    std::string password;   // plain for simplicity (hash in production)
    std::string phone;
    std::string createdAt;
};
