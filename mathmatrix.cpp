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

MathMatrix MathMatrix::clone() const {
    MathMatrix c(*this);
    d->detach(&(c.d));
    return c;
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

double MathMatrix::trace() const {
    Q_ASSERT(d->rows == d->columns && d->vsize);
    double result = 0;
    double *p = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->rows; ++i, p+=(d->rows+1))
        result += (*p);
    return result;
}

double MathMatrix::determinant() const {
    Q_ASSERT(d->rows == d->columns && d->vsize > 0);
    if (d->vsize == 1) return d->v[0];
    if (d->vsize == 4) return (d->v[0] * d->v[3] - d->v[1] * d->v[2]);
    // Gauss elimination;
    // this can be sliced in multithreaded applications
    double result = 1;
    MathMatrix mod = clone();
    unsigned int n = d->rows;
    for (unsigned int j=0; j<n-1; ++j) {
        for (unsigned int i=0; (i+j)<(n-1); ++i) {
            double scalar = mod.at(i+1, j) / mod.at(i, j);
            for (unsigned int k=j; k<n; ++k)
                mod.setItem(i, k, (mod.at(i+1, k) / scalar) - mod.at(i, k));
        }
    }
    double *p = mod.internal_pointer() + (n-1);
    for (unsigned int i=0; i<n; ++i, p+=(n-1))
        result *= *p;
    return -result;
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

MathMatrix MathMatrix::transposed() const {
    Q_ASSERT(d->vsize > 0);
    MathMatrix result(d->columns, d->rows);
    double* pvR = result.internal_pointer();
    double* pv0 = internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pvR, pv0+=d->columns) {
        pv0 = i % d->rows ? pv0 : internal_pointer() + (i/d->rows);
        *pvR = *pv0;
    }
    return result;
}

MathMatrix& MathMatrix::transpose() {
    Q_ASSERT_X(d->vsize > 0, "MathMatrix::transpose", "This matrix has no dimensions, cannot be transposed;");
    d->detach(&d);
    double* nv = new double[d->vsize];
    double* pv0 = internal_pointer();
    double* pvR = &(nv[0]);
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i, ++pvR, pv0+=d->columns) {
        pv0 = i % d->rows ? pv0 : internal_pointer() + (i/d->rows);
        *pvR = *pv0;
    }
    // Replacing the internal vector and swap rows and columns values
    unsigned int t = d->rows;
    d->rows = d->columns;
    d->columns = t;
    delete[] d->v;
    d->v = nv;
    return *this;
}

MathMatrix MathMatrix::inverted() const {
    Q_ASSERT_X(d->rows == d->columns, "MathMatrix::inverted", "This matrix is not square, cannot be inverted");
    double det = determinant();
    Q_ASSERT_X(det != 0, "MathMatrix::inverted", "Cannot be inverted because its determinant is zero.");
    MathMatrix adjunct(d->rows, d->columns);
    for (unsigned int i=0; i<d->rows; ++i) {
        for (unsigned int j=0; j<d->columns; ++i) {
            MathMatrix sub = subMatrix(i, j);
            adjunct.setItem(i, j, std::pow(-1, i+j+2)*sub.determinant());
        }
    }
    adjunct.transpose();
    return (1.0/det)*adjunct;
}

MathMatrix& MathMatrix::invert() {
    MathMatrix inv = inverted();
    d->refCounter--;
    if (d->refCounter == 0) delete d;
    d = inv.d;
    d->refCounter++;
    return (*this);
}

MathMatrix MathMatrix::identity(unsigned int size) {
    Q_ASSERT(size > 0);
    MathMatrix result(size, size);
    double* p = result.internal_pointer();
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<size; ++i, p+=(size+1))
        *p = 1.0;
    return result;
}

MathMatrix MathMatrix::diagonal(const std::initializer_list<double>& ditems) {
    Q_ASSERT(ditems.size() > 0);
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
