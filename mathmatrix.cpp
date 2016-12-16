#include "mathmatrix.h"

struct Data {
    Data(unsigned int r, unsigned int c) :
        refCounter(1),
        rows(r),
        columns(c),
        vsize(r*c)
    {
        if (vsize) v = new double[vsize];
    }

    ~Data() {
        if (vsize) delete[] v;
    }

    unsigned int refCounter;
    unsigned int rows;
    unsigned int columns;
    unsigned int vsize;

    double* v;

    unsigned int position(unsigned int r, unsigned int c) const {
        return r * columns + c;
    }

    // this can be sliced in multithreaded applications
    Data* copy() {
        Data* nd = new Data(rows, columns);
        for (unsigned int i = 0; i<vsize; ++i)
            nd->v[i] = v[i];
        return nd;
    }

};

MathMatrix::MathMatrix() :
    d(new Data(0, 0))
{

}

MathMatrix::MathMatrix(unsigned int rows, unsigned int cols) :
    d(new Data(rows, cols))
{
}

MathMatrix::MathMatrix(const MathMatrix &matrix) :
    d(matrix.d)
{
    d->refCounter++;
}

MathMatrix::~MathMatrix() {
    d->refCounter--;
    if (d->refCounter == 0)
        delete d;
}

unsigned int MathMatrix::rows() const {
    return d->rows;
}

unsigned int MathMatrix::columns() const {
    return d->columns;
}

MathMatrix& MathMatrix::operator=(const MathMatrix& matrix) {
    d->refCounter--;
    if (d->refCounter == 0)
        delete d;
    d = matrix.d;
    d->refCounter++;
    return *this;
}
