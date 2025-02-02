/*
  Copyright 2020 Equinor ASA

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FPGA_UTILS_HEADER_INCLUDED
#define FPGA_UTILS_HEADER_INCLUDED

namespace Opm
{
namespace Accelerator
{

union double2int
{
    unsigned long int int_val;
    double double_val;
};

double second(void);
bool even(int n);
int roundUpTo(int i, int n);
bool fileExists(const char *filename);

} // namespace Accelerator
} // namespace Opm

#endif // FPGA_UTILS_HEADER_INCLUDED
