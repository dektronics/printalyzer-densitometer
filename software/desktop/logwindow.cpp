#include "logwindow.h"
#include "ui_logwindow.h"

#include "logger.h"

LogWindow::LogWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogWindow),
    logger_(new Logger)
{
    ui->setupUi(this);
    logger_->setEnabled(true);
    setCentralWidget(logger_);
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    emit opened();
}

void LogWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit closed();
}

void LogWindow::appendLogLine(const QByteArray &line)
{
    logger_->putData(line);
}
