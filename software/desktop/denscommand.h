#ifndef DENSCOMMAND_H
#define DENSCOMMAND_H

#include <QSharedDataPointer>

class DensCommandData;

class DensCommand
{
public:
    enum CommandType {
        TypeSet,
        TypeGet,
        TypeInvoke,
        TypeDensityReflection,
        TypeDensityTransmission,
        TypeUnknown = -1
    };
    enum CommandCategory {
        CategorySystem,
        CategoryMeasurement,
        CategoryCalibration,
        CategoryDiagnostics,
        CategoryUnknown = -1
    };

    DensCommand();
    DensCommand(CommandType type, CommandCategory category, const QString &action, const QStringList &args = QStringList());
    DensCommand(const DensCommand &other);
    DensCommand &operator=(const DensCommand &other);
    ~DensCommand();

    bool isValid() const;
    bool isMatch(const DensCommand &other);
    bool isDensity() const;

    static DensCommand parse(const QByteArray &data);

    CommandType type() const;
    CommandCategory category() const;
    QString action() const;
    QStringList args() const;

    QByteArray buffer() const;
    void setBuffer(const QByteArray &buffer);

    QString toString() const;

private:
    static QStringList splitLine(const QByteArray &line);
    QSharedDataPointer<DensCommandData> d;
};

#endif // DENSCOMMAND_H
