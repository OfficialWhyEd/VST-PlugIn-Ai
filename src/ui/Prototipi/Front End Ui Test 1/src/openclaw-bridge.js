/**
 * WhyCremisi Bridge - Official Aura Protocol v1.0
 * Direct communication between React and JUCE C++
 */

export class OpenClawBridge {
  constructor() {
    this.messageListeners = new Map();
    this.isInitialized = false;
    
    // Bind receive function to window for JUCE access
    window.receiveFromPlugin = (jsonString) => {
      try {
        const message = JSON.parse(jsonString);
        this._handleMessage(message);
      } catch (e) {
        console.error('[Bridge] Error parsing JUCE message:', e);
      }
    };

    // Also support Aura's new naming convention
    window.__openclawBridge = {
      receiveMessage: window.receiveFromPlugin,
      sendMessage: (msg) => this.send(msg.type, msg.payload)
    };
  }

  init() {
    if (this.isInitialized) return;
    this.isInitialized = true;
    console.log('[Bridge] WhyCremisi Tactical Bridge Ready');
    this.send('plugin.init', { capabilities: ['vst3', 'ai', 'hybrid_ui'] });
  }

  send(type, payload = {}) {
    const message = {
      type,
      id: crypto.randomUUID(),
      timestamp: Date.now(),
      payload
    };
    
    // Aura Protocol: URL Interception
    const jsonStr = JSON.stringify(message);
    window.location.href = `app://message/${encodeURIComponent(jsonStr)}`;
  }

  on(type, callback) {
    if (!this.messageListeners.has(type)) {
      this.messageListeners.set(type, []);
    }
    this.messageListeners.get(type).push(callback);
    return () => {
      const listeners = this.messageListeners.get(type);
      const index = listeners.indexOf(callback);
      if (index > -1) listeners.splice(index, 1);
    };
  }

  _handleMessage(message) {
    const listeners = this.messageListeners.get(message.type);
    if (listeners) {
      listeners.forEach(cb => cb(message.payload, message));
    }
    // Global event dispatch for useOpenClaw hook
    window.dispatchEvent(new CustomEvent('openclaw-message', { detail: message }));
  }
}

export const bridge = new OpenClawBridge();
export default bridge;
