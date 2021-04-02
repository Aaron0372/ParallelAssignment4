all: mpi-io-bench.c
	mpicc -O3 mpi-io-bench.c -o mpi-io-bench.out