#include "denscalvalues.h"

class DensCalLightData : public QSharedData
{
public:
    int reflectionValue = 0;
    int transmissionValue = 0;
};

class DensCalGainData : public QSharedData
{
public:
    float low0 = qSNaN();
    float low1 = qSNaN();
    float med0 = qSNaN();
    float med1 = qSNaN();
    float high0 = qSNaN();
    float high1 = qSNaN();
    float max0 = qSNaN();
    float max1 = qSNaN();
};

class DensCalSlopeData : public QSharedData
{
public:
    float b0 = qSNaN();
    float b1 = qSNaN();
    float b2 = qSNaN();
};

class DensCalTargetData : public QSharedData
{
public:
    float loDensity = qSNaN();
    float loReading = qSNaN();
    float hiDensity = qSNaN();
    float hiReading = qSNaN();
};

DensCalLight::DensCalLight() : data(new DensCalLightData)
{
}

DensCalLight::DensCalLight(const DensCalLight &rhs)
    : data{rhs.data}
{
}

DensCalLight &DensCalLight::operator=(const DensCalLight &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

DensCalLight::~DensCalLight()
{
}

void DensCalLight::setReflectionValue(int reflectionValue) { data->reflectionValue = reflectionValue; }
int DensCalLight::reflectionValue() const { return data->reflectionValue; }

void DensCalLight::setTransmissionValue(int transmissionValue) { data->transmissionValue = transmissionValue; }
int DensCalLight::transmissionValue() const { return data->transmissionValue; }

bool DensCalLight::isValid() const
{
    if (data->reflectionValue <= 0 || data->transmissionValue <= 0) {
        return false;
    }

    if (data->reflectionValue > 128 || data->transmissionValue > 128) {
        return false;
    }

    return true;
}

DensCalGain::DensCalGain() : data(new DensCalGainData)
{
}

DensCalGain::DensCalGain(const DensCalGain &rhs)
    : data{rhs.data}
{
}

DensCalGain &DensCalGain::operator=(const DensCalGain &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

DensCalGain::~DensCalGain()
{
}

void DensCalGain::setLow0(float low0) { data->low0 = low0; }
float DensCalGain::low0() const { return data->low0; }

void DensCalGain::setLow1(float low1) { data->low1 = low1; }
float DensCalGain::low1() const { return data->low1; }

void DensCalGain::setMed0(float med0) { data->med0 = med0; }
float DensCalGain::med0() const { return data->med0; }

void DensCalGain::setMed1(float med1) { data->med1 = med1; }
float DensCalGain::med1() const { return data->med1; }

void DensCalGain::setHigh0(float high0) { data->high0 = high0; }
float DensCalGain::high0() const { return data->high0; }

void DensCalGain::setHigh1(float high1) { data->high1 = high1; }
float DensCalGain::high1() const { return data->high1; }

void DensCalGain::setMax0(float max0) { data->max0 = max0; }
float DensCalGain::max0() const { return data->max0; }

void DensCalGain::setMax1(float max1) { data->max1 = max1; }
float DensCalGain::max1() const { return data->max1; }

bool DensCalGain::isValid() const
{
    // Invalid if any values are NaN
    if (qIsNaN(data->low0) || qIsNaN(data->low1)
            || qIsNaN(data->med0) || qIsNaN(data->med1)
            || qIsNaN(data->high0) || qIsNaN(data->high1)
            || qIsNaN(data->max0) || qIsNaN(data->max1)) {
        return false;
    }

    // Invalid if low values are not effectively 1x
    if (qAbs(1.0F - data->low0) > 0.001F || qAbs(1.0F - data->low1) > 0.001F) {
        return false;
    }

    // Invalid if low is greater than medium
    if (data->low0 >= data->med0 || data->low1 >= data->med1) {
        return false;
    }

    // Invalid if medium is greater than high
    if (data->med0 >= data->high0 || data->med1 >= data->high1) {
        return false;
    }

    // Invalid if high is greater than max
    if (data->high0 >= data->max0 || data->high1 >= data->max1) {
        return false;
    }

    return true;
}

DensCalSlope::DensCalSlope() : data(new DensCalSlopeData)
{
}

DensCalSlope::DensCalSlope(const DensCalSlope &rhs)
    : data{rhs.data}
{
}

DensCalSlope &DensCalSlope::operator=(const DensCalSlope &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

DensCalSlope::~DensCalSlope()
{
}

void DensCalSlope::setB0(float b0) { data->b0 = b0; }
float DensCalSlope::b0() const { return data->b0; }

void DensCalSlope::setB1(float b1) { data->b1 = b1; }
float DensCalSlope::b1() const { return data->b1; }

void DensCalSlope::setB2(float b2) { data->b2 = b2; }
float DensCalSlope::b2() const { return data->b2; }

bool DensCalSlope::isValid() const
{
    // Invalid if any values are NaN
    if (qIsNaN(data->b0) || qIsNaN(data->b1) || qIsNaN(data->b2)) {
        return false;
    }

    return true;
}


DensCalTarget::DensCalTarget() : data(new DensCalTargetData)
{
}

DensCalTarget::DensCalTarget(const DensCalTarget &rhs)
    : data{rhs.data}
{
}

DensCalTarget &DensCalTarget::operator=(const DensCalTarget &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

DensCalTarget::~DensCalTarget()
{
}

void DensCalTarget::setLoDensity(float loDensity) { data->loDensity = loDensity; }
float DensCalTarget::loDensity() const { return data->loDensity; }

void DensCalTarget::setLoReading(float loValue) { data->loReading = loValue; }
float DensCalTarget::loReading() const { return data->loReading; }

void DensCalTarget::setHiDensity(float hiDensity) { data->hiDensity = hiDensity; }
float DensCalTarget::hiDensity() const { return data->hiDensity; }

void DensCalTarget::setHiReading(float hiValue) { data->hiReading = hiValue; }
float DensCalTarget::hiReading() const { return data->hiReading; }

bool DensCalTarget::isValid() const
{
    // Invalid if any values are NaN
    if (qIsNaN(data->loDensity) || qIsNaN(data->loReading)
            || qIsNaN(data->hiDensity) || qIsNaN(data->hiReading)) {
        return false;
    }

    // Invalid if any values are less than zero
    if (data->loDensity < 0 || data->loReading < 0
            || data->hiDensity < 0 || data->hiReading < 0) {
        return false;
    }

    // Invalid if low density is greater than high density
    if (data->loDensity >= data->hiDensity) {
        return false;
    }

    // Invalid if low reading is less than high reading
    if (data->loReading <= data->hiReading) {
        return false;
    }

    return true;
}

bool DensCalTarget::isValidReflection() const
{
    // Do general validity check
    if (!isValid()) {
        return false;
    }

    // Low density must be greater than zero
    if (data->loDensity < 0.01F) {
        return false;
    }

    return true;
}

bool DensCalTarget::isValidTransmission() const
{
    // Do general validity check
    if (!isValid()) {
        return false;
    }

    // Low density must be effectively zero
    if (qAbs(data->loDensity) > 0.001F) {
        return false;
    }

    return true;
}
