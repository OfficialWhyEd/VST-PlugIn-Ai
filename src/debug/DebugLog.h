/*
  ==============================================================================
  DebugLog.h — diario di bordo del plugin

  Serve a capire cosa e' successo dentro il plugin mentre girava dentro il
  DAW, dove non c'e' una console da guardare e non si puo' attaccare un
  debugger senza rovinare la sessione.

  Il file sta sempre nello stesso posto:
      Windows   %APPDATA%\WhyCremisi\whycremisi.log
      macOS     ~/Library/Application Support/WhyCremisi/whycremisi.log
      Linux     ~/.config/WhyCremisi/whycremisi.log

  Prima il log esisteva solo nelle build di debug e finiva in /tmp, che su
  Windows non esiste: durante una prova vera non restava traccia di niente.
  Ora e' attivo sempre — un plugin che non si puo' diagnosticare mentre lo
  si usa e' un plugin che non si puo' correggere.
  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>

namespace whycremisi
{
    /** Cartella dei dati del plugin, creata se non c'e'. */
    inline juce::File appDataDirectory()
    {
        auto dir =
           #if JUCE_MAC
            juce::File::getSpecialLocation (juce::File::userHomeDirectory)
                       .getChildFile ("Library/Application Support/WhyCremisi");
           #elif JUCE_WINDOWS
            juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                       .getChildFile ("WhyCremisi");
           #else
            juce::File::getSpecialLocation (juce::File::userHomeDirectory)
                       .getChildFile (".config/WhyCremisi");
           #endif

        if (! dir.exists())
            dir.createDirectory();

        return dir;
    }

    /** Il file di log corrente. */
    inline juce::File debugLogFile()
    {
        return appDataDirectory().getChildFile ("whycremisi.log");
    }

    /** Scrive una riga con l'ora e chi l'ha scritta.

        Il file si tiene sotto i 5 MB: al superamento la versione vecchia
        diventa whycremisi.log.1 e si riparte. Senza rotazione, dopo qualche
        sessione lunga il file diventa illeggibile e occupa spazio per niente.

        La scrittura e' protetta da un lock perche' arriva da piu' thread —
        audio, rete, interfaccia — e senza le righe si sovrappongono.
    */
    inline void log (const juce::String& area, const juce::String& messaggio)
    {
        static juce::CriticalSection lock;
        const juce::ScopedLock sl (lock);

        auto file = debugLogFile();

        if (file.existsAsFile() && file.getSize() > 5 * 1024 * 1024)
        {
            auto vecchio = file.getSiblingFile ("whycremisi.log.1");
            vecchio.deleteFile();
            file.moveFileTo (vecchio);
        }

        const auto ora = juce::Time::getCurrentTime();
        file.appendText (ora.formatted ("%H:%M:%S")
                         + "." + juce::String (ora.getMilliseconds()).paddedLeft ('0', 3)
                         + "  [" + area + "] " + messaggio + juce::newLine);
    }

    /** Riga di intestazione a ogni avvio: senza, in un file lungo non si
        capisce dove finisce una sessione e comincia la successiva. */
    inline void logSessionStart (const juce::String& contesto)
    {
        const auto adesso = juce::Time::getCurrentTime();
        log ("avvio", "──────────────────────────────────────────────");
        log ("avvio", "WhyCremisi " + contesto);
        log ("avvio", adesso.toString (true, true, false, true));
    }
}
