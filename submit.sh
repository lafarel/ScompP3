#!/bin/bash
#
#SBATCH --job-name=SC_mpi
#SBATCH --output=outputs/quicktest.txt
#
#SBATCH --ntasks=8
#SBATCH --time=2:00
#SBATCH --mem-per-cpu=100

module load OpenMPI
time make
