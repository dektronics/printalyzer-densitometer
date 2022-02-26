#include "densinterface.h"

#include <QDebug>

#include "denscommand.h"
#include "util.h"

DensInterface::DensInterface(QObject *parent)
    : QObject(parent)
    , serialPort_(nullptr)
    , multilinePending_(false)
    , connecting_(false)
    , connected_(false)
    , deviceUnrecognized_(false)
    , remoteControlEnabled_(false)
    , buildChecksum_(0)
    , freeRtosHeapSize_(0)
    , freeRtosHeapWatermark_(0)
    , freeRtosTaskCount_(0)
    , gainValues_{0,0,0,0,0,0,0,0}
    , slopeValues_{0,0,0}
    , reflValues_{0,0,0,0}
    , tranValues_{0,0,0,0}
{
}

bool DensInterface::connectToDevice(QSerialPort *serialPort)
{
    if (serialPort_) { return false; }
    if (!serialPort || !serialPort->isOpen()) {
        return false;
    }
    if (connected_ || connecting_) {
        return false;
    }

    // Set to connecting state
    connecting_ = true;
    deviceUnrecognized_ = false;
    remoteControlEnabled_ = false;

    // Connect to signals for non-blocking command use
    serialPort_ = serialPort;
    connect(serialPort_, &QSerialPort::errorOccurred, this, &DensInterface::handleError);
    connect(serialPort_, &QSerialPort::readyRead, this, &DensInterface::readData);

    // Send command to get system version, to verify connected device
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "V");
    return sendCommand(command);
}

void DensInterface::disconnectFromDevice()
{
    bool notify = connected_ || connecting_;
    if (serialPort_) {
        disconnect(serialPort_, &QSerialPort::errorOccurred, this, &DensInterface::handleError);
        disconnect(serialPort_, &QSerialPort::readyRead, this, &DensInterface::readData);
        serialPort_ = nullptr;
    }
    multilineResponse_ = DensCommand();
    multilineBuffer_.clear();
    multilinePending_ = false;
    connecting_ = false;
    connected_ = false;
    remoteControlEnabled_ = false;
    if (notify) {
        emit connectionClosed();
    }
}

void DensInterface::sendGetSystemVersion()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "V");
    sendCommand(command);
}

void DensInterface::sendGetSystemBuild()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "B");
    sendCommand(command);
}

void DensInterface::sendGetSystemDeviceInfo()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "DEV");
    sendCommand(command);
}

void DensInterface::sendGetSystemRtosInfo()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "RTOS");
    sendCommand(command);
}

void DensInterface::sendGetSystemUID()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "UID");
    sendCommand(command);
}

void DensInterface::sendGetSystemInternalSensors()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategorySystem, "ISEN");
    sendCommand(command);
}

void DensInterface::sendInvokeSystemRemoteControl(bool enabled)
{
    QStringList args;
    args.append(enabled ? "1" : "0");

    DensCommand command(DensCommand::TypeInvoke, DensCommand::CategorySystem, "REMOTE", args);
    sendCommand(command);
}

void DensInterface::sendSetMeasurementFormat(DensInterface::DensityFormat format)
{
    QStringList args;
    if (format == FormatBasic) {
        args.append("BASIC");
    } else if (format == FormatExtended) {
        args.append("EXT");
    } else {
        qWarning() << "Unsupported format:" << format;
        return;
    }

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryMeasurement, "FORMAT", args);
    sendCommand(command);
}

void DensInterface::sendSetAllowUncalibratedMeasurements(bool allow)
{
    QStringList args;
    if (allow) {
        args.append("1");
    } else {
        args.append("0");
    }

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryMeasurement, "UNCAL", args);
    sendCommand(command);
}

void DensInterface::sendGetDiagDisplayScreenshot()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategoryDiagnostics, "DISP");
    sendCommand(command);
}

void DensInterface::sendSetDiagLightRefl(int value)
{
    if (value < 0) { value = 0; }
    else if (value > 128) { value = 128; }

    QStringList args;
    args.append(QString::number(value));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryDiagnostics, "LR", args);
    sendCommand(command);
}

void DensInterface::sendSetDiagLightTran(int value)
{
    if (value < 0) { value = 0; }
    else if (value > 128) { value = 128; }

    QStringList args;
    args.append(QString::number(value));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryDiagnostics, "LT", args);
    sendCommand(command);
}

void DensInterface::sendInvokeDiagSensorStart()
{
    DensCommand command(DensCommand::TypeInvoke, DensCommand::CategoryDiagnostics, "S",
                        QStringList() << "START");
    sendCommand(command);
}

