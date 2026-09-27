window.supabaseReady = (async function () {
  const config = await fetch('/api/config').then(function (response) { return response.json(); });
  await new Promise(function (resolve, reject) {
    const script = document.createElement('script');
    script.src = 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2';
    script.onload = resolve;
    script.onerror = reject;
    document.head.appendChild(script);
  });
  return window.supabase.createClient(config.url, config.anonKey);
})();