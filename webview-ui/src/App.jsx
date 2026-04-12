import React, { useState, useEffect } from 'react'
import './App.css'
import Toolbox from './components/Toolbox'

function App() {
  const [connectionStatus, setConnectionStatus] = useState('disconnected')
  const [lastMessage, setLastMessage] = useState(null)

  useEffect(() => {
    // Connessione al plugin C++ via window.receiveFromPlugin
    window.receiveFromPlugin = (jsonString) => {
      try {
        const message = JSON.parse(jsonString)
        setLastMessage(message)
        setConnectionStatus('connected')
        console.log('📥 Ricevuto dal plugin:', message)
      } catch (e) {
        console.error('Errore parsing JSON:', e)
      }
    }

    // Simula connessione dopo 1 secondo
    setTimeout(() => {
      setConnectionStatus('connected')
    }, 1000)

    return () => {
      window.receiveFromPlugin = null
    }
  }, [])

  return (
    <div className="app">
      <header className="app-header">
        <h1>🎵 OpenClaw VST Bridge AI</h1>
        <div className={`status ${connectionStatus}`}>
          {connectionStatus === 'connected' ? '🟢 Connesso' : '🔴 Disconnesso'}
        </div>
      </header>

      <main className="app-main">
        <Toolbox lastMessage={lastMessage} />
      </main>

      <footer className="app-footer">
        <p>Plugin VST AI - v0.1</p>
      </footer>
    </div>
  )
}

export default App
