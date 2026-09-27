#pragma once
#include <string>

struct Note {
    int id = 0;
    int userId = 0;
    std::string title;
    std::string content;
    bool isImportant = false;
    std::string notePassword;   // only used when isImportant == true
    std::string createdAt;
};
