#include "headlesstask.h"

#include <iostream>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include "qsimplesignalaggregator.h"
#include "settingsexporter.h"

HeadlessTask::HeadlessTask(QObject *parent)
    : QObject{parent}
    , serialPort_(new QSerialPort(this))
    , densInterface_(new DensInterface(this))
{
}

void HeadlessTask::setPort(const QString &portName)
{
    portName_ = portName;
}

void HeadlessTask::setCommand(HeadlessTask::Command command, const QString &commandArg)
{
    command_ = command;
    commandArg_ = commandArg;
}

void HeadlessTask::run()
{
    if (!connectToDevice()) {
        emit finished();
        return;
    }

    densInterface_->sendSetMeasurementFormat(DensInterface::FormatExtended);
    densInterface_->sendSetAllowUncalibratedMeasurements(true);

    if (command_ == HeadlessTask::CommandSystemInfo) {
        systemInfoStart();
    } else if (command_ == HeadlessTask::CommandExportSettings) {
        exportSettingStart();
    } else {
        emit finished();
    }
}

bool HeadlessTask::connectToDevice()
{
    if (portName_.isEmpty()) {
        const auto infos = QSerialPortInfo::availablePorts();
        QSerialPortInfo selectedPort;
        for (const QSerialPortInfo &info : infos) {
            // Filter the list to only contain devices that match the VID/PID
            // actually assigned to the Printalyzer Densitometer
            if (!(info.vendorIdentifier() == 0x16D0 && info.productIdentifier() == 0x10EB)) {
                continue;
            }
            selectedPort = info;
            break;
        }

        if (!selectedPort.isNull()) {
            qDebug() << "Detected device at:" << selectedPort.portName();
            serialPort_->setPort(selectedPort);
        } else  {
            qWarning() << "No devices found";
        }
    } else {
        qDebug() << "Connecting to:" << portName_;
        serialPort_->setPortName(portName_);
    }

    serialPort_->setBaudRate(QSerialPort::Baud115200);
    serialPort_->setDataBits(QSerialPort::Data8);
    serialPort_->setParity(QSerialPort::NoParity);
    serialPort_->setStopBits(QSerialPort::OneStop);
    serialPort_->setFlowControl(QSerialPort::NoFlowControl);

    if (serialPort_->open(QIODevice::ReadWrite)) {
        serialPort_->setDataTerminalReady(true);
        if (densInterface_->connectToDevice(serialPort_)) {
            qDebug() << "Connected to device";
            return true;
        } else {
            serialPort_->close();
            qWarning() << "Unrecognized device";
            return false;
        }
    }

    qDebug() << "Error opening device";
    return false;
}

void HeadlessTask::systemInfoStart()
{
    QSimpleSignalAggregator *aggregator = new QSimpleSignalAggregator(this);
    connect(aggregator, &QSimpleSignalAggregator::done, this, &HeadlessTask::systemInfoFinished);
    connect(aggregator, &QSimpleSignalAggregator::done, aggregator, &QObject::deleteLater);
    aggregator->aggregate(densInterface_, SIGNAL(systemBuildResponse()));
    aggregator->aggregate(densInterface_, SIGNAL(systemDeviceResponse()));
    aggregator->aggregate(densInterface_, SIGNAL(systemUniqueId()));
    aggregator->aggregate(densInterface_, SIGNAL(systemInternalSensors()));

    densInterface_->sendGetSystemBuild();
    densInterface_->sendGetSystemDeviceInfo();
    densInterface_->sendGetSystemUID();
    densInterface_->sendGetSystemInternalSensors();
}

void HeadlessTask::systemInfoFinished()
{
    std::cout << "Printalyzer Densitometer" << std::endl;
    std::cout << "Version: " << densInterface_->version().toStdString() << std::endl;
    std::cout << "Date: " << densInterface_->buildDate().toString("yyyy-MM-dd hh:mm").toStdString() << std::endl;
    std::cout << "Commit: " << densInterface_->buildDescribe().toStdString() << std::endl;
    std::cout << "Checksum: " << QString::number(densInterface_->buildChecksum(), 16).toStdString() << std::endl;
    std::cout << "UID: " << densInterface_->uniqueId().toStdString() << std::endl;
    std::cout << "Vdda: " << densInterface_->mcuVdda().toStdString() << std::endl;
    std::cout << "Temperature: " << densInterface_->mcuTemp().toStdString() << std::endl;

    emit finished();
}

void HeadlessTask::exportSettingStart()
{
    SettingsExporter *exporter = new SettingsExporter(densInterface_, this);
    connect(exporter, &SettingsExporter::exportReady, this, [this, exporter]() {
        exporter->saveExport(commandArg_);
        exporter->deleteLater();
        emit finished();
    });
    connect(exporter, &SettingsExporter::exportFailed, this, [this, exporter]() {
        exporter->deleteLater();
        emit finished();
    });
    exporter->prepareExport();
}
