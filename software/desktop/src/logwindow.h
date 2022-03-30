#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QMainWindow>

namespace Ui {
class LogWindow;
}
class Logger;

class LogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = nullptr);
    ~LogWindow();

public slots:
    void appendLogLine(const QByteArray &line);

signals:
    void opened();
    void closed();

private slots:
    void onFollowToggled(bool checked);
    void onClearTriggered();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::LogWindow *ui;
    Logger *logger_ = nullptr;;
};

#endif // LOGWINDOW_H
