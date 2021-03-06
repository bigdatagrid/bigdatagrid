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

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include "Grid.h"
#include "Field.h"
#include "Timer.h"

// CUDA functions
extern void waitCUDA();

extern void prepareCUDA(float **, float **, float *, float *);
extern void testCUDA(float *, float *);
extern void finishCUDA(float *, float *, float *);

extern void testCUDA_thrust(thrust::device_vector<float> &, const thrust::device_vector<float> &);

extern void prepareCUDA_cublas(float **, float **, float *, float *);
extern void testCUDA_cublas(float *, float *);
extern void finishCUDA_cublas(float *, float *, float *);

int main(int argc, char *argv[])
{
  try
  {
    try
    {
      auto grid = createGrid<float>(384, 384, 384);

      auto a = createField<float>(grid, "a");

      // fill field with random numbers
      for(auto &i : a.data)
        i = std::rand() % 10;

      // make a copy of a
      auto b(a);

      Timer timer1("CPU");
      timer1.start();
      for(int n=0; n<100; ++n)
        a += b;
      timer1.end();

      float *a_gpu, *b_gpu;
      prepareCUDA(&a_gpu, &b_gpu, &b.data[0], &b.data[0]);

      Timer timer2("GPU (manual)");
      timer2.start();
      for(int n=0; n<100; ++n)
        testCUDA(a_gpu, b_gpu);
      waitCUDA();
      timer2.end();

      auto acuda = createField<float>(grid, "acuda");
      finishCUDA(a_gpu, b_gpu, &acuda.data[0]);

      // THRUST
      thrust::device_vector<float> athrust(b.data.begin(), b.data.end());
      thrust::device_vector<float> bthrust(b.data.begin(), b.data.end());

      Timer timer3("GPU (Thrust)");
      timer3.start();
      for(int n=0; n<100; ++n)
        testCUDA_thrust(athrust, bthrust);
      waitCUDA();
      timer3.end();

      auto athrustout = createField<float>(grid, "athrustout");
      thrust::copy(athrust.begin(), athrust.end(), athrustout.data.begin());

      // cuBLAS
      prepareCUDA_cublas(&a_gpu, &b_gpu, &b.data[0], &b.data[0]);

      Timer timer4("GPU (cuBLAS)");
      timer4.start();
      for(int n=0; n<100; ++n)
        testCUDA_cublas(a_gpu, b_gpu);
      waitCUDA();
      timer4.end();

      auto acublas = createField<float>(grid, "acublas");
      finishCUDA_cublas(a_gpu, b_gpu, &acublas.data[0]);

      std::ostringstream message;
      message << "Elapsed time (s): "
              << std::setprecision(5) << timer1.getTotal() << ", "
              << std::setprecision(5) << timer2.getTotal() << ", "
              << std::setprecision(5) << timer3.getTotal() << ", "
              << std::setprecision(5) << timer4.getTotal() << "\n"
              << "Speedup CUDA (no thrust): " << timer1.getTotal() / timer2.getTotal() << "\n"
              << "Speedup CUDA (thrust)   : " << timer1.getTotal() / timer3.getTotal() << "\n"
              << "Speedup CUDA (cublas)   : " << timer1.getTotal() / timer4.getTotal() << "\n";
      Master &master = Master::getInstance();
      master.printMessage(message.str());

      for(int n=3; n<a.data.size(); n+=384*384*20)
      {
        std::ostringstream message;
        message << std::setw(8);
        message << n << " = {" 
          << std::setw(6) <<          b.data[n] << ", "
          << std::setw(6) <<          a.data[n] << ", "
          << std::setw(6) <<      acuda.data[n] << ", "
          << std::setw(6) << athrustout.data[n] << ", "
          << std::setw(6) <<    acublas.data[n] << " }\n";
        master.printMessage(message.str());
      }
    }

    // catch std::exceptions
    catch (std::exception &e)
    {
      std::ostringstream message;
      message << "Exception caught: " << e.what() << "\n";
      Master &master = Master::getInstance();
      master.printError(message.str());
      throw 1;
    }
    
    // catch unknown errors
    catch (...)
    {
      Master &master = Master::getInstance();
      master.printError("Oops! An unknown error has occured!\n");
      throw 1;
    }
  }

  catch (...)
  {
    return 1;
  }

  return 0;
}
