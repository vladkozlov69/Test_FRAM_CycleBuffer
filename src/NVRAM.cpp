#include "NVRAM.h"

NVRAM::NVRAM(void (*writeByte)(uint16_t addr, byte data), byte (*readByte)(uint16_t addr))
{
    m_WriteByte = writeByte;
    m_ReadByte = readByte;
}

void NVRAM::write(uint16_t addr, byte * data, size_t dataSize)
{
    for (uint16_t i = 0; i < dataSize; i++)
    {
        byte b = *((unsigned char*)data + i);
        //Serial.println(b, HEX);
        m_WriteByte(addr + i, b);
    }
}

void NVRAM::read(uint16_t addr, byte * data, size_t dataSize)
{
    for (uint16_t i = 0; i < dataSize; i++)
    {
        byte b = m_ReadByte(addr + i);
        //Serial.println(b, HEX);
        *((unsigned char*)data + i) = b;
    }
}
