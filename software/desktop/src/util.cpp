#include "util.h"

#include <QIntValidator>
#include <QDoubleValidator>
#include <string.h>

namespace util
{

void copy_from_u32(uint8_t *buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

uint32_t copy_to_u32(const uint8_t *buf)
{
    return (uint32_t)buf[0] << 24
        | (uint32_t)buf[1] << 16
        | (uint32_t)buf[2] << 8
        | (uint32_t)buf[3];
}

void copy_from_f32(uint8_t *buf, float val)
{
    uint32_t int_val;
    memcpy(&int_val, &val, sizeof(float));
    copy_from_u32(buf, int_val);
}

float copy_to_f32(const uint8_t *buf)
{
    float val;
    uint32_t int_val = copy_to_u32(buf);
    memcpy(&val, &int_val, sizeof(float));
    return val;
}

QString encode_f32(float val)
{
    QByteArray buf(4, Qt::Uninitialized);
    copy_from_f32(reinterpret_cast<uint8_t *>(buf.data()), val);
    QString result = buf.toHex().toUpper();
    return result;
}

float decode_f32(const QString &val)
{
    const QByteArray bytes = QByteArray::fromHex(val.toLatin1());
    return copy_to_f32((const uint8_t *)bytes.data());
}

double **make2DArray(const size_t rows, const size_t cols)
{
    double **array;

    array = new double*[rows];
    for (size_t i = 0; i < rows; i++) {
        array[i] = new double[cols];
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            array[i][j] = 0.;
        }
    }

    return array;
}

void free2DArray(double **array, const size_t rows)
{
    for (size_t i = 0; i < rows; i++) {
        delete[] array[i];
    }
    delete[] array;
}

QValidator *createIntValidator(int min, int max, QObject *parent)
{
    QIntValidator *validator = new QIntValidator(min, max, parent);
    return validator;
}

QValidator *createFloatValidator(double min, double max, int decimals, QObject *parent)
{
    QDoubleValidator *validator = new QDoubleValidator(min, max, decimals, parent);
    validator->setNotation(QDoubleValidator::StandardNotation);
    return validator;
}

}
