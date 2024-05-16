#!/bin/bash
#
#SBATCH --job-name=SC_mpi
#SBATCH --output=outputs/1_CPU.txt
#
#SBATCH --ntasks=1
#SBATCH --time=30:00
#SBATCH --mem-per-cpu=100

time mpiexec -n 1 ./poisson.out
