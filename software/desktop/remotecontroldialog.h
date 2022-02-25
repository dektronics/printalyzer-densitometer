#ifndef REMOTECONTROLDIALOG_H
#define REMOTECONTROLDIALOG_H

#include <QDialog>
#include "densinterface.h"

namespace Ui {
class RemoteControlDialog;
}

class RemoteControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteControlDialog(DensInterface *densInterface, QWidget *parent = nullptr);
    ~RemoteControlDialog();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onSystemRemoteControl(bool enabled);
    void onDiagLightChanged();
    void onDiagSensorInvoked();
    void onDiagSensorChanged();
    void onDiagSensorReading(int ch0, int ch1);

    void onReflOffClicked();
    void onReflOnClicked();
    void onReflSetClicked();
    void onReflSpinBoxValueChanged(int value);

    void onTranOffClicked();
    void onTranOnClicked();
    void onTranSetClicked();
    void onTranSpinBoxValueChanged(int value);

    void onSensorStartClicked();
    void onSensorStopClicked();
    void onSensorGainIndexChanged(int index);
    void onSensorIntIndexChanged(int index);
    void onSensorNextReadingClicked();

private:
    void ledControlState(bool enabled);
    void sensorControlState(bool enabled);

    Ui::RemoteControlDialog *ui;
    DensInterface *densInterface_;
    bool sensorStarted_;
    bool sensorConfigOnStart_;
};

#endif // REMOTECONTROLDIALOG_H
