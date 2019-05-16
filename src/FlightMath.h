// Copyright (c) 2019 Peter Kvitek.
//
// Author: Peter Kvitek (pete@kvitek.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Flight math functions.

#ifndef LANDEX_FLIGHTMATH_H
#define LANDEX_FLIGHTMATH_H

#define _USE_MATH_DEFINES
#include <math.h>

inline float RoundOff(float value, float factor = 10.0f) {
  return rintf(value * factor) / factor;
}

inline float MetersPerSecondToFeetPerMinute(float meters_per_second) {
  return meters_per_second * 196.8504f;
}

inline float MetersPerSecondToKnots(float meters_per_second) {
  return meters_per_second * 1.94384f;
}

inline float MetersToFeet(float meters) {
  return meters * 3.28084f;
}

inline double DegreeToRadian(double angle) {
  return M_PI * angle / 180.0;
}

#endif // #ifndef LANDEX_FLIGHTMATH_H
