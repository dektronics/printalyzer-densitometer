#ifndef DENSINTERFACE_H
#define DENSINTERFACE_H

#include <stdint.h>
#include <QObject>
#include <QSerialPort>
#include <QDateTime>
#include "denscommand.h"

class DensInterface : public QObject
{
    Q_OBJECT
public:
    enum DensityType {
        DensityReflection,
        DensityTransmission,
        DensityUnknown = -1
    };
    Q_ENUM(DensityType)

    enum DensityFormat {
        FormatBasic,
        FormatExtended
    };
    Q_ENUM(DensityFormat)

    explicit DensInterface(QObject *parent = nullptr);
    bool connectToDevice(QSerialPort *serialPort);
    void disconnectFromDevice();

public slots:
    void sendGetSystemVersion();
    void sendGetSystemBuild();
    void sendGetSystemDeviceInfo();
    void sendGetSystemRtosInfo();
    void sendGetSystemUID();
    void sendGetSystemInternalSensors();

    void sendSetMeasurementFormat(DensityFormat format);

    void sendGetDiagDisplayScreenshot();
    void sendSetDiagLoggingModeUsb();
    void sendSetDiagLoggingModeDebug();

    void sendGetCalGain();
    void sentSetCalGain(
            float medium0, float medium1, float high0, float high1,
            float maximum0, float maximum1);
    void sendGetCalSlope();
    void sendSetCalSlope(float b0, float b1, float b2);
    void sendGetCalReflection();
    void sendSetCalReflection(float loDensity, float loReading, float hiDensity, float hiReading);
    void sendGetCalTransmission();
    void sendSetCalTransmission(float loDensity, float loReading, float hiDensity, float hiReading);

public:
    bool connected() const;

    QString projectName() const;
    QString version() const;

    QDateTime buildDate() const;
    QString buildDescribe() const;
    uint32_t buildChecksum() const;

    QString halVersion() const;
    QString mcuDeviceId() const;
    QString mcuRevisionId() const;
    QString mcuSysClock() const;

    QString freeRtosVersion() const;
    uint32_t freeRtosHeapSize() const;
    uint32_t freeRtosHeapWatermark() const;
    uint32_t freeRtosTaskCount() const;

    QString uniqueId() const;

    QString mcuVdda() const;
    QString mcuTemp() const;

    float calGainLow0() const;
    float calGainLow1() const;
    float calGainMedium0() const;
    float calGainMedium1() const;
    float calGainHigh0() const;
    float calGainHigh1() const;
    float calGainMaximum0() const;
    float calGainMaximum1() const;

    float calSlopeB0() const;
    float calSlopeB1() const;
    float calSlopeB2() const;

    float calReflectionLoDensity() const;
    float calReflectionLoReading() const;
    float calReflectionHiDensity() const;
    float calReflectionHiReading() const;

    float calTransmissionLoDensity() const;
    float calTransmissionLoReading() const;
    float calTransmissionHiDensity() const;
    float calTransmissionHiReading() const;

signals:
    void connectionError();

    void densityReading(DensityType type, float dValue, float rawValue, float corrValue);
    void measurementFormatChanged();

    void systemVersionResponse();
    void systemBuildResponse();
    void systemDeviceResponse();
    void systemRtosResponse();
    void systemUniqueId();
    void systemInternalSensors();

    void diagDisplayScreenshot(const QByteArray &data);

    void calGainResponse();
    void calSlopeResponse();
    void calReflectionResponse();
    void calTransmissionResponse();

private slots:
    void readData();
    void handleError(QSerialPort::SerialPortError error);

private:
    static bool isLogLine(const QByteArray &line);
    void readDensityResponse(const DensCommand &response);
    void readCommandResponse(const DensCommand &response);
    void readSystemResponse(const DensCommand &response);
    void readMeasurementResponse(const DensCommand &response);
    void readCalibrationResponse(const DensCommand &response);
    void readDiagnosticsResponse(const DensCommand &response);

    void sendCommand(const DensCommand &command);

    QSerialPort *serialPort_;
    bool multilinePending_;
    DensCommand multilineResponse_;
    QByteArray multilineBuffer_;
    bool connected_;

    QString projectName_;
    QString version_;
    QDateTime buildDate_;
    QString buildDescribe_;
    uint32_t buildChecksum_;
    QString halVersion_;
    QString mcuRevisionId_;
    QString mcuDeviceId_;
    QString mcuSysClock_;
    QString freeRtosVersion_;
    uint32_t freeRtosHeapSize_;
    uint32_t freeRtosHeapWatermark_;
    uint32_t freeRtosTaskCount_;
    QString uniqueId_;
    QString mcuVdda_;
    QString mcuTemp_;
    float gainValues_[8];
    float slopeValues_[3];
    float reflValues_[4];
    float tranValues_[4];
};

#endif // DENSINTERFACE_H
