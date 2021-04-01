#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include <mpi.h>

/*
 *  * 64 bit, free running clock for POWER9/AiMOS system
 *   *  Has 512MHz resolution.
 *    */

unsigned long long aimos_clock_read(void)
{
  unsigned int tbl, tbu0, tbu1;

  do {
    __asm__ __volatile__ ("mftbu %0" : "=r"(tbu0));
    __asm__ __volatile__ ("mftb %0" : "=r"(tbl));
    __asm__ __volatile__ ("mftbu %0" : "=r"(tbu1));
  } while (tbu0 != tbu1);

  return (((unsigned long long)tbu0) << 32) | tbl;
}

int main(int argc, char *argv[]){

    int blockSize = atoi(argv[1]);
    char size = argv[2][0];
    int multi = 0;

    if(size == 'K'){
        multi = 1024;
    }
    else if(size == 'M'){
        multi = 1024*1024;
    }

    MPI_File file;
    int myrank, numranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);

    int file_size = multi * blockSize * 32;
    int bufsize = file_size/(numranks*32);
    char* buf = (char *) malloc(bufsize);
    int nchars = bufsize/sizeof(char);
    MPI_Status status;

    double start, end;
    if(myrank == 0){
      start = aimos_clock_read();
    }

    for(int i = 0; i < nchar; i++){
        buf[i] = '1';
    }

    MPI_File_open(MPI_COMM_WORLD, "testfile", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);

    for(int i = 0; i < 32; i++){
        MPI_Offset offset = i*multi*blockSize + (myrank*bufsize);
        MPI_File_write_at(file, offset, buf, nchars, MPI_CHAR, &status);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(myrank == 0){
      printf("Running test with block size %s %s, with %d ranks ...\n", argv[1], argv[2], numranks);
      end = aimos_clock_read();
      double tmp = end-start;
      printf("Write time: %.5lf\n", tmp/512000000);
    }

    MPI_File_close(&file);
    free(buf);

    if(myrank == 0){
      start = aimos_clock_read();
    }

    buf = (char *) malloc(bufsize);
    MPI_File_open(MPI_COMM_WORLD, "testfile", MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
    for(int i = 0; i < 32; i++){
        MPI_Offset offset = i*multi*blockSize + (myrank*bufsize);
        MPI_File_read_at(file, offset, buf, nchars, MPI_CHAR, &status);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(myrank == 0){
      end = aimos_clock_read();
      double tmp = end-start;
      printf("Read time: %.5lf\n", tmp/512000000);
    }

    MPI_File_close(&file);
    free(buf);
    MPI_Finalize();
    return 0;
}
