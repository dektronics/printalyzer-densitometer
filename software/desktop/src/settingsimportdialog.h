#ifndef SETTINGSIMPORTDIALOG_H
#define SETTINGSIMPORTDIALOG_H

#include <QDialog>

#include "denscalvalues.h"

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
    int parseInt(const QJsonValue &value);
    float parseFloat(const QJsonValue &value);

    Ui::SettingsImportDialog *ui;

    DensCalGain calGain_;
    DensCalSlope calSlope_;

    DensCalTarget calReflection_;
    DensCalTarget calTransmission_;
};

#endif // SETTINGSIMPORTDIALOG_H
