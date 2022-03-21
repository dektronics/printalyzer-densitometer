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
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

    QString portName() const;

private slots:
    void showPortInfo(int idx);
    virtual void accept();

private:
    void fillPortsInfo();

    Ui::ConnectDialog *ui;
    QString portName_;
};

#endif // CONNECTDIALOG_H
