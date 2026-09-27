# Deploy Notes to Vercel

## 1. Push project to GitHub

Create a GitHub repo and upload the `notes-cpp` folder contents (or this whole project root that contains `public/`, `api/`, `package.json`, `vercel.json`).

Required files for Vercel:
- `public/` — frontend HTML/CSS/JS
- `api/` — serverless routes (`config`, `send-otp`, `verify-otp`)
- `lib/otp.js`
- `package.json`
- `vercel.json`
- `.env` is **not** uploaded — set env vars in Vercel dashboard

## 2. Import on Vercel

1. Go to [https://vercel.com](https://vercel.com) → **Add New Project**
2. Import your GitHub repository
3. Framework Preset: **Other**
4. Root Directory: leave default (project root)
5. Click **Deploy** (env vars next)

## 3. Environment variables (Vercel → Project → Settings → Environment Variables)

Add these for **Production** (and Preview if you want):

| Name | Value |
|------|--------|
| `SUPABASE_URL` | `https://vlfzltywcieindwmrbas.supabase.co` |
| `SUPABASE_ANON_KEY` | your Supabase anon key |
| `SMTP_HOST` | `smtp.gmail.com` |
| `SMTP_PORT` | `587` |
| `SMTP_SECURE` | `false` |
| `SMTP_USER` | your Gmail |
| `SMTP_PASS` | Gmail App Password |
| `SMTP_FROM` | `Notes <your-gmail@gmail.com>` |
| `OTP_SECRET` | any long random string (optional but recommended) |

Then **Redeploy**.

## 4. Fix Supabase redirect URLs

In Supabase → **Authentication → URL Configuration**:

- **Site URL**: `https://YOUR-PROJECT.vercel.app`
- **Redirect URLs** (add all):
  - `https://YOUR-PROJECT.vercel.app/**`
  - `https://YOUR-PROJECT.vercel.app/reset-password.html`
  - `http://localhost:8000/**` (keep for local)

## 5. Open the app

After deploy, open:
`https://YOUR-PROJECT.vercel.app`

## Local still works

```bash
npm install
npm start
```
→ http://localhost:8000

## Notes

- C++ console app is separate and is **not** deployed to Vercel (browser only).
- Password reset uses Supabase email link → `reset-password.html`.
- Locked-note OTP uses Gmail SMTP via `/api/send-otp` and `/api/verify-otp`.
