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
// Plugin flight loop client interface.

#ifndef LANDEX_FLIGHTLOOP_CLIENT_H
#define LANDEX_FLIGHTLOOP_CLIENT_H

namespace xplmpp {

// Flying info data.
struct FlyingInfo {
  FlyingInfo(float ground_speed = 0, float vertical_speed = 0, float agl = 0)
  : ground_speed(ground_speed)
  , vertical_speed(vertical_speed)
  , agl(agl) {}

  float ground_speed;
  float vertical_speed;
  float agl;
};

// Landing info data.
struct LandingInfo {
  LandingInfo(float ground_speed = 0, float vertical_speed = 0, float gforce = 0)
  : ground_speed(ground_speed)
  , vertical_speed(vertical_speed)
  , gforce(gforce) {}

  float ground_speed;
  float vertical_speed;
  float gforce;
};

// Flight loop client interface.
struct FlightLoopClient {
  virtual void OnAirplaneFlying(const FlyingInfo& info) = 0;
  virtual void OnAirplaneLanded(const LandingInfo& info) = 0;
};

}  // namespace xplmpp

#endif  // #ifndef LANDEX_FLIGHTLOOP_CLIENT_H
