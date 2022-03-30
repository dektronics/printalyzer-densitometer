#ifndef SETTINGSIMPORTDIALOG_H
#define SETTINGSIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsImportDialog;
}

class DensInterface;

class SettingsImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsImportDialog(QWidget *parent = nullptr);
    ~SettingsImportDialog();

    bool loadFile(const QString &filename);
    void sendSelectedSettings(DensInterface *densInterface);

private slots:
    void onCheckBoxChanged();

private:
    bool parseHeader(const QJsonObject &root);
    void parseCalSensor(const QJsonObject &jsonCalSensor);
    void parseCalTarget(const QJsonObject &jsonCalTarget);
    float parseFloat(const QJsonValue &value);

    Ui::SettingsImportDialog *ui;

    float calGainLow0_ = qSNaN();
    float calGainLow1_ = qSNaN();
    float calGainMed0_ = qSNaN();
    float calGainMed1_ = qSNaN();
    float calGainHigh0_ = qSNaN();
    float calGainHigh1_ = qSNaN();
    float calGainMax0_ = qSNaN();
    float calGainMax1_ = qSNaN();

    float calSlopeB0_ = qSNaN();
    float calSlopeB1_ = qSNaN();
    float calSlopeB2_ = qSNaN();

    float calReflLoD_ = qSNaN();
    float calReflLoR_ = qSNaN();
    float calReflHiD_ = qSNaN();
    float calReflHiR_ = qSNaN();

    float calTranLoD_ = qSNaN();
    float calTranLoR_ = qSNaN();
    float calTranHiD_ = qSNaN();
    float calTranHiR_ = qSNaN();
};

#endif // SETTINGSIMPORTDIALOG_H
