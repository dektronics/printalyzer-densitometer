#include "logwindow.h"
#include "ui_logwindow.h"

#include <QDebug>

#include "logger.h"

LogWindow::LogWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogWindow),
    logger_(new Logger)
{
    ui->setupUi(this);
    logger_->setEnabled(true);
    setCentralWidget(logger_);

    ui->actionFollow->setChecked(true);

    connect(ui->actionFollow, &QAction::toggled, this, &LogWindow::onFollowToggled);
    connect(ui->actionClear, &QAction::triggered, this, &LogWindow::onClearTriggered);
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

void LogWindow::onFollowToggled(bool checked)
{
    logger_->setAutoScroll(checked);
}

void LogWindow::onClearTriggered()
{
    logger_->clear();
}
