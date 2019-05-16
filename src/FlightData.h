// Copyright (c) 2019 Peter Kvitek. All rights reserved.
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
// Plugin flight data.

#ifndef LANDEX_FLIGHTDATA_H
#define LANDEX_FLIGHTDATA_H

#include <vector>

#include "Common.h"
#include "xplmpp/Rect.h"

namespace xplmpp {

// Represents the plugin flight data
struct Data {
  Data() {} // random data
  Data(float time, float ground_speed, float vertical_speed, float agl,
    double lat, double lon, float heading, bool flying)
  : time(time)
  , ground_speed(ground_speed)
  , vertical_speed(vertical_speed)
  , agl(agl)
  , lat(lat)
  , lon(lon)
  , heading(heading)
  , flying(flying) {}

  float time;
  float ground_speed;
  float vertical_speed;
  float agl;
  double lat;
  double lon;
  float heading;
  bool flying;
};

class FlightData : public std::vector<Data> {
public:
  FlightData() = default;
  ~FlightData() = default;

  void Add(const Data& data);

  bool HasLanding() const {
    return landing_index_ > 0;
  }

  bool GetLanding(const_iterator& it) const;
  bool GetLanding(Data& data) const;

  bool GetLast(Data& data) const;

  void Reset();

private:
  size_t landing_index_ = 0;

};

extern FlightData g_flight_data;

}  // namespace xplmpp

#endif  // #ifndef LANDEX_FLIGHTDATA_H
