const crypto = require('crypto');
const nodemailer = require('nodemailer');
const { sign, cookieOptions } = require('../lib/otp');

function readBody(req) {
  return new Promise((resolve, reject) => {
    let data = '';
    req.on('data', (chunk) => { data += chunk; });
    req.on('end', () => {
      try { resolve(JSON.parse(data || '{}')); }
      catch { reject(new Error('Invalid JSON')); }
    });
    req.on('error', reject);
  });
}

module.exports = async function handler(req, res) {
  res.setHeader('Content-Type', 'application/json; charset=utf-8');
  if (req.method !== 'POST') {
    res.statusCode = 405;
    return res.end(JSON.stringify({ error: 'Method not allowed' }));
  }
  try {
    const data = await readBody(req);
    const email = String(data.email || '').trim().toLowerCase();
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
      res.statusCode = 400;
      return res.end(JSON.stringify({ error: 'Enter a valid email address.' }));
    }
    const required = ['SMTP_HOST', 'SMTP_PORT', 'SMTP_USER', 'SMTP_PASS', 'SMTP_FROM'];
    if (required.some((key) => !process.env[key])) {
      res.statusCode = 500;
      return res.end(JSON.stringify({ error: 'Email service is not configured on the server.' }));
    }
    const otp = String(crypto.randomInt(100000, 1000000));
    const exp = Date.now() + 10 * 60 * 1000;
    const proof = sign(email, otp, exp);

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

    res.setHeader('Set-Cookie', `notes_otp_proof=${proof}; ${cookieOptions(10 * 60 * 1000)}`);
    res.statusCode = 200;
    return res.end(JSON.stringify({ ok: true }));
  } catch (error) {
    res.statusCode = 500;
    return res.end(JSON.stringify({ error: error.message || 'Failed to send OTP' }));
  }
};
