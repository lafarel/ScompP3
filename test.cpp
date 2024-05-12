#include <stddef.h>			// IGNACIO: added to run in windows
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <mpi.h>

#include "utils.hpp"
#include "SubMatrix.hpp"

void submatrixtest(int rank, int other_rank) {
    SubMatrix a(2, 2);
    if (rank == 0) {
        a(1, 1) = 1.; a(1, 2) = 2;
        a(2, 1) = 3; a(2, 2) = 4;
    } else {
        assert(rank == 1);
        a(1, 1) = 5.; a(1, 2) = 6;
        a(2, 1) = 7; a(2, 2) = 8;
    }

   a.SendReceiveColumns(1, other_rank, 2, other_rank, 0, MPI_COMM_WORLD);

   SubMatrix b(2, 2);
   if (rank == 0) {
       b(1, 1) = 1; b(1, 2) = 5;
       b(2, 1) = 3; b(2, 2) = 7;
   } else {
       assert(rank == 1);
       b(1, 1) = 5; b(1, 2) = 1;
       b(2, 1) = 7; b(2, 2) = 3;
   }
   testisapprox(a, b, "After SendReceiveColumns", rank);

   a.SendReceiveRows(2, other_rank, 1, other_rank, 1, MPI_COMM_WORLD);

   if (rank == 0) {
       b(1, 1) = 7; b(1, 2) = 3;
       b(2, 1) = 3; b(2, 2) = 7;
   } else {
       assert(rank == 1);
       b(1, 1) = 3; b(1, 2) = 7;
       b(2, 1) = 7; b(2, 2) = 3;
   }
   testisapprox(a, b, "After SendReceiveRows", rank);
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    assert(numprocs == 2);
    int myid;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    submatrixtest(myid, 1-myid);

    MPI_Finalize();
    return 0;
}
