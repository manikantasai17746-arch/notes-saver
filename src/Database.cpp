#include "Database.hpp"
#include <algorithm>
#include <ctime>
#include <iomanip>

Database::Database(const std::string& dataDir) : dataDir(dataDir) {
    usersFile = dataDir + "/users.txt";
    notesFile = dataDir + "/notes.txt";
    // create data directory if needed
    system(("mkdir -p " + dataDir).c_str());
    loadUsers();
    loadNotes();
}

std::string Database::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void Database::loadUsers() {
    users.clear();
    std::ifstream file(usersFile);
    if (!file) return;

    std::string line;
    User u;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line == "---") {
            if (!u.username.empty()) {
                users.push_back(u);
                if (u.id >= nextUserId) nextUserId = u.id + 1;
            }
            u = User{};
            continue;
        }
        if (line.rfind("ID:", 0) == 0) u.id = std::stoi(trim(line.substr(3)));
        else if (line.rfind("USERNAME:", 0) == 0) u.username = trim(line.substr(9));
        else if (line.rfind("PASSWORD:", 0) == 0) u.password = trim(line.substr(9));
        else if (line.rfind("PHONE:", 0) == 0) u.phone = trim(line.substr(6));
        else if (line.rfind("CREATED:", 0) == 0) u.createdAt = trim(line.substr(8));
    }
    if (!u.username.empty()) {
        users.push_back(u);
        if (u.id >= nextUserId) nextUserId = u.id + 1;
    }
}

void Database::writeUsers() {
    std::ofstream file(usersFile);
    for (const auto& u : users) {
        file << "ID: " << u.id << "\n";
        file << "USERNAME: " << u.username << "\n";
        file << "PASSWORD: " << u.password << "\n";
        file << "PHONE: " << u.phone << "\n";
        file << "CREATED: " << u.createdAt << "\n";
        file << "---\n";
    }
}

void Database::loadNotes() {
    notes.clear();
    std::ifstream file(notesFile);
    if (!file) return;

    std::string line;
    Note n;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line == "---") {
            if (!n.title.empty()) {
                notes.push_back(n);
                if (n.id >= nextNoteId) nextNoteId = n.id + 1;
            }
            n = Note{};
            continue;
        }
        if (line.rfind("ID:", 0) == 0) n.id = std::stoi(trim(line.substr(3)));
        else if (line.rfind("USER_ID:", 0) == 0) n.userId = std::stoi(trim(line.substr(8)));
        else if (line.rfind("TITLE:", 0) == 0) n.title = trim(line.substr(6));
        else if (line.rfind("CONTENT:", 0) == 0) n.content = trim(line.substr(8));
        else if (line.rfind("IMPORTANT:", 0) == 0) n.isImportant = (trim(line.substr(10)) == "1");
        else if (line.rfind("NOTE_PASS:", 0) == 0) n.notePassword = trim(line.substr(10));
        else if (line.rfind("CREATED:", 0) == 0) n.createdAt = trim(line.substr(8));
    }
    if (!n.title.empty()) {
        notes.push_back(n);
        if (n.id >= nextNoteId) nextNoteId = n.id + 1;
    }
}

void Database::writeNotes() {
    std::ofstream file(notesFile);
    for (const auto& n : notes) {
        file << "ID: " << n.id << "\n";
        file << "USER_ID: " << n.userId << "\n";
        file << "TITLE: " << n.title << "\n";
        file << "CONTENT: " << n.content << "\n";
        file << "IMPORTANT: " << (n.isImportant ? "1" : "0") << "\n";
        file << "NOTE_PASS: " << n.notePassword << "\n";
        file << "CREATED: " << n.createdAt << "\n";
        file << "---\n";
    }
}

bool Database::saveUser(const User& user) {
    User u = user;
    if (u.id == 0) u.id = nextUserId++;

    users.push_back(u);
    writeUsers();
    return true;
}

bool Database::updateUserPassword(const std::string& username, const std::string& newPassword) {
    for (auto& u : users) {
        if (u.username == username) {
            u.password = newPassword;
            writeUsers();
            return true;

            
        }
    }
    return false;
}

User* Database::findUserByUsername(const std::string& username) {
    for (auto& u : users) {
        if (u.username == username) return &u;
    }
    return nullptr;
}

User* Database::findUserByPhone(const std::string& phone) {
    for (auto& u : users) {
        if (u.phone == phone) return &u;
    }
    return nullptr;
}

std::vector<User> Database::getAllUsers() {
    return users;
}

bool Database::saveNote(const Note& note) {
    Note n = note;
    if (n.id == 0) n.id = nextNoteId++;
    notes.push_back(n);
    writeNotes();
    return true;
}

std::vector<Note> Database::getNotesByUserId(int userId) {
    std::vector<Note> result;
    for (const auto& n : notes) {
        if (n.userId == userId) result.push_back(n);
    }
    return result;
}

Note* Database::findNoteById(int noteId) {
    for (auto& n : notes) {
        if (n.id == noteId) return &n;
    }
    return nullptr;
}

bool Database::deleteNote(int noteId) {
    auto it = std::remove_if(notes.begin(), notes.end(),
        [noteId](const Note& n) { return n.id == noteId; });
    if (it == notes.end()) return false;
    notes.erase(it, notes.end());
    writeNotes();
    return true;
}
