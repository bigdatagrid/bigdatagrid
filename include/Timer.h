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

#ifndef TIMER
#define TIMER

#include <vector>
#include <sstream>
#include <iomanip>
#include "Master.h"

class Timer
{
  public:
    Timer(std::string);
    virtual ~Timer();

    void start();
    void sample();
    void end();

    double getTotal();

  protected:
    std::vector<double> samples;
    std::string name;
};


// IMPLEMENTATION BELOW
inline Timer::Timer(std::string namein)
{
  name = namein;
}

inline Timer::~Timer()
{
}

inline void Timer::start()
{
  Master &master = Master::getInstance();
  std::ostringstream message;
  message << "Start timer " << name << "\n";
  master.printMessage(message.str());
  samples.push_back(master.getTime());
}

inline void Timer::end()
{
  Master &master = Master::getInstance();
  samples.push_back(master.getTime());
  std::ostringstream message;
  message << "End timer " << name << ", elapsed time (s): "
          << std::setprecision(5) << getTotal() << "\n";
  master.printMessage(message.str());
}

inline void Timer::sample()
{
  Master &master = Master::getInstance();
  samples.push_back(master.getTime());
}

inline double Timer::getTotal()
{
  return samples.back() - samples.front();
}
#endif
