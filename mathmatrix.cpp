#include "mathmatrix.h"

struct Data {
    Data(unsigned int r, unsigned int c) :
        refCounter(1),
        rows(r),
        columns(c),
        vsize(r*c)
    {
        if (vsize) v = new double[vsize];
        double* pv = &(v[0]);
        for (unsigned int i=0; i<vsize; ++i, ++pv)
            *pv = 0;
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
//    Data* detach() {
//        Data* nd = new Data(rows, columns);
//        for (unsigned int i = 0; i<vsize; ++i)
//            nd->v[i] = v[i];
//        return nd;
//    }

    void detach(Data**d) {
        if (refCounter > 1) {
            refCounter--;
            Data* nd = new Data(rows, columns);
            for (unsigned int i = 0; i<vsize; ++i)
                nd->v[i] = v[i];
            *d = nd;
        }
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

double* MathMatrix::internal_pointer() const {
    return &(d->v[0]);
}

unsigned int MathMatrix::rows() const {
    return d->rows;
}

unsigned int MathMatrix::columns() const {
    return d->columns;
}

double MathMatrix::at(unsigned int r, unsigned int c) const {
    Q_ASSERT(r < d->rows && c < d->columns);
    return d->v[d->position(r, c)];
}

void MathMatrix::setItem(unsigned int r, unsigned int c, double value) {
    Q_ASSERT(r < d->rows && c < d->columns);
    d->detach(&d);
    d->v[d->position(r, c)] = value;
}

void MathMatrix::setRow(unsigned int r, const std::initializer_list<double>& list) {
    Q_ASSERT(r < d->rows && list.size() >= d->columns);
    d->detach(&d);
    // this can be sliced in multithreaded applications
    auto ilist = list.begin();
    double *pv = &(d->v[d->position(r,0)]);
    for (unsigned int c=0; c<d->columns; ++c, ++ilist, ++pv)
        *pv = *ilist;
}

void MathMatrix::setColumn(unsigned int c, const std::initializer_list<double>& list) {
    Q_ASSERT(c < d->columns && list.size() >= d->rows);
    d->detach(&d);
    // this can be sliced in multithreaded applications
    auto ilist = list.begin();
    double *pv = &(d->v[d->position(0, c)]);
    for (unsigned int r=0; r<d->rows; ++r, ++ilist, pv+=d->columns)
        *pv = *ilist;
}

void MathMatrix::setMatrix(const std::initializer_list<std::initializer_list<double>>& matrix) {
    Q_ASSERT(matrix.size() >= d->rows);
    d->detach(&d);
    // this can be sliced in multithreaded applications
    auto rlist = matrix.begin();
    double *pv = &(d->v[0]);
    for (unsigned int r=0; r<d->rows; ++r, ++rlist) {
        Q_ASSERT(rlist->size() >= d->columns);
        auto ilist = rlist->begin();
        for (unsigned int c=0; c<d->columns; ++c, ++ilist, ++pv)
            *pv = *ilist;
    }
}

MathMatrix& MathMatrix::operator=(const MathMatrix& matrix) {
    d->refCounter--;
    if (d->refCounter == 0)
        delete d;
    d = matrix.d;
    d->refCounter++;
    return *this;
}

const double& MathMatrix::operator()(unsigned int r, unsigned int c) const {
    Q_ASSERT(r < d->rows && c < d->columns);
    return d->v[d->position(r, c)];
}

double& MathMatrix::operator()(unsigned int r, unsigned int c) {
    Q_ASSERT(r < d->rows && c < d->columns);
    d->detach(&d);
    return d->v[d->position(r, c)];
}

MathMatrix MathMatrix::operator+(const MathMatrix& m)  const {
    Q_ASSERT(d->rows == m.rows() && d->columns == m.columns());
    MathMatrix result(d->rows, d->columns);
    double* pvR = result.internal_pointer();
    double* pv0 = internal_pointer();
    double* pv1 = m.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pvR, ++pv0, ++pv1)
        *pvR = *pv0 + *pv1;
    return result;
}

MathMatrix MathMatrix::operator-(const MathMatrix& m)  const {
    Q_ASSERT(d->rows == m.rows() && d->columns == m.columns());
    MathMatrix result(d->rows, d->columns);
    double* pvR = result.internal_pointer();
    double* pv0 = internal_pointer();
    double* pv1 = m.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pvR, ++pv0, ++pv1)
        *pvR = *pv0 - *pv1;
    return result;
}

MathMatrix MathMatrix::operator*(double x) const {
    MathMatrix result(d->rows, d->columns);
    double* pvR = result.internal_pointer();
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pvR, ++pv0)
        *pvR = *pv0 * x;
    return result;
}

