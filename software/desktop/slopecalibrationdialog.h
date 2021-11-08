#ifndef SLOPECALIBRATIONDIALOG_H
#define SLOPECALIBRATIONDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QList>
#include <QPair>
#include <tuple>
#include "densinterface.h"

namespace Ui {
class SlopeCalibrationDialog;
}

class SlopeCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SlopeCalibrationDialog(DensInterface *densInterface, QWidget *parent = nullptr);
    ~SlopeCalibrationDialog();

    std::tuple<float, float, float> calValues() const;

private slots:
    void onDensityReading(DensInterface::DensityType type, float dValue, float rawValue, float corrValue);
    void onActionCut();
    void onActionCopy();
    void onActionPaste();
    void onCalculateResults();
    void onClearReadings();

private:
    std::tuple<float, float, float> polyfit(const QList<float> &xList, const QList<float> &yList);
    void gaussEliminationLS(int m, int n, double **a /*[m][n]*/, double *x /*[n-1]*/);
    QPair<int, int> upperLeftActiveIndex() const;
    float itemValueAsFloat(int row, int col) const;

    Ui::SlopeCalibrationDialog *ui;
    QStandardItemModel *model_;
    DensInterface *densInterface_;
    std::tuple<float, float, float> calValues_;
};

#endif // SLOPECALIBRATIONDIALOG_H
