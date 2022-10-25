#ifndef GAINCALIBRATIONDIALOG_H
#define GAINCALIBRATIONDIALOG_H

#include <QDialog>
#include "densinterface.h"

namespace Ui {
class GainCalibrationDialog;
}

class GainCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GainCalibrationDialog(DensInterface *densInterface, QWidget *parent = nullptr);
    ~GainCalibrationDialog();

    bool success() const;

public slots:
    virtual void accept() override;
    virtual void reject() override;

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onSystemRemoteControl(bool enabled);
    void onCalGainCalStatus(int status, int param);
    void onCalGainCalFinished();
    void onCalGainCalError();

private:
    QString gainParamText(int param);
    QString lightParamText(int param);
    void addText(const QString &text);
    Ui::GainCalibrationDialog *ui;
    DensInterface *densInterface_;
    bool started_;
    bool running_;
    bool success_;
    int lastStatus_;
    int lastParam_;
};

#endif // GAINCALIBRATIONDIALOG_H