void DensInterface::sendInvokeDiagSensorStop()
{
    DensCommand command(DensCommand::TypeInvoke, DensCommand::CategoryDiagnostics, "S",
                        QStringList() << "STOP");
    sendCommand(command);
}

void DensInterface::sendSetDiagSensorConfig(int gain, int integration)
{
    if (gain < 0) { gain = 0; }
    else if (gain > 3) { gain = 3; }
    if (integration < 0) { integration = 0; }
    else if (integration > 5) { integration = 5; }

    QStringList args;
    args.append("CFG");
    args.append(QString::number(gain));
    args.append(QString::number(integration));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryDiagnostics, "S", args);
    sendCommand(command);
}

void DensInterface::sendInvokeDiagRead(DensInterface::SensorLight light, int gain, int integration)
{
    QStringList args;
    if (light == SensorLight::SensorLightReflection) {
        args.append("R");
    } else if (light == SensorLight::SensorLightTransmission) {
        args.append("T");
    } else {
        args.append("0");
    }
    args.append(QString::number(gain));
    args.append(QString::number(integration));

    DensCommand command(DensCommand::TypeInvoke, DensCommand::CategoryDiagnostics, "READ", args);
    sendCommand(command);
}

void DensInterface::sendSetDiagLoggingModeUsb()
{
    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryDiagnostics,
                        "LOG", QStringList() << "U");
    sendCommand(command);
}

void DensInterface::sendSetDiagLoggingModeDebug()
{
    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryDiagnostics,
                        "LOG", QStringList() << "D");
    sendCommand(command);
}

void DensInterface::sendGetCalGain()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategoryCalibration, "GAIN");
    sendCommand(command);
}

void DensInterface::sentSetCalGain(
        float medium0, float medium1, float high0, float high1,
        float maximum0, float maximum1)
{
    QStringList args;
    args.append(util::encode_f32(medium0));
    args.append(util::encode_f32(medium1));
    args.append(util::encode_f32(high0));
    args.append(util::encode_f32(high1));
    args.append(util::encode_f32(maximum0));
    args.append(util::encode_f32(maximum1));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryCalibration, "GAIN", args);
    sendCommand(command);
}

void DensInterface::sendGetCalSlope()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategoryCalibration, "SLOPE");
    sendCommand(command);
}

void DensInterface::sendSetCalSlope(float b0, float b1, float b2)
{
    QStringList args;
    args.append(util::encode_f32(b0));
    args.append(util::encode_f32(b1));
    args.append(util::encode_f32(b2));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryCalibration, "SLOPE", args);
    sendCommand(command);
}

void DensInterface::sendGetCalReflection()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategoryCalibration, "REFL");
    sendCommand(command);
}

void DensInterface::sendSetCalReflection(float loDensity, float loReading, float hiDensity, float hiReading)
{
    QStringList args;
    args.append(util::encode_f32(loDensity));
    args.append(util::encode_f32(loReading));
    args.append(util::encode_f32(hiDensity));
    args.append(util::encode_f32(hiReading));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryCalibration, "REFL", args);
    sendCommand(command);
}

void DensInterface::sendGetCalTransmission()
{
    DensCommand command(DensCommand::TypeGet, DensCommand::CategoryCalibration, "TRAN");
    sendCommand(command);
}

void DensInterface::sendSetCalTransmission(float loDensity, float loReading, float hiDensity, float hiReading)
{
    QStringList args;
    args.append(util::encode_f32(loDensity));
    args.append(util::encode_f32(loReading));
    args.append(util::encode_f32(hiDensity));
    args.append(util::encode_f32(hiReading));

    DensCommand command(DensCommand::TypeSet, DensCommand::CategoryCalibration, "TRAN", args);
    sendCommand(command);
}

bool DensInterface::connected() const { return connected_; }
bool DensInterface::deviceUnrecognized() const { return deviceUnrecognized_; }
bool DensInterface::remoteControlEnabled() const { return remoteControlEnabled_; }

QString DensInterface::projectName() const { return projectName_; }
QString DensInterface::version() const { return version_; }
QDateTime DensInterface::buildDate() const { return buildDate_; }
QString DensInterface::buildDescribe() const { return buildDescribe_; }
uint32_t DensInterface::buildChecksum() const { return buildChecksum_; }

QString DensInterface::halVersion() const { return halVersion_; }
QString DensInterface::mcuDeviceId() const { return mcuDeviceId_; }
QString DensInterface::mcuRevisionId() const { return mcuRevisionId_; }
QString DensInterface::mcuSysClock() const { return mcuSysClock_; }

