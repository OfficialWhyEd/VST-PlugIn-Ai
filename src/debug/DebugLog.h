/*
  ==============================================================================
  DebugLog.h — percorso del log di debug, uguale su tutte le piattaforme

  Il file finisce nella cartella temporanea di sistema:
    macOS/Linux  /tmp/whycremisi-debug.log
    Windows      %LOCALAPPDATA%\Temp\whycremisi-debug.log

  Prima era scritto "/tmp/..." a mano: su Windows quel percorso non esiste e i
  log finivano in C:\tmp o non venivano scritti affatto.
  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>

namespace whycremisi
{
    inline juce::File debugLogFile()
    {
        return juce::File::getSpecialLocation (juce::File::tempDirectory)
                          .getChildFile ("whycremisi-debug.log");
    }
}
