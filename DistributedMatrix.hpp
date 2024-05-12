#include <mpi.h>
#include "SubMatrix.hpp"

#ifndef DistributedMatrixHEADERDEF
#define DistributedMatrixHEADERDEF
class DistributedMatrix
{
    private:
        SubMatrix *m; // Block of this process
        bool init;        // Whether ghost point are initialized
        int firstRow;     // first row of block
        int lastRow;      // last row of block
        int firstCol;     // first column of block
        int lastCol;      // last column of block
        int rankbottom;   // rank of bottom process in comm
        int ranktop;      // rank of top process in comm
        int rankleft;     // rank of left process in comm
        int rankright;    // rank of right process in comm
        int numCols;      // Total number of columns
        int numRows;      // Total number of rows
        MPI_Comm comm;    // Communicator for 2D grid
    public:
        DistributedMatrix(int numRows, int numCols);
        ~DistributedMatrix();
        int Size(int) const;
        double Read(int i, int j) const;
        double& operator()(int, int);
        int FirstReadRow() const;
        int LastReadRow() const;
        int FirstReadColumn() const;
        int LastReadColumn() const;
        int FirstWriteRow() const;
        int LastWriteRow() const;
        int FirstWriteColumn() const;
        int LastWriteColumn() const;
        void Initialized(); // Call when boundary points have been initialized hence the user should be prevented to write ghost points
        void Synchronize(); // Synchronize ghost points with neighbors
        SubMatrix& localblock();
};
#endif
