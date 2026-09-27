const http = require('http');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');
require('dotenv').config();
const nodemailer = require('nodemailer');
const { sign, verify, cookieOptions } = require('./lib/otp');

const root = path.join(__dirname, 'frontend');
const contentTypes = {
  '.html': 'text/html; charset=utf-8',
  '.css': 'text/css',
  '.js': 'application/javascript',
  '.png': 'image/png'
};

function json(response, status, body, extraHeaders) {
  const headers = { 'Content-Type': 'application/json; charset=utf-8', ...(extraHeaders || {}) };
  response.writeHead(status, headers);
  response.end(JSON.stringify(body));
}

function body(request) {
  return new Promise((resolve, reject) => {
    let data = '';
    request.on('data', (chunk) => { data += chunk; });
    request.on('end', () => {
      try { resolve(JSON.parse(data || '{}')); }
      catch { reject(new Error('Invalid JSON')); }
    });
    request.on('error', reject);
  });
}

function getCookie(request, name) {
  const header = request.headers.cookie || '';
  const parts = header.split(';');
  for (const part of parts) {
    const [k, ...rest] = part.trim().split('=');
    if (k === name) return rest.join('=');
  }
  return '';
}

async function sendOtpMail(email, otp) {
  const required = ['SMTP_HOST', 'SMTP_PORT', 'SMTP_USER', 'SMTP_PASS', 'SMTP_FROM'];
  if (required.some((key) => !process.env[key])) {
    throw new Error('Email service is not configured on the server.');
  }
  const transporter = nodemailer.createTransport({
    host: process.env.SMTP_HOST,
    port: Number(process.env.SMTP_PORT),
    secure: process.env.SMTP_SECURE === 'true',
    auth: { user: process.env.SMTP_USER, pass: process.env.SMTP_PASS }
  });
  await transporter.sendMail({
    from: process.env.SMTP_FROM,
    to: email,
    subject: 'Your Notes password recovery code',
    text: `Your Notes verification code is ${otp}. It expires in 10 minutes.`
  });
}

const server = http.createServer(async (request, response) => {
  if (request.method === 'GET' && request.url === '/api/config') {
    return json(response, 200, {
      url: process.env.SUPABASE_URL,
      anonKey: process.env.SUPABASE_ANON_KEY
    });
  }

  if (request.method === 'POST' && request.url === '/api/send-otp') {
    try {
      const data = await body(request);
      const email = String(data.email || '').trim().toLowerCase();
      if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
        return json(response, 400, { error: 'Enter a valid email address.' });
      }
      const otp = String(crypto.randomInt(100000, 1000000));
      const exp = Date.now() + 10 * 60 * 1000;
      const proof = sign(email, otp, exp);
      await sendOtpMail(email, otp);
      return json(response, 200, { ok: true }, {
        'Set-Cookie': `notes_otp_proof=${proof}; ${cookieOptions(10 * 60 * 1000)}`
      });
    } catch (error) {
      return json(response, 500, { error: error.message });
    }
  }

  if (request.method === 'POST' && request.url === '/api/verify-otp') {
    try {
      const data = await body(request);
      const email = String(data.email || '').trim().toLowerCase();
      const otp = String(data.otp || '').trim();
      const proof = getCookie(request, 'notes_otp_proof');
      if (!email || !otp || !verify(email, otp, proof)) {
        return json(response, 400, { error: 'Invalid or expired OTP.' });
      }
      return json(response, 200, { ok: true }, {
        'Set-Cookie': 'notes_otp_proof=; Path=/; Max-Age=0'
      });
    } catch (error) {
      return json(response, 400, { error: error.message });
    }
  }

  const requested = request.url === '/' ? '/index.html' : request.url.split('?')[0];
  const filePath = path.normalize(path.join(root, requested));
  if (!filePath.startsWith(root)) return json(response, 403, { error: 'Forbidden' });
  fs.readFile(filePath, (error, data) => {
    if (error) {
      response.writeHead(404);
      return response.end('Not found');
    }
    response.writeHead(200, {
      'Content-Type': contentTypes[path.extname(filePath)] || 'application/octet-stream'
    });
    response.end(data);
  });
});

server.listen(8000, () => console.log('Notes running at http://localhost:8000'));
