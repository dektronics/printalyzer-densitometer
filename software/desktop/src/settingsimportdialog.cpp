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
            int version = jsonHeader["version"].toInt(0);
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
            calGainLow0_ = parseFloat(jsonCalGain["L0"]);
        }
        if (jsonCalGain.contains("L1")) {
            calGainLow1_ = parseFloat(jsonCalGain["L1"]);
        }
        if (jsonCalGain.contains("M0")) {
            calGainMed0_ = parseFloat(jsonCalGain["M0"]);
        }
        if (jsonCalGain.contains("M1")) {
            calGainMed1_ = parseFloat(jsonCalGain["M1"]);
        }
        if (jsonCalGain.contains("H0")) {
            calGainHigh0_ = parseFloat(jsonCalGain["H0"]);
        }
        if (jsonCalGain.contains("H1")) {
            calGainHigh1_ = parseFloat(jsonCalGain["H1"]);
        }
        if (jsonCalGain.contains("X0")) {
            calGainMax0_ = parseFloat(jsonCalGain["X0"]);
        }
        if (jsonCalGain.contains("X1")) {
            calGainMax1_ = parseFloat(jsonCalGain["X1"]);
        }

        // Assign UI labels
        ui->lowCh0Label->setText(QString::number(calGainLow0_, 'f', 1));
        ui->lowCh1Label->setText(QString::number(calGainLow1_, 'f', 1));
        ui->medCh0Label->setText(QString::number(calGainMed0_, 'f', 6));
        ui->medCh1Label->setText(QString::number(calGainMed1_, 'f', 6));
        ui->highCh0Label->setText(QString::number(calGainHigh0_, 'f', 6));
        ui->highCh1Label->setText(QString::number(calGainHigh1_, 'f', 6));
        ui->maxCh0Label->setText(QString::number(calGainMax0_, 'f', 6));
        ui->maxCh1Label->setText(QString::number(calGainMax1_, 'f', 6));

        // Do basic validation to enable the checkbox
        if (!qIsNaN(calGainLow0_) && !qIsNaN(calGainLow1_)
                && !qIsNaN(calGainMed0_) && !qIsNaN(calGainMed1_)
                && !qIsNaN(calGainHigh0_) && !qIsNaN(calGainHigh1_)
                && !qIsNaN(calGainMax0_) && !qIsNaN(calGainMax1_)
                && qAbs(1.0F - calGainLow0_) < 0.001F && qAbs(1.0F - calGainLow1_) < 0.001F
                && calGainLow0_ < calGainMed0_ && calGainLow1_ < calGainMed1_
                && calGainMed0_ < calGainHigh0_ && calGainMed1_ < calGainHigh1_
                && calGainHigh0_ < calGainMax0_ && calGainHigh1_ < calGainMax1_) {
            ui->importGainCheckBox->setEnabled(true);
        } else {
            ui->importGainCheckBox->setEnabled(false);
        }
    }
    if (jsonCalSensor.contains("slope") && jsonCalSensor["slope"].isObject()) {
        QJsonObject jsonCalSlope = jsonCalSensor["slope"].toObject();
        if (jsonCalSlope.contains("B0")) {
            calSlopeB0_ = parseFloat(jsonCalSlope["B0"]);
        }
        if (jsonCalSlope.contains("B1")) {
            calSlopeB1_ = parseFloat(jsonCalSlope["B1"]);
        }
        if (jsonCalSlope.contains("B2")) {
            calSlopeB2_ = parseFloat(jsonCalSlope["B2"]);
        }

        // Assign UI labels
        ui->slopeB0Label->setText(QString::number(calSlopeB0_, 'f', 6));
        ui->slopeB1Label->setText(QString::number(calSlopeB1_, 'f', 6));
        ui->slopeB2Label->setText(QString::number(calSlopeB2_, 'f', 6));

        // Do basic validation to enable the checkbox
        if (!qIsNaN(calSlopeB0_) && !qIsNaN(calSlopeB1_) && !qIsNaN(calSlopeB2_)) {
            ui->importSlopeCheckBox->setEnabled(true);
        } else {
            ui->importSlopeCheckBox->setEnabled(false);
        }
    }
}

