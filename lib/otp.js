const crypto = require('crypto');

function secret() {
  return process.env.OTP_SECRET || process.env.SMTP_PASS || process.env.SUPABASE_ANON_KEY || 'notes-otp-fallback';
}

function sign(email, otp, exp) {
  const payload = `${String(email).toLowerCase()}|${otp}|${exp}`;
  const sig = crypto.createHmac('sha256', secret()).update(payload).digest('hex');
  return `${sig}.${exp}`;
}

function parseProof(proof) {
  if (!proof || typeof proof !== 'string') return null;
  const i = proof.lastIndexOf('.');
  if (i < 0) return null;
  return { sig: proof.slice(0, i), exp: Number(proof.slice(i + 1)) };
}

function verify(email, otp, proof) {
  const parsed = parseProof(proof);
  if (!parsed || !parsed.exp || Date.now() > parsed.exp) return false;
  const expected = sign(email, String(otp).trim(), parsed.exp);
  try {
    return crypto.timingSafeEqual(Buffer.from(expected), Buffer.from(proof));
  } catch {
    return false;
  }
}

function cookieOptions(maxAgeMs) {
  const secure = process.env.VERCEL === '1' || process.env.NODE_ENV === 'production';
  return `Path=/; HttpOnly; SameSite=Lax; Max-Age=${Math.floor(maxAgeMs / 1000)}${secure ? '; Secure' : ''}`;
}

module.exports = { sign, verify, cookieOptions };
