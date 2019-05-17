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
// Plugin flight data implementation.

#include "FlightMath.h"

namespace xplmpp {

// Calculate distance between two points on Earth using Haversine Formula
double CalcEarthDistance(double lat1, double lon1, double lat2, double lon2) {
  lat1 = DegreeToRadian(lat1);
  lat2 = DegreeToRadian(lat2);
  lon1 = DegreeToRadian(lon1);
  lon2 = DegreeToRadian(lon2);

  double dlon = lon2 - lon1;
  double dlat = lat2 - lat1;

  double a = pow(sin(dlat / 2.0), 2.0) +
    cos(lat1) * cos(lat2) * pow(sin(dlon / 2.0), 2.0);
  double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
  static const double kEarthRadius = 6372.8e3; // meters
  return c * kEarthRadius;
}

}  // namespace xplmpp
