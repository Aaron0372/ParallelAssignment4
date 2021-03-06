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
    //On command line "mpirun <c file> <integer size of block (128 or 1,etc.)> <char block multiplier ("M" or "K")>"
    int blockSize = atoi(argv[1]);
    char size = argv[2][0];

    //initialize multiplier for blockSize
    int multi = 0;

    char filename[100] = "testfile";

    //sets path for nvme if parameters are passed in
    if (argc == 5){
      //On command line "mpirun <c file> <integer size of block> <char block multiplier> <$SLURM_JOB_UID> <$SLURM_JOB_ID>"
      char* uid = argv[3];
      char* job_id = argv[4];
      snprintf(filename, 100, "/mnt/nvme/uid_%s/job_%s/testfile", uid, job_id);
    }
    
    //set multiplier based on blockSize
    if(size == 'K'){
        multi = 1024;
    }
    else if(size == 'M'){
        multi = 1024*1024;
    }

    MPI_File file;
    int myrank, numranks, color;
    int stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);

    //initialize files_size, and buffer values
    int file_size = multi * blockSize * 32;
    int bufsize = file_size/(numranks*32);
    char* buf = (char *) malloc(bufsize);
    int nchars = bufsize/sizeof(char);
    MPI_Status status;

    //split into two comms if its on nvme with 64 ranks
    if (argc == 5){
      color = myrank / 32;
    }
    else{
      color = 0;
    }
    MPI_Comm row_comm;
    stat = MPI_Comm_split(MPI_COMM_WORLD, color, myrank, &row_comm);
    if (stat != 0){
          printf("Rank %d: MPI_Comm_split error code %d\n", myrank, stat);
    }

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);

    //initializes start and end times
    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);

    /* START OF WRITE */

    //set start using clock_read
    if(myrank == 0){
          printf("%s\n", filename);
          printf("Running test with block size %s %s, with %d ranks ...\n", argv[1], argv[2], numranks);
          start = aimos_clock_read();
    }

    // initialize buffer
    for(int i = 0; i < nchars; i++){
        buf[i] = '1';
    }

    //Open file with write only, create it if it doesn't exist
    stat = MPI_File_open(row_comm, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
    if (stat != 0){
      printf("Rank %d: MPI_File_open error code %d\n", myrank, stat);
    }

    //write to the offset at each of the 32 blocks
    for(int i = 0; i < 32; i++){
        MPI_Offset offset = i*multi*blockSize + (myrank*bufsize);
        stat = MPI_File_write_at(file, offset, buf, nchars, MPI_CHAR, &status);
        if (stat != 0){
          printf("Rank %d: MPI_File_write_at error code %d\n", myrank, stat);
        }
        //Barrier to write each block in a row
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //wait for all data to be written
    MPI_Barrier(MPI_COMM_WORLD);

    //get end time and print total time with cycle -> second conversion
    if(myrank == 0){
      end = aimos_clock_read();
      double tmp = end-start;
      printf("Write time: %.5lf\n", tmp/512000000);
    }

    //close and free necessary values
    MPI_File_close(&file);
    free(buf);

    /* START OF READ */

    //set start time for file reads
    if(myrank == 0){
      start = aimos_clock_read();
    }

    //reallocate buffer and reopen file with read only
    buf = (char *) malloc(bufsize);
    stat = MPI_File_open(row_comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
    if (stat != 0){
      printf("Rank %d: MPI_File_open error code %d\n", myrank, stat);
    }
    
    //read at the offset for each of the 32 blocks
    for(int i = 0; i < 32; i++){
        MPI_Offset offset = i*multi*blockSize + (myrank*bufsize);
        stat = MPI_File_read_at(file, offset, buf, nchars, MPI_CHAR, &status);
        if (stat != 0){
          printf("Rank %d: MPI_File_read_at error code: %d\n", myrank, stat);
        }
        //Barrier to read each block in a row
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //wait for all data to be read
    MPI_Barrier(MPI_COMM_WORLD);

    //get end time and print total time with cycle -> second conversion
    if(myrank == 0){
      end = aimos_clock_read();
      double tmp = end-start;
      printf("Read time: %.5lf\n", tmp/512000000);
    }

    //close, free, and finalize necessary
    MPI_File_close(&file);
    free(buf);
    MPI_Finalize();
    return 0;
}
