#include "NVRAMBaselineDetector.h"

NVRAMBaselineDetector::NVRAMBaselineDetector(
        double kalmanFilterCoefficient, 
        uint16_t addr, 
        uint16_t segments, 
        uint32_t milliSecondsPerSegment,
        NVRAM * nvram,
        double (*chooseValueFunc)(double, double))
	{
        m_CurrentKalmanFilterValue = NAN;
        m_ChooseValue = chooseValueFunc;
        m_Segments = segments;
        m_MilliSecondsPerSegment = milliSecondsPerSegment;
        m_KalmanFilterCoefficient = kalmanFilterCoefficient;
        m_Buffer = new CyclicNVRAMBuffer(addr, m_Segments, nvram);
        m_LastBufferUpdate = millis();
	}

void NVRAMBaselineDetector::init()
{
    m_Buffer->initialize();
}

double NVRAMBaselineDetector::getCurrentBaseline()
{
    return isnan(m_CurrentBaseLine) ? m_CurrentTempBaseline : m_CurrentBaseLine;
}

void NVRAMBaselineDetector::updateCurrentBaseline()
{
    m_CurrentBaseLine = chooseValue(m_CurrentTempBaseline, findBaseline(m_Buffer, m_Segments));
}

double NVRAMBaselineDetector::chooseValue(double x, double y) 
{
    if(isnan(x) && !isnan(y)) return y;
    if(!isnan(x) && isnan(y)) return x;
    if(isnan(x) && isnan(y)) return NAN;
    return m_ChooseValue(x,y);
}

double NVRAMBaselineDetector::doKalman(double value)
{
    if (isnan(m_CurrentKalmanFilterValue))
    {
        m_CurrentKalmanFilterValue = value;
    }
    else
    {
        m_CurrentKalmanFilterValue = m_CurrentKalmanFilterValue - m_KalmanFilterCoefficient * (m_CurrentKalmanFilterValue - value);
    }

    return m_CurrentKalmanFilterValue;
}

double NVRAMBaselineDetector::findBaseline(CyclicNVRAMBuffer * buffer, int size)
{
    if (size < 1)
    {
        return NAN;
    }

    double result = buffer->get(0);
    for (int i = 1; i < size; i++)
    {
        result = chooseValue(result, buffer->get(i));
    }

    return result;
}

void NVRAMBaselineDetector::push(double value)
{
    double kv = doKalman(value);

    Serial.print(F("BD push = "));
    Serial.print(value, 2);

    if (isnan(m_CurrentTempBaseline))
    {
        m_CurrentTempBaseline = kv;
    }
    else
    {
        m_CurrentTempBaseline = chooseValue(m_CurrentTempBaseline, kv);
    }

    Serial.print(F(" | KV = "));
    Serial.print(kv, 2);
    Serial.print(F(" | CTBL = "));
    Serial.println(m_CurrentTempBaseline, 2);

    if (millis() - m_LastBufferUpdate > m_MilliSecondsPerSegment)
    {
        m_Buffer->put(m_CurrentTempBaseline);

        for (int i = 0; i < m_Segments; i++)
        {
            Serial.print(m_Buffer->get(i), 2); Serial.print("|");
        }

        Serial.println(m_Buffer->getPosition());

        m_LastBufferUpdate = millis();

        updateCurrentBaseline();

        m_CurrentTempBaseline = NAN;

        Serial.println(getCurrentBaseline());
    }
}
