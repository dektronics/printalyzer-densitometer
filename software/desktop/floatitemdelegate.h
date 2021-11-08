#ifndef FLOATITEMDELEGATE_H
#define FLOATITEMDELEGATE_H

#include <QStyledItemDelegate>

class FloatItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FloatItemDelegate(double min, double max, int decimals, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
private:
    double min_;
    double max_;
    int decimals_;
};

#endif // FLOATITEMDELEGATE_H
