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
    MathMatrix clone() const;
    MathMatrix& swapRows(unsigned int row1, unsigned int row2);
    MathMatrix& swapColumns(unsigned int row1, unsigned int row2);

    MathMatrix& operator=(const MathMatrix& matrix);
    const double& operator()(unsigned int r, unsigned int c) const;
    double& operator()(unsigned int r, unsigned int c);

    MathMatrix operator+(const MathMatrix& m) const;
    MathMatrix operator-(const MathMatrix& m) const;
    MathMatrix operator*(double x) const;
    MathMatrix operator*(const MathMatrix& m) const;
    MathMatrix operator/(double x) const;
    MathMatrix& operator+=(const MathMatrix& m);
    MathMatrix& operator-=(const MathMatrix& m);
    MathMatrix& operator*=(double x);
    MathMatrix& operator/=(double x);
    MathMatrix& operator++();
    MathMatrix& operator--();

    double trace() const;
    double determinant() const;
    MathMatrix subMatrix(unsigned int row, unsigned int column) const;
    MathMatrix transposed() const;
    MathMatrix& transpose();
    MathMatrix inverse() const;
    MathMatrix& invert();

    double* internal_pointer() const;

    static MathMatrix identity(unsigned int size);
    static MathMatrix diagonal(const std::initializer_list<double>& ditems);

    operator QString() const;

private:
    Data * d;

};

MathMatrix operator*(double x, MathMatrix& m);
MathMatrix operator/(double x, MathMatrix& m);


#endif // MATHMATRIX_H
