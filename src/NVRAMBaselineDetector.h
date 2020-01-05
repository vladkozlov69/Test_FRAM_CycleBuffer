#ifndef BaselineDetector_h_
#define BaselineDetector_h_

#include <math.h>
#include "CyclicNVRAMBuffer.h"

class NVRAMBaselineDetector
{
private:
    double m_KalmanFilterCoefficient;
    double m_CurrentKalmanFilterValue;
    double m_CurrentTempBaseline = NAN;
    unsigned long m_LastBufferUpdate;
    double m_CurrentBaseLine;
    uint16_t m_Segments;
    CyclicNVRAMBuffer * m_Buffer;
public:
	NVRAMBaselineDetector(
        double kalmanFilterCoefficient, 
        uint16_t addr, 
        uint16_t segments, 
        NVRAM * nvram,
        double (*chooseValueFunc)(double, double)); 

    void init();
    double getCurrentBaseline();
    void push(double value);
private:
    double (*m_ChooseValue)(double, double);
    void updateCurrentBaseline();   
    double chooseValue(double x, double y); 
    //double getCurrentTempBaseline();
    double doKalman(double value);
    double findBaseline(CyclicNVRAMBuffer * buffer, int size);
};


#endif