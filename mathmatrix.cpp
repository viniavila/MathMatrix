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

MathMatrix& MathMatrix::operator+(const MathMatrix& m) {
    Q_ASSERT(d->rows == m.rows() && d->columns == m.columns());
    d->detach(&d);
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i)
        d->v[i] += m.d->v[i];
    return *this;
}

MathMatrix& MathMatrix::operator-(const MathMatrix& m) {
    Q_ASSERT(d->rows == m.rows() && d->columns == m.columns());
    d->detach(&d);
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i)
        d->v[i] -= m.d->v[i];
    return *this;
}

MathMatrix& MathMatrix::operator*(double x) {
    d->detach(&d);
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i)
        d->v[i] *= x;
    return *this;
}

MathMatrix& MathMatrix::operator/(double x) {
    Q_ASSERT(x != 0);
    d->detach(&d);
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<d->vsize; ++i)
        d->v[i] /= x;
    return *this;
}


MathMatrix& operator*(double x, MathMatrix& m) {
    (m.d)->detach(&(m.d));
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<(m.d)->vsize; ++i)
        (m.d)->v[i] *= x;
    return m;
}

MathMatrix& operator/(double x, MathMatrix& m) {
    Q_ASSERT(x != 0);
    (m.d)->detach(&(m.d));
    // this can be sliced in multithreaded applications
    for (unsigned int i=0; i<(m.d)->vsize; ++i)
        (m.d)->v[i] = x / (m.d)->v[i];
    return m;
}
