#include "settingsimportdialog.h"
#include "ui_settingsimportdialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "densinterface.h"

SettingsImportDialog::SettingsImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsImportDialog)
{
    ui->setupUi(this);

    // Clear out any designer default text from the UI fields
    ui->deviceNameLabel->setText(QString());
    ui->deviceVersionLabel->setText(tr("Version: %1").arg(QString()));
    ui->deviceUidLabel->setText(tr("Device UID: %1").arg(QString()));
    ui->exportDateLabel->setText(tr("Export Date: %1").arg(QString()));
    ui->lowCh0Label->setText(QString());
    ui->lowCh1Label->setText(QString());
    ui->medCh0Label->setText(QString());
    ui->medCh1Label->setText(QString());
    ui->highCh0Label->setText(QString());
    ui->highCh1Label->setText(QString());
    ui->maxCh0Label->setText(QString());
    ui->maxCh1Label->setText(QString());
    ui->slopeB0Label->setText(QString());
    ui->slopeB1Label->setText(QString());
    ui->slopeB2Label->setText(QString());
    ui->reflCalLoDensityLabel->setText(QString());
    ui->reflCalLoReadingLabel->setText(QString());
    ui->reflCalHiDensityLabel->setText(QString());
    ui->reflCalHiReadingLabel->setText(QString());
    ui->tranCalLoDensityLabel->setText(QString());
    ui->tranCalLoReadingLabel->setText(QString());
    ui->tranCalHiDensityLabel->setText(QString());
    ui->tranCalHiReadingLabel->setText(QString());

    // Set all checkboxes to an initial disabled state
    ui->importGainCheckBox->setEnabled(false);
    ui->importSlopeCheckBox->setEnabled(false);
    ui->importReflCheckBox->setEnabled(false);
    ui->importTranCheckBox->setEnabled(false);

    connect(ui->importGainCheckBox, &QCheckBox::stateChanged, this, &SettingsImportDialog::onCheckBoxChanged);
    connect(ui->importSlopeCheckBox, &QCheckBox::stateChanged, this, &SettingsImportDialog::onCheckBoxChanged);
    connect(ui->importReflCheckBox, &QCheckBox::stateChanged, this, &SettingsImportDialog::onCheckBoxChanged);
    connect(ui->importTranCheckBox, &QCheckBox::stateChanged, this, &SettingsImportDialog::onCheckBoxChanged);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    onCheckBoxChanged();
}

SettingsImportDialog::~SettingsImportDialog()
{
    delete ui;
}

bool SettingsImportDialog::loadFile(const QString &filename)
{
    if (filename.isEmpty()) { return false; }

    QFile importFile(filename);

    if (!importFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open import file.";
        return false;
    }

    QByteArray importData = importFile.readAll();
    importFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(importData);
    if (doc.isEmpty()) {
        return false;
    }

    QJsonObject root = doc.object();
    if (!parseHeader(root)) {
        return false;
    }

    if (root.contains("calibration") && root["calibration"].isObject()) {
        QJsonObject jsonCal = root["calibration"].toObject();
        if (jsonCal.contains("sensor") && jsonCal["sensor"].isObject()) {
            QJsonObject jsonCalSensor = jsonCal["sensor"].toObject();
            parseCalSensor(jsonCalSensor);
        }
        if (jsonCal.contains("target") && jsonCal["target"].isObject()) {
            QJsonObject jsonCalTarget = jsonCal["target"].toObject();
            parseCalTarget(jsonCalTarget);
        }
    }

    onCheckBoxChanged();

    return true;
}

bool SettingsImportDialog::parseHeader(const QJsonObject &root)
{
    QString exportDate;
    QString deviceName;
    QString deviceVersion;
    QString deviceUid;
    if (root.contains("header") && root["header"].isObject()) {
        QJsonObject jsonHeader = root["header"].toObject();
        if (jsonHeader.contains("version")) {
            int version = parseInt(jsonHeader["version"]);
            if (version != 1) {
                qWarning() << "Unexpected version:" << version;
                return false;
            }
        }
        if (jsonHeader.contains("date")) {
            exportDate = jsonHeader["date"].toString();
        }
    }
    if (root.contains("system") && root["system"].isObject()) {
        QJsonObject jsonSystem = root["system"].toObject();

        if (jsonSystem.contains("name")) {
            deviceName = jsonSystem["name"].toString();
        }
        if (jsonSystem.contains("version")) {
            deviceVersion = jsonSystem["version"].toString();
        }
        if (jsonSystem.contains("uid")) {
            deviceUid = jsonSystem["uid"].toString();
        }
    }
    ui->deviceNameLabel->setText(deviceName);
    ui->deviceVersionLabel->setText(tr("Version: %1").arg(deviceVersion));
    ui->deviceUidLabel->setText(tr("Device UID: %1").arg(deviceUid));
    ui->exportDateLabel->setText(tr("Export Date: %1").arg(exportDate));
    return true;
}

