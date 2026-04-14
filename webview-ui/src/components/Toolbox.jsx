import React, { useState, useEffect } from 'react'
import './Toolbox.css'
import WidgetSlider from './WidgetSlider'
import WidgetKnob from './WidgetKnob'
import WidgetButton from './WidgetButton'
import { openclaw } from '../openclaw-bridge.js'

function Toolbox({ lastMessage, addLog }) {
  const [widgets, setWidgets] = useState([])
  const [proposals, setProposals] = useState([])
  const [logs, setLocalLogs] = useState([])

  const addLocalLog = (msg) => {
    setLocalLogs(prev => [{ time: new Date().toLocaleTimeString(), msg }, ...prev].slice(0, 10))
    if (addLog) addLog(msg)
  }

  useEffect(() => {
    if (!lastMessage) return

    const type = lastMessage.type || lastMessage.message_type || 'unknown'
    addLocalLog(`Ricevuto: ${type}`)

    if (type === 'daw.transport') {
      addLocalLog(`Transport: play=${lastMessage.isPlaying} rec=${lastMessage.isRecording} bpm=${lastMessage.bpm}`)
    }
    else if (type === 'daw.param_updated' || type === 'osc') {
      const proposal = generateProposal(lastMessage)
      if (proposal) {
        setProposals(prev => [...prev, proposal])
        addLocalLog(`Proposta: ${proposal.widget_config.type}`)
      }
    }
  }, [lastMessage])

  const generateProposal = (message) => {
    const id = `proposal-${Date.now()}`

    if (message.type === 'osc') {
      const addr = message.address || ''
      const value = message.value ?? 0.5
      if (addr.includes('volume') || addr.includes('gain')) {
        return { id, message, widget_config: { type: 'slider', id: `widget-${Date.now()}`, label: addr, min: 0, max: 1, default: value, address: addr } }
      }
      if (addr.includes('pan')) {
        return { id, message, widget_config: { type: 'knob', id: `widget-${Date.now()}`, label: addr, min: -1, max: 1, default: value, address: addr } }
      }
      if (addr.includes('play') || addr.includes('stop') || addr.includes('record')) {
        return { id, message, widget_config: { type: 'button', id: `widget-${Date.now()}`, label: addr, address: addr } }
      }
    }

    if (message.event) {
      const { event } = message
      if (event.type === 'parameter_change' && event.category === 'volume') {
        return { id, message, widget_config: { type: 'slider', id: `widget-${Date.now()}`, label: `Volume ${event.target?.track_name || event.target?.track_id || ''}`, min: 0, max: 1, default: event.value?.raw || 0.5, param: event.target?.param_name, track_id: event.target?.track_id } }
      }
      if (event.type === 'parameter_change' && event.category === 'pan') {
        return { id, message, widget_config: { type: 'knob', id: `widget-${Date.now()}`, label: `Pan ${event.target?.track_name || event.target?.track_id || ''}`, min: -1, max: 1, default: event.value?.raw || 0, param: event.target?.param_name, track_id: event.target?.track_id } }
      }
      if (event.type === 'transport') {
        return { id, message, widget_config: { type: 'button', id: `widget-${Date.now()}`, label: 'Transport', action: event.target?.param_name || 'play' } }
      }
    }

    return null
  }

  const acceptProposal = (proposal) => {
    setWidgets(prev => [...prev, proposal.widget_config])
    setProposals(prev => prev.filter(p => p.id !== proposal.id))
    addLocalLog(`Widget aggiunto: ${proposal.widget_config.type}`)
  }

  const handleWidgetChange = (widgetId, value) => {
    const widget = widgets.find(w => w.id === widgetId)
    if (!widget) return

    if (openclaw.isConnected()) {
      if (widget.address) {
        openclaw.send({ type: 'osc.send', address: widget.address, value: value })
      } else if (widget.param) {
        openclaw.sendDAWCommand('setVolume', { trackId: widget.track_id, param: widget.param, value })
      }
      addLocalLog(`Inviato: ${widget.label} = ${value.toFixed(2)}`)
    }
  }

  const handleTransportCommand = (action) => {
    if (openclaw.isConnected()) {
      openclaw.sendDAWCommand(action)
      addLocalLog(`Transport: ${action}`)
    }
  }

  const removeWidget = (widgetId) => {
    setWidgets(prev => prev.filter(w => w.id !== widgetId))
    addLocalLog(`Widget rimosso: ${widgetId}`)
  }

  const addTestWidget = (type) => {
    const configs = {
      slider: { type: 'slider', id: `widget-${Date.now()}`, label: 'Volume Test', min: 0, max: 1, default: 0.5, address: '/track/1/volume' },
      knob: { type: 'knob', id: `widget-${Date.now()}`, label: 'Pan Test', min: -1, max: 1, default: 0, address: '/track/1/pan' },
      button: { type: 'button', id: `widget-${Date.now()}`, label: 'Play/Stop', address: '/transport/play' }
    }
    setWidgets(prev => [...prev, configs[type]])
    addLocalLog(`Widget test aggiunto: ${type}`)
  }

  const renderWidget = (widget) => {
    const props = { ...widget, onRemove: () => removeWidget(widget.id), onChange: (v) => handleWidgetChange(widget.id, v), onAction: () => handleTransportCommand(widget.action || 'play') }

    switch (widget.type) {
      case 'slider': return <WidgetSlider key={widget.id} {...props} />
      case 'knob': return <WidgetKnob key={widget.id} {...props} />
      case 'button': return <WidgetButton key={widget.id} {...props} />
      default: return null
    }
  }

  return (
    <div className="toolbox">
      <section className="test-controls">
        <h2>Test</h2>
        <button className="btn-test" onClick={() => addTestWidget('slider')}>+ Slider</button>
        <button className="btn-test" onClick={() => addTestWidget('knob')}>+ Knob</button>
        <button className="btn-test" onClick={() => addTestWidget('button')}>+ Button</button>
        <div style={{marginTop: '0.5rem'}}>
          <button className="btn-test" onClick={() => { openclaw.sendDAWCommand('play'); addLocalLog('Sent: play') }}>Play</button>
          <button className="btn-test" onClick={() => { openclaw.sendDAWCommand('stop'); addLocalLog('Sent: stop') }}>Stop</button>
          <button className="btn-test" onClick={() => { openclaw.sendDAWCommand('record'); addLocalLog('Sent: record') }}>Rec</button>
        </div>
      </section>

      <section className="proposals">
        <h2>Proposte Widget</h2>
        {proposals.length === 0 ? (
          <p className="empty">Nessuna proposta. Attendo eventi dal DAW...</p>
        ) : (
          proposals.map(p => (
            <div key={p.id} className="proposal-card">
              <div className="proposal-info">
                <strong>{p.widget_config.label}</strong>
                <span className="type">{p.widget_config.type}</span>
              </div>
              <div className="proposal-actions">
                <button className="btn-accept" onClick={() => acceptProposal(p)}>Aggiungi</button>
                <button className="btn-reject" onClick={() => setProposals(prev => prev.filter(x => x.id !== p.id))}>Ignora</button>
              </div>
            </div>
          ))
        )}
      </section>

      <section className="active-widgets">
        <h2>Widget Attivi ({widgets.length})</h2>
        {widgets.length === 0 ? (
          <p className="empty">Nessun widget attivo. Usa i pulsanti Test sopra.</p>
        ) : (
          <div className="widgets-grid">
            {widgets.map(renderWidget)}
          </div>
        )}
      </section>

      <section className="logs">
        <h2>Log</h2>
        <div className="log-entries">
          {logs.map((log, i) => (
            <div key={i} className="log-entry">
              <span className="time">{log.time}</span>
              <span className="msg">{log.msg}</span>
            </div>
          ))}
        </div>
      </section>
    </div>
  )
}

export default Toolbox
