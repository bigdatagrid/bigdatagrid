/*
 * BigDataGrid
 * Copyright (c) 2014-2015 Chiel van Heerwaarden
 *
 * Many of the classes and functions in BigDataGrid are derived from
 * MicroHH (https://github.com/microhh)
 *
 * This file is part of BigDataGrid
 *
 * BigDataGrid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * BigDataGrid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with BigDataGrid.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MASTER
#define MASTER

#ifdef USEMPI
#include <mpi.h>
#else
#include <sys/time.h>
#endif

#include <string>
#include <iostream>
#include <sstream>

class Master
{
  public:
    static Master &getInstance();

    std::string getVersion();

    void printMessage(std::string);
    void printError  (std::string);

    double getTime();

    int mpiid;

  private:
    Master();
    ~Master();

    // \TODO CUDA does not allow yet delete
    Master(const Master &) = delete;
    Master &operator=(const Master &) = delete;

    void cleanup();
    int checkError(int);

    bool allocated;
    bool initialized;

    int nprocs;
};


// IMPLEMENTATION BELOW
inline Master &Master::getInstance()
{
  static Master master;
  return master;
}

inline std::string Master::getVersion()
{
  return std::string(GITHASH);
}

#ifdef USEMPI
inline Master::Master()
{
  initialized = false;
  allocated = false;

  mpiid = 0;

  try
  {
    // initialize the MPI
    int n;
    n = MPI_Init(NULL, NULL);
    if(checkError(n))
    {
      printError("Error in Master constructor\n");
      throw 1;
    }

    initialized = true;

    // get the rank of the current process
    n = MPI_Comm_rank(MPI_COMM_WORLD, &mpiid);
    if(checkError(n))
    {
      printError("Error in Master constructor\n");
      throw 1;
    }

    // get the total number of processors
    n = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if(checkError(n))
    {
      printError("Error in Master constructor\n");
      throw 1;
    }

    std::ostringstream message;
    message << "Starting Master on " << nprocs << " process(es)\n";
    printMessage(message.str());
  }

  catch (...)
  {
    cleanup();
    throw 1;
  }
}
#else
inline Master::Master()
{
  initialized = true;
  allocated = false;

  mpiid = 0;
  nprocs = 1;

  std::ostringstream message;
  message << "Starting Master on " << nprocs << " process(es)\n";
  printMessage(message.str());
}
#endif

inline Master::~Master()
{
  cleanup();

  std::ostringstream message;
  message << "Finished Master on " << nprocs << " process(es)\n";
  printMessage(message.str());
}

inline void Master::cleanup()
{
  #ifdef USEMPI
  if(initialized)
    MPI_Finalize();
  #endif
}

/*
int Master::init()
{
  int n;

  if(nprocs != npx*npy)
  {
    if(mpiid == 0) std::printf("ERROR nprocs = %d does not equal npx*npy = %d*%d\n", nprocs, npx, npy);
    return 1;
  }

  int dims    [2] = {npy, npx};
  int periodic[2] = {true, true};

  // define the dimensions of the 2-D grid layout
  n = MPI_Dims_create(nprocs, 2, dims);
  if(checkerror(n))
    return 1;

  // create a 2-D grid communicator that is optimized for grid to grid transfer
  // first, free our temporary copy of COMM_WORLD
  n = MPI_Comm_free(&commxy);
  if(checkerror(n))
    return 1;
  // for now, do not reorder processes, blizzard gives large performance loss
  n = MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, false, &commxy);
  if(checkerror(n))
    return 1;
  n = MPI_Comm_rank(commxy, &mpiid);
  if(checkerror(n))
    return 1;

  // retrieve the x- and y-coordinates in the 2-D grid for each process
  int mpicoords[2];
  n = MPI_Cart_coords(commxy, mpiid, 2, mpicoords);
  if(checkerror(n))
    return 1;

  mpicoordx = mpicoords[1];
  mpicoordy = mpicoords[0];

  int dimx[2] = {false, true };
  int dimy[2] = {true , false};

  n = MPI_Cart_sub(commxy, dimx, &commx);
  if(checkerror(n))
    return 1;
  n = MPI_Cart_sub(commxy, dimy, &commy);
  if(checkerror(n))
    return 1;

  // find out who are the neighbors of this process to facilitate the communication routines
  n = MPI_Cart_shift(commxy, 1, 1, &nwest , &neast );
  if(checkerror(n))
    return 1;
  n = MPI_Cart_shift(commxy, 0, 1, &nsouth, &nnorth);
  if(checkerror(n))
    return 1;

  // create the requests arrays for the nonblocking sends
  int npmax;
  npmax = std::max(npx, npy);

  // have at least as many communicators as prognostic variables
  npmax = std::max(npmax, 8*4);
  reqs  = new MPI_Request[npmax*2];
  reqsn = 0;

  allocated = true;

  return 0;
}
*/

inline void Master::printMessage(std::string message)
{
  if(mpiid == 0)
    std::cout << message;
}

inline void Master::printError(std::string message)
{
  if(mpiid == 0)
    std::cerr << message;
}

inline int Master::checkError(int n)
{
  #ifdef USEMPI
  char errbuffer[MPI_MAX_ERROR_STRING];
  int errlen;

  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, errbuffer, &errlen);

    std::ostringstream message;
    message << "ERROR MPI "<< std::string(errbuffer) << "\n";
    printMessage(message.str());

    return 1;
  }
  #endif

  return 0;
}

inline double Master::getTime()
{
  #ifdef USEMPI
  return MPI_Wtime();
  #else
  timeval timestruct;
  gettimeofday(&timestruct, NULL);
  double time;
  time = (double)timestruct.tv_sec + (double)timestruct.tv_usec*1.e-6;
  return time;
  #endif
}
#endif
