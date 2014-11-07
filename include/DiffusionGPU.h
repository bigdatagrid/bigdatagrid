/*
 * BigData4D
 * Copyright (c) 2014 Chiel van Heerwaarden
 *
 * Many of the classes and functions in BigData4D are derived from
 * MicroHH (https://github.com/MicroHH)
 *
 * This file is part of BigData4D
 *
 * BigData4D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * BigData4D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with BigData4D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DIFFUSIONGPU
#define DIFFUSIONGPU

#include "Field.h"
#include "Grid.h"

template<class T, class TF>
class DiffusionGPU
{
  public:
    DiffusionGPU(Grid<T> &);
    virtual ~DiffusionGPU();

    void exec(thrust::device_vector<TF> &at_gpu, thrust::device_vector<TF> &a_gpu);

  protected:
    Grid<T> &grid;

  private:
    // __global__ void execDiffusion(TF *, TF *);
};

#include "DiffusionGPU.hxx"
#endif
