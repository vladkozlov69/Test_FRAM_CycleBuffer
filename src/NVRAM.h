#ifndef NVRAMUTILS_H_
#define NVRAMUTILS_H_

#include <Arduino.h>

class NVRAM
{
private:
	void (*m_WriteByte)(uint16_t addr, byte data);
	byte (*m_ReadByte)(uint16_t addr);    
public:
    NVRAM(void (*m_WriteByte)(uint16_t addr, byte data), byte (*m_ReadByte)(uint16_t addr));
    void write(uint16_t addr, byte * data, size_t dataSize);
    void read(uint16_t addr, byte * data, size_t dataSize);
};


#endif