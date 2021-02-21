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

#include "FlightData.h"
#include "FlightMath.h"

namespace xplmpp {

FlightData g_flight_data;

namespace {

static const float kDataDifferenceThreshold = 0.000001f;
static const float kAglChangeResetThreshold = 5.0f;

static const float kLandingHeadingThreshold = 15.0;  // degrees
static const float kLandingDistanceThreshold = 50.0;  // meters

bool DataDifference(const Data& data, const Data& data2) {
  return fabs(data2.ground_speed - data.ground_speed) > kDataDifferenceThreshold ||
         fabs(data2.vertical_speed - data.vertical_speed) > kDataDifferenceThreshold ||
         fabs(data2.agl - data.agl) > kDataDifferenceThreshold ||
         fabs(data2.msl - data.msl) > kDataDifferenceThreshold ||
         fabs(data2.lat - data.lat) > kDataDifferenceThreshold ||
         fabs(data2.lon - data.lon) > kDataDifferenceThreshold ||
         fabs(data2.heading - data.heading) > kDataDifferenceThreshold ||
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

  // Advance time on the last entry if not enough difference
  if (!empty() && !DataDifference(data, back())) {
    back().time = data.time;
    return;
  }

  // Check for abrupt AGL changes and reset dataset since
  // chances are that flight situation was reloaded.
  if (!empty() && fabs(data.agl - back().agl) > kAglChangeResetThreshold)
    Reset();

  // Check if landed and update last landing info
  if (!empty() && back().flying && !data.flying) {
    last_landing_ = data;
    has_last_landing_ = true;
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

bool FlightData::GetLanding(Data& data) const {
  const_iterator it;
  if (!GetLanding(it))
    return false;
  data = *it;
  return true;
}

bool FlightData::GetLast(Data& data) const {
  if (empty())
    return false;

  data = back();
  return true;
}

bool FlightData::GetLastLanding(Data& data) const {
  if (!has_last_landing_)
    return false;

  data = last_landing_;
  return true;
}

bool FlightData::IsLastLandingHeading() const {
  if (empty() || !has_last_landing_)
    return false;

  return IsLastLandingHeading(back().heading);
}

bool FlightData::IsLastLandingHeading(float heading) const {
  if (!has_last_landing_)
    return false;

  float heading_delta = fabs(heading - last_landing_.heading);
  return heading_delta < kLandingHeadingThreshold;
}

float FlightData::GetLastLandingDistance(double lat, double lon) const {
  assert(has_last_landing_);
  double distance = CalcEarthDistance(lat, lon, last_landing_.lat, last_landing_.lon);
  return static_cast<float>(distance);
}

void FlightData::Reset() {
  clear();
  shrink_to_fit();
  landing_index_ = 0;
}

}  // namespace xplmpp
