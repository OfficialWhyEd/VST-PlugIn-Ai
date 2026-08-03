/*
  ==============================================================================
  TestRunner.cpp

  Esegue tutti i juce::UnitTest registrati e riporta l'esito.

  Prima mancava: il CMakeLists dichiarava i test con juce_add_unit_test, una
  funzione che JUCE non ha mai avuto, quindi con BUILD_UNIT_TESTS=ON la
  configurazione falliva e nessuno se ne accorgeva perche' l'opzione e'
  disattivata di default. I test esistevano come file e non erano mai stati
  eseguiti.
  ==============================================================================
*/

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>   // ScopedJuceInitialiser_GUI vive qui
#include <cstdio>

/** Stampa mentre i test girano invece che solo alla fine: se uno crasha,
    l'ultima riga a schermo dice quale. Con il riepilogo finale soltanto,
    un crash non lascia traccia di dove sia avvenuto. */
class RunnerConLog : public juce::UnitTestRunner
{
protected:
    void logMessage (const juce::String& message) override
    {
        std::printf ("   %s\n", message.toRawUTF8());
        std::fflush (stdout);
    }
};

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI initialiser;

    RunnerConLog runner;
    runner.setAssertOnFailure (false);   // vogliamo il riepilogo, non un crash

    // Di norma si eseguono solo i test di questo progetto. Passando "all"
    // si esegue anche la suite interna di JUCE, che dura minuti e serve
    // solo quando si sospetta un problema nel framework stesso.
    const juce::String categoria = (argc > 1) ? juce::String (argv[1]) : "whycremisi";

    if (categoria == "all")
        runner.runAllTests();
    else
        runner.runTestsInCategory (categoria);

    int totali = 0, falliti = 0;

    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        const auto* r = runner.getResult (i);
        if (r == nullptr) continue;

        totali  += r->passes + r->failures;
        falliti += r->failures;

        const juce::String esito = (r->failures > 0) ? "FALLITO" : "ok";
        std::printf ("%-8s %s  (%d superati, %d falliti)\n",
                     esito.toRawUTF8(),
                     r->unitTestName.toRawUTF8(),
                     r->passes,
                     r->failures);

        for (const auto& messaggio : r->messages)
            std::printf ("           %s\n", messaggio.toRawUTF8());
    }

    std::printf ("\n%d controlli, %d falliti\n", totali, falliti);
    return falliti > 0 ? 1 : 0;
}
