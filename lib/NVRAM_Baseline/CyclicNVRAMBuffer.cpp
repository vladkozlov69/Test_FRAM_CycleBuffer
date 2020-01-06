#include "CyclicNVRAMBuffer.h"

CyclicNVRAMBuffer::CyclicNVRAMBuffer(uint16_t addr, uint16_t size, NVRAM * nvram)
{
    m_Addr = addr;
    m_Size = size;
    m_NVRAM = nvram;
}

void CyclicNVRAMBuffer::initialize()
{
    int16_t pos = -1;
    double val = NAN;
    m_NVRAM->write(m_Addr, (byte *)&pos, sizeof(pos));
    for (uint16_t i = 0; i < m_Size; i++)
    {
        m_NVRAM->write(m_Addr + sizeof(pos) + i * sizeof(val), (byte *)&val, sizeof(val));
    }
}

int16_t CyclicNVRAMBuffer::getPosition()
{
    int16_t pos;
    m_NVRAM->read(m_Addr, (byte *)&pos, sizeof(pos));
    return pos;
}

double CyclicNVRAMBuffer::get(uint16_t index)
{
    double val;
    m_NVRAM->read(m_Addr + sizeof(int16_t) + index * sizeof(val), (byte *)&val, sizeof(val));
    return val;
}

void CyclicNVRAMBuffer::put(double val)
{
    int16_t pos = getPosition();
    pos = (pos + 1) % m_Size;

    m_NVRAM->write(m_Addr, (byte *)&pos, sizeof(pos));

    m_NVRAM->write(m_Addr + sizeof(pos) + pos * sizeof(val), (byte *)&val, sizeof(val));
}