void SettingsImportDialog::parseCalSensor(const QJsonObject &jsonCalSensor)
{
    if (jsonCalSensor.contains("gain") && jsonCalSensor["gain"].isObject()) {
        QJsonObject jsonCalGain = jsonCalSensor["gain"].toObject();
        if (jsonCalGain.contains("L0")) {
            calGain_.setLow0(parseFloat(jsonCalGain["L0"]));
        }
        if (jsonCalGain.contains("L1")) {
            calGain_.setLow1(parseFloat(jsonCalGain["L1"]));
        }
        if (jsonCalGain.contains("M0")) {
            calGain_.setMed0(parseFloat(jsonCalGain["M0"]));
        }
        if (jsonCalGain.contains("M1")) {
            calGain_.setMed1(parseFloat(jsonCalGain["M1"]));
        }
        if (jsonCalGain.contains("H0")) {
            calGain_.setHigh0(parseFloat(jsonCalGain["H0"]));
        }
        if (jsonCalGain.contains("H1")) {
            calGain_.setHigh1(parseFloat(jsonCalGain["H1"]));
        }
        if (jsonCalGain.contains("X0")) {
            calGain_.setMax0(parseFloat(jsonCalGain["X0"]));
        }
        if (jsonCalGain.contains("X1")) {
            calGain_.setMax1(parseFloat(jsonCalGain["X1"]));
        }

        // Assign UI labels
        ui->lowCh0Label->setText(QString::number(calGain_.low0(), 'f', 1));
        ui->lowCh1Label->setText(QString::number(calGain_.low1(), 'f', 1));
        ui->medCh0Label->setText(QString::number(calGain_.med0(), 'f', 6));
        ui->medCh1Label->setText(QString::number(calGain_.med1(), 'f', 6));
        ui->highCh0Label->setText(QString::number(calGain_.high0(), 'f', 6));
        ui->highCh1Label->setText(QString::number(calGain_.high1(), 'f', 6));
        ui->maxCh0Label->setText(QString::number(calGain_.max0(), 'f', 6));
        ui->maxCh1Label->setText(QString::number(calGain_.max1(), 'f', 6));

        // Do basic validation to enable the checkbox
        ui->importGainCheckBox->setEnabled(calGain_.isValid());
    }
    if (jsonCalSensor.contains("slope") && jsonCalSensor["slope"].isObject()) {
        QJsonObject jsonCalSlope = jsonCalSensor["slope"].toObject();
        if (jsonCalSlope.contains("B0")) {
            calSlope_.setB0(parseFloat(jsonCalSlope["B0"]));
        }
        if (jsonCalSlope.contains("B1")) {
            calSlope_.setB1(parseFloat(jsonCalSlope["B1"]));
        }
        if (jsonCalSlope.contains("B2")) {
            calSlope_.setB2(parseFloat(jsonCalSlope["B2"]));
        }

        // Assign UI labels
        ui->slopeB0Label->setText(QString::number(calSlope_.b0(), 'f', 6));
        ui->slopeB1Label->setText(QString::number(calSlope_.b1(), 'f', 6));
        ui->slopeB2Label->setText(QString::number(calSlope_.b2(), 'f', 6));

        // Do basic validation to enable the checkbox
        ui->importSlopeCheckBox->setEnabled(calSlope_.isValid());
    }
}

