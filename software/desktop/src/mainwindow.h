#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemModel>
#include "densinterface.h"

QT_BEGIN_NAMESPACE

class QLabel;
class QSerialPort;
class QLineEdit;
class QSpinBox;
class QStandardItemModel;

namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LogWindow;
class RemoteControlDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connectToPort(const QString &portName);

private slots:
    void openConnection();
    void onOpenConnectionDialogFinished(int result);
    void closeConnection();
    void onImportSettings();
    void onExportSettings();
    void onLogger(bool checked);
    void onLoggerOpened();
    void onLoggerClosed();
    void about();

    void onMenuEditAboutToShow();

    void onConnectionOpened();
    void onConnectionClosed();
    void onConnectionError();

    void onDensityReading(DensInterface::DensityType type, float dValue, float dZero, float rawValue, float corrValue);

    void onActionCut();
    void onActionCopy();
    void onActionPaste();
    void onActionDelete();

    void onAddReadingClicked();
    void onCopyTableClicked();
    void onClearTableClicked();

    void onCalGetAllValues();
    void onCalLightSetClicked();
    void onCalGainCalClicked();
    void onCalGainSetClicked();
    void onCalSlopeSetClicked();
    void onCalReflectionSetClicked();
    void onCalTransmissionSetClicked();

    void onCalLightTextChanged();
    void onCalGainTextChanged();
    void onCalSlopeTextChanged();
    void onCalReflectionTextChanged();
    void onCalTransmissionTextChanged();

    void onSystemVersionResponse();
    void onSystemBuildResponse();
    void onSystemDeviceResponse();
    void onSystemUniqueId();
    void onSystemInternalSensors();

    void onDiagDisplayScreenshot(const QByteArray &data);

    void onCalLightResponse();
    void onCalGainResponse();
    void onCalSlopeResponse();
    void onCalReflectionResponse();
    void onCalTransmissionResponse();

    void onRemoteControl();
    void onRemoteControlFinished();

    void onSlopeCalibrationTool();
    void onSlopeCalibrationToolFinished(int result);

private:
    void openConnectionToPort(const QString &portName);
    void refreshButtonState();
    void updateLineEditDirtyState(QLineEdit *lineEdit, int value);
    void updateLineEditDirtyState(QLineEdit *lineEdit, float value, int prec);
    void measTableAddReading(DensInterface::DensityType type, float density, float offset);
    void measTableCut();
    void measTableCopy();
    void measTableCopyList(const QModelIndexList &indexList, bool includeEmpty);
    void measTablePaste();
    void measTableDelete();

    Ui::MainWindow *ui = nullptr;
    QLabel *statusLabel_ = nullptr;
    QSerialPort *serialPort_ = nullptr;
    DensInterface *densInterface_ = nullptr;
    LogWindow *logWindow_ = nullptr;
    QStandardItemModel *measModel_ = nullptr;
    RemoteControlDialog *remoteDialog_ = nullptr;
    DensInterface::DensityType lastReadingType_ = DensInterface::DensityUnknown;
    float lastReadingDensity_ = qSNaN();
    float lastReadingOffset_ = qSNaN();
};

#endif // MAINWINDOW_H
