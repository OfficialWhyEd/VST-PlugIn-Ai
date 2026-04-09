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
    running = true;
    // TODO: Implement OSC socket binding and listener
}

void OscHandler::stop()
{
    running = false;
    // TODO: Close OSC socket
}

void OscHandler::sendMessage(const juce::String& address, float value)
{
    juce::ignoreUnused(address, value);
    // TODO: Implement OSC message sending
}

void OscHandler::sendMessage(const juce::String& address, const juce::String& value)
{
    juce::ignoreUnused(address, value);
    // TODO: Implement OSC message sending
}