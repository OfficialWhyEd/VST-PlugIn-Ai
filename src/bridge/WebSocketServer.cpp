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
#include <cstring>

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

    running.store(true);
    DBG("[WebSocketServer] Bound to port " + juce::String(port) + ", starting accept thread");

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
        // Check for waiting connection every 100ms
        if (serverSocket && serverSocket->isConnected())
        {
            // Try to accept a connection (with timeout via listen queue)
            juce::StreamingSocket* newClient = serverSocket->waitForNextConnection();

            if (newClient != nullptr)
            {
                DBG("[WebSocketServer] New connection accepted");

                // Create client info
                auto clientInfo = std::make_unique<ClientInfo>();
                clientInfo->id = nextClientId.fetch_add(1);
                clientInfo->socket.reset(newClient);
                clientInfo->connected.store(true);
                clientInfo->handshakeComplete = false;

                // Add to clients list (store raw pointer for thread)
                ClientInfo* clientPtr = clientInfo.get();
                {
                    std::lock_guard<std::mutex> lock(clientsMutex);
                    clients.push_back(std::move(clientInfo));
                }

                // Start client thread with raw pointer (ownership in vector)
                clientPtr->thread = std::make_unique<std::thread>([this, clientPtr]() {
                    clientLoop(clientPtr);
                });
            }
        }

        // Sleep a bit before next check
        juce::Thread::sleep(100);
    }

    DBG("[WebSocketServer] Accept loop ended");
}

