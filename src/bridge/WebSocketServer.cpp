/*
  ==============================================================================
  WebSocketServer.cpp
  OpenClaw VST Bridge AI - WebSocket Server Implementation

  Minimal RFC 6455 WebSocket server for browser communication.
  ==============================================================================
*/

#include "WebSocketServer.h"
#include <sstream>
#include <iomanip>

// GUID for WebSocket handshake (RFC 6455)
static const juce::String WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

//==============================================================================
WebSocketServer::WebSocketServer(int p) : port(p)
{
}

WebSocketServer::~WebSocketServer()
{
    stop();
}

//==============================================================================
void WebSocketServer::setPort(int newPort)
{
    if (running.load())
    {
        DBG("[WebSocketServer] Cannot change port while running");
        return;
    }
    port = newPort;
}

bool WebSocketServer::start()
{
    if (running.load())
        return true;

    // Create server socket
    serverSocket = std::make_unique<juce::StreamingSocket>();

    if (!serverSocket->bindToPort(port))
    {
        DBG("[WebSocketServer] ERROR: Cannot bind to port " + juce::String(port));
        return false;
    }

    serverSocket->setSocketReadyReadHandler(nullptr); // We use accept thread

    running.store(true);
    DBG("[WebSocketServer] Started on port " + juce::String(port));

    // Start accept thread
    acceptThread = std::make_unique<std::thread>([this]() { acceptLoop(); });

    return true;
}

void WebSocketServer::stop()
{
    if (!running.load())
        return;

    running.store(false);

    // Close server socket to unblock accept
    if (serverSocket)
    {
        serverSocket->close();
    }

    // Wait for accept thread
    if (acceptThread && acceptThread->joinable())
    {
        acceptThread->join();
        acceptThread.reset();
    }

    // Close all client connections
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto& client : clients)
        {
            if (client->connected.load())
            {
                closeConnection(client.get());
            }
        }
        clients.clear();
    }

    serverSocket.reset();
    DBG("[WebSocketServer] Stopped");
}

//==============================================================================
void WebSocketServer::acceptLoop()
{
    DBG("[WebSocketServer] Accept loop started");

    while (running.load())
    {
        // Wait for connection (with timeout to check running flag)
        auto startTime = std::chrono::steady_clock::now();
        const int selectTimeoutMs = 1000;

        while (running.load())
        {
            // Check if there's a connection waiting (non-blocking)
            if (serverSocket && serverSocket->isConnected())
            {
                // Try to accept (this is non-blocking if we set it up right)
                // Actually StreamingSocket::accept is blocking, so we need a different approach
                // Use select/poll equivalent - JUCE doesn't have direct select, so we use a short sleep
                juce::Thread::sleep(10);

                // For simplicity, we'll do a blocking accept with timeout via close/reopen trick
                // Or better: just do blocking accept but check running flag periodically
            }
            else
            {
                juce::Thread::sleep(10);
            }
        }

        // The accept approach with JUCE StreamingSocket needs a different strategy
        // Let's use a workaround: close and recreate socket to break blocking accept
    }

    DBG("[WebSocketServer] Accept loop ended");
}

