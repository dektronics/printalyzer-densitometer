#include "remotecontroldialog.h"
#include "ui_remotecontroldialog.h"

#include <QDebug>

#define EMC_TEST

#ifdef EMC_TEST
#include <QCheckBox>
#include <QTimer>
#endif

RemoteControlDialog::RemoteControlDialog(DensInterface *densInterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteControlDialog),
    densInterface_(densInterface),
    sensorStarted_(false),
    sensorConfigOnStart_(true)
{
    ui->setupUi(this);
    connect(densInterface_, &DensInterface::systemRemoteControl, this, &RemoteControlDialog::onSystemRemoteControl);
    connect(densInterface_, &DensInterface::diagLightReflChanged, this, &RemoteControlDialog::onDiagLightChanged);
    connect(densInterface_, &DensInterface::diagLightTranChanged, this, &RemoteControlDialog::onDiagLightChanged);
    connect(densInterface_, &DensInterface::diagSensorInvoked, this, &RemoteControlDialog::onDiagSensorInvoked);
    connect(densInterface_, &DensInterface::diagSensorChanged, this, &RemoteControlDialog::onDiagSensorChanged);
    connect(densInterface_, &DensInterface::diagSensorGetReading, this, &RemoteControlDialog::onDiagSensorGetReading);
    connect(densInterface_, &DensInterface::diagSensorInvokeReading, this, &RemoteControlDialog::onDiagSensorInvokeReading);

    connect(ui->reflOffPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onReflOffClicked);
    connect(ui->reflOnPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onReflOnClicked);
    connect(ui->reflSetPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onReflSetClicked);
    connect(ui->reflSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &RemoteControlDialog::onReflSpinBoxValueChanged);

    connect(ui->tranOffPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onTranOffClicked);
    connect(ui->tranOnPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onTranOnClicked);
    connect(ui->tranSetPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onTranSetClicked);
    connect(ui->tranSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &RemoteControlDialog::onTranSpinBoxValueChanged);

    connect(ui->sensorStartPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onSensorStartClicked);
    connect(ui->sensorStopPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onSensorStopClicked);
    connect(ui->gainComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RemoteControlDialog::onSensorGainIndexChanged);
    connect(ui->intComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RemoteControlDialog::onSensorIntIndexChanged);
    connect(ui->reflReadPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onReflReadClicked);
    connect(ui->tranReadPushButton, &QPushButton::clicked, this, &RemoteControlDialog::onTranReadClicked);

#ifdef EMC_TEST
    QCheckBox *cycleCheckBox = new QCheckBox("EMC Cycle Test", ui->sensorGroupBox);
    cycleCheckBox->setObjectName("cycleCheckBox");
    ui->sensorGroupBox->layout()->addWidget(cycleCheckBox);
#endif

    ledControlState(true);
    sensorControlState(true);
}

RemoteControlDialog::~RemoteControlDialog()
{
    delete ui;
}

void RemoteControlDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (densInterface_->connected()) {
        densInterface_->sendInvokeSystemRemoteControl(true);
    }
}

void RemoteControlDialog::closeEvent(QCloseEvent *event)
{
    if (densInterface_->connected()) {
        densInterface_->sendInvokeSystemRemoteControl(false);
    }
    QDialog::closeEvent(event);
}

void RemoteControlDialog::onSystemRemoteControl(bool enabled)
{
    qDebug() << "Remote control:" << enabled;
}

void RemoteControlDialog::onDiagLightChanged()
{
    ui->reflSpinBox->setStyleSheet("QSpinBox { background-color: lightgreen; }");
    ui->tranSpinBox->setStyleSheet("QSpinBox { background-color: lightgreen; }");
#ifdef EMC_TEST
    QTimer *timer = this->findChild<QTimer*>("cycleTimer");
    if (!timer || !timer->isActive()) {
        ledControlState(true);
    }
#else
    ledControlState(true);
#endif
}

void RemoteControlDialog::onReflOffClicked()
{
    ledControlState(false);
    ui->reflSpinBox->setValue(0);
    ui->tranSpinBox->setValue(0);
    densInterface_->sendSetDiagLightRefl(0);
}

