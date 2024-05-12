#include "SubMatrix.hpp"
#include <stdlib.h>


SubMatrix::SubMatrix(int numRows, int numCols) {
    this->numCols = numCols;
    this->numRows = numRows;

    // allocate and initialize matrix to 0
    size_t nElem = numCols * numRows;
    this->mData = (double*) malloc(nElem * sizeof(double));
    for (size_t i = 0; i < nElem; i++) {
        this->mData[i] = 0;
    }
    MPI_Type_vector(numRows, 1, numCols, MPI_DOUBLE, &this->stride);
    MPI_Type_commit(&this->stride);
}

SubMatrix::~SubMatrix() {
    free(this->mData);
    MPI_Type_free(&this->stride);
}

/**
 * @brief Return the number of rows or columns of the submatrix
 * 
 * @param a 1 for rows, 2 for columns
 * @return int number of rows or columns
 */
int SubMatrix::Size(int a) const {
    assert(1 <= a && a <= 2);
    if (a == 1)
        return this->numRows;
    else
        return this->numCols;
}

double SubMatrix::Read(int i, int j) const {
    size_t pos = (i-1) * this->numCols + (j-1); // position of the element in the array for row major indexing
    return this->mData[pos];
}

double& SubMatrix::operator()(int i, int j) {
    size_t pos = (i-1) * this->numCols + (j-1);
    return this->mData[pos];
}

void SubMatrix::SendReceiveRows(int rowsend, int ranksend, int rowrecv, int rankrecv, int tag, MPI_Comm comm) {
    // TODO 
    MPI_Status status;
    MPI_Sendrecv(this->mData + (rowsend-1)*this->numCols, 
                 this->numCols, // there are numCols elements per row
                 MPI_DOUBLE,    // Datatype is Double
                 ranksend,
                 tag,
                 this->mData + (rowrecv-1)*this->numCols,
                 this->numCols, // there are numCols elements per row
                 MPI_DOUBLE,    // Datatype is Double
                 rankrecv,
                 tag,
                 comm,
                 &status);
}

void SubMatrix::SendReceiveColumns(int colsend, int ranksend, int colrecv, int rankrecv, int tag, MPI_Comm comm) {

    MPI_Status status;
    MPI_Sendrecv(this->mData + (colsend-1), 
                 1,      // 1 stride has all the data
                 this->stride, // Datatype is stride (contains whole column)
                 ranksend,
                 tag,
                 this->mData + (colrecv-1),
                 1,      // 1 stride has all the data
                 this->stride, // Datatype is stride (contains whole column)
                 rankrecv,
                 tag,
                 comm,
                 &status);
}