void SettingsImportDialog::parseCalTarget(const QJsonObject &jsonCalTarget)
{
    if (jsonCalTarget.contains("reflection") && jsonCalTarget["reflection"].isObject()) {
        QJsonObject jsonCalRefl = jsonCalTarget["reflection"].toObject();

        if (jsonCalRefl.contains("cal-lo") && jsonCalRefl["cal-lo"].isObject()) {
            QJsonObject jsonCalReflLo = jsonCalRefl["cal-lo"].toObject();

            if (jsonCalReflLo.contains("density")) {
                calReflLoD_ = parseFloat(jsonCalReflLo["density"]);
            }
            if (jsonCalReflLo.contains("reading")) {
                calReflLoR_ = parseFloat(jsonCalReflLo["reading"]);
            }
        }
        if (jsonCalRefl.contains("cal-hi") && jsonCalRefl["cal-hi"].isObject()) {
            QJsonObject jsonCalReflHi = jsonCalRefl["cal-hi"].toObject();

            if (jsonCalReflHi.contains("density")) {
                calReflHiD_ = parseFloat(jsonCalReflHi["density"]);
            }
            if (jsonCalReflHi.contains("reading")) {
                calReflHiR_ = parseFloat(jsonCalReflHi["reading"]);
            }
        }
    }

    // Assign UI labels
    ui->reflCalLoDensityLabel->setText(QString::number(calReflLoD_, 'f', 2));
    ui->reflCalLoReadingLabel->setText(QString::number(calReflLoR_, 'f', 6));
    ui->reflCalHiDensityLabel->setText(QString::number(calReflHiD_, 'f', 2));
    ui->reflCalHiReadingLabel->setText(QString::number(calReflHiR_, 'f', 6));

    // Do basic validation and enable the checkbox
    if (!qIsNaN(calReflLoD_) && !qIsNaN(calReflLoR_)
            && !qIsNaN(calReflHiD_) && !qIsNaN(calReflHiR_)
            && calReflLoD_ > 0 && calReflLoR_ > 0
            && calReflHiD_ > 0 && calReflHiR_ > 0
            && calReflLoD_ < calReflHiD_
            && calReflLoR_ > calReflHiR_) {
        ui->importReflCheckBox->setEnabled(true);
    } else {
        ui->importReflCheckBox->setEnabled(false);
    }

    if (jsonCalTarget.contains("transmission") && jsonCalTarget["transmission"].isObject()) {
        QJsonObject jsonCalTran = jsonCalTarget["transmission"].toObject();

        if (jsonCalTran.contains("cal-lo") && jsonCalTran["cal-lo"].isObject()) {
            QJsonObject jsonCalTranLo = jsonCalTran["cal-lo"].toObject();

            if (jsonCalTranLo.contains("density")) {
                calTranLoD_ = parseFloat(jsonCalTranLo["density"]);
            }
            if (jsonCalTranLo.contains("reading")) {
                calTranLoR_ = parseFloat(jsonCalTranLo["reading"]);
            }
        }
        if (jsonCalTran.contains("cal-hi") && jsonCalTran["cal-hi"].isObject()) {
            QJsonObject jsonCalTranHi = jsonCalTran["cal-hi"].toObject();

            if (jsonCalTranHi.contains("density")) {
                calTranHiD_ = parseFloat(jsonCalTranHi["density"]);
            }
            if (jsonCalTranHi.contains("reading")) {
                calTranHiR_ = parseFloat(jsonCalTranHi["reading"]);
            }
        }
    }

    // Assign UI labels
    ui->tranCalLoDensityLabel->setText(QString::number(calTranLoD_, 'f', 2));
    ui->tranCalLoReadingLabel->setText(QString::number(calTranLoR_, 'f', 6));
    ui->tranCalHiDensityLabel->setText(QString::number(calTranHiD_, 'f', 2));
    ui->tranCalHiReadingLabel->setText(QString::number(calTranHiR_, 'f', 6));

    // Do basic validation and enable the checkbox
    if (!qIsNaN(calTranLoD_) && !qIsNaN(calTranLoR_)
            && !qIsNaN(calTranHiD_) && !qIsNaN(calTranHiR_)
            && qAbs(calTranLoD_) < 0.001F && calTranLoR_ > 0
            && calTranHiD_ > 0 && calTranHiR_ > 0
            && calTranLoD_ < calTranHiD_
            && calTranLoR_ > calTranHiR_) {
        ui->importTranCheckBox->setEnabled(true);
    } else {
        ui->importTranCheckBox->setEnabled(false);
    }
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
        densInterface->sendSetCalGain(
                    calGainMed0_, calGainMed1_,
                    calGainHigh0_, calGainHigh1_,
                    calGainMax0_, calGainMax1_);
    }
    if (ui->importSlopeCheckBox->isChecked()) {
        densInterface->sendSetCalSlope(
                    calSlopeB0_,
                    calSlopeB1_,
                    calSlopeB2_);
    }
    if (ui->importReflCheckBox->isChecked()) {
        densInterface->sendSetCalReflection(
                    calReflLoD_, calReflLoR_,
                    calReflHiD_, calReflHiR_);
    }
    if (ui->importTranCheckBox->isChecked()) {
        densInterface->sendSetCalTransmission(
                    calTranLoD_, calTranLoR_,
                    calTranHiD_, calTranHiR_);
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