void RemoteControlDialog::onReflOnClicked()
{
    ledControlState(false);
    ui->reflSpinBox->setValue(128);
    ui->tranSpinBox->setValue(0);
    densInterface_->sendSetDiagLightRefl(128);
}

void RemoteControlDialog::onReflSetClicked()
{
    ledControlState(false);
    ui->tranSpinBox->setValue(0);
    densInterface_->sendSetDiagLightRefl(ui->reflSpinBox->value());
}

void RemoteControlDialog::onReflSpinBoxValueChanged(int value)
{
    Q_UNUSED(value)
    ui->reflSpinBox->setStyleSheet(styleSheet());
}

void RemoteControlDialog::onTranOffClicked()
{
    ledControlState(false);
    ui->reflSpinBox->setValue(0);
    ui->tranSpinBox->setValue(0);
    densInterface_->sendSetDiagLightTran(0);
}

void RemoteControlDialog::onTranOnClicked()
{
    ledControlState(false);
    ui->reflSpinBox->setValue(0);
    ui->tranSpinBox->setValue(128);
    densInterface_->sendSetDiagLightTran(128);
}

void RemoteControlDialog::onTranSetClicked()
{
    ledControlState(false);
    ui->reflSpinBox->setValue(0);
    densInterface_->sendSetDiagLightTran(ui->tranSpinBox->value());
}

void RemoteControlDialog::onTranSpinBoxValueChanged(int value)
{
    Q_UNUSED(value)
    ui->tranSpinBox->setStyleSheet(styleSheet());
}

void RemoteControlDialog::ledControlState(bool enabled)
{
    ui->reflOffPushButton->setEnabled(enabled);
    ui->reflOnPushButton->setEnabled(enabled);
    ui->reflSetPushButton->setEnabled(enabled);
    ui->reflSpinBox->setEnabled(enabled);

    ui->tranOffPushButton->setEnabled(enabled);
    ui->tranOnPushButton->setEnabled(enabled);
    ui->tranSetPushButton->setEnabled(enabled);
    ui->tranSpinBox->setEnabled(enabled);
}

void RemoteControlDialog::onSensorStartClicked()
{
    sensorControlState(false);
    sensorStarted_ = true;
    if (sensorConfigOnStart_) {
        densInterface_->sendSetDiagSensorConfig(ui->gainComboBox->currentIndex(), ui->intComboBox->currentIndex());
    }
    densInterface_->sendInvokeDiagSensorStart();

#ifdef EMC_TEST
    QCheckBox *cycleCheckBox = ui->sensorGroupBox->findChild<QCheckBox*>("cycleCheckBox");
    if (cycleCheckBox->isChecked()) {
        QTimer *timer = new QTimer(this);
        timer->setObjectName("cycleTimer");
        connect(timer, &QTimer::timeout, this, [this, timer]() {
            int state = timer->property("state").toInt();

            if (state == 0) {
                densInterface_->sendSetDiagLightRefl(0);
                ui->reflSpinBox->setValue(0);
                ui->tranSpinBox->setValue(0);
                densInterface_->sendSetSystemDisplayText("Reflection\nOff");
            } else if (state == 1) {
                densInterface_->sendSetDiagLightRefl(32);
                ui->reflSpinBox->setValue(32);
                ui->tranSpinBox->setValue(0);
                densInterface_->sendSetSystemDisplayText("Reflection\nIdle");
            } else if (state == 2) {
                densInterface_->sendSetDiagLightRefl(128);
                ui->reflSpinBox->setValue(128);
                ui->tranSpinBox->setValue(0);
                densInterface_->sendSetSystemDisplayText("Reflection\nMeasure");
            } else if (state == 3) {
                densInterface_->sendSetDiagLightTran(0);
                ui->reflSpinBox->setValue(0);
                ui->tranSpinBox->setValue(0);
                densInterface_->sendSetSystemDisplayText("Transmission\nOff");
            } else if (state == 4) {
                densInterface_->sendSetDiagLightTran(8);
                ui->reflSpinBox->setValue(0);
                ui->tranSpinBox->setValue(8);
                densInterface_->sendSetSystemDisplayText("Transmission\nIdle");
            } else if (state == 5) {
                densInterface_->sendSetDiagLightTran(128);
                ui->reflSpinBox->setValue(0);
                ui->tranSpinBox->setValue(128);
                densInterface_->sendSetSystemDisplayText("Transmission\nMeasure");
            }

            state++;
            if (state > 5) { state = 0; }
            timer->setProperty("state", state);
        });
        ledControlState(false);
        timer->start(1000);
    }
#endif

}

