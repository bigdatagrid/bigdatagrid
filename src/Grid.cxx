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

#include "Grid.h"
#include "Master.h"

Grid::Grid(Master &masterin, int itotin, int jtotin, int ktotin)
{
  master = &masterin;

  itot = itotin;
  jtot = jtotin;
  ktot = ktotin;
  ntot = itot*jtot*ktot;

  master->printMessage("Constructed Grid\n");
}

Grid::~Grid()
{
  master->printMessage("Destructed Grid\n");
}

int Grid::getntot()
{
  return ntot;
}
