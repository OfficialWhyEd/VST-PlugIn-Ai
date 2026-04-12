/**
 * OpenClaw Bridge - JavaScript Bridge per comunicazione con C++
 * 
 * Da includere nel progetto React (webview-ui/src/)
 */

/**
 * Genera UUID v4
 */
function generateUUID() {
  return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
    const r = Math.random() * 16 | 0;
    const v = c === 'x' ? r : (r & 0x3 | 0x8);
    return v.toString(16);
  });
}

/**
 * Classe principale del bridge OpenClaw
 */
class OpenClawBridge {
  constructor() {
    this.messageListeners = new Map();
    this.pendingRequests = new Map();
    this.isInitialized = false;
    this.version = '1.0.0';
    
    // Bind metodi
    this.receiveMessage = this.receiveMessage.bind(this);
    this.sendMessage = this.sendMessage.bind(this);
    
    // Setup listener globale
    this._setupGlobalListener();
  }
  
  /**
   * Inizializza il bridge
   */
  init() {
    if (this.isInitialized) return;
    
    // Esponi globalmente per C++
    window.__openclawBridge = {
      receiveMessage: this.receiveMessage,
      sendMessage: this.sendMessage
    };
    
    this.isInitialized = true;
    console.log('[OpenClaw] Bridge v' + this.version + ' inizializzato');
    
    // Notifica C++ che siamo pronti
    this.sendMessage('plugin.init', {
      version: this.version,
      capabilities: ['widgets', 'ai', 'osc']
    });
  }
  
  /**
   * Riceve messaggi da C++
   */
  receiveMessage(jsonString) {
    try {
      const message = JSON.parse(jsonString);
      this._handleMessage(message);
    } catch (e) {
      console.error('[OpenClaw] Errore parsing messaggio:', e);
    }
  }
  
  /**
   * Invia messaggio a C++
   */
  sendMessage(type, payload = {}, options = {}) {
    const id = options.id || generateUUID();
    const message = {
      type,
      id,
      timestamp: Date.now(),
      payload
    };
    
    // Se c'è un callback per risposta, salva
    if (options.onResponse) {
      this.pendingRequests.set(id, {
        callback: options.onResponse,
        timeout: options.timeout || 30000,
        timer: setTimeout(() => {
          this.pendingRequests.delete(id);
          options.onResponse({ error: 'Timeout' });
        }, options.timeout || 30000)
      });
    }
    
    // Invia a C++ via location.href
    const jsonStr = JSON.stringify(message);
    window.location.href = 'app://message/' + encodeURIComponent(jsonStr);
    
    return id;
  }
  
  /**
   * Registra listener per un tipo di messaggio
   */
  on(type, callback) {
    if (!this.messageListeners.has(type)) {
      this.messageListeners.set(type, []);
    }
    this.messageListeners.get(type).push(callback);
    
    // Ritorna funzione per unsubscribe
    return () => {
      const listeners = this.messageListeners.get(type);
      const index = listeners.indexOf(callback);
      if (index > -1) {
        listeners.splice(index, 1);
      }
    };
  }
  
  /**
   * Rimuovi tutti i listener
   */
  off(type) {
    if (type) {
      this.messageListeners.delete(type);
    } else {
      this.messageListeners.clear();
    }
  }
  
  /**
   * Richiede info al DAW
   */
  requestDAWInfo(requestType, targetId = null) {
    return new Promise((resolve, reject) => {
      const id = this.sendMessage('daw.request', {
        request: requestType,
        trackId: targetId
      }, {
        onResponse: (response) => {
          if (response.error) {
            reject(response.error);
          } else {
            resolve(response);
          }
        },
        timeout: 5000
      });
    });
  }
  
  /**
   * Invia comando al DAW
   */
  sendDAWCommand(command, params = {}) {
    return this.sendMessage('daw.command', {
      command,
      ...params
    });
  }
  
  /**
   * Invia prompt all'AI
   */
  sendAIPrompt(prompt, options = {}) {
    const { provider = 'ollama', model = 'llama3.2', stream = false, context = {} } = options;
    
    return this.sendMessage('ai.prompt', {
      prompt,
      provider,
      model,
      stream,
      context
    }, {
      onResponse: options.onResponse,
      timeout: 60000
    });
  }
  
  /**
   * Crea widget dinamico
   */
  createWidget(widgetType, title, config = {}) {
    const widgetId = config.widgetId || generateUUID();
    return this.sendMessage('ui.widget.create', {
      widgetType,
      title,
      widgetId,
      ...config
    });
  }
  
