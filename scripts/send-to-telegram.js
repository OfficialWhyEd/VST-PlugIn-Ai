import { execSync } from 'child_process';
import path from 'path';
import screenshot from 'screenshot-desktop';

console.log("Catturando screenshot...");

const outputPath = 'C:/Users/Whyed/Desktop/VST-PlugIn-Ai/src/ui/frontend/dist/screenshot.png';

screenshot({ filename: outputPath }).then((img) => {
    console.log("Screenshot salvato, invio a Telegram...");
    execSync(`npx openclaw message send --message "UI Aggiornata" --file "${outputPath}"`);
    console.log("Inviato!");
}).catch((err) => {
    console.error("Errore screenshot:", err);
});