void SettingsImportDialog::parseCalTarget(const QJsonObject &jsonCalTarget)
{
    if (jsonCalTarget.contains("reflection") && jsonCalTarget["reflection"].isObject()) {
        QJsonObject jsonCalRefl = jsonCalTarget["reflection"].toObject();

        if (jsonCalRefl.contains("cal-lo") && jsonCalRefl["cal-lo"].isObject()) {
            QJsonObject jsonCalReflLo = jsonCalRefl["cal-lo"].toObject();

            if (jsonCalReflLo.contains("density")) {
                calReflection_.setLoDensity(parseFloat(jsonCalReflLo["density"]));
            }
            if (jsonCalReflLo.contains("reading")) {
                calReflection_.setLoReading(parseFloat(jsonCalReflLo["reading"]));
            }
        }
        if (jsonCalRefl.contains("cal-hi") && jsonCalRefl["cal-hi"].isObject()) {
            QJsonObject jsonCalReflHi = jsonCalRefl["cal-hi"].toObject();

            if (jsonCalReflHi.contains("density")) {
                calReflection_.setHiDensity(parseFloat(jsonCalReflHi["density"]));
            }
            if (jsonCalReflHi.contains("reading")) {
                calReflection_.setHiReading(parseFloat(jsonCalReflHi["reading"]));
            }
        }
    }

    // Assign UI labels
    ui->reflCalLoDensityLabel->setText(QString::number(calReflection_.loDensity(), 'f', 2));
    ui->reflCalLoReadingLabel->setText(QString::number(calReflection_.loReading(), 'f', 6));
    ui->reflCalHiDensityLabel->setText(QString::number(calReflection_.hiDensity(), 'f', 2));
    ui->reflCalHiReadingLabel->setText(QString::number(calReflection_.hiReading(), 'f', 6));

    // Do basic validation and enable the checkbox
    ui->importReflCheckBox->setEnabled(calReflection_.isValidReflection());

    if (jsonCalTarget.contains("transmission") && jsonCalTarget["transmission"].isObject()) {
        QJsonObject jsonCalTran = jsonCalTarget["transmission"].toObject();

        if (jsonCalTran.contains("cal-lo") && jsonCalTran["cal-lo"].isObject()) {
            QJsonObject jsonCalTranLo = jsonCalTran["cal-lo"].toObject();

            if (jsonCalTranLo.contains("density")) {
                calTransmission_.setLoDensity(parseFloat(jsonCalTranLo["density"]));
            }
            if (jsonCalTranLo.contains("reading")) {
                calTransmission_.setLoReading(parseFloat(jsonCalTranLo["reading"]));
            }
        }
        if (jsonCalTran.contains("cal-hi") && jsonCalTran["cal-hi"].isObject()) {
            QJsonObject jsonCalTranHi = jsonCalTran["cal-hi"].toObject();

            if (jsonCalTranHi.contains("density")) {
                calTransmission_.setHiDensity(parseFloat(jsonCalTranHi["density"]));
            }
            if (jsonCalTranHi.contains("reading")) {
                calTransmission_.setHiReading(parseFloat(jsonCalTranHi["reading"]));
            }
        }
    }

    // Assign UI labels
    ui->tranCalLoDensityLabel->setText(QString::number(calTransmission_.loDensity(), 'f', 2));
    ui->tranCalLoReadingLabel->setText(QString::number(calTransmission_.loReading(), 'f', 6));
    ui->tranCalHiDensityLabel->setText(QString::number(calTransmission_.hiDensity(), 'f', 2));
    ui->tranCalHiReadingLabel->setText(QString::number(calTransmission_.hiReading(), 'f', 6));

    // Do basic validation and enable the checkbox
    ui->importTranCheckBox->setEnabled(calTransmission_.isValidTransmission());
}

void SettingsImportDialog::onCheckBoxChanged()
{
    if (ui->importGainCheckBox->isChecked() || ui->importSlopeCheckBox->isChecked()
            || ui->importReflCheckBox->isChecked() || ui->importTranCheckBox->isChecked()) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void SettingsImportDialog::sendSelectedSettings(DensInterface *densInterface)
{
    if (!densInterface) { return; }

    if (ui->importGainCheckBox->isChecked()) {
        densInterface->sendSetCalGain(calGain_);
    }
    if (ui->importSlopeCheckBox->isChecked()) {
        densInterface->sendSetCalSlope(calSlope_);
    }
    if (ui->importReflCheckBox->isChecked()) {
        densInterface->sendSetCalReflection(calReflection_);
    }
    if (ui->importTranCheckBox->isChecked()) {
        densInterface->sendSetCalTransmission(calTransmission_);
    }
}

int SettingsImportDialog::parseInt(const QJsonValue &value)
{
    if (value.isDouble()) {
        return (int)value.toDouble(0);
    } else if (value.isString()) {
        bool ok;
        float result = value.toString().toInt(&ok);
        if (ok) {
            return result;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

float SettingsImportDialog::parseFloat(const QJsonValue &value)
{
    if (value.isDouble()) {
        return value.toDouble(qSNaN());
    } else if (value.isString()) {
        bool ok;
        float result = value.toString().toFloat(&ok);
        if (ok) {
            return result;
        } else {
            return qSNaN();
        }
    } else {
        return qSNaN();
    }
}
