# Notes — Complete Project (C++ + Web Frontend)

Secure multi-user personal notes application.

## Parts

1. **C++ console app** — file-based notes (college-style project)
2. **Web frontend** — EduVault-style UI with Supabase auth + Node server (email OTP for locked notes)

No features were changed from your project. Only `SUPABASE_URL` in `.env` was corrected from a Postgres connection string to the proper HTTPS API URL so the web app can work.

---

## 1. C++ Console App

### Build & run
```bash
cd notes-cpp
make
./notes
```

Or:
```bash
g++ -std=c++17 -Iinclude src/main.cpp src/Database.cpp -o notes
./notes
```

### Features
- Create account (username + password + 10-digit phone)
- Login / Forgot password (via phone)
- Add notes, mark Important (lock with separate password)
- View / unlock / delete your own notes
- Data in `data/users.txt` and `data/notes.txt`

---

## 2. Web Frontend (Supabase + Node)

### Setup Supabase (once)
1. Open your Supabase project SQL Editor
2. Run the full script in `supabase/schema.sql`
3. Confirm Auth → URL configuration allows `http://localhost:8000` and redirect to `/reset-password.html`

### Configure
- `.env` already has your SMTP and Supabase values (URL fixed)
- Or copy `.env.example` → `.env` and fill in values

### Install & start
```bash
cd notes-cpp
npm install
npm start
```
Open **http://localhost:8000**

### Web features
- Register / Login (username or email)
- Password reset via email link
- Create / edit / view notes
- Lock notes + OTP email unlock (Gmail SMTP)
- Soft-delete + Recycle bin restore

---

## Project structure
```
notes-cpp/
├── include/          User.hpp, Note.hpp, Database.hpp
├── src/              main.cpp, Database.cpp
├── frontend/         HTML, CSS, JS (web UI)
├── supabase/         schema.sql (RLS + profiles)
├── server.js         Static server + /api/config + OTP email
├── package.json
├── CMakeLists.txt / Makefile
├── .env / .env.example
└── data/             (created at runtime for C++)
```

## Requirements
- g++ with C++17
- Node.js 18+
- Supabase project + Gmail App Password (for OTP)

## Security note
Do not commit real `.env` secrets to public repositories. The service role key must never be placed in frontend files.
