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

    MathMatrix& operator=(const MathMatrix& matrix);
    double& operator()(unsigned int r, unsigned int c);

private:
    Data * d;
};

#endif // MATHMATRIX_H
