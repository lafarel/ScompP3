/*
 * A solution to the Poisson problem using Jacobi
 * iteration and a 2-d decomposition
 *
 * The size of the domain is read by processor 0 and broadcasted to
 * all other processors. The Jacobi iteration is run until the
 * change in successive elements is small or a maximum number of
 * iterations is reached. The difference is printed out at each
 * step.
 */

#include <stddef.h> // added to run in windows
#include <mpi.h>
#include <iostream>

#include "utils.hpp"
#include "SubMatrix.hpp"
#include "DistributedMatrix.hpp"
#include "poisson.hpp"

void initialize(DistributedMatrix &a) {
    for (int i = a.FirstReadRow(); i <= a.LastReadRow(); i++)
        for (int j = a.FirstReadColumn(); j <= a.LastReadColumn(); j++)
            if (i == 1 || i == a.Size(1) || j == 1)
                a(i, j) = 1.0;
    a.Initialized();
}

// destructor for SubMatrix need to be called before MPI_Finalize so
// they cannot be local variable of the main function
double solve(int nx, int ny, int iterationmax) {
    int displayed_processor = 0;
    int myid = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    // Initalize the right-hand side (f) and the inital solution guess (a)
    DistributedMatrix a(nx, ny);
    initialize(a);
    DistributedMatrix b(nx, ny);
    initialize(b);
    DistributedMatrix f(nx, ny);
    f.Initialized();
    //std::cout << a;

    // Actually do the computation. Note the use of a collective operation to
    // check for convergence, and a do-loop to bound the number of iterations
    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    int it = 0;
    double diffnorm = 0.0;
    for (it = 0 ; it < iterationmax ; it++) {
        b.Synchronize();
        jacobi_iteration(b, f, a);
        //std::cout << a;
        a.Synchronize();
        //std::cout << a;
        jacobi_iteration(a, f, b);
        //std::cout << b;
        double diffnorm = sum_squares(a, b);
        /*
        if (myid == displayed_processor)
            printf(ANSI_COLOR_BLUE "diffnorm in iteration %d: %.8f\n" ANSI_COLOR_RESET, it, diffnorm);
        if (diffnorm < 1.0e-8) break;*/
    }
    double t2 = MPI_Wtime();
    
    
    if (myid == displayed_processor) {
        
        //printf(ANSI_COLOR_YELLOW "Exiting after %d iterations in %.5e secs\n" ANSI_COLOR_RESET,
                //2*it, t2-t1);
        return t2-t1;
        /*
        if (it >= iterationmax)
            printf(ANSI_COLOR_RED "Failed to converge\n" ANSI_COLOR_RESET);
        else
            printf(ANSI_COLOR_GREEN "Converged to a solution\n" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_YELLOW "Exiting after %d iterations in %.5e secs\n" ANSI_COLOR_RESET,
                2*it, t2-t1);
        printf(ANSI_COLOR_BLUE "diffnorm is %.1f\n" ANSI_COLOR_RESET, diffnorm);
        std::cout << "Solution is:" << std::endl;
        */
    }
    //std::cout << a;
}

int main(int argc, char* argv[]) {

    size_t N_min = 32;
    size_t N_max = 10000;
    size_t n_iter = 250;
    double max_duration = 60.0; // maximum time 
    double duration;
    bool stop = false;
    MPI_Init(&argc, &argv);
    
    int myid = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (myid == 0) {
        std::cout << "N,duration\n";
    }

    for (size_t N = N_min; N < N_max; N *= 2) {

        double local_duration = solve(N, N, n_iter);
        
        MPI_Allreduce(&local_duration, &duration, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        if (myid == 0) {
            std::cout << N << "," <<duration << "\n";
        }
        if (duration > max_duration / 4.0) {
            stop = true;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        if (stop) break;
    }
    MPI_Finalize();
}
