#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QSerialPort>

QT_BEGIN_NAMESPACE

namespace Ui {
class ConnectDialog;
}

QT_END_NAMESPACE

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };

    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

    Settings settings() const;

private slots:
    void showPortInfo(int idx);
    virtual void accept();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

    Ui::ConnectDialog *ui;
    Settings currentSettings_;
};

#endif // CONNECTDIALOG_H