  /**
   * Aggiorna widget
   */
  updateWidget(widgetId, values) {
    return this.sendMessage('ui.widget.update', {
      widgetId,
      ...values
    });
  }
  
  /**
   * Rimuovi widget
   */
  removeWidget(widgetId) {
    return this.sendMessage('ui.widget.remove', { widgetId });
  }
  
  /**
   * Invia messaggio OSC raw
   */
  sendOSC(address, value, valueType = 'float') {
    return this.sendMessage('osc.send', {
      address,
      value,
      valueType
    });
  }
  
  /**
   * Setup listener globale via CustomEvent
   * @private
   */
  _setupGlobalListener() {
    window.addEventListener('openclaw-message', (event) => {
      this._handleMessage(event.detail);
    });
  }
  
  /**
   * Gestisce un messaggio ricevuto
   * @private
   */
  _handleMessage(message) {
    const { type, id, payload } = message;
    
    // Gestisci risposte pendenti
    if (id && this.pendingRequests.has(id)) {
      const pending = this.pendingRequests.get(id);
      clearTimeout(pending.timer);
      pending.callback(payload);
      this.pendingRequests.delete(id);
    }
    
    // Chiama listeners registrati
    const listeners = this.messageListeners.get(type);
    if (listeners) {
      listeners.forEach(callback => {
        try {
          callback(payload, message);
        } catch (e) {
          console.error('[OpenClaw] Errore in listener:', e);
        }
      });
    }
    
    // Log per debug
    console.log('[OpenClaw] Ricevuto:', type, payload);
  }
}

// Esporta singleton
export const openclaw = new OpenClawBridge();

// Hook React
export function useOpenClaw() {
  const { useState, useEffect, useCallback } = require('react');
  
  const [transport, setTransport] = useState({
    isPlaying: false,
    isRecording: false,
    bpm: 120,
    positionSeconds: 0
  });
  
  const [tracks, setTracks] = useState([]);
  const [widgets, setWidgets] = useState([]);
  const [aiStatus, setAiStatus] = useState('idle');
  
  useEffect(() => {
    // Inizializza bridge
    openclaw.init();
    
    // Registra listeners
    const unsubTransport = openclaw.on('daw.transport', (payload) => {
      setTransport(payload);
    });
    
    const unsubTrack = openclaw.on('daw.track', (payload) => {
      setTracks(prev => {
        const index = prev.findIndex(t => t.trackId === payload.trackId);
        if (index >= 0) {
          const updated = [...prev];
          updated[index] = payload;
          return updated;
        }
        return [...prev, payload];
      });
    });
    
    const unsubWidget = openclaw.on('ui.widget.create', (payload) => {
      setWidgets(prev => [...prev, payload]);
    });
    
    const unsubWidgetUpdate = openclaw.on('ui.widget.update', (payload) => {
      setWidgets(prev => prev.map(w => 
        w.widgetId === payload.widgetId ? { ...w, ...payload } : w
      ));
    });
    
    const unsubWidgetRemove = openclaw.on('ui.widget.remove', (payload) => {
      setWidgets(prev => prev.filter(w => w.widgetId !== payload.widgetId));
    });
    
    const unsubAI = openclaw.on('ai.response', () => {
      setAiStatus('complete');
    });
    
    const unsubAIStream = openclaw.on('ai.stream', () => {
      setAiStatus('streaming');
    });
    
    const unsubError = openclaw.on('plugin.error', (payload) => {
      console.error('[OpenClaw] Errore:', payload);
    });
    
    // Cleanup
    return () => {
      unsubTransport();
      unsubTrack();
      unsubWidget();
      unsubWidgetUpdate();
      unsubWidgetRemove();
      unsubAI();
      unsubAIStream();
      unsubError();
    };
  }, []);
  
  const sendCommand = useCallback((type, payload) => {
    return openclaw.sendMessage(type, payload);
  }, []);
  
  const askAI = useCallback((prompt, options = {}) => {
    setAiStatus('thinking');
    return openclaw.sendAIPrompt(prompt, options);
  }, []);
  
  return {
    transport,
    tracks,
    widgets,
    aiStatus,
    sendCommand,
    askAI,
    createWidget: openclaw.createWidget.bind(openclaw),
    updateWidget: openclaw.updateWidget.bind(openclaw),
    removeWidget: openclaw.removeWidget.bind(openclaw),
    sendOSC: openclaw.sendOSC.bind(openclaw),
    requestDAWInfo: openclaw.requestDAWInfo.bind(openclaw)
  };
}

export default openclaw;