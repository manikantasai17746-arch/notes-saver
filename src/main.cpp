#include <iostream>
#include <string>
#include <limits>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Database.hpp"
#include "User.hpp"
#include "Note.hpp"

using namespace std;

Database db("data");
User* currentUser = nullptr;

string getDateTime() {
    time_t now = time(0);
    tm* t = localtime(&now);
    stringstream ss;
    ss << setfill('0')
       << setw(2) << t->tm_mday << "-"
       << setw(2) << (t->tm_mon + 1) << "-"
       << (t->tm_year + 1900) << " "
       << setw(2) << t->tm_hour << ":"
       << setw(2) << t->tm_min << ":"


       
       << setw(2) << t->tm_sec;
    return ss.str();
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ==================== REGISTER ====================
void registerUser() {
    cout << "\n=================================\n";
    cout << "         CREATE ACCOUNT\n";
    cout << "=================================\n";

    string username, password, confirm, phone;
    cout << "Enter username: ";
    cin >> username;

    if (db.findUserByUsername(username)) {
        cout << "Username already exists!\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;
    cout << "Confirm password: ";
    cin >> confirm;
    if (password != confirm) {
        cout << "Passwords do not match!\n";
        return;
    }

    cout << "Enter phone number (10 digits): ";
    cin >> phone;
    if (phone.size() != 10) {
        cout << "Invalid phone number!\n";
        return;
    }

    User u;
    u.username = username;
    u.password = password;
    u.phone = phone;
    u.createdAt = getDateTime();
    db.saveUser(u);

    cout << "\nAccount created successfully! You can now login.\n";
}

// ==================== LOGIN ====================
bool login() {
    cout << "\n=================================\n";
    cout << "             LOGIN\n";
    cout << "=================================\n";

    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    User* u = db.findUserByUsername(username);
    if (!u || u->password != password) {
        cout << "Wrong username or password!\n";
        return false;
    }

    currentUser = u;
    cout << "\nWelcome, " << currentUser->username << "!\n";
    return true;
}

// ==================== FORGOT PASSWORD ====================
void forgotPassword() {
    cout << "\n=================================\n";
    cout << "        FORGOT PASSWORD\n";
    cout << "=================================\n";

    string phone;
    cout << "Enter registered phone number: ";
    cin >> phone;

    User* u = db.findUserByPhone(phone);
    if (!u) {
        cout << "No account found with this phone number.\n";
        return;
    }

    string newPass, confirm;
    cout << "Phone verified for user: " << u->username << "\n";
    cout << "Enter new password: ";
    cin >> newPass;
    cout << "Confirm new password: ";
    cin >> confirm;

    if (newPass != confirm) {
        cout << "Passwords do not match!\n";
        return;
    }

    db.updateUserPassword(u->username, newPass);
    cout << "Password updated successfully! You can now login.\n";
}

// ==================== ADD NOTE ====================
void addNote() {
    cout << "\n=================================\n";
    cout << "           ADD NOTE\n";
    cout << "=================================\n";

    clearInput();
    string title, content;
    cout << "Enter title: ";
    getline(cin, title);
    cout << "Enter note content:\n";
    getline(cin, content);

    cout << "\nIs this note IMPORTANT / LOCKED?\n";
    cout << "1. Yes (set password)\n";
    cout << "2. No (normal note)\n";
    cout << "Choice: ";
    int choice;
    cin >> choice;

    Note n;
    n.userId = currentUser->id;
    n.title = title;
    n.content = content;
    n.createdAt = getDateTime();

    if (choice == 1) {
        n.isImportant = true;
        string pass, confirm;
        cout << "Enter password for this note: ";
        cin >> pass;
        cout << "Confirm note password: ";
        cin >> confirm;
        if (pass != confirm) {
            cout << "Passwords do not match. Note not saved.\n";
            return;
        }
        n.notePassword = pass;
    }

    db.saveNote(n);
    cout << "\nNote saved successfully!\n";
}

// ==================== VIEW NOTES ====================
void viewNotes() {
    cout << "\n=================================\n";
    cout << "          MY NOTES\n";
    cout << "=================================\n";

    auto notes = db.getNotesByUserId(currentUser->id);
    if (notes.empty()) {
        cout << "No notes found.\n";
        return;
    }

    for (size_t i = 0; i < notes.size(); ++i) {
        const auto& n = notes[i];
        cout << "\n[" << (i + 1) << "] " << n.title;
        if (n.isImportant) cout << "  [LOCKED]";
        cout << "\n    Date: " << n.createdAt << "\n";
    }

    cout << "\nEnter note number to open (0 to go back): ";
    int num;
    cin >> num;
    if (num < 1 || num > (int)notes.size()) return;

    Note& selected = notes[num - 1];

    if (selected.isImportant) {
        string pass;
        cout << "This note is locked. Enter note password: ";
        cin >> pass;
        if (pass != selected.notePassword) {
            cout << "Wrong password!\n";
            return;
        }
    }

    cout << "\n----------------------------------------\n";
    cout << "TITLE: " << selected.title << "\n";
    cout << "DATE : " << selected.createdAt << "\n";
    cout << "----------------------------------------\n";
    cout << selected.content << "\n";
    cout << "----------------------------------------\n";
}

// ==================== DELETE NOTE ====================
void deleteNote() {
    auto notes = db.getNotesByUserId(currentUser->id);
    if (notes.empty()) {
        cout << "No notes to delete.\n";
        return;
    }

    cout << "\nYour notes:\n";
    for (size_t i = 0; i < notes.size(); ++i) {
        cout << (i + 1) << ". " << notes[i].title;
        if (notes[i].isImportant) cout << " [LOCKED]";
        cout << "\n";
    }

    cout << "Enter number to delete (0 cancel): ";
    int num;
    cin >> num;
    if (num < 1 || num > (int)notes.size()) return;

    db.deleteNote(notes[num - 1].id);
    cout << "Note deleted.\n";
}

// ==================== MAIN MENU ====================
void mainMenu() {
    int choice;
    do {
        cout << "\n\n========================================\n";
        cout << "           NOTES - MAIN MENU\n";
        cout << "========================================\n";
        cout << "Logged in as: " << currentUser->username << "\n";
        cout << "----------------------------------------\n";
        cout << "1. Add Note\n";
        cout << "2. View My Notes\n";
        cout << "3. Delete Note\n";
        cout << "4. Logout\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addNote(); break;
            case 2: viewNotes(); break;
            case 3: deleteNote(); break;
            case 4:
                currentUser = nullptr;
                cout << "Logged out.\n";
                return;
            case 0:
                cout << "Thank you for using Notes!\n";
                exit(0);
            default:
                cout << "Invalid choice.\n";
        }
    } while (true);
}

// ==================== STARTUP ====================
void startupMenu() {
    int choice;
    do {
        cout << "\n\n========================================\n";
        cout << "              NOTES\n";
        cout << "       Secure Personal Notes\n";
        cout << "========================================\n";
        cout << "1. Login\n";
        cout << "2. Create Account\n";
        cout << "3. Forgot Password\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                if (login()) mainMenu();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                forgotPassword();
                break;
            case 0:
                cout << "Goodbye!\n";
                return;
            default:
                cout << "Invalid choice.\n";
        }
    } while (true);
}

int main() {
    cout << "\n========================================\n";
    cout << "          NOTES - C++ PROJECT\n";
    cout << "========================================\n";
    startupMenu();
    return 0;
}
