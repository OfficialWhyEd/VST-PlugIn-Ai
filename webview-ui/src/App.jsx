import React, { useState, useEffect, useCallback } from 'react'
import './App.css'
import Toolbox from './components/Toolbox'
import { openclaw, ConnectionState } from './openclaw-bridge.js'

function App() {
  const [connectionStatus, setConnectionStatus] = useState('disconnected')
  const [lastMessage, setLastMessage] = useState(null)
  const [logs, setLogs] = useState([])

  const addLog = useCallback((msg) => {
    setLogs(prev => [{ time: new Date().toLocaleTimeString(), msg }, ...prev].slice(0, 20))
  }, [])

  useEffect(() => {
    openclaw.onAny((message) => {
      console.log('[OpenClaw UI] Ricevuto:', message)
      setLastMessage(message)
      addLog(`Ricevuto: ${message.type || 'unknown'}`)
    })

    const stateUnsub = openclaw.onStateChange((state) => {
      setConnectionStatus(state)
      addLog(`Stato: ${state}`)
    })

    openclaw.connect().then(() => {
      addLog('Connesso al plugin via WebSocket')
    }).catch((e) => {
      addLog(`Errore connessione: ${e.message}`)
    })

    return () => {
      openclaw.off('*')
      stateUnsub()
      openclaw.disconnect()
    }
  }, [addLog])

  return (
    <div className="app">
      <header className="app-header">
        <h1>OpenClaw VST Bridge AI</h1>
        <div className={`status ${connectionStatus}`}>
          {connectionStatus === ConnectionState.CONNECTED ? 'Connesso' : 
           connectionStatus === ConnectionState.CONNECTING ? 'Connessione...' :
           connectionStatus === ConnectionState.RECONNECTING ? 'Riconnessione...' :
           'Disconnesso'}
        </div>
      </header>

      <main className="app-main">
        <Toolbox lastMessage={lastMessage} addLog={addLog} />
      </main>

      <footer className="app-footer">
        <p>Plugin VST AI - v0.1 | WebSocket :8080 | OSC :9000</p>
      </footer>
    </div>
  )
}

export default App
