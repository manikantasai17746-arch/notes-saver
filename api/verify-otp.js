const { verify } = require('../lib/otp');

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

function getCookie(req, name) {
  const header = req.headers.cookie || '';
  const parts = header.split(';');
  for (const part of parts) {
    const [k, ...rest] = part.trim().split('=');
    if (k === name) return rest.join('=');
  }
  return '';
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
    const otp = String(data.otp || '').trim();
    const proof = getCookie(req, 'notes_otp_proof');
    if (!email || !otp || !verify(email, otp, proof)) {
      res.statusCode = 400;
      return res.end(JSON.stringify({ error: 'Invalid or expired OTP.' }));
    }
    res.setHeader('Set-Cookie', 'notes_otp_proof=; Path=/; Max-Age=0');
    res.statusCode = 200;
    return res.end(JSON.stringify({ ok: true }));
  } catch (error) {
    res.statusCode = 400;
    return res.end(JSON.stringify({ error: error.message || 'Invalid request' }));
  }
};
