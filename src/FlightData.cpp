// Copyright 2019 Peter Kvitek.
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

#include "FlightData.h"

namespace xplmpp {

FlightData g_flight_data;

namespace {

static const float kDataDifferenceThreshold = 0.000001f;

bool DataDifference(const Data& data, const Data& data2) {
  return fabs(data2.ground_speed - data.ground_speed) > kDataDifferenceThreshold ||
         fabs(data2.vertical_speed - data.vertical_speed) > kDataDifferenceThreshold ||
         fabs(data2.agl - data.agl) > kDataDifferenceThreshold ||
         data2.flying != data.flying;
}

} // namespace

void FlightData::Add(const Data& data) {
  if (!data.flying) {
    // Avoid adding entries before flying
    if (empty())
      return;
    // Cache first landing entry
    if (!landing_index_) {
      landing_index_ = size();
    }
  }

  // Advance time on the last entry if not enough differene
  if (!empty() && !DataDifference(data, back())) {
    back().time = data.time;
    return;
  }

  // TODO(kvitekp): bound size by keeping track of the distance
  // travelled and limiting it to 3nm
  emplace_back(data);
}

bool FlightData::GetLanding(const_iterator& it) const {
  if (!landing_index_)
    return false;

  it = cbegin();
  advance(it, landing_index_);
  return true;
}

bool FlightData::GetLast(Data& data) const {
  if (empty())
    return false;

  data = back();
  return true;
}

void FlightData::Reset() {
  clear();
  shrink_to_fit();
  landing_index_ = 0;
}

}  // namespace xplmpp