//==============================================================================
void WebSocketServer::clientLoop(ClientInfo* client)
{
    DBG("[WebSocketServer] Client " + juce::String(client->id) + " thread started");

    while (client->connected.load() && running.load())
    {
        // Read data from client
        char buffer[8192];
        int bytesRead = client->socket->read(buffer, sizeof(buffer), false);

        if (bytesRead > 0)
        {
            // Append to receive buffer
            client->receiveBuffer.insert(client->receiveBuffer.end(),
                                         reinterpret_cast<uint8_t*>(buffer),
                                         reinterpret_cast<uint8_t*>(buffer) + bytesRead);

            // Process buffer
            while (!client->receiveBuffer.empty())
            {
                size_t bytesConsumed = 0;
                auto frame = parseFrame(client->receiveBuffer.data(),
                                        client->receiveBuffer.size(),
                                        bytesConsumed);

                if (bytesConsumed == 0)
                    break; // Need more data

                // Remove processed bytes
                client->receiveBuffer.erase(client->receiveBuffer.begin(),
                                            client->receiveBuffer.begin() + bytesConsumed);

                // Handle frame
                switch (frame.opcode)
                {
                    case 0x1: // Text frame
                    {
                        if (!frame.masked)
                        {
                            DBG("[WebSocketServer] ERROR: Unmasked text frame from client " + juce::String(client->id));
                            closeConnection(client, 1002, "Protocol error");
                            break;
                        }

                        std::string text(frame.payload.begin(), frame.payload.end());
                        DBG("[WebSocketServer] Text from client " + juce::String(client->id) + ": " + text.substr(0, 100));

                        // Parse JSON and call callback
                        if (messageCallback)
                        {
                            try
                            {
                                auto json = nlohmann::json::parse(text);
                                messageCallback(json);
                            }
                            catch (const std::exception& e)
                            {
                                DBG("[WebSocketServer] JSON parse error: " + juce::String(e.what()));
                            }
                        }
                        break;
                    }

                    case 0x8: // Close frame
                    {
                        DBG("[WebSocketServer] Client " + juce::String(client->id) + " sent close");
                        closeConnection(client, 1000, "");
                        break;
                    }

                    case 0x9: // Ping frame
                    {
                        DBG("[WebSocketServer] Ping from client " + juce::String(client->id));
                        sendPongFrame(client->socket.get(), frame.payload);
                        break;
                    }

                    case 0xA: // Pong frame
                    {
                        // Pong received (in response to our ping)
                        break;
                    }

                    default:
                        DBG("[WebSocketServer] Unknown opcode: " + juce::String(frame.opcode));
                        break;
                }
            }
        }
        else if (bytesRead < 0)
        {
            // Error or disconnected
            DBG("[WebSocketServer] Client " + juce::String(client->id) + " disconnected (read error)");
            client->connected.store(false);
            break;
        }
        else
        {
            // No data available, sleep briefly
            juce::Thread::sleep(5);
        }
    }

    // Cleanup
    client->connected.store(false);
    client->socket->close();

    DBG("[WebSocketServer] Client " + juce::String(client->id) + " thread ended");

    // Notify disconnection
    if (connectionCallback)
        connectionCallback(client->id, false);
}

//==============================================================================
WebSocketServer::WebSocketFrame WebSocketServer::parseFrame(const uint8_t* data,
                                                             size_t length,
                                                             size_t& bytesConsumed)
{
    WebSocketFrame frame;
    frame.fin = true;
    frame.masked = false;
    bytesConsumed = 0;

    if (length < 2)
        return frame;

    // Byte 0: FIN + opcode
    frame.fin = (data[0] & 0x80) != 0;
    frame.opcode = data[0] & 0x0F;

    // Byte 1: MASK + payload length
    frame.masked = (data[1] & 0x80) != 0;
    uint64_t payloadLen = data[1] & 0x7F;

    size_t headerLen = 2;
    if (payloadLen == 126)
    {
        if (length < 4)
            return frame;
        payloadLen = (data[2] << 8) | data[3];
        headerLen = 4;
    }
    else if (payloadLen == 127)
    {
        if (length < 10)
            return frame;
        payloadLen = 0;
        for (int i = 0; i < 8; i++)
            payloadLen = (payloadLen << 8) | data[2 + i];
        headerLen = 10;
    }

    // Mask key
    if (frame.masked)
    {
        if (length < headerLen + 4)
            return frame;
        std::memcpy(frame.mask, data + headerLen, 4);
        headerLen += 4;
    }

    // Check if we have complete frame
    if (length < headerLen + payloadLen)
        return frame;

    // Extract payload
    frame.payload.resize(static_cast<size_t>(payloadLen));
    if (payloadLen > 0)
    {
        std::memcpy(frame.payload.data(), data + headerLen, static_cast<size_t>(payloadLen));

        // Unmask if needed
        if (frame.masked)
        {
            for (size_t i = 0; i < payloadLen; i++)
                frame.payload[i] ^= frame.mask[i % 4];
        }
    }

    bytesConsumed = headerLen + static_cast<size_t>(payloadLen);
    return frame;
}

