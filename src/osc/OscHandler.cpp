/*
<<<<<<< HEAD
   ==============================================================================
   OscHandler.cpp
   OpenClaw VST Bridge AI - OSC Communication Handler Implementation
   ==============================================================================
=======
  ==============================================================================
  OscHandler.cpp
  OpenClaw VST Bridge AI - OSC Communication Handler Implementation
  
  Phase 1: Receive-only
  - Listens on UDP port (default 9000)
  - Parses incoming OSC messages
  - Logs messages for debugging
  - Notifies plugin via callback
  
  Multipiattaforma: funziona su Linux, Windows, macOS
  ==============================================================================
>>>>>>> 9e81b5a766b9004070b2c5c33e55118510f20989
*/

#include "OscHandler.h"
#include <cstring>

//==============================================================================
OscHandler::OscHandler(int p) : port(p)
{
}

OscHandler::~OscHandler()
{
    stop();
}

//==============================================================================
void OscHandler::start()
{
<<<<<<< HEAD
    if (running)
        return;
        
    running = true;
    
    // Bind to localhost only for security
    juce::SocketAddress bindAddress("127.0.0.1", port);
    
    if (socket.bindTo(bindAddress, true))
    {
        // Start receiver thread
        receiverThread = std::make_unique<juce::Thread>("OSC Receiver");
        receiverThread->startThread([this] { receiverThreadFunction(); });
    }
    else
    {
        running = false;
        // TODO: Log error - could not bind to port
    }
=======
    if (running.load())
        return; // Already running
    
    running.store(true);
    
    // Create UDP socket
    socket = std::make_unique<juce::DatagramSocket>();
    
    if (!socket->bindToPort(port))
    {
        addToLog("[OSC] ERROR: Cannot bind to port " + juce::String(port));
        connected.store(false);
        return;
    }
    
    connected.store(true);
    addToLog("[OSC] Listening on port " + juce::String(port));
    
    // Start listener thread
    listenerThread = std::make_unique<std::thread>([this]() { listenerLoop(); });
>>>>>>> 9e81b5a766b9004070b2c5c33e55118510f20989
}

void OscHandler::stop()
{
<<<<<<< HEAD
    if (!running)
        return;
        
    running = false;
    
    if (receiverThread)
    {
        receiverThread->signalThreadShouldExit();
        receiverThread->waitForThreadToExit(-1); // Wait indefinitely
        receiverThread.reset();
    }
    
    socket.close();
=======
    if (!running.load())
        return;
    
    running.store(false);
    connected.store(false);
    
    // Close socket to unblock listener
    if (socket)
        socket->shutdown();
    
    // Wait for thread to finish
    if (listenerThread && listenerThread->joinable())
    {
        listenerThread->join();
        listenerThread.reset();
    }
    
    socket.reset();
    addToLog("[OSC] Stopped");
>>>>>>> 9e81b5a766b9004070b2c5c33e55118510f20989
}

//==============================================================================
void OscHandler::listenerLoop()
{
    // Buffer for incoming packets (64KB max UDP)
    std::vector<char> buffer(65536);
    
    while (running.load())
    {
        if (!socket)
        {
            juce::Thread::sleep(100);
            continue;
        }
        
        // Wait for data (blocking)
        juce::String senderIp;
        int senderPort = 0;
        int bytesRead = socket->read(buffer.data(), static_cast<int>(buffer.size()), true, senderIp, senderPort);
        
        if (bytesRead <= 0)
        {
            // Socket closed or error
            juce::Thread::sleep(10);
            continue;
        }
        
        // Handle the packet
        handleOscPacket(buffer.data(), bytesRead);
    }
}

//==============================================================================
void OscHandler::handleOscPacket(const char* data, int size)
{
    // OSC packets start with '/' for messages or '#' for bundles
    if (size < 4)
        return;
    
    if (data[0] == '/')
    {
        // Regular OSC message
        const char* ptr = data;
        
        // Extract address pattern (null-terminated, padded to 4 bytes)
        juce::String addressPattern;
        while (*ptr != '\0' && ptr < data + size)
        {
            addressPattern += *ptr++;
        }
        // Skip padding
        ptr += (4 - ((addressPattern.length() + 1) % 4)) % 4;
        if (ptr >= data + size) return;
        ptr++; // Skip null terminator
        
        // Align to 4 bytes
        while (ptr < data + size && (ptr - data) % 4 != 0) ptr++;
        
        // Extract type tag (starts with ',')
        if (ptr >= data + size || *ptr != ',')
        {
            addToLog("[OSC] WARNING: No type tag in message from " + addressPattern);
            return;
        }
        
        juce::String typeTag;
        ptr++; // Skip ','
        while (*ptr != '\0' && ptr < data + size)
        {
            typeTag += *ptr++;
        }
        // Skip to arguments (aligned)
        while (ptr < data + size && (ptr - data) % 4 != 0) ptr++;
        
        // Parse arguments
        const char* args = ptr;
        int argsSize = size - static_cast<int>(args - data);
        
        handleOscMessage(addressPattern, typeTag, args, argsSize);
    }
    else if (data[0] == '#' && size >= 16)
    {
        // OSC bundle (starts with "#bundle")
        // For Phase 1, we just log bundles
        addToLog("[OSC] Bundle received (not parsed in Phase 1)");
    }
}

