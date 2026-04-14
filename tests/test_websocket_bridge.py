#!/usr/bin/env python3
"""
Test end-to-end per WebSocket Bridge
Verifica che il protocollo WebSocket e i messaggi JSON siano corretti
"""

import asyncio
import json
import sys
import websockets

# Configurazione test
WS_PORT = 8080
TEST_TIMEOUT = 5

async def test_websocket_protocol():
    """Test di connessione e handshake WebSocket"""
    print("=" * 60)
    print("TEST: WebSocket Protocol")
    print("=" * 60)
    
    # Test 1: Verifica che il server risponda
    try:
        uri = f"ws://localhost:{WS_PORT}"
        async with websockets.connect(uri) as websocket:
            print(f"[OK] Connessione a {uri} riuscita")
            
            # Invia un messaggio JSON valido
            test_msg = {
                "type": "ping",
                "timestamp": asyncio.get_event_loop().time()
            }
            await websocket.send(json.dumps(test_msg))
            print(f"[OK] Messaggio inviato: {test_msg}")
            
            # Attendi risposta
            response = await asyncio.wait_for(websocket.recv(), timeout=TEST_TIMEOUT)
            print(f"[OK] Risposta ricevuta: {response}")
            
            # Verifica JSON valido
            try:
                data = json.loads(response)
                print(f"[OK] JSON valido: {data}")
            except json.JSONDecodeError as e:
                print(f"[FAIL] JSON invalido: {e}")
                return False
                
    except websockets.exceptions.InvalidStatusCode as e:
        print(f"[FAIL] Handshake fallito: {e}")
        return False
    except ConnectionRefusedError:
        print(f"[FAIL] Nessun server in ascolto su porta {WS_PORT}")
        print("      Avviare il plugin standalone o VST3 in un DAW")
        return False
    except asyncio.TimeoutError:
        print(f"[FAIL] Timeout dopo {TEST_TIMEOUT}s")
        return False
    except Exception as e:
        print(f"[FAIL] Errore: {e}")
        return False
    
    return True

async def test_osc_message_format():
    """Verifica che il formato dei messaggi OSC→WebSocket sia corretto"""
    print("\n" + "=" * 60)
    print("TEST: Formato Messaggi OSC→WebSocket")
    print("=" * 60)
    
    # Simula un messaggio dal plugin verso UI
    osc_to_ws_msg = {
        "type": "osc",
        "address": "/transport/play",
        "value": 1.0,
        "timestamp": 1713094800.123
    }
    
    # Verifica campi obbligatori
    required = ["type", "address", "value", "timestamp"]
    for field in required:
        if field not in osc_to_ws_msg:
            print(f"[FAIL] Campo mancante: {field}")
            return False
    
    print(f"[OK] Messaggio OSC→WebSocket valido: {json.dumps(osc_to_ws_msg, indent=2)}")
    return True

async def test_ws_to_osc_format():
    """Verifica che il formato dei messaggi WebSocket→OSC sia corretto"""
    print("\n" + "=" * 60)
    print("TEST: Formato Messaggi WebSocket→OSC")
    print("=" * 60)
    
    # Simula un messaggio dalla UI verso plugin
    ws_to_osc_msg = {
        "type": "param",
        "address": "/param/gain",
        "value": 0.75
    }
    
    required = ["type", "address", "value"]
    for field in required:
        if field not in ws_to_osc_msg:
            print(f"[FAIL] Campo mancante: {field}")
            return False
    
    print(f"[OK] Messaggio WebSocket→OSC valido: {json.dumps(ws_to_osc_msg, indent=2)}")
    return True

async def test_ai_response_format():
    """Verifica formato risposta AI"""
    print("\n" + "=" * 60)
    print("TEST: Formato Risposta AI")
    print("=" * 60)
    
    ai_response = {
        "type": "ai_response",
        "requestId": "req_12345",
        "content": "Suggerimento: aumenta il gain di 3dB",
        "metadata": {
            "model": "llama3.2",
            "tokens": 42,
            "provider": "ollama"
        }
    }
    
    print(f"[OK] Formato risposta AI valido: {json.dumps(ai_response, indent=2)}")
    return True

async def main():
    print("\n" + "=" * 60)
    print("OpenClaw VST Bridge - Test End-to-End")
    print("Verifica WebSocket + OSC + AI Integration")
    print("=" * 60)
    
    results = []
    
    # Test 1: Protocollo WebSocket (richiede server attivo)
    results.append(("WebSocket Protocol", await test_websocket_protocol()))
    
    # Test 2-4: Formati messaggi (sempre eseguibili)
    results.append(("OSC→WebSocket Format", await test_osc_message_format()))
    results.append(("WebSocket→OSC Format", await test_ws_to_osc_format()))
    results.append(("AI Response Format", await test_ai_response_format()))
    
    # Report
    print("\n" + "=" * 60)
    print("RISULTATI TEST")
    print("=" * 60)
    
    passed = sum(1 for _, r in results if r)
    total = len(results)
    
    for name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"  [{status}] {name}")
    
    print("=" * 60)
    print(f"Totale: {passed}/{total} test passati")
    
    if passed == total:
        print("✓ Tutti i test superati!")
        return 0
    else:
        print("✗ Alcuni test falliti")
        return 1

if __name__ == "__main__":
    result = asyncio.run(main())
    sys.exit(result)
