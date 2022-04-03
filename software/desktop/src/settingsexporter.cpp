#include "settingsexporter.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QDebug>

SettingsExporter::SettingsExporter(DensInterface *densInterface, QObject *parent)
    : QObject{parent},
      densInterface_(densInterface)
{
    timer_ = new QTimer(this);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, this, QOverload<>::of(&SettingsExporter::onPrepareTimeout));

    connect(densInterface_, &DensInterface::connectionClosed, this, &SettingsExporter::onConnectionClosed);
    connect(densInterface_, &DensInterface::systemVersionResponse, this, &SettingsExporter::onSystemVersionResponse);
    connect(densInterface_, &DensInterface::systemBuildResponse, this, &SettingsExporter::onSystemBuildResponse);
    connect(densInterface_, &DensInterface::systemUniqueId, this, &SettingsExporter::onSystemUniqueId);

    connect(densInterface_, &DensInterface::calGainResponse, this, &SettingsExporter::onCalGainResponse);
    connect(densInterface_, &DensInterface::calSlopeResponse, this, &SettingsExporter::onCalSlopeResponse);
    connect(densInterface_, &DensInterface::calReflectionResponse, this, &SettingsExporter::onCalReflectionResponse);
    connect(densInterface_, &DensInterface::calTransmissionResponse, this, &SettingsExporter::onCalTransmissionResponse);
}

void SettingsExporter::prepareExport()
{
    qDebug() << "Getting all settings for export";
    densInterface_->sendGetSystemVersion();
    densInterface_->sendGetSystemBuild();
    densInterface_->sendGetSystemUID();
    densInterface_->sendGetCalGain();
    densInterface_->sendGetCalSlope();
    densInterface_->sendGetCalReflection();
    densInterface_->sendGetCalTransmission();

    timer_->start(5000);
}

