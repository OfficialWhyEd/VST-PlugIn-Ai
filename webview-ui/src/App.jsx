import React, { useState, useEffect, useMemo, useRef, useCallback } from 'react'
import { motion } from 'framer-motion'
import { openclaw, ConnectionState } from './openclaw-bridge'
import { BotFace } from './components/BotFace'
import Toolbox from './components/Toolbox'
import './index.css'

export default function App() {
  const [connectionStatus, setConnectionStatus] = useState(ConnectionState.DISCONNECTED)
  const [botState, setBotState] = useState('idle')
  const [inputValue, setInputValue] = useState('')
  const [messages, setMessages] = useState([
    { id: 1, type: 'system', text: '[14:21:45] INITIALIZING NEURAL MATRIX... OK' },
    { id: 2, type: 'system', text: '[14:21:50] SCANNING SPECTRAL DENSITY... COMPLETE' },
    { id: 3, type: 'system', text: '[14:22:00] ANALYZING TRANSIENT RESPONSE IN 440HZ RANGE... DONE.' }
  ])
  const [lastMessage, setLastMessage] = useState(null)
  const chatEndRef = useRef(null)

  const scrollToBottom = () => {
    chatEndRef.current?.scrollIntoView({ behavior: 'smooth' })
  }

  useEffect(() => {
    scrollToBottom()
  }, [messages])

  useEffect(() => {
    openclaw.onAny((message) => {
      setLastMessage(message)
    })

    const stateUnsub = openclaw.onStateChange((state) => {
      setConnectionStatus(state)
    })

    window.addEventListener('openclaw-botstate', (e) => {
      setBotState(e.detail)
    })

    openclaw.connect().catch(() => {
      console.warn('[OpenClaw] Connessione WebSocket fallita, tentativo in background...')
    })

    const unsubAI = openclaw.on('ai.response', (payload) => {
      setBotState('success')
      setTimeout(() => setBotState('idle'), 2000)
      if (payload && payload.content) {
        setMessages(prev => [...prev, {
          id: Date.now(),
          type: 'bot',
          text: payload.content,
          time: new Date().toLocaleTimeString('en-US', { hour12: false }),
          telemetry: true
        }])
      }
    })

    const unsubAIStream = openclaw.on('ai.stream', (payload) => {
      setBotState('typing')
    })

    const unsubError = openclaw.on('plugin.error', (payload) => {
      setBotState('error')
      setTimeout(() => setBotState('idle'), 3000)
      setMessages(prev => [...prev, {
        id: Date.now(),
        type: 'system',
        text: `[ERROR] ${payload.message || payload.code || 'Unknown error'}`
      }])
    })

    const unsubTransport = openclaw.on('daw.transport', (payload) => {
      setMessages(prev => [...prev, {
        id: Date.now(),
        type: 'system',
        text: `[TRANSPORT] Play=${payload.isPlaying} Rec=${payload.isRecording} BPM=${payload.bpm}`
      }])
    })

    return () => {
      openclaw.off('*')
      stateUnsub()
      unsubAI()
      unsubAIStream()
      unsubError()
      unsubTransport()
      openclaw.disconnect()
    }
  }, [])

  const handleCommand = (e) => {
    if (e.key === 'Enter' && inputValue.trim() && botState === 'idle') {
      const newMsgId = Date.now()
      const timeStr = new Date().toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' })
      const command = inputValue.trim()

      setMessages(prev => [...prev, {
        id: newMsgId,
        type: 'user',
        text: command,
        time: timeStr
      }])

      setInputValue('')
      setBotState('thinking')

      if (openclaw.isConnected()) {
        openclaw.sendAIPrompt(command, {
          onResponse: (payload) => {
            setBotState('success')
            setTimeout(() => setBotState('idle'), 2000)
          }
        })
      } else {
        setBotState('error')
        setTimeout(() => setBotState('idle'), 2000)
        setMessages(prev => [...prev, {
          id: Date.now(),
          type: 'system',
          text: '[ERROR] Not connected to plugin'
        }])
      }
    }
  }

  const simulateTyping = async (text, messageId, endState = 'idle') => {
    setBotState('typing')
    let currentText = ''

    setMessages(prev => [...prev, {
      id: messageId,
      type: 'bot',
      text: '',
      telemetry: false
    }])

    for (let i = 0; i < text.length; i++) {
      currentText += text[i]
      setMessages(prev => prev.map(msg =>
        msg.id === messageId ? { ...msg, text: currentText } : msg
      ))
      await new Promise(r => setTimeout(r, 15 + Math.random() * 25))
    }

    setBotState('success')
    setTimeout(() => setBotState(endState), 1500)
  }

  const executeSuggestedChain = () => {
    if (botState !== 'idle') return
    setBotState('loading')
    setTimeout(() => {
      simulateTyping("Executing suggested chain. Dynamic dip of -2.4dB applied at 300Hz. Transient preservation locked at 84%. Spectral clarity increased.", Date.now())
    }, 1000)
  }

  const handleTransport = (action) => {
    if (openclaw.isConnected()) {
      openclaw.sendDAWCommand(action)
    }
  }

  const lastBotMsgId = useMemo(() => {
    const botMsgs = messages.filter(m => m.type === 'bot')
    return botMsgs.length > 0 ? botMsgs[botMsgs.length - 1].id : null
  }, [messages])

  return (
    <div className="bg-background select-none h-screen w-screen overflow-hidden text-[#e5e2e1] font-['Space_Grotesk']">
      <div className="crt-overlay"></div>

      {/* Top Navigation */}
      <header className="bg-[#131313] flex justify-between items-center w-full px-6 py-3 border-b border-[#222222] z-50 relative">
        <div className="flex items-center gap-6">
          <h1 className="text-xl font-black tracking-tighter text-[#DC143C] uppercase">WHYCREMISI</h1>
          <div className="flex gap-4">
            <nav className="flex gap-6 font-['Space_Grotesk'] uppercase tracking-[0.1em] font-bold text-sm">
              <a className="text-[#FFB000] border-b-2 border-[#FFB000] pb-1" href="#">COMMAND</a>
              <a className="text-[#4d4d4d] hover:text-[#FFB000] transition-colors" href="#">MASTER</a>
              <a className="text-[#4d4d4d] hover:text-[#FFB000] transition-colors" href="#">TELEMETRY</a>
              <a className="text-[#4d4d4d] hover:text-[#FFB000] transition-colors" href="#">VECTORS</a>
            </nav>
          </div>
        </div>
        <div className="flex items-center gap-6 text-[10px] font-mono text-[#FFB000] opacity-80">
          <div className="flex items-center gap-2">
            <div className={`w-2 h-2 rounded-full ${connectionStatus === ConnectionState.CONNECTED ? 'bg-[#00FFaa] led-amber-active' : 'bg-[#DC143C] led-red-active'}`}></div>
            <span>{connectionStatus === ConnectionState.CONNECTED ? 'CONNECTED' : connectionStatus === ConnectionState.CONNECTING ? 'CONNECTING...' : 'DISCONNECTED'}</span>
          </div>
        </div>
      </header>

      {/* Side Module */}
      <aside className="fixed left-0 top-12 h-[calc(100vh-3rem)] w-20 flex flex-col items-center py-4 bg-[#131313] border-r border-[#222222] z-40">
        <div className="text-[10px] font-mono text-[#4d4d4d] uppercase mb-8 rotate-180" style={{ writingMode: 'vertical-lr' }}>MODULES // V1.0.4-STABLE</div>
        <div className="flex flex-col w-full">
          <div className="text-[#DC143C] bg-[#1a1a1a] border-l-4 border-[#DC143C] w-full py-4 flex flex-col items-center gap-1 cursor-pointer">
            <span className="material-symbols-outlined text-xl">memory</span>
            <span className="font-['Space_Grotesk'] font-mono text-[10px] uppercase">AI ENGINE</span>
          </div>
          <div className="text-[#4d4d4d] py-4 flex flex-col items-center gap-1 hover:text-[#FFB000] hover:bg-[#1a1a1a] transition-all cursor-pointer" onClick={() => handleTransport('play')}>
            <span className="material-symbols-outlined text-xl">play_arrow</span>
            <span className="font-['Space_Grotesk'] font-mono text-[10px] uppercase">TRANSPORT</span>
          </div>
          <div className="text-[#4d4d4d] py-4 flex flex-col items-center gap-1 hover:text-[#FFB000] hover:bg-[#1a1a1a] transition-all cursor-pointer" onClick={() => handleTransport('stop')}>
            <span className="material-symbols-outlined text-xl">stop</span>
            <span className="font-['Space_Grotesk'] font-mono text-[10px] uppercase">STOP</span>
          </div>
          <div className="text-[#4d4d4d] py-4 flex flex-col items-center gap-1 hover:text-[#FFB000] hover:bg-[#1a1a1a] transition-all cursor-pointer" onClick={() => handleTransport('record')}>
            <span className="material-symbols-outlined text-xl">fiber_manual_record</span>
            <span className="font-['Space_Grotesk'] font-mono text-[10px] uppercase">REC</span>
          </div>
        </div>
      </aside>

      <main className="ml-20 mt-0 h-[calc(100vh-3rem)] grid grid-cols-12 grid-rows-6 p-1 gap-1 overflow-hidden bg-[#0d0d0d] relative z-10">
        {/* AI Chat Interface Panel */}
        <section className="col-span-9 row-span-4 bg-[#0e0e0e] relative border border-[#222222] flex flex-col overflow-hidden">
          <div className="bg-[#1a1a1a] px-4 py-2 flex justify-between items-center border-b border-[#222222]">
            <div className="flex items-center gap-2">
              <div className="w-2 h-2 rounded-full bg-[#FFB000] led-amber-active"></div>
              <span className="text-[10px] font-bold text-[#FFB000] tracking-widest uppercase">AI COMMAND CONSOLE</span>
            </div>
            <span className="text-[9px] text-[#4d4d4d] font-mono">NEURAL_MASTERING_V4.2.0</span>
          </div>

          {/* Message History Area */}
          <div className="flex-1 p-6 font-mono overflow-y-auto custom-scrollbar space-y-6">
            {messages.map((msg) => {
              if (msg.type === 'system') {
                return (
                  <div key={msg.id} className="opacity-40 text-[10px] font-mono">
                    {msg.text?.split('...')[0]}... <span className="text-[#FFB000]">{msg.text?.split('...')[1]}</span>
                  </div>
                )
              }

              if (msg.type === 'user') {
                return (
                  <div key={msg.id} className="flex flex-col items-end space-y-1 mt-4">
                    <div className="bg-[#222222]/50 border border-[#FFB000]/20 px-4 py-2 max-w-[80%] text-sm text-white/90 font-mono italic">
                      {msg.text}
                    </div>
                    <span className="text-[9px] text-[#4d4d4d] mr-2 uppercase font-bold tracking-widest">User Request // {msg.time}</span>
                  </div>
                )
              }

              if (msg.type === 'bot') {
                const isLatest = msg.id === lastBotMsgId
                return (
                  <div key={msg.id} className="flex gap-4 mt-6">
                    <div className="flex-shrink-0 mt-1 w-16 h-16 flex items-center justify-center">
                      {isLatest ? (
                        <BotFace state={botState} className="w-16 h-16" />
                      ) : (
                        <div className="w-8 h-8 rounded-full border border-[#FFB000]/20 bg-[#1a1a1a] flex items-center justify-center opacity-30">
                          <span className="material-symbols-outlined text-[12px] text-[#FFB000]">smart_toy</span>
                        </div>
                      )}
                    </div>

                    <div className="flex-1 bg-[#1a1a1a] border border-[#FFB000]/10 p-4 relative font-mono">
                      <div className="absolute top-0 left-0 w-[2px] h-full bg-[#FFB000]"></div>
                      <div className="flex justify-between items-start mb-2">
                        <span className="text-[#FFB000] text-[11px] font-bold tracking-tighter uppercase">AI_TELEMETRY_ENGINE</span>
                        <span className="text-[9px] text-[#4d4d4d]">DATA_FETCH: SUCCESS</span>
                      </div>
                      <div className="text-sm text-[#e5e2e1] space-y-4">
                        <p className="leading-relaxed whitespace-pre-wrap">
                          {msg.text}
                          {botState === 'typing' && isLatest && (
                            <span className="inline-block w-1.5 h-4 bg-[#FFB000] ml-1 animate-pulse align-middle"></span>
                          )}
                        </p>
                      </div>
                    </div>
                  </div>
                )
              }
              return null
            })}
            <div ref={chatEndRef} />
          </div>

          {/* Terminal Input Area */}
          <div className="h-14 bg-[#131313] border-t border-[#222222] flex items-center px-6 gap-4">
            <span className="text-[#FFB000] font-bold text-sm tracking-widest shrink-0">CMD &gt;</span>
            <div className="flex-1 relative flex items-center">
              <input
                className="w-full bg-transparent border-none text-white font-mono text-sm focus:ring-0 focus:outline-none placeholder-[#4d4d4d] p-0 disabled:opacity-50"
                placeholder={botState !== 'idle' ? "PROCESSING..." : "Type command (e.g. /analyze_spectral, play, stop)..."}
                type="text"
                value={inputValue}
                onChange={(e) => setInputValue(e.target.value)}
                onKeyDown={handleCommand}
                disabled={botState !== 'idle'}
              />
              {botState === 'idle' && <div className="terminal-cursor"></div>}
            </div>
            <div className="flex gap-2 text-[#4d4d4d]">
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]" onClick={() => handleTransport('play')}>play_arrow</span>
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]" onClick={() => handleTransport('stop')}>stop</span>
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]" onClick={() => handleTransport('record')}>fiber_manual_record</span>
            </div>
          </div>
        </section>

        {/* Right Panel - Toolbox + Transport */}
        <section className="col-span-3 row-span-4 bg-[#131313] border border-[#222222] flex flex-col overflow-hidden">
          <div className="bg-[#1a1a1a] px-4 py-2 border-b border-[#222222]">
            <span className="text-[10px] font-bold text-[#DC143C] tracking-widest uppercase">TOOLBOX + TRANSPORT</span>
          </div>
          <div className="flex-1 overflow-y-auto custom-scrollbar">
            <Toolbox lastMessage={lastMessage} />
          </div>
        </section>

        {/* Bottom - Vector Scope placeholder */}
        <section className="col-span-12 row-span-2 bg-[#0e0e0e] border border-[#222222] relative overflow-hidden flex flex-col">
          <div className="absolute top-2 left-4 z-10 flex items-center gap-4">
            <span className="text-[10px] font-bold text-[#FFB000] uppercase opacity-80">Vector Scope: STEREO_FIELD</span>
            <div className="h-[1px] w-48 bg-gradient-to-r from-[#FFB000] to-transparent"></div>
          </div>
          <div className="flex-1 flex items-center justify-center">
            <span className="text-[#4d4d4d] text-sm font-mono">Awaiting audio signal data...</span>
          </div>
        </section>
      </main>

      {/* Footer */}
      <footer className="fixed bottom-0 left-0 w-full z-50 flex justify-between items-center h-10 border-t border-[#222222] bg-[#0d0d0d] px-6">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-2 px-4 text-[#4d4d4d] font-mono text-[12px]">
            <span>OSC :9000</span>
            <span>|</span>
            <span>WS :8080</span>
          </div>
        </div>
        <div className="flex items-center gap-4">
          <div className="text-[10px] font-mono text-[#FFB000]">
            {connectionStatus === ConnectionState.CONNECTED ? '[CONNECTED] // READY' : '[OFFLINE] // AWAITING SIGNAL'}
          </div>
          <div className={`h-4 w-4 ${connectionStatus === ConnectionState.CONNECTED ? 'bg-[#FFB000] led-amber-active' : 'bg-[#4d4d4d]'}`}></div>
        </div>
      </footer>
    </div>
  )
}