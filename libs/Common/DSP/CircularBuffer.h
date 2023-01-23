/*
  ==============================================================================

    CircularBuffer.h
    Created: 7 Feb 2020 5:01:51pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer() {}
    
    void reset()
    {
        writeIndex = 0;
        buffer.clear();
    }
    
    int findNextPow2(int bufferLength)
    {
        return static_cast<int>((std::pow(2, std::ceil(std::log(bufferLength) / std::log(2)))));
    }
    
    void setSize(int inBufferLength)
    {
        bufferLength = findNextPow2(inBufferLength);
        wrapMask = bufferLength - 1;
        buffer.setSize(1, bufferLength, false, false, true);
        reset();
    }
    
    void writeBuffer(T input)
    {
        /** setting a buffer size that is power of two and creating
            a mask to ANDOR the read/write index with. a faster way
            of implementing the wrap.
         
            bufferSize = <some power of two>
            mask = bufferSize - 1
         
         */
        buffer.setSample(0, writeIndex++, input);
        writeIndex &= wrapMask;
    }
    
    T readBuffer(int delayInSamples, bool readBeforeWrite)
    {
        // This implementation assumes that data will be read, then written
        // Pirkle implementation has a version with bool readBeforeWrite
        // commented out. Decided to bring it back here. TODO: remember why.
        int readIndex = (writeIndex - (readBeforeWrite ? 1 : 0)) - delayInSamples;
        readIndex &= wrapMask;
        return buffer.getSample(0, readIndex);
    }
    
    
private:
    juce::AudioBuffer<T> buffer;
    int writeIndex = 0;
    int bufferLength = 1024; // power of 2
    int wrapMask = bufferLength - 1;
};
