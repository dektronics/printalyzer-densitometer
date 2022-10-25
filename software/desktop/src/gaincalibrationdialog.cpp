#include "gaincalibrationdialog.h"
#include "ui_gaincalibrationdialog.h"

#include <QScrollBar>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDebug>

GainCalibrationDialog::GainCalibrationDialog(DensInterface *densInterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GainCalibrationDialog),
    densInterface_(densInterface),
    started_(false),
    running_(false),
    success_(false),
    lastStatus_(-1),
    lastParam_(-1)
{
    ui->setupUi(this);
    ui->plainTextEdit->document()->setMaximumBlockCount(100);
    ui->plainTextEdit->setReadOnly(true);

    connect(densInterface_, &DensInterface::systemRemoteControl, this, &GainCalibrationDialog::onSystemRemoteControl);
    connect(densInterface_, &DensInterface::calGainCalStatus, this, &GainCalibrationDialog::onCalGainCalStatus);
    connect(densInterface_, &DensInterface::calGainCalFinished, this, &GainCalibrationDialog::onCalGainCalFinished);
    connect(densInterface_, &DensInterface::calGainCalError, this, &GainCalibrationDialog::onCalGainCalError);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &GainCalibrationDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &GainCalibrationDialog::reject);
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(false);
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

void GainCalibrationDialog::onCalGainCalStatus(int status, int param)
{
    if (status == lastStatus_ && param == lastParam_) {
        return;
    }

    switch (status) {
    case 0:
        if (param == 0) {
            addText(tr("Initializing..."));
        }
        break;
    case 1:
        addText(tr("Measuring medium gain... [%1]").arg(gainParamText(param)));
        break;
    case 2:
        addText(tr("Measuring high gain... [%1]").arg(gainParamText(param)));
        break;
    case 3:
        addText(tr("Measuring maximum gain... [%1]").arg(gainParamText(param)));
        break;
    case 5:
        addText(tr("Finding gain measurement brightness... [%1]").arg(lightParamText(param)));
        break;
    case 6:
        if (param == 0) {
            addText(tr("Waiting between measurements..."));
        }
        break;
    }

    lastStatus_ = status;
    lastParam_ = param;
}

QString GainCalibrationDialog::gainParamText(int param)
{
    if (param == 0) {
        return tr("lower");
    } else if (param == 1) {
        return tr("higher");
    } else {
        return QString::number(param);
    }
}

QString GainCalibrationDialog::lightParamText(int param)
{
    if (param == 0) {
        return tr("init");
    } else {
        return QString::number(param);
    }
}

void GainCalibrationDialog::onCalGainCalFinished()
{
    addText(tr("Gain calibration complete!"));
    running_ = false;
    success_ = true;
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
}

void GainCalibrationDialog::onCalGainCalError()
{
    addText(tr("Gain calibration failed!"));
    running_ = false;
    success_ = false;
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
}

void GainCalibrationDialog::addText(const QString &text)
{
    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->insertPlainText("\n");
    QScrollBar *bar = ui->plainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}
