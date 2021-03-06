#!/bin/bash -x

if [ "x$SLURM_NPROCS" = "x" ]
then
  if [ "x$SLURM_NTASKS_PER_NODE" = "x" ]
  then
    SLURM_NTASKS_PER_NODE=1
  fi
  SLURM_NPROCS=`expr $SLURM_JOB_NUM_NODES \* $SLURM_NTASKS_PER_NODE`
else
  if [ "x$SLURM_NTASKS_PER_NODE" = "x" ]
  then
    SLURM_NTASKS_PER_NODE=`expr $SLURM_NPROCS / $SLURM_JOB_NUM_NODES`
  fi
fi

srun hostname -s | sort -u > /tmp/hosts.$SLURM_JOB_ID
awk "{ print \$0 \"-ib slots=$SLURM_NTASKS_PER_NODE\"; }" /tmp/hosts.$SLURM_JOB_ID >/tmp/tmp.$SLURM_JOB_ID
mv /tmp/tmp.$SLURM_JOB_ID /tmp/hosts.$SLURM_JOB_ID

module load xl_r spectrum-mpi

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 128 K $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_128k_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 256 K $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_256k_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 512 K $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_512k_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 1 M $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_1M_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 2 M $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_2M_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 4 M $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_4M_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 8 M $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_8M_$SLURM_NPROCS.txt

taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -hostfile /tmp/hosts.$SLURM_JOB_ID -np $SLURM_NPROCS /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/mpi-io-bench.out 16 M $SLURM_JOB_UID $SLURM_JOB_ID > /gpfs/u/home/PCPA/PCPAnbll/barn/csci_4320/assignment04/outputs/results_16M_$SLURM_NPROCS.txt

rm /tmp/hosts.$SLURM_JOB_ID