MathMatrix MathMatrix::operator*(const MathMatrix& m) const {
    Q_ASSERT(d->columns == m.rows());
    MathMatrix result(d->rows, m.columns());
    double *pvR = result.internal_pointer();

    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<result.rows(); ++i) {
        for (unsigned int j=0; j<result.columns(); ++j, ++pvR) {
            double *pv0 = internal_pointer() + d->columns * i;
            double *pv1 = m.internal_pointer() + j;
            for (unsigned int k=0; k<d->columns; ++k, ++pv0, pv1+=m.columns())
                *pvR += (*pv0 * *pv1);
        }
    }
    return result;
}

MathMatrix MathMatrix::operator/(double x) const {
    Q_ASSERT(x != 0);
    MathMatrix result(d->rows, d->columns);
    double* pvR = result.internal_pointer();
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pvR, ++pv0)
        *pvR = *pv0 / x;
    return result;
}

MathMatrix& MathMatrix::operator+=(const MathMatrix& m) {
    Q_ASSERT(d->rows == m.rows() && d->columns == m.columns());
    d->detach(&d);
    double* pv0 = internal_pointer();
    double* pv1 = m.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pv0, ++pv1)
        *pv0 += *pv1;
    return *this;
}

MathMatrix& MathMatrix::operator-=(const MathMatrix& m) {
    Q_ASSERT(d->rows == m.rows() && d->columns == m.columns());
    d->detach(&d);
    double* pv0 = internal_pointer();
    double* pv1 = m.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pv0, ++pv1)
        *pv0 -= *pv1;
    return *this;
}

MathMatrix& MathMatrix::operator*=(double x) {
    d->detach(&d);
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pv0)
        *pv0 *= x;
    return *this;
}

MathMatrix& MathMatrix::operator/=(double x) {
    Q_ASSERT(x != 0);
    d->detach(&d);
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pv0)
        *pv0 /= x;
    return *this;
}

MathMatrix& MathMatrix::operator++() {
    d->detach(&d);
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pv0)
        (*pv0)++;
    return *this;
}

MathMatrix& MathMatrix::operator--() {
    d->detach(&d);
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pv0)
        (*pv0)--;
    return *this;
}

MathMatrix MathMatrix::subMatrix(unsigned int row, unsigned int column) const {
    Q_ASSERT(d->rows > 1 && d->columns > 1);
    MathMatrix result(d->rows-1, d->columns-1);
    double* pvR = result.internal_pointer();
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<result.rows(); ++i) {
        pv0 += (i==row ? d->columns : 0);
        for (unsigned int j=0; j<result.columns(); ++j, ++pvR, ++pv0) {
            pv0 += (j==column ? 1 : 0);
            *pvR = *pv0;
        }
    }
    return result;
}

MathMatrix MathMatrix::identity(unsigned int size) {
    MathMatrix result(size, size);
    double* p = result.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<size; ++i, p+=(size+1))
        *p = 1.0;
    return result;
}

MathMatrix MathMatrix::diagonal(const std::initializer_list<double>& ditems) {
    MathMatrix result(ditems.size(), ditems.size());
    double *p = result.internal_pointer();
    auto iter = ditems.begin();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<ditems.size(); ++i, ++iter, p+=(ditems.size()+1))
        *p = *iter;
    return result;
}

MathMatrix operator*(double x, MathMatrix& m) {
    MathMatrix result(m.rows(), m.columns());
    double* pvR = result.internal_pointer();
    double* pv0 = m.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<(m.rows() * m.columns()); ++i, ++pvR, ++pv0)
        *pvR = *pv0 * x;
    return result;
}

MathMatrix operator/(double x, MathMatrix& m) {
    Q_ASSERT(x != 0);
    MathMatrix result(m.rows(), m.columns());
    double* pvR = result.internal_pointer();
    double* pv0 = m.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<(m.rows() * m.columns()); ++i, ++pvR, ++pv0)
        *pvR = *pv0 / x;
    return result;
}
