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
// Plugin flight loop.

#ifndef LANDEX_FLIGHTLOOP_H
#define LANDEX_FLIGHTLOOP_H

#include "Common.h"

#define WRITE_TRACE_FILE 0

#if WRITE_TRACE_FILE
#include <iostream>
#include <fstream>
#include <iomanip>
#endif

#include "xplmpp/XPLMData.h"

#include "FlightLoopClient.h"

namespace xplmpp {

// Represents the plugin menu
class FlightLoop {
public:
  FlightLoop(FlightLoopClient* client);
  ~FlightLoop();

  static std::unique_ptr<FlightLoop> Create(FlightLoopClient* client);

private:
  void FindDataSources();

  bool HasLanded();
  bool IsFlying() { return !HasLanded();  }

  void UpdateState();

  float OnFlightLoopCallback(float elapsed_since_last_call,
                             float elapsedTimeSinceLastFlightLoop);

  static float FlightLoopCallback(float elapsed_since_last_call,
    float elapsed_time_since_last_flightLoop,
    int counter, void* refcon);

  FlightLoopClient* client_;

  enum class State {
    unknown,
    landed,
    flying,
  } state_ = State::unknown;

  float first_elapsed_time_since_last_flightLoop_ = 0.0;

  #define DATAREF_F(getter, member) \
    XPLMData member; \
    float getter() { return member.GetDataf(); }

  DATAREF_F(FaxilGear, faxil_gear_) // Gear/ground forces - backward - ACF Z, newtons
  DATAREF_F(GroundSpeed, ground_speed_) // Ground speed, meters/sec
  DATAREF_F(VerticalSpeed, vertical_speed_) // The vertical velocity, meters/sec
  DATAREF_F(GForce, gforce_) // G force, meters/sec^2
  DATAREF_F(Agl, agl_) // Altitude above ground level, meters

#if WRITE_TRACE_FILE
  std::ofstream file_;
#endif
};

}  // namespace xplmpp

#endif  // #ifndef LANDEX_FLIGHTLOOP_H
