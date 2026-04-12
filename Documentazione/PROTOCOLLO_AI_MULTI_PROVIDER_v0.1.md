# Protocollo AI — Multi-Provider API

**Versione:** 0.1  
**Data:** 2026-04-12  
**Scopo:** Unificare chiamate a Ollama, OpenAI, Anthropic, Google, DeepSeek, Z.ai

---

## 🎯 Obiettivo

Il plugin **non sa** quale AI sta usando.  
Invia una richiesta in un formato standard → il bridge la traduce per il provider specifico.

---

## 🔌 Provider Supportati

| Provider | Tipo | Auth | Endpoint Esempio |
|----------|------|------|------------------|
| **Ollama** | Locale | No | `http://localhost:11434/api/generate` |
| **OpenAI** | Cloud | API Key | `https://api.openai.com/v1/chat/completions` |
| **Anthropic** | Cloud | API Key | `https://api.anthropic.com/v1/messages` |
| **Google Gemini** | Cloud | API Key | `https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent` |
| **DeepSeek** | Cloud | API Key | `https://api.deepseek.com/v1/chat/completions` |
| **Z.ai** | Cloud | API Key | `https://api.z.ai/v1/chat/completions` |

---

## 📐 Formato Richiesta Standard (Plugin → Bridge)

```json
{
  "message_type": "ai_request",
  "timestamp": 1715505600.123,
  "request_id": "uuid-v4-unico",
  "context": {
    "provider": "ollama|openai|anthropic|gemini|deepseek|zai",
    "model": "llama3|gpt-4|claude-3-opus|gemini-pro|deepseek-chat",
    "system_prompt": "Sei un assistente..."
  },
  "conversation": {
    "session_id": "plugin-session-001",
    "history": [
      {"role": "user", "content": "Analizza questo pattern"},
      {"role": "assistant", "content": "Ecco l'analisi..."}
    ]
  },
  "current_message": {
    "role": "user",
    "content": "Rilevato volume che sale su Track 1. Cosa suggerisci?"
  },
  "tools": [
    {
      "name": "suggest_widget",
      "description": "Suggerisce un widget UI",
      "parameters": {
        "type": "object",
        "properties": {
          "widget_type": {"enum": ["slider", "knob", "button"]},
          "label": {"type": "string"},
          "reasoning": {"type": "string"}
        }
      }
    }
  ]
}
```

---

## 📤 Formato Risposta Standard (Bridge → Plugin)

```json
{
  "message_type": "ai_response",
  "timestamp": 1715505605.456,
  "request_id": "uuid-v4-unico",
  "status": "success|error|timeout",
  "provider_used": "ollama",
  "model_used": "llama3",
  "response": {
    "content": "Suggerisco di aggiungere uno slider per controllare il volume...",
    "tool_calls": [
      {
        "name": "suggest_widget",
        "arguments": {
          "widget_type": "slider",
          "label": "Track 1 Volume",
          "reasoning": "Valore modificato frequentemente"
        }
      }
    ]
  },
  "usage": {
    "input_tokens": 150,
    "output_tokens": 80,
    "total_tokens": 230,
    "cost_usd": 0.0
  },
  "latency_ms": 450
}
```

---

## 🔧 Configurazione Provider

### File: `ai_config.json`

```json
{
  "default_provider": "ollama",
  "providers": {
    "ollama": {
      "enabled": true,
      "base_url": "http://localhost:11434",
      "default_model": "llama3",
      "timeout_ms": 10000,
      "retry_attempts": 2
    },
    "openai": {
      "enabled": true,
      "api_key": "${OPENAI_API_KEY}",
      "base_url": "https://api.openai.com/v1",
      "default_model": "gpt-4-turbo-preview",
      "timeout_ms": 15000,
      "max_tokens": 4096
    },
    "anthropic": {
      "enabled": true,
      "api_key": "${ANTHROPIC_API_KEY}",
      "base_url": "https://api.anthropic.com/v1",
      "default_model": "claude-3-opus-20240229",
      "timeout_ms": 15000
    },
    "gemini": {
      "enabled": true,
      "api_key": "${GOOGLE_API_KEY}",
      "base_url": "https://generativelanguage.googleapis.com/v1beta",
      "default_model": "gemini-pro",
      "timeout_ms": 15000
    },
    "deepseek": {
      "enabled": true,
      "api_key": "${DEEPSEEK_API_KEY}",
      "base_url": "https://api.deepseek.com/v1",
      "default_model": "deepseek-chat",
      "timeout_ms": 15000
    },
    "zai": {
      "enabled": true,
      "api_key": "${ZAI_API_KEY}",
      "base_url": "https://api.z.ai/v1",
      "default_model": "zai-large",
      "timeout_ms": 15000
    }
  },
  "fallback_chain": ["ollama", "openai", "anthropic"]
}
```

---

## 🔄 Fallback Automatico

Se un provider fallisce, il bridge prova il prossimo:

```
1. User richiede Ollama
2. Ollama non risponde (timeout)
3. → Prova OpenAI (primo fallback)
4. OpenAI ok → Risponde
5. User viene avvisato: "Fallback usato: OpenAI"
```

---

## 🔒 Gestione API Keys

### Sicurezza
- **Mai** hardcodare API keys nel codice
- Usare variabili d'ambiente: `OPENAI_API_KEY`, `ANTHROPIC_API_KEY`, ecc.
- Su Linux/Mac: file `.env` o `~/.bashrc`
- Su Windows: variabili d'ambiente sistema

