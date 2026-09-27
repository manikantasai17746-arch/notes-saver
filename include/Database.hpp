#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "User.hpp"
#include "Note.hpp"

class Database {
public:
    explicit Database(const std::string& dataDir = "data");

    // Users
    bool saveUser(const User& user);
    bool updateUserPassword(const std::string& username, const std::string& newPassword);
    User* findUserByUsername(const std::string& username);
    User* findUserByPhone(const std::string& phone);
    std::vector<User> getAllUsers();

    // Notes
    bool saveNote(const Note& note);
    std::vector<Note> getNotesByUserId(int userId);
    Note* findNoteById(int noteId);
    bool deleteNote(int noteId);

private:
    std::string dataDir;
    std::string usersFile;
    std::string notesFile;
    std::vector<User> users;
    std::vector<Note> notes;
    int nextUserId = 1;
    int nextNoteId = 1;

    void loadUsers();
    void loadNotes();
    void writeUsers();
    void writeNotes();
    std::string trim(const std::string& s);
};
