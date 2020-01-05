#ifndef BaselineDetector_h_
#define BaselineDetector_h_

#include <math.h>

template<int DAYS, int DAY_SEGMENTS>
class BaselineDetector
{
public:
	BaselineDetector(double kalmanFilterCoefficient, double (*chooseValueFunc)(double, double))
	{
        m_ChooseValue = chooseValueFunc;
        this->m_KalmanFilterCoefficient = kalmanFilterCoefficient;
		for(int i = 0; i < DAYS; i++) m_DaysBuffer[i] = NAN;
        for(int i = 0; i < DAY_SEGMENTS; i++) m_HoursBuffer[i] = NAN;
        m_LastHoursBufferUpdate = millis();
	}  

    double getBaseline();
    void push(double value);
    void restoreBaseline(double value);

    double getCurrentTempBaseline()
    {
        return m_CurrentTempBaseline;
    }

private:
    double (*m_ChooseValue)(double, double);
    
    double chooseValue(double x, double y) 
    {
        if(isnan(x) && !isnan(y)) return y;
        if(!isnan(x) && isnan(y)) return x;
        if(isnan(x) && isnan(y)) return NAN;
        return m_ChooseValue(x,y);
    }

    double doKalman(double value)
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


    double m_KalmanFilterCoefficient;
    double m_CurrentKalmanFilterValue;
    double m_CurrentTempBaseline = NAN;
    unsigned long m_LastHoursBufferUpdate;
    int m_DaysBufferPos = -1;
    int m_HoursBufferPos = -1;
    double m_DaysBuffer[DAYS];
    double m_HoursBuffer[DAY_SEGMENTS];
    
    double findBaseline(double values[], int size)
    {
        if (size < 1)
        {
            return NAN;
        }

        double result = values[0];
        for (int i = 1; i < size; i++)
        {
            result = chooseValue(result, values[i]);
        }

        return result;
    }
};

template<int DAYS, int DAY_SEGMENTS>
double BaselineDetector<DAYS, DAY_SEGMENTS>::getBaseline()
{
    return chooseValue(findBaseline(m_HoursBuffer, DAY_SEGMENTS), findBaseline(m_DaysBuffer, DAYS));
}

template<int DAYS, int DAY_SEGMENTS>
void BaselineDetector<DAYS, DAY_SEGMENTS>::push(double value)
{
    double kv = doKalman(value);

    // Serial.print(F("BD push = "));
    // Serial.print(value, 2);
    if (isnan(m_CurrentTempBaseline))
    {
        m_CurrentTempBaseline = kv;
    }
    else
    {
        m_CurrentTempBaseline = chooseValue(m_CurrentTempBaseline, kv);
    }

    // Serial.print(F(" | CTBL = "));
    // Serial.println(m_CurrentTempBaseline, 2);

    if (millis() - m_LastHoursBufferUpdate > 1000L*60L*60L*24L/DAY_SEGMENTS)
    {
        if (this->m_HoursBufferPos == DAY_SEGMENTS -1)
        {
            // calculate baseline of HoursBuffer, transfer it to DaysBuffer
            double hoursBaseline = findBaseline(m_HoursBuffer, DAY_SEGMENTS);
            if (!isnan(hoursBaseline))
            {
                if (this->m_DaysBufferPos == DAYS - 1)
                {
                    this->m_DaysBufferPos = -1;
                }

                this->m_DaysBufferPos++;
                m_DaysBuffer[m_DaysBufferPos] = hoursBaseline;
            }
            // restart HoursBuffer
            this->m_HoursBufferPos = -1;
            for (int i = 0; i < DAY_SEGMENTS; i++)
            {
                this->m_HoursBuffer[i] = NAN;
            }
        }

        m_HoursBufferPos++;
        m_HoursBuffer[m_HoursBufferPos] = m_CurrentTempBaseline;

        m_CurrentTempBaseline = NAN;

        // for (int i = 0; i < DAY_SEGMENTS; i++)
        // {
        //     Serial.print(m_HoursBuffer[i], 2); Serial.print("|");
        // }
        // Serial.println(m_HoursBufferPos);

        // for (int i = 0; i < DAYS; i++)
        // {
        //     Serial.print(m_DaysBuffer[i], 2); Serial.print("|");
        // }
        // Serial.println(m_DaysBufferPos);

        m_LastHoursBufferUpdate = millis();
    }
}

template<int DAYS, int DAY_SEGMENTS>
void BaselineDetector<DAYS, DAY_SEGMENTS>::restoreBaseline(double value)
{
    this->m_CurrentKalmanFilterValue = value;
    this->m_DaysBufferPos = 0;
    this->m_DaysBuffer[0] = value;
    this->m_HoursBufferPos = 0;
    this->m_HoursBuffer[0] = value;
}

#endif