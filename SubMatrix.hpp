#include "mpi.h"
#include <cassert>

#ifndef SubMatrixHEADERDEF
#define SubMatrixHEADERDEF
class SubMatrix
{
    private:
        double * mData;
        int numCols;
        int numRows;
        MPI_Datatype stride;
        double * ptr(int, int); // return ptr in mData for the furnished line and column
    public:
        SubMatrix(int numRows, int numCols);
        ~SubMatrix();
        int Size(int) const;
        double Read(int i, int j) const;
        double& operator()(int, int);
        void SendReceiveRows(int rowsend, int ranksend, int rowrecv, int rankrecv, int tag, MPI_Comm comm);
        void SendReceiveColumns(int colsend, int ranksend, int colrecv, int rankrecv, int tag, MPI_Comm comm);
};
#endif
