#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPlainTextEdit>

class Logger : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Logger(QWidget *parent = nullptr);
    void putData(const QByteArray &data);
};

#endif // LOGGER_H