bool SettingsExporter::saveExport(const QString &filename)
{
    if (prepareFailed_ || !hasAllData_ || filename.isEmpty()) { return false; }
    qDebug() << "Saving data to file:" << filename;

    // File header information
    QJsonObject jsonHeader;
    jsonHeader["version"] = QString::number(1);
    jsonHeader["date"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");

    // General system properties, for reference
    QJsonObject jsonSystem;
    jsonSystem["name"] = densInterface_->projectName();
    jsonSystem["version"] = densInterface_->version();
    jsonSystem["buildDate"] = densInterface_->buildDate().toString("yyyy-MM-dd hh:mm");
    jsonSystem["buildDescribe"] = densInterface_->buildDescribe();
    jsonSystem["checksum"] = QString::number(densInterface_->buildChecksum(), 16);
    jsonSystem["uid"] = densInterface_->uniqueId();

    // Sensor calibration
    QJsonObject jsonCalGain;
    const DensCalGain calGain = densInterface_->calGain();
    jsonCalGain["L0"] = QString::number(calGain.low0(), 'f', 6);
    jsonCalGain["L1"] = QString::number(calGain.low1(), 'f', 6);
    jsonCalGain["M0"] = QString::number(calGain.med0(), 'f', 6);
    jsonCalGain["M1"] = QString::number(calGain.med1(), 'f', 6);
    jsonCalGain["H0"] = QString::number(calGain.high0(), 'f', 6);
    jsonCalGain["H1"] = QString::number(calGain.high1(), 'f', 6);
    jsonCalGain["X0"] = QString::number(calGain.max0(), 'f', 6);
    jsonCalGain["X1"] = QString::number(calGain.max1(), 'f', 6);

    QJsonObject jsonCalSlope;
    const DensCalSlope calSlope = densInterface_->calSlope();
    jsonCalSlope["B0"] = QString::number(calSlope.b0(), 'f', 6);
    jsonCalSlope["B1"] = QString::number(calSlope.b1(), 'f', 6);
    jsonCalSlope["B2"] = QString::number(calSlope.b2(), 'f', 6);

    QJsonObject jsonCalSensor;
    jsonCalSensor["gain"] = jsonCalGain;
    jsonCalSensor["slope"] = jsonCalSlope;

    // Target calibration
    const DensCalTarget calReflection = densInterface_->calReflection();
    const DensCalTarget calTransmission = densInterface_->calTransmission();

    QJsonObject jsonCalReflLo;
    jsonCalReflLo["density"] = QString::number(calReflection.loDensity(), 'f', 2);
    jsonCalReflLo["reading"] = QString::number(calReflection.loReading(), 'f', 6);

    QJsonObject jsonCalReflHi;
    jsonCalReflHi["density"] = QString::number(calReflection.hiDensity(), 'f', 2);
    jsonCalReflHi["reading"] = QString::number(calReflection.hiReading(), 'f', 6);

    QJsonObject jsonCalRefl;
    jsonCalRefl["cal-lo"] = jsonCalReflLo;
    jsonCalRefl["cal-hi"] = jsonCalReflHi;

    QJsonObject jsonCalTranLo;
    jsonCalTranLo["density"] = QString::number(calTransmission.loDensity(), 'f', 2);
    jsonCalTranLo["reading"] = QString::number(calTransmission.loReading(), 'f', 6);

    QJsonObject jsonCalTranHi;
    jsonCalTranHi["density"] = QString::number(calTransmission.hiDensity(), 'f', 2);
    jsonCalTranHi["reading"] = QString::number(calTransmission.hiReading(), 'f', 6);

    QJsonObject jsonCalTran;
    jsonCalTran["cal-lo"] = jsonCalTranLo;
    jsonCalTran["cal-hi"] = jsonCalTranHi;

    QJsonObject jsonCalTarget;
    jsonCalTarget["reflection"] = jsonCalRefl;
    jsonCalTarget["transmission"] = jsonCalTran;

    // Combining calibration objects
    QJsonObject jsonCal;
    jsonCal["sensor"] = jsonCalSensor;
    jsonCal["target"] = jsonCalTarget;

    // Top level JSON object
    QJsonObject jsonExport;
    jsonExport["header"] = jsonHeader;
    jsonExport["system"] = jsonSystem;
    jsonExport["calibration"] = jsonCal;

    QFile exportFile(filename);

    if (!exportFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open export file.";
        return false;
    }

    exportFile.write(QJsonDocument(jsonExport).toJson(QJsonDocument::Indented));
    exportFile.close();
    return true;
}

void SettingsExporter::onPrepareTimeout()
{
    if (!hasAllData_) {
        timer_->stop();
        prepareFailed_ = true;
        emit exportFailed();
    }
}

void SettingsExporter::onConnectionClosed()
{
    if (!hasAllData_) {
        timer_->stop();
        prepareFailed_ = true;
        emit exportFailed();
    }
}

void SettingsExporter::onSystemVersionResponse()
{
    hasSystemVersion_ = true;
    checkResponses();
}

void SettingsExporter::onSystemBuildResponse()
{
    hasSystemBuild_ = true;
    checkResponses();
}

void SettingsExporter::onSystemUniqueId()
{
    hasSystemUid_ = true;
    checkResponses();
}

void SettingsExporter::onCalGainResponse()
{
    hasCalGain_ = true;
    checkResponses();
}

void SettingsExporter::onCalSlopeResponse()
{
    hasCalSlope_ = true;
    checkResponses();
}

void SettingsExporter::onCalReflectionResponse()
{
    hasCalReflection_ = true;
    checkResponses();
}

void SettingsExporter::onCalTransmissionResponse()
{
    hasCalTransmission_ = true;
    checkResponses();
}

void SettingsExporter::checkResponses()
{
    if (hasSystemVersion_ && hasSystemBuild_ && hasSystemUid_
            && hasCalGain_ && hasCalSlope_
            && hasCalReflection_ && hasCalTransmission_) {
        hasAllData_ = true;
        timer_->stop();
        emit exportReady();
    }
}
