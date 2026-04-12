import React, { useState, useEffect } from 'react'
import './Toolbox.css'
import WidgetSlider from './WidgetSlider'
import WidgetKnob from './WidgetKnob'
import WidgetButton from './WidgetButton'

function Toolbox({ lastMessage }) {
  const [widgets, setWidgets] = useState([])
  const [proposals, setProposals] = useState([])
  const [logs, setLogs] = useState([])

  // Ricevi messaggi dal plugin
  useEffect(() => {
    if (!lastMessage) return

    addLog(`Ricevuto: ${lastMessage.message_type}`)

    if (lastMessage.message_type === 'event') {
      // Genera proposta widget automaticamente
      const proposal = generateProposal(lastMessage)
      if (proposal) {
        setProposals(prev => [...prev, proposal])
        addLog(`Proposta generata: ${proposal.widget_config.type}`)
      }
    }
  }, [lastMessage])

  const addLog = (msg) => {
    setLogs(prev => [{ time: new Date().toLocaleTimeString(), msg }, ...prev].slice(0, 10))
  }

  const generateProposal = (message) => {
    const { event } = message
    if (!event) return null

    const id = `proposal-${Date.now()}`
    
    if (event.type === 'parameter_change' && event.category === 'volume') {
      return {
        id,
        message,
        widget_config: {
          type: 'slider',
          id: `widget-${Date.now()}`,
          label: `Volume ${event.target.track_name || `Track ${event.target.track_id}`}`,
          min: 0,
          max: 1,
          default: event.value?.raw || 0.5,
          param: event.target.param_name,
          track_id: event.target.track_id
        }
      }
    }

    if (event.type === 'parameter_change' && event.category === 'pan') {
      return {
        id,
        message,
        widget_config: {
          type: 'knob',
          id: `widget-${Date.now()}`,
          label: `Pan ${event.target.track_name || `Track ${event.target.track_id}`}`,
          min: -1,
          max: 1,
          default: event.value?.raw || 0,
          param: event.target.param_name,
          track_id: event.target.track_id
        }
      }
    }

    if (event.type === 'transport') {
      return {
        id,
        message,
        widget_config: {
          type: 'button',
          id: `widget-${Date.now()}`,
          label: 'Transport',
          action: event.target?.param_name || 'play'
        }
      }
    }

    return null
  }

  const acceptProposal = (proposal) => {
    setWidgets(prev => [...prev, proposal.widget_config])
    setProposals(prev => prev.filter(p => p.id !== proposal.id))
    addLog(`Widget aggiunto: ${proposal.widget_config.type}`)
    
    // Notifica plugin
    if (window.sendToPlugin) {
      window.sendToPlugin({
        message_type: 'command',
        toolbox_action: {
          type: 'add_widget',
          widget_id: proposal.widget_config.id,
          accepted: true
        }
      })
    }
  }

  const rejectProposal = (proposal) => {
    setProposals(prev => prev.filter(p => p.id !== proposal.id))
    addLog('Proposta rifiutata')
  }

  const removeWidget = (widgetId) => {
    setWidgets(prev => prev.filter(w => w.id !== widgetId))
    addLog(`Widget rimosso: ${widgetId}`)
  }

  const renderWidget = (widget) => {
    const props = { ...widget, onRemove: () => removeWidget(widget.id) }
    
    switch (widget.type) {
      case 'slider':
        return <WidgetSlider key={widget.id} {...props} />
      case 'knob':
        return <WidgetKnob key={widget.id} {...props} />
      case 'button':
        return <WidgetButton key={widget.id} {...props} />
      default:
        return null
    }
  }

  return (
    <div className="toolbox">
      <section className="proposals">
        <h2>📋 Proposte Widget</h2>
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
                <button className="btn-accept" onClick={() => acceptProposal(p)}>✓ Aggiungi</button>
                <button className="btn-reject" onClick={() => rejectProposal(p)}>✗ Ignora</button>
              </div>
            </div>
          ))
        )}
      </section>

      <section className="active-widgets">
        <h2>🎛️ Widget Attivi ({widgets.length})</h2>
        {widgets.length === 0 ? (
          <p className="empty">Nessun widget attivo. Accetta una proposta per aggiungerne uno.</p>
        ) : (
          <div className="widgets-grid">
            {widgets.map(renderWidget)}
          </div>
        )}
      </section>

      <section className="logs">
        <h2>📝 Log</h2>
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
