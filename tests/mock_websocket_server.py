#!/usr/bin/env python3
"""
Mock WebSocket Server per test end-to-end
Simula il comportamento del plugin OpenClaw VST Bridge
"""

import asyncio
import json
import websockets
from datetime import datetime

# Stato simulato
connected_clients = set()
mock_daw_state = {
    "isPlaying": False,
    "isRecording": False,
    "bpm": 120.0,
    "position": 0.0,
    "params": {
        "/param/gain": 0.75,
        "/param/filter": 0.5,
        "/param/reverb": 0.3
    }
}

async def broadcast(message):
    """Invia messaggio a tutti i client connessi"""
    if connected_clients:
        await asyncio.gather(
            *[client.send(json.dumps(message)) for client in connected_clients],
            return_exceptions=True
        )

async def handle_client(websocket, path):
    """Gestisce una connessione client"""
    client_id = id(websocket)
    print(f"[{datetime.now().isoformat()}] Client {client_id} connesso")
    connected_clients.add(websocket)
    
    # Invia stato iniziale
    await websocket.send(json.dumps({
        "type": "status",
        "connected": True,
        "version": "0.1.0-mock"
    }))
    
    try:
        async for message in websocket:
            try:
                data = json.loads(message)
                msg_type = data.get("type", "unknown")
                
                print(f"[{datetime.now().isoformat()}] Ricevuto [{msg_type}]: {json.dumps(data)[:100]}")
                
                # Gestione messaggi in entrata
                if msg_type == "ping":
                    await websocket.send(json.dumps({
                        "type": "pong",
                        "timestamp": data.get("timestamp")
                    }))
                    
                elif msg_type == "param":
                    # Simula aggiornamento parametro
                    address = data.get("address", "/param/unknown")
                    value = data.get("value", 0.0)
                    mock_daw_state["params"][address] = value
                    
                    # Broadcast conferma
                    await broadcast({
                        "type": "param_updated",
                        "address": address,
                        "value": value,
                        "source": "ui"
                    })
                    
                elif msg_type == "transport":
                    action = data.get("action")
                    if action == "play":
                        mock_daw_state["isPlaying"] = True
                    elif action == "stop":
                        mock_daw_state["isPlaying"] = False
                    elif action == "record":
                        mock_daw_state["isRecording"] = not mock_daw_state["isRecording"]
                    
                    await broadcast({
                        "type": "transport",
                        **mock_daw_state
                    })
                    
                elif msg_type == "ai_request":
                    # Simula risposta AI
                    await asyncio.sleep(0.5)  # Simula latenza
                    await websocket.send(json.dumps({
                        "type": "ai_response",
                        "requestId": data.get("requestId"),
                        "content": f"Suggerimento per '{data.get('prompt', 'mixer')}': prova ad aumentare il gain di 3dB e ridurre la reverb al 20%.",
                        "metadata": {
                            "model": "llama3.2-mock",
                            "tokens": 42,
                            "provider": "ollama"
                        }
                    }))
                    
                elif msg_type == "get_params":
                    # Invia lista parametri
                    await websocket.send(json.dumps({
                        "type": "params_list",
                        "params": [
                            {"address": addr, "value": val, "name": addr.split("/")[-1].title()}
                            for addr, val in mock_daw_state["params"].items()
                        ]
                    }))
                    
                else:
                    await websocket.send(json.dumps({
                        "type": "error",
                        "message": f"Tipo messaggio non supportato: {msg_type}"
                    }))
                    
            except json.JSONDecodeError as e:
                print(f"[ERROR] JSON invalido: {e}")
                await websocket.send(json.dumps({
                    "type": "error",
                    "message": "JSON invalido"
                }))
                
    except websockets.exceptions.ConnectionClosed:
        print(f"[{datetime.now().isoformat()}] Client {client_id} disconnesso")
    finally:
        connected_clients.discard(websocket)

async def simulate_osc_from_daw():
    """Simula messaggi OSC che arrivano dal DAW"""
    while True:
        await asyncio.sleep(3)
        if connected_clients and mock_daw_state["isPlaying"]:
            mock_daw_state["position"] += 1.0
            await broadcast({
                "type": "osc",
                "address": "/transport/position",
                "value": mock_daw_state["position"],
                "timestamp": asyncio.get_event_loop().time()
            })
            print(f"[{datetime.now().isoformat()}] Simulated OSC: position={mock_daw_state['position']}")

async def main():
    print("=" * 60)
    print("OpenClaw VST Bridge - Mock WebSocket Server")
    print("Simula il plugin per test end-to-end")
    print("=" * 60)
    print(f"WebSocket: ws://localhost:8080")
    print(f"Premi Ctrl+C per terminare")
    print("=" * 60)
    
    # Avvia server WebSocket
    server = await websockets.serve(handle_client, "localhost", 8080)
    
    # Avvia simulazione DAW
    asyncio.create_task(simulate_osc_from_daw())
    
    try:
        await asyncio.Future()  # Run forever
    except KeyboardInterrupt:
        print("\nServer terminato")
        server.close()
        await server.wait_closed()

if __name__ == "__main__":
    asyncio.run(main())
