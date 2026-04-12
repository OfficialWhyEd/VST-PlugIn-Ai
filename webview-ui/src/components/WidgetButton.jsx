import React, { useState } from 'react'
import './WidgetButton.css'

function WidgetButton({ id, label, action, onRemove }) {
  const [isActive, setIsActive] = useState(false)

  const handleClick = () => {
    setIsActive(!isActive)
    
    if (window.sendToPlugin) {
      window.sendToPlugin({
        message_type: 'command',
        action: {
          type: 'trigger_transport',
          target: { widget_id: id, action: action || 'play' },
          value: !isActive
        }
      })
    }
  }

  return (
    <div className="widget button-widget">
      <div className="widget-header">
        <label>{label}</label>
        <button className="remove-btn" onClick={onRemove} title="Rimuovi">×</button>
      </div>
      <div className="widget-body">
        <button 
          className={`action-btn ${isActive ? 'active' : ''}`}
          onClick={handleClick}
        >
          {isActive ? '⏹ Stop' : '▶ Play'}
        </button>
      </div>
    </div>
  )
}

export default WidgetButton
