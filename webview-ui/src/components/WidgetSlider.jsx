import React, { useState } from 'react'
import './WidgetSlider.css'

function WidgetSlider({ id, label, min, max, default: defaultValue, onRemove }) {
  const [value, setValue] = useState(defaultValue || 0.5)

  const handleChange = (e) => {
    const newValue = parseFloat(e.target.value)
    setValue(newValue)
    
    // Invia al plugin C++
    if (window.sendToPlugin) {
      window.sendToPlugin({
        message_type: 'command',
        action: {
          type: 'set_parameter',
          target: { widget_id: id, param_name: 'volume' },
          value: newValue
        }
      })
    }
  }

  return (
    <div className="widget slider-widget">
      <div className="widget-header">
        <label>{label}</label>
        <button className="remove-btn" onClick={onRemove} title="Rimuovi">×</button>
      </div>
      <div className="widget-body">
        <input
          type="range"
          min={min || 0}
          max={max || 1}
          step={0.01}
          value={value}
          onChange={handleChange}
          className="slider-input"
        />
        <span className="value-display">{value.toFixed(2)}</span>
      </div>
    </div>
  )
}

export default WidgetSlider
