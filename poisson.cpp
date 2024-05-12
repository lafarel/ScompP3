#include "poisson.hpp"

void jacobi_iteration(const DistributedMatrix &a, const DistributedMatrix &f, DistributedMatrix &b) {

    size_t n = a.Size(1) - 1;
    double h = 1.0 / (double) n;

    for (size_t i = b.FirstWriteRow(); i <= b.LastWriteRow(); i++) { // iterate on rows

        if (i==1 || i==n+1) continue; // boundary point

        for (size_t j = b.FirstWriteColumn(); j <= b.LastWriteColumn(); j++) { // iterate on columns

            if (j==1 || j==n+1) continue; // boundary point
            
            // Compute Jacobi iteration :
            b(i, j) = (a.Read(i-1, j) + a.Read(i+1, j) + a.Read(i, j-1) + a.Read(i, j+1) - h * h * f.Read(i, j)) / 4;
        }
    }
};

double sum_squares(const DistributedMatrix &a, const DistributedMatrix &b) {

    double local_error = 0.0; // buffer for this process' error
    size_t n = a.Size(1) - 1;

    for (size_t i = b.FirstWriteRow(); i <= b.LastWriteRow(); i++) { // iterate on rows

        if (i==1 || i==n+1) continue; // boundary point

        for (size_t j = b.FirstWriteColumn(); j <= b.LastWriteColumn(); j++) { // iterate on columns

            if (j==1 || j==n+1) continue; // boundary point
            
            // add to sum
            double error = b.Read(i, j) - a.Read(i, j);
            local_error += error * error;
        }
    }

    double global_error; // buffer for receiving data

    MPI_Allreduce(&local_error, &global_error, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    
    return global_error;
}