### Esempio `.env`
```bash
# ~/.openclaw_vst_env
export OPENAI_API_KEY="sk-..."
export ANTHROPIC_API_KEY="sk-ant-..."
export GOOGLE_API_KEY="..."
export DEEPSEEK_API_KEY="..."
export ZAI_API_KEY="..."
```

---

## 🌐 Traduzione Provider → Bridge

### Ollama
```
Richiesta JSON standard → POST /api/generate
Body: {
  "model": "llama3",
  "prompt": "...",
  "stream": false
}
```

### OpenAI
```
Richiesta JSON standard → POST /chat/completions
Body: {
  "model": "gpt-4",
  "messages": [...],
  "tools": [...]
}
```

### Anthropic
```
Richiesta JSON standard → POST /messages
Headers: {"x-api-key": "..."}
Body: {
  "model": "claude-3-opus",
  "messages": [...]
}
```

### Gemini
```
Richiesta JSON standard → POST /models/gemini-pro:generateContent
Query: ?key=API_KEY
Body: {"contents": [...]}
```

---

## ⚡ Timeout e Performance

| Provider | Timeout Default | Caratteristiche |
|----------|-----------------|-----------------|
| **Ollama** | 10s | Locale, veloce, no quota |
| **OpenAI** | 15s | Cloud, rate limit, costo |
| **Anthropic** | 15s | Cloud, rate limit, costo |
| **Gemini** | 15s | Cloud, free tier disponibile |
| **DeepSeek** | 15s | Cloud, costo basso |
| **Z.ai** | 15s | Cloud, dipende da piano |

---

## 🛠️ Tool Calling

Il protocollo supporta **function calling** (solo provider che lo supportano):

| Provider | Supporto Tool |
|----------|---------------|
| Ollama | ⚠️ Via JSON mode |
| OpenAI | ✅ Nativo |
| Anthropic | ✅ Nativo |
| Gemini | ⚠️ Function calling |
| DeepSeek | ⚠️ Via JSON mode |
| Z.ai | ❌ No (solo testo) |

**Fallback:** Se provider non supporta tools, il bridge estrae JSON dalla risposta testuale.

---

## 📊 Monitoraggio

Log da implementare:
```
[2026-04-12 10:00:01] REQUEST ollama/llama3 session-001
[2026-04-12 10:00:02] SUCCESS ollama/llama3 latency=450ms tokens=230
[2026-04-12 10:00:05] REQUEST openai/gpt-4 session-002
[2026-04-12 10:00:08] TIMEOUT openai/gpt-4
[2026-04-12 10:00:08] FALLBACK anthropic/claude-3
[2026-04-12 10:00:09] SUCCESS anthropic/claude-3 latency=890ms
```

---

## 🔌 Implementazione C++ (per Edo)

### Classe AiEngine

```cpp
class AiEngine {
public:
    struct Request {
        String provider;      // "ollama", "openai", ...
        String model;
        String message;
        var conversation_history;
    };
    
    struct Response {
        bool success;
        String content;
        String error;
        String provider_used;
        int latency_ms;
    };
    
    // Invia richiesta
    void sendRequest(const Request& req, 
                     std::function<void(const Response&)> callback);
    
private:
    // Traduce formato interno in formato provider
    String translateToProvider(const Request& req);
    
    // Gestisce risposta provider
    Response parseProviderResponse(const String& raw, const String& provider);
    
    // Fallback chain
    Array<String> fallback_chain = {"ollama", "openai", "anthropic"};
};
```

---

## 🧪 Test Multi-Provider

### Comandi curl per testare

**Ollama:**
```bash
curl http://localhost:11434/api/generate \
  -d '{"model":"llama3","prompt":"Ciao","stream":false}'
```

**OpenAI:**
```bash
curl https://api.openai.com/v1/chat/completions \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"model":"gpt-4","messages":[{"role":"user","content":"Ciao"}]}'
```

**Anthropic:**
```bash
curl https://api.anthropic.com/v1/messages \
  -H "x-api-key: $ANTHROPIC_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"model":"claude-3-opus","messages":[{"role":"user","content":"Ciao"}]}'
```

---

## ✅ Checklist Implementazione

- [ ] Caricare `ai_config.json` all'avvio
- [ ] Implementare translator per Ollama
- [ ] Implementare translator per OpenAI
- [ ] Implementare translator per Anthropic
- [ ] Implementare translator per Gemini
- [ ] Implementare translator per DeepSeek
- [ ] Implementare translator per Z.ai
- [ ] Gestione API keys da variabili d'ambiente
- [ ] Sistema fallback automatico
- [ ] Logging richieste/risposte
- [ ] Test con tutti i provider

---

## 📚 Riferimenti API

- **Ollama:** https://github.com/ollama/ollama/blob/main/docs/api.md
- **OpenAI:** https://platform.openai.com/docs/api-reference
- **Anthropic:** https://docs.anthropic.com/claude/reference/getting-started-with-the-api
- **Gemini:** https://ai.google.dev/tutorials/rest_quickstart
- **DeepSeek:** https://platform.deepseek.com/api-docs
- **Z.ai:** https://docs.z.ai/ (verificare URL)

---

*Protocollo creato per supportare qualsiasi provider AI senza cambiare codice del plugin.*
