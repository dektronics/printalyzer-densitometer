#include "gaincalibrationdialog.h"
#include "ui_gaincalibrationdialog.h"

#include <QScrollBar>
#include <QDebug>

GainCalibrationDialog::GainCalibrationDialog(DensInterface *densInterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GainCalibrationDialog),
    densInterface_(densInterface),
    started_(false),
    running_(false),
    success_(false),
    lastStatus_(-1)
{
    ui->setupUi(this);
    ui->plainTextEdit->document()->setMaximumBlockCount(100);
    ui->plainTextEdit->setReadOnly(true);

    connect(densInterface_, &DensInterface::systemRemoteControl, this, &GainCalibrationDialog::onSystemRemoteControl);
    connect(densInterface_, &DensInterface::calGainCalStatus, this, &GainCalibrationDialog::onCalGainCalStatus);
    connect(densInterface_, &DensInterface::calGainCalFinished, this, &GainCalibrationDialog::onCalGainCalFinished);
    connect(densInterface_, &DensInterface::calGainCalError, this, &GainCalibrationDialog::onCalGainCalError);
}

GainCalibrationDialog::~GainCalibrationDialog()
{
    delete ui;
}

bool GainCalibrationDialog::success() const
{
    return success_;
}

void GainCalibrationDialog::accept()
{
    if (running_) { return; }

    if (densInterface_->connected()) {
        densInterface_->sendInvokeSystemRemoteControl(false);
    }
    QDialog::accept();
}

void GainCalibrationDialog::reject()
{
    if (running_) { return; }

    if (densInterface_->connected()) {
        densInterface_->sendInvokeSystemRemoteControl(false);
    }
    QDialog::reject();
}

void GainCalibrationDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (densInterface_->connected()) {
        densInterface_->sendInvokeSystemRemoteControl(true);
    }
}

void GainCalibrationDialog::onSystemRemoteControl(bool enabled)
{
    qDebug() << "Remote control:" << enabled;
    if (enabled && !started_) {
        started_ = true;
        running_ = true;
        densInterface_->sendInvokeCalGain();
    }
}

void GainCalibrationDialog::onCalGainCalStatus(int status)
{
    if (status == lastStatus_) {
        return;
    }

    switch (status) {
    case 0:
        addText(tr("Initializing..."));
        break;
    case 1:
        addText(tr("Measuring medium gain..."));
        break;
    case 2:
        addText(tr("Measuring high gain..."));
        break;
    case 3:
        addText(tr("Measuring maximum gain..."));
        break;
    case 5:
        addText(tr("Finding gain measurement brightness..."));
        break;
    case 6:
        addText(tr("Waiting between measurements..."));
        break;
    }

    lastStatus_ = status;
}

void GainCalibrationDialog::onCalGainCalFinished()
{
    addText(tr("Gain calibration complete!"));
    running_ = false;
    success_ = true;
}

void GainCalibrationDialog::onCalGainCalError()
{
    addText(tr("Gain calibration failed!"));
    running_ = false;
    success_ = false;
}

void GainCalibrationDialog::addText(const QString &text)
{
    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->insertPlainText("\n");
    QScrollBar *bar = ui->plainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}
