import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import { exec } from 'child_process'

// Plugin custom per inviare screenshot a Telegram dopo ogni build
function telegramScreenshotPlugin() {
  return {
    name: 'telegram-screenshot',
    writeBundle() {
      console.log('Build completata: Inviando screenshot a Telegram...');
      // Esegue uno script Node che usa il bot Telegram per inviare lo screenshot
      exec('node ../../../scripts/send-to-telegram.js', (err, stdout, stderr) => {
        if (err) console.error('Errore invio screenshot:', stderr);
      });
    }
  }
}

export default defineConfig({
  plugins: [react(), telegramScreenshotPlugin()],
  server: {
    port: 5173,
    hmr: true
  }
})