//==============================================================================
void WebSocketServer::clientLoop(ClientInfo* client)
{
    DBG("[WebSocketServer] Client " + juce::String(client->id) + " thread started");

    // First: perform WebSocket handshake
    if (!performHandshake(client))
    {
        DBG("[WebSocketServer] Handshake failed for client " + juce::String(client->id));
        client->connected.store(false);
        client->socket->close();
        if (connectionCallback)
            connectionCallback(client->id, false);
        return;
    }

    client->handshakeComplete = true;
    DBG("[WebSocketServer] Handshake completed for client " + juce::String(client->id));

    // Notify connection
    if (connectionCallback)
        connectionCallback(client->id, true);

    // Now read messages
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
bool WebSocketServer::performHandshake(ClientInfo* client)
{
    if (!client || !client->socket || !client->socket->isConnected())
        return false;

    // Read HTTP upgrade request
    // The request looks like:
    // GET / HTTP/1.1\r\n
    // Host: localhost:8080\r\n
    // Connection: Upgrade\r\n
    // Upgrade: websocket\r\n
    // Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n
    // ...

    char buffer[4096];
    int bytesRead = client->socket->read(buffer, sizeof(buffer) - 1, false);

    if (bytesRead <= 0)
        return false;

    buffer[bytesRead] = '\0';
    juce::String request(buffer, bytesRead);

    DBG("[WebSocketServer] Handshake request:\n" + request.substring(0, juce::jmin(500, bytesRead)));

    // Check it's a valid HTTP GET request
    if (!request.startsWith("GET"))
        return false;

    // Extract Sec-WebSocket-Key
    juce::String key;
    juce::StringArray lines = juce::StringArray::fromLines(request);
    for (const auto& line : lines)
    {
        if (line.startsWithIgnoreCase("Sec-WebSocket-Key:"))
        {
            key = line.substring(16).trim();
            break;
        }
    }

    if (key.isEmpty())
    {
        DBG("[WebSocketServer] No Sec-WebSocket-Key found");
        return false;
    }

    // Compute accept key: SHA1(key + GUID), base64 encoded
    juce::String acceptKey = computeWebSocketAcceptKey(key);

    // Send HTTP 101 response
    juce::String response =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + acceptKey + "\r\n"
        "\r\n";

    int sent = client->socket->write(response.toRawUTF8(), response.getNumBytesAsUTF8());
    DBG("[WebSocketServer] Handshake response sent (" + juce::String(sent) + " bytes)");

    return sent > 0;
}

//==============================================================================
juce::String WebSocketServer::computeWebSocketAcceptKey(const juce::String& clientKey)
{
    // RFC 6455: acceptKey = Base64(SHA1(clientKey + GUID))
    juce::String combined = clientKey + WEBSOCKET_GUID;

    // SHA1 implementation inline (no external deps)
    unsigned char sha1Result[20];
    {
        struct SHA1_CTX {
            uint32_t state[5];
            uint64_t count;
            unsigned char buffer[64];
        };

        SHA1_CTX ctx;
        ctx.state[0] = 0x67452301;
        ctx.state[1] = 0xEFCDAB89;
        ctx.state[2] = 0x98BADCFE;
        ctx.state[3] = 0x10325476;
        ctx.state[4] = 0xC3D2E1F0;
        ctx.count = 0;

        auto rol = [](uint32_t x, int n) { return ((x) << (n)) | ((x) >> (32 - (n))); };

        auto sha1Block = [&](const unsigned char* block) {
            uint32_t w[80];
            for (int i = 0; i < 16; i++)
                w[i] = (uint32_t(block[i*4]) << 24) | (uint32_t(block[i*4+1]) << 16) |
                       (uint32_t(block[i*4+2]) << 8) | uint32_t(block[i*4+3]);
            for (int i = 16; i < 80; i++)
                w[i] = rol(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);

            uint32_t a = ctx.state[0], b = ctx.state[1], c = ctx.state[2];
            uint32_t d = ctx.state[3], e = ctx.state[4];

            for (int i = 0; i < 20; i++) {
                uint32_t f = (b & c) | ((~b) & d);
                uint32_t k = 0x5A827999;
                uint32_t temp = rol(a, 5) + f + e + k + w[i];
                e = d; d = c; c = rol(b, 30); b = a; a = temp;
            }
            for (int i = 20; i < 40; i++) {
                uint32_t f = b ^ c ^ d;
                uint32_t k = 0x6ED9EBA1;
                uint32_t temp = rol(a, 5) + f + e + k + w[i];
                e = d; d = c; c = rol(b, 30); b = a; a = temp;
            }
            for (int i = 40; i < 60; i++) {
                uint32_t f = (b & c) | (b & d) | (c & d);
                uint32_t k = 0x8F1BBCDC;
                uint32_t temp = rol(a, 5) + f + e + k + w[i];
                e = d; d = c; c = rol(b, 30); b = a; a = temp;
            }
            for (int i = 60; i < 80; i++) {
                uint32_t f = b ^ c ^ d;
                uint32_t k = 0xCA62C1D6;
                uint32_t temp = rol(a, 5) + f + e + k + w[i];
                e = d; d = c; c = rol(b, 30); b = a; a = temp;
            }

            ctx.state[0] += a; ctx.state[1] += b; ctx.state[2] += c;
            ctx.state[3] += d; ctx.state[4] += e;
        };

        const auto* data = (const unsigned char*)combined.toRawUTF8();
        size_t len = combined.getNumBytesAsUTF8();

        // Pad to 64 bytes
        unsigned char padded[128];
        memcpy(padded, data, len);
        padded[len] = 0x80;
        memset(padded + len + 1, 0, (len >= 56 ? 120 : 56) - len - 1);
        *(uint64_t*)(padded + (len >= 56 ? 112 : 56)) = (uint64_t)len * 8;

        sha1Block(padded);
        if (len >= 56) sha1Block(padded + 64);

        for (int i = 0; i < 20; i++)
            sha1Result[i] = (unsigned char)(ctx.state[i / 4] >> ((3 - i % 4) * 8));
    }

    // Base64 encode
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char result[32];
    int j = 0;
    for (int i = 0; i < 20; i += 3) {
        uint32_t triple = (sha1Result[i] << 16) | (i+1 < 20 ? sha1Result[i+1] << 8 : 0) | (i+2 < 20 ? sha1Result[i+2] : 0);
        result[j++] = b64[(triple >> 18) & 0x3F];
        result[j++] = b64[(triple >> 12) & 0x3F];
        result[j++] = i+1 < 20 ? b64[(triple >> 6) & 0x3F] : '=';
        result[j++] = i+2 < 20 ? b64[triple & 0x3F] : '=';
    }
    result[j] = '\0';

    return juce::String(result);
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
int WebSocketServer::getConnectedClientsCount()
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

juce::String WebSocketServer::getCurrentTimestamp() const
{
    return juce::Time::getCurrentTime().formatted("%H:%M:%S");
}