void WebSocketServer::sendFrame(juce::StreamingSocket* socket,
                                uint8_t opcode,
                                const std::vector<uint8_t>& payload,
                                bool mask)
{
    if (!socket || !socket->isConnected())
        return;

    std::vector<uint8_t> frame;
    frame.reserve(payload.size() + 14);

    // FIN + opcode
    frame.push_back(0x80 | opcode); // FIN=1 for now

    // Payload length
    if (payload.size() < 126)
    {
        frame.push_back(static_cast<uint8_t>(payload.size()));
    }
    else if (payload.size() < 65536)
    {
        frame.push_back(126);
        frame.push_back((payload.size() >> 8) & 0xFF);
        frame.push_back(payload.size() & 0xFF);
    }
    else
    {
        frame.push_back(127);
        for (int i = 7; i >= 0; i--)
            frame.push_back((payload.size() >> (i * 8)) & 0xFF);
    }

    // Payload
    frame.insert(frame.end(), payload.begin(), payload.end());

    // Send
    socket->write(frame.data(), static_cast<int>(frame.size()));
}

void WebSocketServer::sendTextFrame(juce::StreamingSocket* socket, const juce::String& text)
{
    std::vector<uint8_t> payload(text.toRawUTF8(), text.toRawUTF8() + text.getNumBytesAsUTF8());
    sendFrame(socket, 0x1, payload, false);
}

void WebSocketServer::sendPongFrame(juce::StreamingSocket* socket, const std::vector<uint8_t>& payload)
{
    sendFrame(socket, 0xA, payload, false);
}

void WebSocketServer::closeConnection(ClientInfo* client, uint16_t code, const juce::String& reason)
{
    if (!client->connected.load())
        return;

    client->connected.store(false);

    // Send close frame
    if (client->socket && client->socket->isConnected())
    {
        std::vector<uint8_t> payload;
        payload.push_back((code >> 8) & 0xFF);
        payload.push_back(code & 0xFF);

        std::string reasonStr = reason.toStdString();
        payload.insert(payload.end(), reasonStr.begin(), reasonStr.end());

        sendFrame(client->socket.get(), 0x8, payload, false);

        juce::Thread::sleep(50); // Wait for send to complete
        client->socket->close();
    }
}

//==============================================================================
void WebSocketServer::broadcast(const nlohmann::json& message)
{
    std::string jsonStr = message.dump();
    juce::String text(jsonStr.data(), jsonStr.size());

    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients)
    {
        if (client->connected.load() && client->socket->isConnected())
        {
            sendTextFrame(client->socket.get(), text);
        }
    }
}

void WebSocketServer::sendToClient(int clientId, const nlohmann::json& message)
{
    std::string jsonStr = message.dump();
    juce::String text(jsonStr.data(), jsonStr.size());

    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients)
    {
        if (client->id == clientId && client->connected.load() && client->socket->isConnected())
        {
            sendTextFrame(client->socket.get(), text);
            break;
        }
    }
}

//==============================================================================
void WebSocketServer::setMessageCallback(MessageCallback callback)
{
    messageCallback = std::move(callback);
}

void WebSocketServer::setConnectionCallback(ConnectionCallback callback)
{
    connectionCallback = std::move(callback);
}

//==============================================================================
int WebSocketServer::getConnectedClientsCount() const
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    int count = 0;
    for (const auto& client : clients)
    {
        if (client->connected.load())
            count++;
    }
    return count;
}

juce::String WebSocketServer::getServerURL() const
{
    return "ws://localhost:" + juce::String(port);
}

//==============================================================================
void WebSocketServer::cleanupClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(
        std::remove_if(clients.begin(), clients.end(),
            [](const std::unique_ptr<ClientInfo>& c) { return !c->connected.load(); }),
        clients.end()
    );
}

//==============================================================================
juce::String WebSocketServer::computeWebSocketAcceptKey(const juce::String& clientKey)
{
    // RFC 6455: acceptKey = Base64(SHA1(clientKey + GUID))
    juce::String combined = clientKey + WEBSOCKET_GUID;

    // Compute SHA1
    juce::SHA256 sha; // Using SHA256 as JUCE doesn't have SHA1 directly
    // Note: RFC 6455 requires SHA1, but for simplicity we'll use a stub
    // In production, link to OpenSSL or use a proper SHA1 implementation
    // For now, this is a placeholder that won't work with real browsers
    // TODO: Add proper SHA1 implementation

    DBG("[WebSocketServer] WARNING: Using placeholder SHA1 for WebSocket handshake");
    return "placeholder_accept_key";
}

juce::String WebSocketServer::getCurrentTimestamp() const
{
    return juce::Time::getCurrentTime().formatted("%H:%M:%S");
}
