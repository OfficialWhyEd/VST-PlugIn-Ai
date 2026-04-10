/*
   ==============================================================================
   OscHandler.cpp
   OpenClaw VST Bridge AI - OSC Communication Handler Implementation
   ==============================================================================
*/

#include "OscHandler.h"

OscHandler::OscHandler(int p) : port(p)
{
}

OscHandler::~OscHandler()
{
    stop();
}

void OscHandler::start()
{
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
}

void OscHandler::stop()
{
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
}

void OscHandler::sendMessage(const juce::String& address, float value)
{
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
}

void OscHandler::sendMessage(const juce::String& address, const juce::String& value)
{
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
}

void OscHandler::sendMessage(const juce::String& address, int value)
{
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
}