//==============================================================================
void OscHandler::handleOscMessage(const juce::String& address, const juce::String& typeTag, 
                                  const char* arguments, int argSize)
{
    messagesReceived.fetch_add(1);
    
    // Learn mode: capture first address
    if (learnMode.load() && learnedAddress.isEmpty())
    {
        learnedAddress = address;
        addToLog("[OSC] LEARNED: " + address);
        learnMode.store(false);
    }
    
    // Parse arguments based on type tag
    const char* ptr = arguments;
    
    for (int i = 0; i < typeTag.length() && ptr < arguments + argSize; i++)
    {
        char type = typeTag[i];
        
        switch (type)
        {
            case 'f': // float32
            {
                if (ptr + 4 > arguments + argSize) break;
                
                uint8_t bytes[4];
                std::memcpy(bytes, ptr, 4);
                
                // Convert from big-endian
                float value;
                uint32_t intValue = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
                std::memcpy(&value, &intValue, 4);
                
                addToLog("[OSC] " + address + " → " + juce::String(value, 3));
                
                // Call callback
                if (callback)
                    callback(address, value);
                
                ptr += 4;
                break;
            }
            
            case 'i': // int32
            {
                if (ptr + 4 > arguments + argSize) break;
                
                uint8_t bytes[4];
                std::memcpy(bytes, ptr, 4);
                int32_t value = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
                
                addToLog("[OSC] " + address + " → " + juce::String(value));
                
                // Call callback with float conversion
                if (callback)
                    callback(address, static_cast<float>(value));
                
                ptr += 4;
                break;
            }
            
            case 's': // string
            {
                juce::String str;
                while (ptr < arguments + argSize && *ptr != '\0')
                {
                    str += *ptr++;
                }
                
                addToLog("[OSC] " + address + " → \"" + str + "\"");
                
                if (stringCallback)
                    stringCallback(address, str);
                
                // Skip padding
                ptr += (4 - ((str.length() + 1) % 4)) % 4;
                if (ptr < arguments + argSize && *ptr == '\0') ptr++;
                while (ptr < arguments + argSize && (ptr - arguments) % 4 != 0) ptr++;
                break;
            }
            
            default:
                // Unknown type, skip (would need proper padding calculation)
                addToLog("[OSC] " + address + " → <unknown type: " + juce::String::charToString(type) + ">");
                break;
        }
    }
}

//==============================================================================
void OscHandler::sendMessage(const juce::String& address, float value)
{
<<<<<<< HEAD
    if (!running)
        return;
        
    juce::MemoryBlock buffer;
    juce::MemoryOutputStream mo(buffer, true);
    
    mo.write("<");
    mo.write(address.toRawUTF8());
    mo.write(">f");
    mo.writeFloat(value);
    
    // Add padding to 4-byte boundary
    int padding = (4 - (buffer.getSize() % 4)) % 4;
    for (int i = 0; i < padding; ++i)
        mo.writeByte(0);
        
    juce::SocketAddress destAddr("127.0.0.1", port);
    socket.write(destAddr, buffer.getData(), buffer.getSize());
=======
    if (!socket || !connected.load())
    {
        addToLog("[OSC] ERROR: Not connected, cannot send");
        return;
    }
    
    // Phase 2: implement sending
    // For now, just log
    addToLog("[OSC] SEND (not implemented): " + address + " → " + juce::String(value, 3));
>>>>>>> 9e81b5a766b9004070b2c5c33e55118510f20989
}

