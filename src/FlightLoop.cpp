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
// Plugin flight loop implementation.

#include "FlightLoop.h"

#include "XPLMProcessing.h"

#include "FlightData.h"

namespace xplmpp {

static const float kFlightLoopIntervalSeconds = 0.05f;
static const float kInitialSettleDownTimeout = 3.0f;
static const float kFlyingCallbackPeriod = 1.0f;
static const float kLandingHeadingThreshold = 15.0;  // degrees

std::unique_ptr<FlightLoop> FlightLoop::Create(FlightLoopClient* client) {
  return std::make_unique<FlightLoop>(client);
}

FlightLoop::FlightLoop(FlightLoopClient* client)
: client_(client) {
#if WRITE_TRACE_FILE
  file_.open("/tmp/0/landex_data.txt");
  file_.precision(6);
  file_ << std::fixed;
#endif

  FindDataSources();

  ::XPLMRegisterFlightLoopCallback(FlightLoopCallback,
                                  kFlightLoopIntervalSeconds,
                                  this);
}

FlightLoop::~FlightLoop() {
  ::XPLMUnregisterFlightLoopCallback(FlightLoopCallback, this);
  g_flight_data.Reset();
#if WRITE_TRACE_FILE
  file_.close();
#endif
}

void FlightLoop::FindDataSources() {
  replay_mode_.Find("sim/operation/prefs/replay_mode");
  faxil_gear_.Find("sim/flightmodel/forces/faxil_gear");
  ground_speed_.Find("sim/flightmodel/position/groundspeed");
  vertical_speed_.Find("sim/flightmodel/position/vh_ind");
  gforce_.Find("sim/flightmodel2/misc/gforce_normal");
  agl_.Find("sim/flightmodel/position/y_agl");
  latitude_.Find("sim/flightmodel/position/latitude");
  longitude_.Find("sim/flightmodel/position/longitude");
  heading_.Find("sim/flightmodel/position/true_psi");
}

bool FlightLoop::IsFlying() {
  return (!ReplayMode() && FaxilGear() == 0.0) || Agl() > 0.25;
}

bool FlightLoop::UpdateState() {
  switch (state_) {
  case State::unknown:
    state_ = IsFlying() ? State::flying : State::landed;
    break;
  case State::flying:
    if (HasLanded()) {
      state_ = State::landed;
      LandingInfo info(GroundSpeed(), VerticalSpeed(), GForce());
      client_->OnAirplaneLanded(info);
      return true;
    }
    break;
  case State::landed:
    if (IsFlying()) {
      state_ = State::flying;
      FlyingInfo info(GroundSpeed(), VerticalSpeed(), Agl());
      client_->OnAirplaneFlying(info);
      time_since_last_flying_report_ = 0.0;
      return true;
    }
    break;
  }

  return false;
}

float FlightLoop::OnFlightLoopCallback(float elapsed_since_last_call,
                                       float elapsed_time_since_last_flightLoop) {
  if (!first_elapsed_time_since_last_flightLoop_) {
    first_elapsed_time_since_last_flightLoop_ = elapsed_time_since_last_flightLoop;
  } else
  if (elapsed_time_since_last_flightLoop - first_elapsed_time_since_last_flightLoop_
      < kInitialSettleDownTimeout) {
    // Do nothing letting things to settle down
  } else {
    // Update state and provide periodic flying callback
    if (!UpdateState() && state_ == State::flying) {
      time_since_last_flying_report_ += elapsed_since_last_call;
      if (time_since_last_flying_report_ >= kFlyingCallbackPeriod) {
        FlyingInfo info(GroundSpeed(), VerticalSpeed(), Agl());
        client_->OnAirplaneFlying(info);
        time_since_last_flying_report_ = 0.0;
      }
    }

    // Check if turned crosswind or otherwise deviated from landing heading and
    // reset collected flight data if so.
    Data landing_data;
    if (IsFlying() && g_flight_data.GetLanding(landing_data)) {
      float heading_delta = fabs(Heading() - landing_data.heading);
      if (heading_delta > kLandingHeadingThreshold)
        g_flight_data.Reset();
    }

    // Append flight data
    g_flight_data.Add(
      Data(elapsed_time_since_last_flightLoop, GroundSpeed(), VerticalSpeed(),
           Agl(), Latitude(), Longitude(), Heading(), IsFlying()));
  }

#if WRITE_TRACE_FILE
  file_ << "state=" << (int)state_
        << " gs=" << GroundSpeed()
        << " vs=" << VerticalSpeed()
        << " gf=" << GForce()
        << " agl=" << Agl()
        << " pos=(" << Latitude() << ", " << Longitude() << ")"
        << " hdi=" << Heading()
#if 0
        << " elapsed_since_last_call=" << elapsed_since_last_call
        << " elapsed_time_since_last_flightLoop=" << elapsed_time_since_last_flightLoop
        << " replay_mode=" << ReplayMode()
#endif
        << "\n";
#endif

  return kFlightLoopIntervalSeconds;
}

float FlightLoop::FlightLoopCallback(float elapsed_since_last_call,
                                     float elapsed_time_since_last_flightLoop,
                                     int counter, void* refcon) {
  return static_cast<FlightLoop*>(refcon)->OnFlightLoopCallback(
      elapsed_since_last_call, elapsed_time_since_last_flightLoop);
}


}  // namespace xplmpp
