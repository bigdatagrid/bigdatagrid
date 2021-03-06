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
#include "Master.h"
#include "Grid.h"
#include "Field.h"

int main(int argc, char *argv[])
{
  try
  {
    auto grid = createGrid<double>(1, 1, 10);

    auto a = createField<double>(grid, "a");
    auto b = createField<double>(grid, "b");

    // fill field with random numbers
    for(auto &i : a.data)
      i = std::rand() % 10;

    for(auto &i : b.data)
      i = std::rand() % 10;

    // test the copy operator
    auto aa = createField<double>(grid, "aa");
    aa = a;

    // test the assignment operator
    auto bb(b);

    // test the compound addition operator
    auto c = a;
    c += b;

    // test the addition operator
    auto d = createField<double>(grid, "d");
    d = a + b + c;

    auto e = a + b + c;

    for(int n=0; n<a.data.size(); ++n)
    {
      std::ostringstream message;
      message << std::setw(2);
      message << n << " = {" 
        << std::setw(6) <<  a.data[n] << ", "
        << std::setw(6) << aa.data[n] << ", "
        << std::setw(6) <<  b.data[n] << ", "
        << std::setw(6) << bb.data[n] << ", "
        << std::setw(6) <<  c.data[n] << ", "
        << std::setw(6) <<  d.data[n] << ", "
        << std::setw(6) <<  e.data[n] << " }\n";
      Master &master = Master::getInstance();
      master.printMessage(message.str());
    }
  }

  catch (...)
  {
    return 1;
  }

  return 0;
}
