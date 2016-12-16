#ifndef MATHMATRIX_H
#define MATHMATRIX_H

#include "mathmatrix_global.h"

struct Data;

class MATHMATRIXSHARED_EXPORT MathMatrix {
public:
    MathMatrix();
    MathMatrix(unsigned int rows, unsigned int cols);
    MathMatrix(const MathMatrix& matrix);
    ~MathMatrix();

    unsigned int rows() const;
    unsigned int columns() const;
    double at(unsigned int r, unsigned int c) const;
    void setItem(unsigned int r, unsigned int c, double value);
    void setRow(unsigned int r, const std::initializer_list<double>& list);
    void setColumn(unsigned int c, const std::initializer_list<double>& list);
    void setMatrix(const std::initializer_list<std::initializer_list<double>>& matrix);

    MathMatrix& operator=(const MathMatrix& matrix);
    const double& operator()(unsigned int r, unsigned int c) const;
    double& operator()(unsigned int r, unsigned int c);

    MathMatrix& operator+(const MathMatrix& m);
    MathMatrix& operator-(const MathMatrix& m);
    MathMatrix& operator*(double x);
    MathMatrix& operator/(double x);

    Data * d;
};

MathMatrix& operator*(double x, MathMatrix& m);
MathMatrix& operator/(double x, MathMatrix& m);


#endif // MATHMATRIX_H
