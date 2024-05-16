#!/bin/bash
#
#SBATCH --job-name=SC_mpi
#SBATCH --output=outputs/CPU=1_N=32.txt
#
#SBATCH --ntasks=1
#SBATCH --time=2:00
#SBATCH --mem-per-cpu=100

ml OpenMPI
module load OpenMPI
time make
