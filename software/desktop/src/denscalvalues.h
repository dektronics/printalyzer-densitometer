#ifndef DENSCALVALUES_H
#define DENSCALVALUES_H

#include <QSharedDataPointer>

class DensCalLightData;
class DensCalGainData;
class DensCalSlopeData;
class DensCalTargetData;

class DensCalLight
{
public:
    DensCalLight();
    DensCalLight(const DensCalLight &);
    DensCalLight &operator=(const DensCalLight &);
    ~DensCalLight();

    void setReflectionValue(int reflectionValue);
    int reflectionValue() const;

    void setTransmissionValue(int transmissionValue);
    int transmissionValue() const;

    bool isValid() const;

private:
    QSharedDataPointer<DensCalLightData> data;
};

class DensCalGain
{
public:
    DensCalGain();
    DensCalGain(const DensCalGain &);
    DensCalGain &operator=(const DensCalGain &);
    ~DensCalGain();

    void setLow0(float low0);
    float low0() const;

    void setLow1(float low1);
    float low1() const;

    void setMed0(float med0);
    float med0() const;

    void setMed1(float med1);
    float med1() const;

    void setHigh0(float high0);
    float high0() const;

    void setHigh1(float high1);
    float high1() const;

    void setMax0(float max0);
    float max0() const;

    void setMax1(float max1);
    float max1() const;

    bool isValid() const;

private:
    QSharedDataPointer<DensCalGainData> data;
};

class DensCalSlope
{
public:
    DensCalSlope();
    DensCalSlope(const DensCalSlope &);
    DensCalSlope &operator=(const DensCalSlope &);
    ~DensCalSlope();

    void setB0(float b0);
    float b0() const;

    void setB1(float b1);
    float b1() const;

    void setB2(float b2);
    float b2() const;

    bool isValid() const;

private:
    QSharedDataPointer<DensCalSlopeData> data;
};

class DensCalTarget
{
public:
    DensCalTarget();
    DensCalTarget(const DensCalTarget &);
    DensCalTarget &operator=(const DensCalTarget &);
    ~DensCalTarget();

    void setLoDensity(float loDensity);
    float loDensity() const;

    void setLoReading(float loValue);
    float loReading() const;

    void setHiDensity(float hiDensity);
    float hiDensity() const;

    void setHiReading(float hiValue);
    float hiReading() const;

    bool isValidReflection() const;
    bool isValidTransmission() const;

private:
    bool isValid() const;
    QSharedDataPointer<DensCalTargetData> data;
};

#endif // DENSCALVALUES_H