QString DensInterface::freeRtosVersion() const { return freeRtosVersion_; }
uint32_t DensInterface::freeRtosHeapSize() const { return freeRtosHeapSize_; }
uint32_t DensInterface::freeRtosHeapWatermark() const { return freeRtosHeapWatermark_; }
uint32_t DensInterface::freeRtosTaskCount() const { return freeRtosTaskCount_; }

QString DensInterface::uniqueId() const { return uniqueId_; }

QString DensInterface::mcuVdda() const { return mcuVdda_; }
QString DensInterface::mcuTemp() const { return mcuTemp_; }

float DensInterface::calGainLow0() const { return gainValues_[0]; }
float DensInterface::calGainLow1() const { return gainValues_[1]; }
float DensInterface::calGainMedium0() const { return gainValues_[2]; }
float DensInterface::calGainMedium1() const { return gainValues_[3]; }
float DensInterface::calGainHigh0() const { return gainValues_[4]; }
float DensInterface::calGainHigh1() const { return gainValues_[5]; }
float DensInterface::calGainMaximum0() const { return gainValues_[6]; }
float DensInterface::calGainMaximum1() const { return gainValues_[7]; }

float DensInterface::calSlopeB0() const { return slopeValues_[0]; }
float DensInterface::calSlopeB1() const { return slopeValues_[1]; }
float DensInterface::calSlopeB2() const { return slopeValues_[2]; }

float DensInterface::calReflectionLoDensity() const { return reflValues_[0]; }
float DensInterface::calReflectionLoReading() const { return reflValues_[1]; }
float DensInterface::calReflectionHiDensity() const { return reflValues_[2]; }
float DensInterface::calReflectionHiReading() const { return reflValues_[3]; }

float DensInterface::calTransmissionLoDensity() const { return tranValues_[0]; }
float DensInterface::calTransmissionLoReading() const { return tranValues_[1]; }
float DensInterface::calTransmissionHiDensity() const { return tranValues_[2]; }
float DensInterface::calTransmissionHiReading() const { return tranValues_[3]; }

void DensInterface::readData()
{
    while (serialPort_->canReadLine()) {
        const QByteArray line = serialPort_->readLine();
        if (connecting_) {
            // In connecting mode we expect to only receive very specific
            // information from the device. Anything else will cause the
            // connection check to fail.
            DensCommand response = DensCommand::parse(line);
            if (response.isDensity()) {
                // Density responses are the only thing the device can send
                // without first receiving a command or mode change request.
                // Therefore, we need to ignore those here.
                continue;
            } else if (response.isValid()
                        && response.category() == DensCommand::CategorySystem
                        && response.type() == DensCommand::TypeGet
                        && response.action() == QLatin1String("V")) {
                // System version responses are the only thing expected and
                // handled in this state, so pre-validate the response type
                // before calling the normal parsing code.

                // Save and clear old values, in case the parsing failed
                QString oldProjectName = projectName_;
                QString oldVersion = version_;
                projectName_ = QString();
                version_ = QString();

                // Call the normal command parser
                readCommandResponse(response);

                if (!projectName_.isEmpty() && !version_.isEmpty()) {
                    connecting_ = false;
                    connected_ = true;
                    emit connectionOpened();
                    emit systemVersionResponse();
                } else {
                    // Failing to cleanly parse the version response should
                    // be treated like a connection failure
                    projectName_ = oldProjectName;
                    version_ = oldVersion;
                    qWarning() << "Unexpected version response:" << line;
                    deviceUnrecognized_ = true;
                    disconnectFromDevice();
                }
            } else {
                // Any response other than what is explicitly expected should
                // be treated as a connection failure
                qWarning() << "Unexpected response:" << line;
                deviceUnrecognized_ = true;
                disconnectFromDevice();
            }
            return;
        }

        if (multilinePending_) {
            if (line == "]]\r\n") {
                multilineResponse_.setBuffer(multilineBuffer_);
                multilineBuffer_.clear();
                multilinePending_ = false;

                if (multilineResponse_.isValid()) {
                    readCommandResponse(multilineResponse_);
                } else {
                    qWarning() << "Unrecognized line:" << line;
                }
                multilineResponse_ = DensCommand();
                continue;
            } else {
                multilineBuffer_.append(line);
                continue;
            }
        }

        if (isLogLine(line)) {
            emit diagLogLine(line);
        } else {
            DensCommand response = DensCommand::parse(line);

            if (response.args().size() == 1 && response.args().at(0) == QLatin1String("NAK")) {
                qWarning() << "Invalid command:" << response.toString();
            } else if (response.args().size() == 1 && response.args().at(0) == QLatin1String("[[")) {
                multilineResponse_ = response;
                multilineBuffer_.clear();
                multilinePending_ = true;
            } else {
                if (response.isDensity()) {
                    readDensityResponse(response);
                } else if (response.isValid()) {
                    readCommandResponse(response);
                } else {
                    qWarning() << "Unrecognized line:" << line;
                }
            }
        }
    }
}

