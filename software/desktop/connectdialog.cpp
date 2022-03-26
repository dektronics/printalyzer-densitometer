#include "connectdialog.h"
#include "ui_connectdialog.h"

#include <QSerialPortInfo>
#include <QPushButton>

static const char blankString[] = QT_TRANSLATE_NOOP("ConnectDialog", "N/A");

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    connect(ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ConnectDialog::showPortInfo);

    fillPortsInfo();
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

QString ConnectDialog::portName() const
{
    return portName_;
}

void ConnectDialog::showPortInfo(int idx)
{
    if (idx == -1) {
        return;
    }

    const QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void ConnectDialog::accept()
{
    portName_ = ui->serialPortInfoListBox->currentText();
    QDialog::accept();
}

void ConnectDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        // Filter the list to only contain devices that match the VID/PID
        // actually assigned to the Printalyzer Densitometer
        if (!(info.vendorIdentifier() == 0x16D0 && info.productIdentifier() == 0x10EB)) {
            continue;
        }
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->serialPortInfoListBox->count() > 0);
}
