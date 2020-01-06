#ifndef CYCLICNVRAMBUFFER_H_
#define CYCLICNVRAMBUFFER_H_

#include "NVRAM.h"

class CyclicNVRAMBuffer
{
private:
    NVRAM * m_NVRAM;
    uint16_t m_Addr;    
    uint16_t m_Size; 
public:
    CyclicNVRAMBuffer(uint16_t addr, uint16_t size, NVRAM * nvram);
    void initialize();
    int16_t getPosition();
    double get(uint16_t index);
    void put(double value);
};

#endif