void DensInterface::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << "Serial port error:" << error;
    emit connectionError();
}

bool DensInterface::isLogLine(const QByteArray &line)
{
    return line.size() > 2 && line[1] == '/'
            && (line[0] == 'A' || line[0] == 'E' || line[0] == 'W'
            || line[0] == 'I' || line[0] == 'D' || line[0] == 'V');
}

void DensInterface::readDensityResponse(const DensCommand &response)
{
    qDebug() << "Read:" << response.toString();
    if (!response.args().isEmpty() && response.args().at(0).endsWith(QLatin1Char('D'))) {
        DensityType densityType;
        float dValue;
        float rawValue = qSNaN();
        float corrValue = qSNaN();

        if (response.type() == DensCommand::TypeDensityReflection) {
            densityType = DensityReflection;
        } else if (response.type() == DensCommand::TypeDensityTransmission) {
            densityType = DensityTransmission;
        } else {
            return;
        }

        if (response.args().size() > 1) {
            dValue = util::decode_f32(response.args().at(1));
            if (response.args().size() > 2) {
                rawValue = util::decode_f32(response.args().at(2));
            }
            if (response.args().size() > 3) {
                corrValue = util::decode_f32(response.args().at(3));
            }
        } else {
            QString readingStr = response.args().at(0);
            readingStr.chop(1);
            bool ok;
            dValue = readingStr.toFloat(&ok);
            if (!ok) {
                qWarning() << "Bad reading value:" << response.args().at(0);
                return;
            }
        }

        emit densityReading(densityType, dValue, rawValue, corrValue);
    }
}

void DensInterface::readCommandResponse(const DensCommand &response)
{
    switch (response.category()) {
    case DensCommand::CategorySystem:
        readSystemResponse(response);
        break;
    case DensCommand::CategoryMeasurement:
        readMeasurementResponse(response);
        break;
    case DensCommand::CategoryCalibration:
        readCalibrationResponse(response);
        break;
    case DensCommand::CategoryDiagnostics:
        readDiagnosticsResponse(response);
        break;
    default:
        break;
    }
}

void DensInterface::readSystemResponse(const DensCommand &response)
{
    if (response.type() == DensCommand::TypeGet) {
        const QStringList args = response.args();
        if (response.action() == QLatin1String("V")) {
            if (args.length() > 0) {
                projectName_ = args.at(0);
            }
            if (args.length() > 1) {
                version_ = args.at(1);
            }
            if (!connecting_) {
                emit systemVersionResponse();
            }
        } else if (response.action() == QLatin1String("B")) {
            if (args.length() > 0) {
                buildDate_ = QDateTime::fromString(args.at(0), "yyyy-MM-dd hh:mm");
            }
            if (args.length() > 1) {
                buildDescribe_ = args.at(1);
            }
            if (args.length() > 2) {
                bool ok;
                buildChecksum_ = args.at(2).toUInt(&ok, 16);
                if (!ok) { buildChecksum_ = 0; }
            }
            emit systemBuildResponse();
        } else if (response.action() == QLatin1String("DEV")) {
            if (args.length() > 0) {
                halVersion_ = args.at(0).trimmed();
            }
            if (args.length() > 1) {
                mcuDeviceId_ = args.at(1);
            }
            if (args.length() > 2) {
                mcuRevisionId_ = args.at(2);
            }
            if (args.length() > 3) {
                mcuSysClock_ = args.at(3);
            }
            emit systemDeviceResponse();
        } else if (response.action() == QLatin1String("RTOS")) {
            if (args.length() > 0) {
                freeRtosVersion_ = args.at(0).trimmed();
            }
            if (args.length() > 1) {
                freeRtosHeapSize_ = args.at(1).toUInt();
            }
            if (args.length() > 2) {
                freeRtosHeapWatermark_ = args.at(2).toUInt();
            }
            if (args.length() > 3) {
                freeRtosTaskCount_ = args.at(3).toUInt();
            }
            emit systemRtosResponse();
        } else if (response.action() == QLatin1String("UID")) {
            if (args.length() > 0) {
                uniqueId_ = args.at(0);
            }
            emit systemUniqueId();
        } else if (response.action() == QLatin1String("ISEN")) {
            if (args.length() > 0) {
                mcuVdda_ = args.at(0);
            }
            if (args.length() > 1) {
                mcuTemp_ = args.at(1);
            }
            emit systemInternalSensors();
        }
    } else if (response.type() == DensCommand::TypeInvoke) {
        const QStringList args = response.args();
        if (response.action() == QLatin1String("REMOTE") && args.length() > 0) {
            remoteControlEnabled_ = (args.at(0) == QLatin1String("1"));
            emit systemRemoteControl(remoteControlEnabled_);
        }
    }
}

