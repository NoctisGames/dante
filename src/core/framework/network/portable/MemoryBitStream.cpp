//
//  MemoryBitStream.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MemoryBitStream.h"

#include "Vector2.h"
#include "Color.h"

void OutputMemoryBitStream::WriteBits(uint8_t inData, uint32_t inBitCount)
{
    uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
    
    if (nextBitHead > mBitCapacity)
    {
        ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
    }
    
    //calculate the byteOffset into our buffer
    //by dividing the head by 8
    //and the bitOffset by taking the last 3 bits
    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;
    
    uint8_t currentMask = ~(0xff << bitOffset);
    mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);
    
    //calculate how many bits were not yet used in
    //our target byte in the buffer
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    
    //if we needed more than that, carry to the next byte
    if (bitsFreeThisByte < inBitCount)
    {
        //we need another byte
        mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
    }
    
    mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* inData, uint32_t inBitCount)
{
    const char* srcByte = static_cast<const char*>(inData);
    //write all the bytes
    while (inBitCount > 8)
    {
        WriteBits(*srcByte, 8);
        ++srcByte;
        inBitCount -= 8;
    }
    //write anything left
    if (inBitCount > 0)
    {
        WriteBits(*srcByte, inBitCount);
    }
}

void OutputMemoryBitStream::Write(const Vector2& inVector)
{
    Write(inVector.getX());
    Write(inVector.getY());
}

void InputMemoryBitStream::Read(Vector2& outVector)
{
    Read(outVector.getXRef());
    Read(outVector.getYRef());
}

void OutputMemoryBitStream::Write(Color& inColor)
{
    Write(inColor.red);
    Write(inColor.green);
    Write(inColor.blue);
    Write(inColor.alpha);
}

void InputMemoryBitStream::Read(Color& outColor)
{
    Read(outColor.red);
    Read(outColor.green);
    Read(outColor.blue);
    Read(outColor.alpha);
}

void OutputMemoryBitStream::ReallocBuffer(uint32_t inNewBitLength)
{
    if (mBuffer == nullptr)
    {
        //just need to memset on first allocation
        mBuffer = static_cast<char*>(std::malloc(inNewBitLength >> 3));
        memset(mBuffer, 0, inNewBitLength >> 3);
    }
    else
    {
        //need to memset, then copy the buffer
        char* tempBuffer = static_cast<char*>(std::malloc(inNewBitLength >> 3));
        memset(tempBuffer, 0, inNewBitLength >> 3);
        memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
        std::free(mBuffer);
        mBuffer = tempBuffer;
    }
    
    //handle realloc failure
    //...
    mBitCapacity = inNewBitLength;
}

void InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t inBitCount)
{
    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;
    
    outData = static_cast<uint8_t>(mBuffer[byteOffset]) >> bitOffset;
    
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    if (bitsFreeThisByte < inBitCount)
    {
        //we need another byte
        outData |= static_cast<uint8_t>(mBuffer[byteOffset + 1]) << bitsFreeThisByte;
    }
    
    //don't forget a mask so that we only read the bit we wanted...
    outData &= (~(0x00ff << inBitCount));
    
    mBitHead += inBitCount;
}

void InputMemoryBitStream::ReadBits(void* outData, uint32_t inBitCount)
{
    uint8_t* destByte = reinterpret_cast< uint8_t* >(outData);
    // read all the bytes
    while (inBitCount > 8)
    {
        ReadBits(*destByte, 8);
        ++destByte;
        inBitCount -= 8;
    }
    
    // read anything left
    if (inBitCount > 0)
    {
        ReadBits(*destByte, inBitCount);
    }
}
