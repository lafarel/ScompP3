#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iomanip>
#include "utils.hpp"

bool isapprox(const SubMatrix& x, const SubMatrix& y, double rtol, double atol)
{
    for (int i = 1; i <= 2; i++) {
        if (x.Size(i) != y.Size(i)) {
            return false;
        }
    }
    for (int i = 1; i <= x.Size(1); i++) {
        for (int j = 1; j <= x.Size(2); j++) {
            double a = x.Read(i, j);
            double b = y.Read(i, j);
            if (fabs(a - b) > atol + rtol*fmax(fabs(a), fabs(b)))
            {
                printf(ANSI_COLOR_RED " x[%d, %d] = %lf != %lf = y[%d, %d]\n" ANSI_COLOR_RESET, i, j, a, b, i, j);
                return false;
            }
        }
    }
    return true;
}

void testisapprox(const SubMatrix &x, const SubMatrix &y, char const *s, int rank)
{
    printf(ANSI_COLOR_BLUE "%s %d" ANSI_COLOR_RESET ":", s, rank);
    if (!isapprox(x, y)) {
        exit(1);
    } else {
        printf(ANSI_COLOR_GREEN " OK" ANSI_COLOR_RESET "\n");
    }
}



void compute_dims(int ndims, int *dims) {
    dims[0] = 0;
    dims[1] = 0;
    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    MPI_Dims_create(numprocs, ndims, dims);
}

void PrintArray(int rank, const SubMatrix &A)
{
    for (int i = 1; i <= A.Size(1); i++) {
        printf("%d: ", rank);
        for (int j = 1; j <= A.Size(2); j++) {
            printf("%.4f%c", A.Read(i, j), j == A.Size(2) ? '\n' : ' ');
        }
    }
    printf("\n");
}
void PrintArray(int rank, DistributedMatrix &A) {
    PrintArray(rank, A.localblock());
}

int ndigits(int n) {
    if (n < 10)
        return 1;
    else
        return 1 + ndigits(n/10);
}
std::ostream& operator<<(std::ostream& stream, const DistributedMatrix& a) {
    double buf[2];
    stream.precision(4);
    stream << std::fixed;
    int myid = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    if (a.FirstReadRow() == 1 && a.FirstReadColumn() == 1) {
        int dims[2];
        compute_dims(2, dims);
        int leftrank = 0;
        for (int i = 1; i <= a.Size(1); i++) {
            stream << ANSI_COLOR_CYAN << std::setw(ndigits(a.Size(1))) << i << ANSI_COLOR_RESET << ":" << ANSI_COLOR_MAGENTA;
            int rank = leftrank;
            for (int j = 1; j <= a.Size(2); j++) {
                stream << " ";
                if (i != 1 && i != a.Size(1) && j == 2)
                    stream << ANSI_COLOR_RESET;
                if (i != 1 && i != a.Size(1) && j == a.Size(2))
                    stream << ANSI_COLOR_MAGENTA;
                if (i == 1 || i == a.Size(1) || j == 1) {
                    stream << 1.;
                } else if (j == a.Size(2)) {
                    stream << 0.;
                } else if (a.FirstWriteRow() <= i && i <= a.LastWriteRow() && a.FirstWriteColumn() <= j  && j <= a.LastWriteColumn()) {
                    stream << a.Read(i, j);
                    if (j == a.LastWriteColumn()) {
                        rank++;
                    }
                    if (i == a.LastWriteRow() && j+1 == a.Size(2))
                        leftrank += dims[1];
                } else {
                    MPI_Recv(buf, 2, MPI_DOUBLE, rank, (i-1)*a.Size(2)+j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    stream << buf[0];
                    if (buf[1] < 0.) {
                        rank++;
                        if(buf[1] == -2 && (leftrank+dims[1]) == rank) {
                            leftrank += dims[1];
                        }
                    }
                }
            }
            stream << std::endl;
        }
        stream << ANSI_COLOR_RESET;
        return stream;
    } else {
        for (int i = a.FirstWriteRow(); i <= a.LastWriteRow(); i++) {
            for (int j = a.FirstWriteColumn(); j <= a.LastWriteColumn(); j++) {
                buf[0] = a.Read(i, j);
                buf[1] = 0.;
                if (j == a.LastWriteColumn()) {
                    buf[1] = -1.;
                    if (i == a.LastWriteRow())
                        buf[1] = -2;
                }
                MPI_Send(buf, 2, MPI_DOUBLE, 0, (i-1)*a.Size(2)+j, MPI_COMM_WORLD);
            }
        }
    }
    return stream;
}
