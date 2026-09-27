(async function () {
  const storageKey = 'notes-demo-user';
  const form = document.getElementById('register-form') || document.getElementById('login-form');
  const message = document.getElementById('message');
  if (!form || !message) return;
  const supabase = await window.supabaseReady;

  function showMessage(text, type) {
    message.textContent = text;
    message.className = 'alert alert-' + type;
    message.hidden = false;
  }

  form.addEventListener('submit', async function (event) {
    event.preventDefault();
    const values = Object.fromEntries(new FormData(form));

    if (form.id === 'register-form') {
      if (values.password !== values.confirm) {
        showMessage('Passwords do not match.', 'error');
        return;
      }
      const result = await supabase.auth.signUp({ email: values.email, password: values.password, options: { data: { username: values.username } } });
      if (result.error) { showMessage(result.error.message, 'error'); return; }
      showMessage('Account created. Check your email if confirmation is required.', 'success');
      setTimeout(function () { window.location.href = '/login.html'; }, 1000);
      return;
    }

    const identifier = values.identifier.trim();
    let email = identifier;
    if (!identifier.includes('@')) {
      const lookup = await supabase.rpc('get_email_for_username', { lookup_username: identifier });
      if (lookup.error || !lookup.data) { showMessage('Username or password is incorrect.', 'error'); return; }
      email = lookup.data;
    }
    const result = await supabase.auth.signInWithPassword({ email: email, password: values.password });
    if (result.error) { showMessage(result.error.message, 'error'); return; }
    const sessionUser = result.data.user;
    localStorage.setItem(storageKey, JSON.stringify({ id: sessionUser.id, username: sessionUser.user_metadata.username || sessionUser.email, email: sessionUser.email }));
    showMessage('Login successful. Opening your notes...', 'success');
    setTimeout(function () { window.location.href = '/notes.html'; }, 500);
  });

  const forgotForm = document.getElementById('forgot-form');
  if (!forgotForm) return;

  forgotForm.addEventListener('submit', async function (event) {
    event.preventDefault();
    const email = document.getElementById('forgot-email').value.trim().toLowerCase();
    const button = document.getElementById('send-forgot-otp'); button.disabled = true;
    try {
      const result = await supabase.auth.resetPasswordForEmail(email, { redirectTo: window.location.origin + '/reset-password.html' });
      if (result.error) throw new Error(result.error.message);
      showMessage('Password reset email sent. Follow the link in your email.', 'success');
    } catch (error) { showMessage(error.message, 'error'); } finally { button.disabled = false; }
  });
})();
