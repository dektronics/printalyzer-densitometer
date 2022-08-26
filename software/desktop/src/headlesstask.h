#ifndef HEADLESSTASK_H
#define HEADLESSTASK_H

#include <QObject>

#include "densinterface.h"

QT_BEGIN_NAMESPACE
class QSerialPort;
QT_END_NAMESPACE

class HeadlessTask : public QObject
{
    Q_OBJECT
public:
    enum Command {
        CommandSystemInfo,
        CommandExportSettings,
        CommandUnknown = -1
    };

    explicit HeadlessTask(QObject *parent = nullptr);

    void setPort(const QString &portName);
    void setCommand(HeadlessTask::Command command, const QString &commandArg);

public slots:
    void run();

signals:
    void finished();

private slots:
    void systemInfoFinished();

private:
    bool connectToDevice();
    void systemInfoStart();
    void exportSettingStart();

    QString portName_;
    HeadlessTask::Command command_ = HeadlessTask::CommandUnknown;
    QString commandArg_;
    QSerialPort *serialPort_ = nullptr;
    DensInterface *densInterface_ = nullptr;
};

#endif // HEADLESSTASK_H
