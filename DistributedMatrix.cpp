#include <cmath>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <algorithm>

#include "DistributedMatrix.hpp"

void decompose_domain(int n, int numprocs, int index, int* s, int* e)
{
    // Euclidean division
    // n = nlocal * numprocs + deficit with deficit < numprocs
    int nlocal  = n / numprocs; // divident
    int deficit = n % numprocs; // remainder

    *s = index * nlocal + 1;
    *s = *s + std::min(index, deficit);
    if (index < deficit) {
        nlocal = nlocal + 1;
    }
    *e = *s + nlocal - 1;
    assert(*e <= n);
}

void decompose_block(int numCols, int numRows,
    int *firstRow, int *lastRow, int *firstCol, int *lastCol,
    int *rankbottom, int *ranktop, int *rankleft, int *rankright,
    MPI_Comm *comm) {

    int dims[2];
    dims[0] = 0;
    dims[1] = 0;
    int periods[2];
    periods[0] = 0;
    periods[1] = 0;

    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    MPI_Dims_create(numprocs, 2, dims);

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, comm);

    // My neighbors are now +/- 1 with my rank. Handle the case of the
    // boundaries by using MPI_PROCNULL
    MPI_Cart_shift(*comm, 0, 1, rankbottom, ranktop);
    MPI_Cart_shift(*comm, 1, 1, rankleft, rankright);

    // Compute the actual decomposition
    int coords[2];
    MPI_Cart_get(*comm, 2, dims, periods, coords);
    decompose_domain(numRows-2, dims[0], coords[0], firstRow, lastRow);
    (*firstRow)++; (*lastRow)++; // one-based indexing
    decompose_domain(numCols-2, dims[1], coords[1], firstCol, lastCol);
    (*firstCol)++; (*lastCol)++; // one-based indexing
    // Extend to include ghost points
    (*firstRow)--;
    (*lastRow)++;
    (*firstCol)--;
    (*lastCol)++;
}

DistributedMatrix::DistributedMatrix(int numRows, int numCols) {
    assert(numRows > 0);
    assert(numCols > 0);
    this->numRows = numRows;
    this->numCols = numCols;

    decompose_block(numCols, numRows, &firstRow, &lastRow, &firstCol, &lastCol, &rankbottom, &ranktop, &rankleft, &rankright, &comm);

    m = new SubMatrix(lastRow-firstRow+1, lastCol-firstCol+1);
    init = false;
}
DistributedMatrix::~DistributedMatrix() {
    delete m;
    MPI_Comm_free(&comm);
}
int DistributedMatrix::Size(int i) const {
    assert(1 <= i && i <= 2);
    if (i == 1)
        return numRows;
    else
        return numCols;
}
int DistributedMatrix::FirstReadRow() const {
    return firstRow;
}
int DistributedMatrix::LastReadRow() const {
    return lastRow;
}
int DistributedMatrix::FirstReadColumn() const {
    return firstCol;
}
int DistributedMatrix::LastReadColumn() const {
    return lastCol;
}
int DistributedMatrix::FirstWriteRow() const {
    return firstRow + init; // Disallow writing ghost points once initialized
}
int DistributedMatrix::LastWriteRow() const {
    return lastRow - init;  // Disallow writing ghost points once initialized
}
int DistributedMatrix::FirstWriteColumn() const {
    return firstCol + init; // Disallow writing ghost points once initialized
}
int DistributedMatrix::LastWriteColumn() const {
    return lastCol - init;  // Disallow writing ghost points once initialized
}
double DistributedMatrix::Read(int i, int j) const {
    assert(FirstReadRow()    <= i && i <= LastReadRow());
    assert(FirstReadColumn() <= j && j <= LastReadColumn());
    return m->Read(i - firstRow + 1, j - firstCol + 1);
}
double& DistributedMatrix::operator()(int i, int j) {
    assert(FirstWriteRow()    <= i && i <= LastWriteRow());
    assert(FirstWriteColumn() <= j && j <= LastWriteColumn());
    return (*m)(i - firstRow + 1, j - firstCol + 1);
}
void DistributedMatrix::Initialized() {
    this->init = true;
}
void DistributedMatrix::Synchronize() {
    m->SendReceiveRows(   m->Size(1)-1, ranktop,    1,             rankbottom, 0, comm);
    m->SendReceiveRows(   2,               rankbottom, m->Size(1), ranktop,    1, comm);
    m->SendReceiveColumns(m->Size(2)-1, rankright,  1,             rankleft,   0, comm);
    m->SendReceiveColumns(2,               rankleft,   m->Size(2), rankright,  1, comm);
}
SubMatrix& DistributedMatrix::localblock() {
    return *m;
}
