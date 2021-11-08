#include "floatitemdelegate.h"

#include <QLineEdit>
#include <QDebug>

FloatItemDelegate::FloatItemDelegate(double min, double max, int decimals, QObject *parent) :
    QStyledItemDelegate(parent),
    min_(min), max_(max), decimals_(decimals)
{
}

QWidget *FloatItemDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    QWidget *widget = QStyledItemDelegate::createEditor(parent, option, index);
    QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(widget);
    if (lineEdit) {
        QDoubleValidator *validator = new QDoubleValidator(min_, max_, decimals_);
        validator->setNotation(QDoubleValidator::StandardNotation);
        lineEdit->setValidator(validator);
    }
    return widget;
}