void RemoteControlDialog::onSensorStopClicked()
{
#ifdef EMC_TEST
    QTimer *timer = this->findChild<QTimer*>("cycleTimer");
    if (timer) {
        if (timer->isActive()) {
            timer->stop();
            ledControlState(true);
        }
        timer->deleteLater();
    }
#endif

    sensorControlState(false);
    sensorStarted_ = false;
    densInterface_->sendInvokeDiagSensorStop();
}

void RemoteControlDialog::onSensorGainIndexChanged(int index)
{
    if (sensorStarted_) {
        sensorControlState(false);
        densInterface_->sendSetDiagSensorConfig(index, ui->intComboBox->currentIndex());
    } else {
        sensorConfigOnStart_ = true;
    }
}

void RemoteControlDialog::onSensorIntIndexChanged(int index)
{
    if (sensorStarted_) {
        sensorControlState(false);
        densInterface_->sendSetDiagSensorConfig(ui->gainComboBox->currentIndex(), index);
    } else {
        sensorConfigOnStart_ = true;
    }
}

void RemoteControlDialog::onReflReadClicked()
{
    ledControlState(false);
    sensorControlState(false);
    ui->reflSpinBox->setValue(128);
    ui->tranSpinBox->setValue(0);
    ui->ch0LineEdit->setEnabled(false);
    ui->ch1LineEdit->setEnabled(false);
    densInterface_->sendInvokeDiagRead(DensInterface::SensorLightReflection,
                                       ui->gainComboBox->currentIndex(),
                                       ui->intComboBox->currentIndex());
}

void RemoteControlDialog::onTranReadClicked()
{
    ledControlState(false);
    sensorControlState(false);
    ui->reflSpinBox->setValue(0);
    ui->tranSpinBox->setValue(128);
    ui->ch0LineEdit->setEnabled(false);
    ui->ch1LineEdit->setEnabled(false);
    densInterface_->sendInvokeDiagRead(DensInterface::SensorLightTransmission,
                                       ui->gainComboBox->currentIndex(),
                                       ui->intComboBox->currentIndex());
}

void RemoteControlDialog::sensorControlState(bool enabled)
{
    ui->sensorStartPushButton->setEnabled(enabled ? !sensorStarted_ : false);
    ui->sensorStopPushButton->setEnabled(enabled ? sensorStarted_ : false);
    ui->gainComboBox->setEnabled(enabled);
    ui->intComboBox->setEnabled(enabled);
    ui->reflReadPushButton->setEnabled(enabled ? !sensorStarted_ : false);
    ui->tranReadPushButton->setEnabled(enabled ? !sensorStarted_ : false);
#ifdef EMC_TEST
    QCheckBox *cycleCheckBox = ui->sensorGroupBox->findChild<QCheckBox*>("cycleCheckBox");
    cycleCheckBox->setEnabled(enabled ? !sensorStarted_ : false);
#endif
}

void RemoteControlDialog::onDiagSensorInvoked()
{
    sensorControlState(true);
}

void RemoteControlDialog::onDiagSensorChanged()
{
    sensorControlState(true);
    sensorConfigOnStart_ = false;
}

void RemoteControlDialog::onDiagSensorGetReading(int ch0, int ch1)
{
    updateSensorReading(ch0, ch1);
    sensorControlState(true);
}

void RemoteControlDialog::onDiagSensorInvokeReading(int ch0, int ch1)
{
    updateSensorReading(ch0, ch1);
    ui->reflSpinBox->setValue(0);
    ui->tranSpinBox->setValue(0);
    ui->ch0LineEdit->setEnabled(true);
    ui->ch1LineEdit->setEnabled(true);
    sensorControlState(true);
    ledControlState(true);
}

void RemoteControlDialog::updateSensorReading(int ch0, int ch1)
{
    ui->ch0LineEdit->setText(QString::number(ch0));
    ui->ch1LineEdit->setText(QString::number(ch1));
}