void DensInterface::readMeasurementResponse(const DensCommand &response)
{
    if (response.type() == DensCommand::TypeSet
            && response.action() == QLatin1String("FORMAT")
            && !response.args().isEmpty()
            && response.args().at(0) == QLatin1String("OK")) {
        emit measurementFormatChanged();
    } else if (response.type() == DensCommand::TypeSet
               && response.action() == QLatin1String("UNCAL")
               && !response.args().isEmpty()
               && response.args().at(0) == QLatin1String("OK")) {
        emit allowUncalibratedMeasurementsChanged();
    }
}

void DensInterface::readCalibrationResponse(const DensCommand &response)
{
    if (response.type() == DensCommand::TypeGet
            && response.action() == QLatin1String("GAIN")
            && response.args().length() == 8) {
        for (int i = 0; i < 8; i++) {
            gainValues_[i] = util::decode_f32(response.args().at(i));
        }
        emit calGainResponse();
    } else if (response.type() == DensCommand::TypeGet
               && response.action() == QLatin1String("SLOPE")
               && response.args().length() == 3) {
        for (int i = 0; i < 3; i++) {
            slopeValues_[i] = util::decode_f32(response.args().at(i));
        }
        emit calSlopeResponse();
    } else if (response.type() == DensCommand::TypeGet
            && response.action() == QLatin1String("REFL")
            && response.args().length() == 4) {
        for (int i = 0; i < 4; i++) {
            reflValues_[i] = util::decode_f32(response.args().at(i));
        }
        emit calReflectionResponse();
    } else if (response.type() == DensCommand::TypeGet
            && response.action() == QLatin1String("TRAN")
            && response.args().length() == 4) {
        for (int i = 0; i < 4; i++) {
            tranValues_[i] = util::decode_f32(response.args().at(i));
        }
        emit calTransmissionResponse();
    }
}

void DensInterface::readDiagnosticsResponse(const DensCommand &response)
{
    if (response.type() == DensCommand::TypeGet
            && response.action() == QLatin1String("DISP")
            && !response.buffer().isEmpty()) {
        emit diagDisplayScreenshot(response.buffer());
    } else if (response.type() == DensCommand::TypeSet
               && response.action() == QLatin1String("LR")
               && response.args().size() == 1
               && response.args().at(0) == QLatin1String("OK")) {
        emit diagLightReflChanged();
    } else if (response.type() == DensCommand::TypeSet
               && response.action() == QLatin1String("LT")
               && response.args().size() == 1
               && response.args().at(0) == QLatin1String("OK")) {
        emit diagLightTranChanged();
    } else if (response.type() == DensCommand::TypeInvoke
               && response.action() == QLatin1String("S")
               && response.args().size() == 1
               && response.args().at(0) == QLatin1String("OK")) {
        emit diagSensorInvoked();
    } else if (response.type() == DensCommand::TypeSet
               && response.action() == QLatin1String("S")
               && response.args().size() == 1
               && response.args().at(0) == QLatin1String("OK")) {
        emit diagSensorChanged();
    } else if (response.type() == DensCommand::TypeGet
               && response.action() == QLatin1String("S")
               && response.args().size() >= 2) {
        emit diagSensorGetReading(
                    response.args().at(0).toInt(),
                    response.args().at(1).toInt());
    } else if (response.type() == DensCommand::TypeInvoke
               && response.action() == QLatin1String("READ")
               && response.args().size() >= 2) {
        emit diagSensorInvokeReading(
                    response.args().at(0).toInt(),
                    response.args().at(1).toInt());
    } else if (response.type() == DensCommand::TypeGet
            && response.action() == QLatin1String("LOG")
            && response.args().size() == 1
            && response.args().at(0) == QLatin1String("OK")) {
        qDebug() << "Logging mode changed";
    } else {
        qDebug() << response.toString();
    }
}

bool DensInterface::sendCommand(const DensCommand &command)
{
    if (!serialPort_ || !serialPort_->isOpen() || !command.isValid()) {
        return false;
    }

    QByteArray commandBytes = command.toString().toLatin1();
    commandBytes.append("\r\n");
    return serialPort_->write(commandBytes) != -1;
}
