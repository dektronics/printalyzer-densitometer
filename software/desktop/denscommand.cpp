#include "denscommand.h"

class DensCommandData : public QSharedData
{
public:
    DensCommandData()
        : type(DensCommand::TypeUnknown)
        , category(DensCommand::CategoryUnknown) { }
    DensCommandData(const DensCommandData &other)
        : QSharedData(other)
        , type(other.type)
        , category(other.category)
        , action(other.action)
        , args(other.args) { }
    ~DensCommandData() { }

    DensCommand::CommandType type;
    DensCommand::CommandCategory category;
    QString action;
    QStringList args;
    QByteArray buffer;
};

DensCommand::DensCommand() : d(new DensCommandData)
{
}

DensCommand::DensCommand(CommandType type, CommandCategory category,
                         const QString &action, const QStringList &args)
    : d(new DensCommandData)
{
    d->type = type;
    d->category = category;
    d->action = action;
    d->args = args;
}

DensCommand::DensCommand(const DensCommand &other) : d(other.d)
{
}

DensCommand &DensCommand::operator=(const DensCommand &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

DensCommand::~DensCommand()
{
}

bool DensCommand::isValid() const
{
    return d->type != TypeUnknown && d->category != CategoryUnknown
            && !d->action.isEmpty();
}

bool DensCommand::isMatch(const DensCommand &other)
{
    return d->type == other.d->type
            && d->category == other.d->category
            && d->action == other.d->action;
}

bool DensCommand::isDensity() const
{
    return (d->type == TypeDensityReflection || d->type == TypeDensityTransmission)
            && d->action.isEmpty() && !d->args.isEmpty();
}

DensCommand DensCommand::parse(const QByteArray &data)
{
    const QStringList elements = splitLine(data);
    CommandType type = TypeUnknown;
    CommandCategory category = CategoryUnknown;
    QString action;
    QStringList args;

    if (elements.count() > 0) {
        const QString cmd = elements[0];
        if (cmd.length() > 0) {
            switch (cmd[0].toLatin1()) {
            case 'S':
                type = TypeSet;
                break;
            case 'G':
                type = TypeGet;
                break;
            case 'I':
                type = TypeInvoke;
                break;
            case 'R':
                type = TypeDensityReflection;
                break;
            case 'T':
                type = TypeDensityTransmission;
                break;
            default:
                type = TypeUnknown;
                break;
            }
        }
        if (type == TypeDensityReflection || type == TypeDensityTransmission) {
            if (cmd.length() > 1) {
                args.append(cmd.mid(1));
            }
            args.append(elements.mid(1));
        } else {
            if (cmd.length() > 1) {
                switch (cmd[1].toLatin1()) {
                case 'S':
                    category = CategorySystem;
                    break;
                case 'M':
                    category = CategoryMeasurement;
                    break;
                case 'C':
                    category = CategoryCalibration;
                    break;
                case 'D':
                    category = CategoryDiagnostics;
                    break;
                default:
                    category = CategoryUnknown;
                    break;
                }
            }
            if (cmd.length() > 3 && cmd[2] == QChar::Space) {
                action = cmd.mid(3);
            }
        }
    }
    if (args.isEmpty() && elements.count() > 1) {
        args.append(elements.mid(1));
    }

    if ((type == TypeDensityReflection || type == TypeDensityTransmission) && !args.isEmpty()) {
        return DensCommand(type, CategoryUnknown, QString(), args);
    } else if (type != TypeUnknown && category != CategoryUnknown && !action.isEmpty()) {
        return DensCommand(type, category, action, args);
    } else {
        return DensCommand();
    }
}

DensCommand::CommandType DensCommand::type() const
{
    return d->type;
}

DensCommand::CommandCategory DensCommand::category() const
{
    return d->category;
}

QString DensCommand::action() const
{
    return d->action;
}

QStringList DensCommand::args() const
{
    return d->args;
}

QByteArray DensCommand::buffer() const
{
    return d->buffer;
}

void DensCommand::setBuffer(const QByteArray &buffer)
{
    d->buffer = buffer;
}

QString DensCommand::toString() const
{
    QChar t_ch;
    QChar c_ch;

    switch (d->type) {
    case TypeSet:
        t_ch = QLatin1Char('S');
        break;
    case TypeGet:
        t_ch = QLatin1Char('G');
        break;
    case TypeInvoke:
        t_ch = QLatin1Char('I');
        break;
    case TypeDensityReflection:
        t_ch = QLatin1Char('R');
        break;
    case TypeDensityTransmission:
        t_ch = QLatin1Char('T');
        break;
    default:
        t_ch = QLatin1Char('?');
        break;
    }

    switch (d->category) {
    case CategorySystem:
        c_ch = QLatin1Char('S');
        break;
    case CategoryMeasurement:
        c_ch = QLatin1Char('M');
        break;
    case CategoryCalibration:
        c_ch = QLatin1Char('C');
        break;
    case CategoryDiagnostics:
        c_ch = QLatin1Char('D');
        break;
    default:
        c_ch = QLatin1Char('?');
        break;
    }

    QString result;
    if (d->type == TypeDensityReflection || d->type == TypeDensityTransmission) {
        result = QString("%1%2").arg(t_ch, d->args.join(QLatin1Char(',')));
    } else {
        result = QString("%1%2 %3").arg(t_ch, c_ch, d->action);
        if (!d->args.isEmpty()) {
            result.append(QLatin1Char(','));
            result.append(d->args.join(QLatin1Char(',')));
        }
    }
    return result;
}

QStringList DensCommand::splitLine(const QByteArray &line)
{
    QStringList result;
    const QList<QByteArray> elementList = line.trimmed().split(',');
    for (const QByteArray &element : elementList) {
        if (element.length() >= 2 && element.startsWith('"') && element.endsWith('"')) {
            QString elementStr = QString::fromLatin1(element.mid(1, element.size() - 2));
            result.append(elementStr);
        } else {
            result.append(QString::fromLatin1(element));
        }
    }
    return result;
}
