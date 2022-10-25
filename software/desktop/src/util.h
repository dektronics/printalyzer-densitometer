#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <stddef.h>
#include <stdint.h>

class QObject;
class QValidator;

namespace util
{

void copy_from_u32(uint8_t *buf, uint32_t val);
uint32_t copy_to_u32(const uint8_t *buf);
void copy_from_f32(uint8_t *buf, float val);
float copy_to_f32(const uint8_t *buf);

QString encode_f32(float val);
float decode_f32(const QString &val);

double **make2DArray(const size_t rows, const size_t cols);
void free2DArray(double **array, const size_t rows);

QValidator *createIntValidator(int min, int max, QObject *parent = nullptr);
QValidator *createFloatValidator(double min, double max, int decimals, QObject *parent = nullptr);

}

#endif // UTIL_H
