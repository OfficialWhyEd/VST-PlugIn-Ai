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
    { id: 3, type: 'system', text: '[14:22:00] ANALYZING TRANSIENT RESPONSE IN 440HZ RANGE... DONE.' },
    { id: 4, type: 'advisory' },
    { id: 5, type: 'bot', text: 'Accessing telemetry... Stereo width is currently correlated at +0.62. Phase alignment looks optimal for broadcast delivery.', telemetry: true }
  ])
  const [lastMessage, setLastMessage] = useState(null)
  const [activeSideModule, setActiveSideModule] = useState('ai')
  const chatEndRef = useRef(null)

  const scrollToBottom = () => {
    chatEndRef.current?.scrollIntoView({ behavior: 'smooth' })
  }

  useEffect(() => { scrollToBottom() }, [messages])

  useEffect(() => {
    openclaw.onAny((message) => { setLastMessage(message) })
    const stateUnsub = openclaw.onStateChange((state) => { setConnectionStatus(state) })
    window.addEventListener('openclaw-botstate', (e) => { setBotState(e.detail) })

    openclaw.connect().catch(() => {
      console.warn('[OpenClaw] Connessione WebSocket fallita, tentativo in background...')
    })

    const unsubAI = openclaw.on('ai.response', (payload) => {
      setBotState('success')
      setTimeout(() => setBotState('idle'), 2000)
      if (payload && payload.content) {
        setMessages(prev => [...prev, {
          id: Date.now(), type: 'bot',
          text: payload.content,
          time: new Date().toLocaleTimeString('en-US', { hour12: false }),
          telemetry: true
        }])
      }
    })

    const unsubAIStream = openclaw.on('ai.stream', () => { setBotState('typing') })

    const unsubError = openclaw.on('plugin.error', (payload) => {
      setBotState('error')
      setTimeout(() => setBotState('idle'), 3000)
      setMessages(prev => [...prev, {
        id: Date.now(), type: 'system',
        text: `[ERROR] ${payload.message || payload.code || 'Unknown error'}`
      }])
    })

    const unsubTransport = openclaw.on('daw.transport', (payload) => {
      setMessages(prev => [...prev, {
        id: Date.now(), type: 'system',
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

      setMessages(prev => [...prev, { id: newMsgId, type: 'user', text: command, time: timeStr }])
      setInputValue('')
      setBotState('thinking')

      if (openclaw.isConnected()) {
        openclaw.sendAIPrompt(command, {
          onResponse: () => {
            setBotState('success')
            setTimeout(() => setBotState('idle'), 2000)
          }
        })
      } else {
        setBotState('error')
        setTimeout(() => setBotState('idle'), 2000)
        setMessages(prev => [...prev, { id: Date.now(), type: 'system', text: '[ERROR] Not connected to plugin' }])
      }
    }
  }

  const simulateTyping = async (text, messageId, endState = 'idle') => {
    setBotState('typing')
    let currentText = ''
    setMessages(prev => [...prev, { id: messageId, type: 'bot', text: '', telemetry: false }])
    for (let i = 0; i < text.length; i++) {
      currentText += text[i]
      setMessages(prev => prev.map(msg => msg.id === messageId ? { ...msg, text: currentText } : msg))
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
    if (openclaw.isConnected()) { openclaw.sendDAWCommand(action) }
  }

  const lastBotMsgId = useMemo(() => {
    const botMsgs = messages.filter(m => m.type === 'bot')
    return botMsgs.length > 0 ? botMsgs[botMsgs.length - 1].id : null
  }, [messages])

  const sideModules = [
    { id: 'ai', icon: 'memory', label: 'AI ENGINE' },
    { id: 'transport', icon: 'play_arrow', label: 'TRANSPORT' },
    { id: 'comp', icon: 'settings_input_component', label: 'COMP' },
    { id: 'limit', icon: 'linear_scale', label: 'LIMIT' },
    { id: 'eq', icon: 'graphic_eq', label: 'EQ' },
    { id: 'meters', icon: 'analytics', label: 'METERS' }
  ]

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
          <div className="flex flex-col items-end">
            <span>SR: 96kHz</span>
            <span>BUF: 512</span>
          </div>
          <div className="flex flex-col items-end border-l border-[#222222] pl-4">
            <span>CPU: 12%</span>
            <span>LATENCY: 4.2ms</span>
          </div>
          <div className="flex items-center gap-2 ml-4">
            <div className={`w-2 h-2 rounded-full ${connectionStatus === ConnectionState.CONNECTED ? 'bg-[#00FFaa] led-amber-active' : 'bg-[#DC143C] led-red-active'}`}></div>
            <span>{connectionStatus === ConnectionState.CONNECTED ? 'CONNECTED' : connectionStatus === ConnectionState.CONNECTING ? 'CONNECTING...' : 'DISCONNECTED'}</span>
          </div>
          <div className="flex gap-3 ml-2">
            <span className="material-symbols-outlined text-sm text-[#4d4d4d] hover:text-[#FFB000] cursor-pointer">settings</span>
            <span className="material-symbols-outlined text-sm text-[#4d4d4d] hover:text-[#FFB000] cursor-pointer">power_settings_new</span>
          </div>
        </div>
      </header>

      {/* Side Module */}
      <aside className="fixed left-0 top-12 h-[calc(100vh-3rem)] w-20 flex flex-col items-center py-4 bg-[#131313] border-r border-[#222222] z-40">
        <div className="text-[10px] font-mono text-[#4d4d4d] uppercase mb-8 rotate-180" style={{ writingMode: 'vertical-lr' }}>MODULES // V1.0.4-STABLE</div>
        <div className="flex flex-col w-full">
          {sideModules.map(mod => (
            <div
              key={mod.id}
              className={`py-4 flex flex-col items-center gap-1 cursor-pointer transition-all ${
                activeSideModule === mod.id
                  ? 'text-[#DC143C] bg-[#1a1a1a] border-l-4 border-[#DC143C]'
                  : 'text-[#4d4d4d] hover:text-[#FFB000] hover:bg-[#1a1a1a]'
              }`}
              onClick={() => {
                setActiveSideModule(mod.id)
                if (mod.id === 'transport') handleTransport('play')
              }}
            >
              <span className="material-symbols-outlined text-xl">{mod.icon}</span>
              <span className="font-['Space_Grotesk'] font-mono text-[10px] uppercase">{mod.label}</span>
            </div>
          ))}
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

          <div className="flex-1 p-6 font-mono overflow-y-auto custom-scrollbar space-y-6">
            {messages.map((msg) => {
              if (msg.type === 'system') {
                return (
                  <div key={msg.id} className="opacity-40 text-[10px] font-mono">
                    {msg.text?.split('...')[0]}... <span className="text-[#FFB000]">{msg.text?.split('...')[1]}</span>
                  </div>
                )
              }

              if (msg.type === 'advisory') {
                return (
                  <div key={msg.id || 'advisory'} className="animate-advisory-in group relative mt-4 mb-6">
                    <div className="absolute -top-4 -right-2 opacity-5 pointer-events-none select-none parallax-item">
                      <pre className="text-[8px] leading-tight text-white font-mono">
                        0x45 0x21 0x88{'\n'}
                        [TRANS_LOCK]{'\n'}
                        0xFF 0x00 0x12
                      </pre>
                    </div>
                    <div className="advisory-card advisory-breathe bg-[#121212] border border-[#DC143C]/40 p-5 relative font-mono transition-all duration-500 hover:bg-[#161616] group-hover:scale-[1.005]">
                      <div className="absolute top-2 right-4 flex items-end gap-[2px] h-6 opacity-40 parallax-item">
                        <div className="w-[2px] h-[40%] bg-[#DC143C]"></div>
                        <div className="w-[2px] h-[70%] bg-[#DC143C]"></div>
                        <div className="w-[2px] h-[55%] bg-[#DC143C]"></div>
                        <div className="w-[2px] h-[90%] bg-[#DC143C]"></div>
                        <div className="w-[2px] h-[65%] bg-[#DC143C]"></div>
                      </div>
                      <div className="flex justify-between items-start mb-4 border-b border-[#222222] pb-2">
                        <div className="flex items-center gap-3">
                          <div className="w-1.5 h-6 bg-[#DC143C] relative overflow-hidden">
                            <div className="absolute inset-0 bg-white/20 animate-pulse"></div>
                          </div>
                          <div>
                            <span className="text-[#DC143C] text-[11px] font-bold tracking-tighter uppercase block">AI_MASTERING_ADVISORY_CORE</span>
                            <span className="text-[8px] text-[#4d4d4d] tracking-[0.2em]">NODE: CREMISI_X9</span>
                          </div>
                        </div>
                        <div className="flex flex-col items-end">
                          <span className="text-[9px] text-white/40 font-bold uppercase tracking-widest">Priority: <span className="text-[#DC143C]">High</span></span>
                          <span className="text-[7px] text-[#4d4d4d] font-mono mt-0.5">ID: #B87-FF01</span>
                        </div>
                      </div>
                      <div className="text-sm leading-relaxed text-[#FFB000] space-y-5 relative">
                        <p className="leading-relaxed drop-shadow-[0_0_8px_rgba(255,176,0,0.1)]">
                          Detected significant harmonic crowding in the <span className="text-white border-b border-[#DC143C]/40 px-1 font-bold">200Hz - 400Hz</span> range.
                          Suggesting a surgical dynamic dip of <span className="bg-[#DC143C] text-white px-1.5 font-bold">-2.4dB</span> to increase spectral clarity.
                          Transient preservation currently at <span className="text-white underline decoration-dotted decoration-[#4d4d4d]">84%</span>.
                        </p>
                        <div className="flex gap-4 opacity-30 text-[8px] font-mono parallax-item">
                          <span>HEX: 0xDC143C</span>
                          <span>VAL: -2.4dB_COR</span>
                          <span>SIG: 0.982</span>
                          <span>LAT: 0.2ms</span>
                        </div>
                        <div className="flex flex-wrap gap-4 pt-2">
                          <button
                            className="group/btn relative bg-[#DC143C] text-white px-5 py-2 text-[10px] font-bold uppercase overflow-hidden transition-all duration-300 active:scale-95 hover:shadow-[0_0_20px_rgba(220,20,60,0.6)] hover:bg-white hover:text-[#DC143C] tracking-widest border border-transparent disabled:opacity-50 disabled:cursor-not-allowed"
                            onClick={executeSuggestedChain}
                            disabled={botState !== 'idle'}
                          >
                            <span className="relative z-10 flex items-center gap-2">
                              <span className="material-symbols-outlined text-[14px]">bolt</span>
                              EXECUTE SUGGESTED CHAIN
                            </span>
                          </button>
                          <button className="group/btn border border-[#4d4d4d] text-[#4d4d4d] px-5 py-2 text-[10px] font-bold uppercase transition-all duration-300 active:scale-95 hover:border-[#FFB000] hover:text-[#FFB000] hover:shadow-[0_0_15px_rgba(255,176,0,0.2)] tracking-widest">
                            ANALYZE FURTHER
                          </button>
                          <button className="text-[#4d4d4d] hover:text-white transition-colors text-[9px] font-bold uppercase self-center hover:underline decoration-[#DC143C]">
                            DISMISS
                          </button>
                        </div>
                      </div>
                    </div>
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

                        {msg.telemetry && (
                          <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 bg-[#0d0d0d]/50 p-3 border border-[#222222]">
                            <div className="space-y-1">
                              <div className="flex justify-between text-[9px] uppercase font-bold">
                                <span className="text-[#4d4d4d]">L/R Balance</span>
                                <span className="text-[#DC143C]">52% R</span>
                              </div>
                              <div className="h-1.5 bg-[#222222] w-full relative">
                                <div className="absolute left-1/2 -translate-x-1/2 w-[1px] h-full bg-[#4d4d4d] z-10"></div>
                                <div className="h-full bg-[#DC143C] w-[52%] transition-all duration-1000" style={{ marginLeft: '48%' }}></div>
                              </div>
                            </div>
                            <div className="space-y-1">
                              <div className="flex justify-between text-[9px] uppercase font-bold">
                                <span className="text-[#4d4d4d]">Side Content</span>
                                <span className="text-[#FFB000]">38.4%</span>
                              </div>
                              <div className="h-1.5 bg-[#222222] w-full">
                                <div className="h-full bg-[#FFB000] w-[38.4%] transition-all duration-1000 shadow-[0_0_5px_rgba(255,176,0,0.4)]"></div>
                              </div>
                            </div>
                          </div>
                        )}
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
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]">mic</span>
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]">attachment</span>
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000] ml-2" onClick={() => handleTransport('play')}>play_arrow</span>
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]" onClick={() => handleTransport('stop')}>stop</span>
              <span className="material-symbols-outlined text-lg cursor-pointer hover:text-[#FFB000]" onClick={() => handleTransport('record')}>fiber_manual_record</span>
            </div>
          </div>
        </section>

        {/* Mastering Rack */}
        <section className="col-span-3 row-span-4 bg-[#131313] border border-[#222222] flex flex-col">
          <div className="bg-[#1a1a1a] px-4 py-2 border-b border-[#222222]">
            <span className="text-[10px] font-bold text-[#DC143C] tracking-widest uppercase">MASTERING RACK</span>
          </div>
          <div className="flex-1 flex p-4 gap-6">
            {/* Precision Gain Slider */}
            <div className="w-16 flex flex-col items-center gap-2">
              <span className="text-[8px] font-mono text-[#FFB000]">+12</span>
              <div className="flex-1 w-8 bg-[#0e0e0e] border border-[#222222] relative flex flex-col justify-end p-1">
                <div className="absolute inset-x-0 h-[1px] bg-[#4d4d4d] opacity-20" style={{ top: '10%' }}></div>
                <div className="absolute inset-x-0 h-[1px] bg-[#4d4d4d] opacity-20" style={{ top: '25%' }}></div>
                <div className="absolute inset-x-0 h-[1px] bg-[#4d4d4d] opacity-20" style={{ top: '50%' }}></div>
                <div className="absolute inset-x-0 h-[1px] bg-[#4d4d4d] opacity-20" style={{ top: '75%' }}></div>
                <div className="w-full h-[60%] bg-gradient-to-t from-[#DC143C]/20 to-[#DC143C] border-t-2 border-[#ffb3b3] shadow-[0_0_10px_rgba(220,20,60,0.5)] smooth-transition"></div>
                <div className="absolute bottom-[60%] left-[-10px] right-[-10px] h-4 bg-[#e5e2e1] cursor-ns-resize shadow-lg z-10 smooth-transition"></div>
              </div>
              <span className="text-[8px] font-mono text-[#FFB000]">-INF</span>
              <span className="text-[10px] font-bold text-[#e5e2e1] mt-2">GAIN</span>
            </div>
            {/* Controls Cluster */}
            <div className="flex-1 flex flex-col gap-6">
              <div className="space-y-4">
                <div className="flex items-center justify-between">
                  <span className="text-[9px] font-bold uppercase text-[#4d4d4d]">Deep Neural</span>
                  <div className="w-4 h-4 bg-[#1a1a1a] border border-[#DC143C]/30 flex items-center justify-center cursor-pointer" onClick={() => setBotState(botState === 'idle' ? 'thinking' : 'idle')}>
                    <div className={`w-2 h-2 rounded-full ${botState !== 'idle' ? 'bg-[#DC143C] led-red-active' : 'bg-[#222222]'}`}></div>
                  </div>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-[9px] font-bold uppercase text-[#4d4d4d]">Master Chain</span>
                  <div className="w-4 h-4 bg-[#1a1a1a] border border-[#4d4d4d] flex items-center justify-center cursor-pointer">
                    <div className="w-2 h-2 rounded-full bg-[#222222]"></div>
                  </div>
                </div>
              </div>
              <div className="flex-1 border-t border-[#222222] pt-4 flex flex-col gap-4">
                <div className="flex flex-col gap-1">
                  <span className="text-[8px] text-[#FFB000] uppercase opacity-60">Saturation Type</span>
                  <div className="bg-[#0e0e0e] border border-[#222222] px-2 py-1 text-[10px] text-[#FFB000] flex justify-between items-center cursor-pointer hover:border-[#FFB000]/40 transition-colors">
                    <span>CRIMSON_TUBE</span>
                    <span className="material-symbols-outlined text-[14px]">expand_more</span>
                  </div>
                </div>
                <div className="flex-1 flex flex-col items-center justify-center relative">
                  <svg className="w-24 h-24 rotate-[-90deg]" viewBox="0 0 100 100">
                    <circle cx="50" cy="50" fill="none" r="40" stroke="#222222" strokeWidth="4"></circle>
                    <circle className="knob-segment" cx="50" cy="50" fill="none" r="40" stroke="#DC143C" strokeDasharray="180 251" strokeWidth="4"></circle>
                  </svg>
                  <div className="absolute flex flex-col items-center">
                    <span className="text-[12px] font-bold text-[#e5e2e1]">72.5</span>
                    <span className="text-[8px] text-[#FFB000] uppercase font-mono">DRIVE</span>
                  </div>
                </div>
              </div>
            </div>
          </div>
          {/* Peak Meter */}
          <div className="h-12 bg-[#0e0e0e] border-t border-[#222222] flex items-center px-4 gap-2">
            <div className="flex-1 h-2 bg-[#1a1a1a] flex gap-[1px]">
              <div className="flex-1 h-full bg-[#FFB000]/40 smooth-transition"></div>
              <div className="flex-1 h-full bg-[#FFB000]/40 smooth-transition"></div>
              <div className="flex-1 h-full bg-[#FFB000]/60 smooth-transition"></div>
              <div className="flex-1 h-full bg-[#FFB000]/80 smooth-transition"></div>
              <div className="flex-1 h-full bg-[#FFB000] smooth-transition"></div>
              <div className="flex-1 h-full bg-[#DC143C]/60 smooth-transition"></div>
              <div className="flex-1 h-full bg-[#DC143C] smooth-transition"></div>
              <div className="w-4 h-full bg-[#DC143C] shadow-[0_0_10px_#DC143C] animate-pulse smooth-transition"></div>
            </div>
            <span className="text-[10px] font-mono text-[#DC143C] font-bold">PEAK!</span>
          </div>
        </section>

        {/* Analog Vector Scope */}
        <section className="col-span-12 row-span-2 bg-[#0e0e0e] border border-[#222222] relative overflow-hidden flex flex-col">
          <div className="absolute top-2 left-4 z-10 flex items-center gap-4">
            <span className="text-[10px] font-bold text-[#FFB000] uppercase opacity-80">Vector Scope: STEREO_FIELD</span>
            <div className="h-[1px] w-48 bg-gradient-to-r from-[#FFB000] to-transparent"></div>
          </div>
          <div className="flex-1 relative">
            <div className="absolute inset-0">
              <div className="absolute inset-0 bg-gradient-to-b from-[#0d0d0d] via-[#0e0e0e] to-[#0d0d0d]"></div>
              <div className="absolute inset-0 opacity-20" style={{ backgroundImage: 'linear-gradient(#4d4d4d 0.5px, transparent 0.5px), linear-gradient(90deg, #4d4d4d 0.5px, transparent 0.5px)', backgroundSize: '40px 40px' }}></div>
              <div className="absolute inset-0 opacity-5" style={{ backgroundImage: 'linear-gradient(#4d4d4d 0.25px, transparent 0.25px), linear-gradient(90deg, #4d4d4d 0.25px, transparent 0.25px)', backgroundSize: '10px 10px' }}></div>
              <div className="absolute inset-0 overflow-hidden pointer-events-none">
                <div className="w-full h-[1px] bg-gradient-to-r from-transparent via-[#FFB000]/20 to-transparent absolute top-1/4 animate-[pulse_3s_infinite]"></div>
                <div className="w-full h-[1px] bg-gradient-to-r from-transparent via-[#DC143C]/20 to-transparent absolute top-3/4 animate-[pulse_4s_infinite]"></div>
              </div>
              <div className="absolute inset-0 flex items-center justify-center opacity-30 pointer-events-none">
                <div className="w-full h-[0.5px] bg-[#FFB000]"></div>
                <div className="h-full w-[0.5px] bg-[#FFB000] absolute"></div>
              </div>
              <div className="absolute bottom-2 right-4 flex gap-4 text-[8px] font-mono text-[#4d4d4d] uppercase">
                <span>Gain_Scale: Linear</span>
                <span>Res: 24-bit/96kHz</span>
              </div>
            </div>
            <svg className="w-full h-full" preserveAspectRatio="none">
              <path className="drop-shadow-[0_0_8px_rgba(220,20,60,0.4)]" d="M0,80 Q50,10 100,90 T200,40 T300,110 T400,20 T500,85 T600,45 T700,95 T800,35 T900,105 T1000,60" fill="none" stroke="#DC143C" strokeOpacity="0.8" strokeWidth="1.5"></path>
              <path className="drop-shadow-[0_0_4px_rgba(255,176,0,0.3)]" d="M0,75 Q55,15 105,85 T205,35 T305,115 T405,25 T505,80 T605,50 T705,90 T805,40 T905,100 T1005,65" fill="none" stroke="#FFB000" strokeOpacity="0.4" strokeWidth="0.8"></path>
            </svg>
          </div>
        </section>
      </main>

      {/* Footer */}
      <footer className="fixed bottom-0 left-0 w-full z-50 flex justify-between items-center h-10 border-t border-[#222222] bg-[#0d0d0d] px-6">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-2 px-4 text-[#FFB000] font-mono text-[12px]">
            <span className="material-symbols-outlined text-sm">equalizer</span>
            <span>LUFS: -14.2</span>
          </div>
          <div className="flex items-center gap-2 px-4 text-[#4d4d4d] font-mono text-[12px]">
            <span className="material-symbols-outlined text-sm">priority_high</span>
            <span>PEAK: -0.1dB</span>
          </div>
          <div className="flex items-center gap-2 px-4 text-[#4d4d4d] font-mono text-[12px]">
            <span className="material-symbols-outlined text-sm">speed</span>
            <span>RMS: -16.4</span>
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