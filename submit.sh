#!/bin/bash
#
#SBATCH --job-name=SC_mpi
#SBATCH --output=outputs/quicktest.txt
#
#SBATCH --ntasks=8
#SBATCH --time=2:00
#SBATCH --mem-per-cpu=100

ml OpenMPI
module load OpenMPI
time make