void OscHandler::sendMessage(const juce::String& address, const juce::String& value)
{
<<<<<<< HEAD
    if (!running)
        return;
        
    juce::MemoryBlock buffer;
    juce::MemoryOutputStream mo(buffer, true);
    
    mo.write("<");
    mo.write(address.toRawUTF8());
    mo.write(">s");
    mo.writeUTF8(value);
    
    // Add null terminator and padding to 4-byte boundary
    mo.writeByte(0);
    int padding = (4 - (buffer.getSize() % 4)) % 4;
    for (int i = 0; i < padding; ++i)
        mo.writeByte(0);
        
    juce::SocketAddress destAddr("127.0.0.1", port);
    socket.write(destAddr, buffer.getData(), buffer.getSize());
=======
    if (!socket || !connected.load())
    {
        addToLog("[OSC] ERROR: Not connected, cannot send");
        return;
    }
    
    // Phase 2: implement sending
    addToLog("[OSC] SEND (not implemented): " + address + " → \"" + value + "\"");
>>>>>>> 9e81b5a766b9004070b2c5c33e55118510f20989
}

void OscHandler::sendMessage(const juce::String& address, int value)
{
<<<<<<< HEAD
    if (!running)
        return;
        
    juce::MemoryBlock buffer;
    juce::MemoryOutputStream mo(buffer, true);
    
    mo.write("<");
    mo.write(address.toRawUTF8());
    mo.write(">i");
    mo.writeInt32(value);
    
    // Add padding to 4-byte boundary
    int padding = (4 - (buffer.getSize() % 4)) % 4;
    for (int i = 0; i < padding; ++i)
        mo.writeByte(0);
        
    juce::SocketAddress destAddr("127.0.0.1", port);
    socket.write(destAddr, buffer.getData(), buffer.getSize());
}

void OscHandler::setMessageCallback(MessageCallback callback)
{
    messageCallback = callback;
}

void OscHandler::receiverThreadFunction()
{
    juce::MemoryBuffer buffer(1024); // 1KB buffer for OSC messages
    
    while (!threadShouldExit() && running)
    {
        juce::SocketAddress sourceAddress;
        int bytesReceived = socket.read(sourceAddress, buffer.getData(), buffer.getSize(), 0);
        
        if (bytesReceived > 0)
        {
            buffer.setSize(bytesReceived);
            
            // Simple OSC parsing - look for null-terminated address
            const char* data = static_cast<const char*>(buffer.getData());
            const char* end = data + bytesReceived;
            
            // Find address end (null terminator)
            const char* addrEnd = std::find(data, end, '\0');
            if (addrEnd != end)
            {
                juce::String address(data, static_cast<size_t>(addrEnd - data));
                
                // Skip null terminator
                const char* argsStart = addrEnd + 1;
                
                // Align to 4-byte boundary
                intptr_t addrOffset = addrEnd - data + 1;
                int padding = (4 - (addrOffset % 4)) % 4;
                const char* alignedArgs = argsStart + padding;
                
                if (alignedArgs < end)
                {
                    // For now, just pass the raw data - more sophisticated parsing could be added
                    juce::MemoryBlock argData(alignedArgs, end - alignedArgs);
                    if (messageCallback)
                        messageCallback(address, argData);
                }
            }
        }
        else if (bytesReceived < 0)
        {
            // Error or disconnection
            break;
        }
        
        // Small sleep to prevent hogging CPU
        juce::Thread::sleep(1);
    }
}

void OscHandler::parseOscMessage(const juce::String& address, juce::MemoryBlock& data, MessageCallback callback)
{
    // This is kept for compatibility but the actual parsing is done in receiverThreadFunction
    if (callback)
        callback(address, data);
=======
    sendMessage(address, static_cast<float>(value));
}

//==============================================================================
void OscHandler::setCallback(OscCallback cb)
{
    callback = std::move(cb);
}

void OscHandler::setStringCallback(OscStringCallback cb)
{
    stringCallback = std::move(cb);
}

//==============================================================================
void OscHandler::setPort(int newPort)
{
    if (newPort == port)
        return;
    
    bool wasRunning = running.load();
    if (wasRunning)
        stop();
    
    port = newPort;
    
    if (wasRunning)
        start();
}

//==============================================================================
void OscHandler::addToLog(const juce::String& msg)
{
    // Thread-safe log
    const juce::ScopedLock sl(logLock);
    
    // Add timestamp
    auto timestamp = juce::Time::getCurrentTime().formatted("%H:%M:%S");
    messageLog.add("[" + timestamp + "] " + msg);
    
    // Keep log size manageable (last 100 messages)
    if (messageLog.size() > 100)
        messageLog.remove(0);
    
    // Also write to debug console
    DBG(msg);
>>>>>>> 9e81b5a766b9004070b2c5c33e55118510f20989
}